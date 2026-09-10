# 跨模块调用链

- 对应源码版本：`main` / `0983c65`。
- 证据状态：以下是源码可见的主链；动态 `load()` 内部编译调用不展开为仓库函数。
- 最后更新：2026-09-10

## 代表链 1：elementwise

```text
kernels/elementwise/elementwise.py:lib
 -> elementwise_add_f32/f32x4/f16...
 -> TORCH_BINDING_ELEM_ADD
 -> elementwise_add_*_kernel
 -> c[idx] = a[idx] + b[idx]
```

证据：`kernels/elementwise/elementwise.py:9-24`；`kernels/elementwise/elementwise.cu:22-127,140-198`。

## 代表链 2：NMS

```text
nms.py check_correctness
 -> lib.nms
 -> nms()
 -> scores.sort + index_select
 -> nms_iou_mask_kernel
 -> nms_resolve_kernel
 -> keep.to(CPU)/order.to(CPU)
 -> torch::tensor(original indices)
```

证据：`kernels/nms/nms.py:39-48,87-121`；`kernels/nms/nms.cu:126-189`。

## 代表链 3：HGEMM

```text
hgemm.py CLI
 -> benchmark helper
 -> loaded pybind extension
 -> selected WMMA/MMA/CuTe/CUDA kernel
 -> synchronize
 -> TFLOPS and optional plot
```

证据：`kernels/hgemm/hgemm.py:18-177,210-328`；`kernels/hgemm/README.md:38-79`。

## 代表链 4：FlashAttention

```text
flash_attn_mma.py
 -> get_build_sources/get_build_cuda_cflags
 -> load(flash_attn_lib)
 -> pybind/flash_attn.cc
 -> selected MMA/CuTe implementation
 -> reference/check_all_close + benchmark
```

证据：`kernels/flash-attn/flash_attn_mma.py:80-227,285-413`。

## Demo 对照链

```text
D01: nms.py → load → nms() → mask kernel → resolve kernel → CPU index mapping
D02: build.sh → notes-v2 binary → CLI → test/bench → CUDA resources → output/cleanup
```

D01 证据：`kernels/nms/nms.py:8-23,87-136`; `kernels/nms/nms.cu:22-192`。D02 证据：`kernels/interview/build.sh:134-185`; `kernels/interview/notes-v2.cu:510-542,2420-2579`。

## M06/M07 深层终点

M06 代表链终点是 `mma.sync` accumulator 和 collective C store，而不是 PyBind；M07 代表链终点是 online `m/l/O` 更新、最终归一化和 O store，而不是 launcher。证据分别为 `[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:769-839,1850-1885]`、`[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:401-797]`。


1. 先定位 Python 入口和 `load()`/setuptools。
2. 再定位 pybind 或 `.cu` 中导出的 binding。
3. 区分 launcher、device kernel 和 benchmark helper；三者不是同一个函数。
4. 对模板/CuTe 代码以实际实例化点为准，不从函数名推断 layout。
