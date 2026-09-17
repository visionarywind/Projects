# 总览：运行时模型

- 文档目的：解释 00-overview/runtime-model.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：主要对象和调用边界已确认；跨仓深层调用部分推断
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/runtime-model.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 对象层次

```text
进程
 ├─ GE 全局状态 / SessionManager
 │   └─ Session → Graph → Executor → Model
 ├─ ACL 全局初始化引用计数
 └─ Runtime Api::Instance()
     ├─ Device
     ├─ Context
     ├─ Stream / Event
     ├─ Memory / DataBuffer
     └─ Model / Kernel / Task
         └─ Driver client / queue / HDC session
```

GE Session 的 `InnerSession` 在构造时初始化并注册，在析构时注销和 Finalize `[ge/api/session/session/ge_session_impl.cc:34-71]`。ACL 初始化状态由互斥锁、引用计数、SoC 缓存和配置字符串组成 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:25-32]`。Runtime 的 C API 以 `Api::Instance()` 为内部对象入口 `[runtime/src/runtime/api/api_c_device.cc:49-119]`。

## 执行模型

- **同步调用**：调用方提交任务，显式同步设备或 Stream，随后读取结果。
- **异步调用**：任务进入 Stream，完成后触发 callback/event；调用方必须管理跨异步边界的对象生命周期。
- **GE V2**：Load 时执行初始化图并加载主图；Execute 时注入 IO 和执行资源；Unload 时卸载主图并执行反初始化图 `[ge/runtime/v2/core/model_v2_executor.cc:201-318]`。
- **设备下沉**：GE 文档描述 Sink/SuperKernel 等机制，用于减少主机逐任务下发 `[ge/docs/zh/design/architecture.md:129-189]`；具体启用条件尚未完整确认。

## 资源所有权

| 资源 | 创建者 | 使用者 | 释放责任 | 证据状态 |
|---|---|---|---|---|
| Session/InnerSession | GE Session | GE API/Graph | Session 析构 | 已确认 |
| Stream/Event/Notify | Runtime 或调用方 allocator | Executor/任务 | 对应 allocator/Unload 路径 | 已确认主要路径 |
| Device memory | Runtime API | Tensor/Model/Kernel | Runtime/调用方按 API 契约 | 推断，需逐 API 核实 |
| HDC client/session | Driver client | queue/通信模块 | client 销毁前关闭 session | 已确认 `[driver/src/ascend_hal/hdc/common/hdc_client.c:114-179]` |
| Queue context | Driver open | ioctl/read/write | file release | 已确认 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-95]` |

## 并发模型

全局初始化/Finalize 具有锁保护；细粒度的 Context、Stream 和 Driver queue 并发规则不能仅从门面代码推断，应以具体接口实现和测试为准。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
