# M02 源码地图

- 文档目的：解释 01-modules/M02-elementwise/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：elementwise 代表路径已确认；激活目录只登记入口。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-elementwise/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 源码位置 | 类型 | 作用 | 入口/调用方 | 依赖 | 重要性 |
|---|---|---|---|---|---|
| `kernels/elementwise/elementwise.py:8-24` | Python build entry | 动态编译并加载 extension | 用户运行脚本/导入脚本 | PyTorch C++ extension、CUDA | 高 |
| `kernels/elementwise/elementwise.py:27-66` | benchmark helper | warmup、同步、计时、输出采样 | module-level benchmark loop | CUDA tensor | 高 |
| `kernels/elementwise/elementwise.py:69-95` | executable example | 生成 FP32/FP16 输入并调用所有 add variant | 脚本主流程 | GPU | 中 |
| `kernels/elementwise/elementwise.cu:22-27` | `__global__` | scalar FP32 add | generated wrapper | global memory | 高 |
| `kernels/elementwise/elementwise.cu:32-49` | `__global__` | FP32 x4 vector add + tail | generated wrapper | `float4` alias | 高 |
| `kernels/elementwise/elementwise.cu:54-72` | `__global__` | FP16 scalar/x2 add | generated wrapper | half/half2 | 高 |
| `kernels/elementwise/elementwise.cu:75-127` | `__global__` | FP16 x8 and 128-bit pack add | generated wrapper | half2/float4 alias | 高 |
| `kernels/elementwise/elementwise.cu:130-138` | macro | stringifies and registers functions | binding macro | PyBind | 高 |
| `kernels/elementwise/elementwise.cu:140-182` | generated C++ wrapper | dtype check, shape-derived launch geometry | PyBind export | PyTorch tensor API | 高 |
| `kernels/elementwise/elementwise.cu:184-189` | instantiation | materializes six wrappers | module init | kernels above | 高 |
| `kernels/elementwise/elementwise.cu:191-198` | PyBind module | exports Python names | extension loader | pybind11 | 高 |
| `kernels/<activation>/<activation>.py` | Python driver | 激活变体的 load/benchmark entry | 用户脚本 | PyTorch/CUDA | 中 |
| `kernels/<activation>/<activation>.cu` | CUDA implementation | 各激活函数的 scalar/packed kernel | 对应 binding | 具体目录实现 | 中 |

## 入口到副作用

```text
 elementwise.py:load
   -> PYBIND11_MODULE
      -> m.def generated wrapper
         -> elementwise_add_*_kernel<<<...>>>
            -> c[...] = a[...] + b[...]
```

关键区别：`load()` 和 `m.def()` 负责构建/导出，只有 kernel 的 global store 才改变用户输出。[kernels/elementwise/elementwise.cu:22-27,43-47,95-127]

## 代表变体矩阵

| Python 名称 | wrapper | kernel | 元素粒度 | 尾部 |
|---|---|---|---:|---|
| `elementwise_add_f32` | `elementwise_add_f32` | `elementwise_add_f32_kernel` | 1 | `idx < N` |
| `elementwise_add_f32x4` | `elementwise_add_f32x4` | `elementwise_add_f32x4_kernel` | 4 | scalar loop |
| `elementwise_add_f16` | `elementwise_add_f16` | `elementwise_add_f16_kernel` | 1 | `idx < N` |
| `elementwise_add_f16x2` | `elementwise_add_f16x2` | `elementwise_add_f16x2_kernel` | 2 | one half |
| `elementwise_add_f16x8` | `elementwise_add_f16x8` | `elementwise_add_f16x8_kernel` | 8 | scalar loop |
| `elementwise_add_f16x8_pack` | `elementwise_add_f16x8_pack` | `elementwise_add_f16x8_pack_kernel` | 8/128 bit | scalar loop |

证据：`[kernels/elementwise/elementwise.cu:54-127,184-198]`。

## 当前未覆盖

- 激活目录的函数名、绑定实现和边界检查未在本表逐项展开。
- 未验证 extension cache 位置、编译架构和不同 CUDA/PyTorch 版本的 ABI 兼容性。

## 相关文档

- [README.md](README.md)
- [implementation.md](implementation.md)
- [execution-flows.md](execution-flows.md)
- [call-chains.md](call-chains.md)

## 源码证据摘要

- `[kernels/elementwise/elementwise.py:8-95]`。
- `[kernels/elementwise/elementwise.cu:19-198]`。

## 未解决问题

需要逐激活目录建立同等粒度的 source map，或在确认其完全复用相同模式后记录复用证据。

## 下一步阅读建议

从 `implementation.md` 的 dispatch 分支开始，再对照 `.cu` 的 pack kernel。
