# 跨模块调用链

- 文档目的：解释 90-cross-module/cross-module-call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：以下是源码可见的主链；动态 `load()` 内部编译调用不展开为仓库函数。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/cross-module-call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
