# M16 Kernel 与设备后端

- 文档目的：解释 01-modules/M16-kernel-device-backend/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：平台抽象、后端选择和 kernel dispatch 的静态路径已确认；具体设备上的编译、数值和性能未验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M16-kernel-device-backend/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 关联：[M05 模型执行](../M05-model-execution/README.md)、[M07 分布式并行](../M07-分布式并行.md)、[M09 Attention/CUDA Graph](../M09-attention-cuda-graph/README.md)。

## 1. 边界与分层

M16 是模型执行所依赖的设备能力层：根据运行环境解析 platform，提供 device/context、通信和 fused operator 能力，再把 Python 层调用落到 Triton、CUDA/C++ extension、AOT kernel 或其他后端。它不决定请求 admission、batch 形成或 sampling 语义；这些分别属于 M04、M05 和 M10。

```text
ServerArgs / environment
  -> platform resolver
  -> SRTPlatform implementation
  -> device capability and process-group setup
  -> kernel/operator registry
  -> fused op dispatch (AOT/JIT/fallback)
  -> model layer / attention / sampler
```

## 2. 平台抽象

`python/sglang/srt/platforms/interface.py` 的 `SRTPlatform` 约定设备名称、device id、可用能力、通信 backend、worker 初始化和相关 runtime hook。CUDA、ROCm、CPU、NPU、XPU、MUSA、MLX 等平台在 `srt/platforms/` 提供实现或适配；平台模块不是简单的设备字符串别名，部分能力（dtype、graph、IPC、通信和 kernel）必须分别判断。

平台解析在启动阶段完成，结果被 ModelRunner、分布式初始化、KV pool、attention backend 和 kernel 选择共同消费。`current_platform` 是惰性单例：显式 `SGLANG_PLATFORM` 只加载选中的 entry point；自动发现要求最多一个 OOT 插件，若没有插件再按 CPU opt-in、CUDA、ROCm、XPU 顺序回退到内置平台。平台插件必须返回可解析且继承 `SRTPlatform` 的类，否则启动失败。[`python/sglang/srt/platforms/__init__.py:49-125`][`python/sglang/srt/platforms/__init__.py:1-125`] 若平台缺少专用实现，代码可能选择通用 PyTorch/Triton 路径或直接报不支持；“存在 fallback”不等于目标平台上的完整功能已验证。

## 3. Kernel dispatch 契约

`python/sglang/kernels/` 聚合量化、attention、sampling、MoE、RoPE、cache copy、通信和融合算子。统一 kernel namespace 先以 `KernelSpec` 元数据登记实现，注册只保存 import path，不立即导入 backend；`get_kernel` 首次调用才通过 selector 进行 capability 过滤和懒加载。[`python/sglang/kernels/spec.py:205-267`][`python/sglang/kernels/registry.py:17-79`][`python/sglang/kernels/selector.py:38-104`] 调用方通常只依赖稳定的高层 wrapper；wrapper 再根据 device、dtype、shape、layout、环境开关和编译产物选择 AOT extension、Triton/JIT 或 torch fallback。kernel 的输入 layout 和 stream 语义是接口的一部分，不能只修改函数名或 tensor shape。

`BaseFusedOp.forward` 的显式 backend 和全局强制 backend 优先；随后先尝试 OOT platform override，再按 priority 和 capability 选择优化 backend，最后走平台方法或 `forward_native`。步骤 3--6 会缓存；实现 `backend_eligible` 的算子则保留按输入 shape/dtype 的动态选择。进入 `torch.compile` 时保存原 dispatch，切换到 compile-safe forward，离开时恢复。[`python/sglang/kernels/fused_op.py:334-379`][`python/sglang/kernels/fused_op.py:538-630`]
量化和 MoE kernel 还要遵守 M06 的 packed weight contract、M07 的 local rank/collective contract 和 M14 的 logical-to-physical expert mapping。CUDA Graph 路径必须使用 graph-safe 地址和固定形状；不满足条件时由 M09 选择 eager/非 graph 路径。

## 4. 初始化、失败和清理

启动时平台 resolver 解析当前 device 与编译能力，随后可加载 native extension、预热 JIT kernel、建立通信资源并设置 stream。JIT 编译失败、extension ABI 不匹配、dtype/layout 不支持或 device capability 不满足时，应记录实际原因并回退到支持的实现，或在没有正确 fallback 时让启动失败；不能把编译配置推断为已成功生成 kernel。

进程退出时需释放 extension/communication/runtime 资源，并与 Engine、distributed group、CUDA IPC 和 CUDA Graph 生命周期一致。跨进程 tensor、stream 和 event 的 ownership 不由 kernel wrapper 自动解决。

## 5. 小例子和断点

例：decode attention 先由 backend 生成固定 layout 的 metadata，wrapper 检查当前 GPU、dtype 和 page size；满足条件时调用专用 paged-attention kernel，否则调用 eager fallback。输出必须与同一 `ForwardBatch` 的 logits 对齐，不能因为 fallback 改变 token/KV 位置语义。

断点：`SRTPlatform` 的解析/能力方法、平台注册表、attention/kernel wrapper、native extension 加载点、Triton compile/cache 路径以及 fallback 分支。

## 6. 测试与未验证

定位入口包括 `test/registered` 下 kernel、attention、quantization、MoE、sampling 和 platform 测试，以及 `benchmark/kernels/`、attention benchmark 和各后端专用测试。本文未运行 CUDA/ROCm/NPU/MUSA/XPU kernel 编译、设备数值校验、CUDA Graph capture 或跨平台性能测试；没有硬件时只能确认静态 dispatch 与接口。

## 7. 修改影响

修改 platform capability 会影响 M07 process-group 初始化、M08 KV allocation、M09 graph eligibility、M14 quant/MoE 和 M18 suite filtering；修改 kernel layout 会影响 M05 `ForwardBatch`、attention metadata、通信和 checkpoint/packed-weight contract。新增后端应同时补齐 resolver、能力声明、fallback、测试注册和 cleanup，而不是只添加一个 import。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M16-kernel-device-backend/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M16-kernel-device-backend/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
