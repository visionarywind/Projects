# M02 调用链

- 对应源码版本：`main` / `4513b31`。
- 证据状态：elementwise add 代表调用链已确认；stream 归属和动态 loader 内部为未知/推断。
- 最后更新：2026-09-10

## 初始化链：`M02-CALL-INIT-001`

```text
Python module import
  -> torch.utils.cpp_extension.load("elementwise_lib", sources=["elementwise.cu"])
    -> compile/load extension (PyTorch runtime)
      -> PYBIND11_MODULE(TORCH_EXTENSION_NAME, m)
        -> m.def("elementwise_add_f32", ...)
        -> ... m.def("elementwise_add_f16x8_pack", ...)
```

源码位置：`[kernels/elementwise/elementwise.py:8-24]`、`[kernels/elementwise/elementwise.cu:130-138,191-198]`。

## 正常核心链：`M02-CALL-MAIN-001`

```text
run_benchmark(lib.elementwise_add_f32x4, a, b, "f32x4", c)
  -> elementwise_add_f32x4(torch::Tensor a, b, c)
    -> CHECK_TORCH_TENSOR_DTYPE(a/b/c, torch::kFloat32)
      -> derive ndim, S, K, N
        -> choose block/grid
          -> elementwise_add_f32x4_kernel<<<grid, block>>>(data_ptrs, N)
            -> idx = pack_start
              -> FLOAT4(a[idx]), FLOAT4(b[idx])
                -> four additions in registers
                  -> FLOAT4(c[idx]) = reg_c
```

对应实现：`[kernels/elementwise/elementwise.py:27-46]`、`[kernels/elementwise/elementwise.cu:140-180]`、`[kernels/elementwise/elementwise.cu:32-49]`。真正输出副作用是 `FLOAT4(c[idx]) = reg_c`；前面的函数均为加载、校验、映射或提交。

## 尾部链：`M02-CALL-TAIL-001`

```text
f32x4 kernel
  -> idx + 3 < N ? vector path : idx < N ? scalar for-loop : no-op
    -> c[idx+i] = a[idx+i] + b[idx+i]
```

证据：`[kernels/elementwise/elementwise.cu:34-48]`。x2/x8/pack 使用相同的“完整 pack + scalar tail”结构，证据分别在 `[kernels/elementwise/elementwise.cu:62-72,77-103,109-127]`。

## 错误链：`M02-CALL-ERROR-001`

```text
wrong dtype
  -> CHECK_TORCH_TENSOR_DTYPE
    -> print Tensor options
      -> throw std::runtime_error
        -> Python caller receives extension exception
```

源码没有在同一链路中检查 shape/device/stride，也没有在 launch 后调用显式 CUDA error API。因此这些错误不会沿着已确认的 dtype 链路被入口拒绝；后果和暴露时机标记为未知/推断。[kernels/elementwise/elementwise.cu:134-145,154-180]

## 观测/同步链：`M02-CALL-OBSERVE-001`

```text
kernel enqueue
  -> Python launches warmup/iters
    -> torch.cuda.synchronize()
      -> read out.flatten().cpu().numpy()
        -> print sample and elapsed time
```

证据：`[kernels/elementwise/elementwise.py:41-66]`。同步是 benchmark 观察边界，不是 kernel wrapper 的返回值语义。

## 修改影响

- 修改 wrapper 的 `n_elements`：同时影响 block 宽度、`idx` 计算、尾部条件和 vector alignment 假设。[kernels/elementwise/elementwise.cu:140-189]
- 修改 Python 输出 tensor 复用：影响 benchmark 的写回所有权与结果观察，但不改变 kernel ABI。[kernels/elementwise/elementwise.py:37-66]
- 修改 kernel pack layout：必须同时更新 wrapper 的 dispatch、输入 contiguous 约束、正确性用例和性能解释。

## 相关文档

- [implementation.md](implementation.md)
- [execution-flows.md](execution-flows.md)
- [source-map.md](source-map.md)

## 源码证据摘要

- `[kernels/elementwise/elementwise.py:8-66]`。
- `[kernels/elementwise/elementwise.cu:32-49,140-198]`。

## 未解决问题

需要通过 `CUDA_LAUNCH_BLOCKING=1` 和非连续/非对齐输入实验确认错误暴露位置。

## 下一步阅读建议

修改前先比较 scalar 与 pack 两条链的输入前提，再阅读 M02 `testing.md`。
