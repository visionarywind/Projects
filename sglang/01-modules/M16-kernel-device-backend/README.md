# M16 Kernel 与设备后端

- 源码证据：`/home/mtuser/workspace/repos/Github/sglang` 当前 checkout。
- 证据状态：平台抽象、后端选择和 kernel dispatch 的静态路径已确认；具体设备上的编译、数值和性能未验证。
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

平台解析在启动阶段完成，结果被 ModelRunner、分布式初始化、KV pool、attention backend 和 kernel 选择共同消费。`current_platform` 是惰性单例：显式 `SGLANG_PLATFORM` 只加载选中的 entry point；自动发现要求最多一个 OOT 插件，若没有插件再按 CPU opt-in、CUDA、ROCm、XPU 顺序回退到内置平台。平台插件必须返回可解析且继承 `SRTPlatform` 的类，否则启动失败。[`python/sglang/srt/platforms/__init__.py:49-160`][`python/sglang/srt/platforms/__init__.py:166-173`] 若平台缺少专用实现，代码可能选择通用 PyTorch/Triton 路径或直接报不支持；“存在 fallback”不等于目标平台上的完整功能已验证。

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
