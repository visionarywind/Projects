# 总览：运行时模型

- 证据状态：主要对象和调用边界已确认；跨仓深层调用部分推断

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
