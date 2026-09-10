# 跨模块：共享数据与类型

- 证据状态：类别和关键边界已确认；完整 typedef/结构布局和设备侧 ABI 仍待补

| 数据/类型 | 生产层 | 消费层 | 兼容风险 |
|---|---|---|---|
| Graph/Node/Tensor/Attribute/Anchor | GE MetaDef/AscendIR | GE Compiler/Executor、框架适配 | 图模型和头文件版本 |
| OM/Model/ExecuteGraph | GE Compiler | GE Executor/Runtime | 模型格式、算子包、Runtime 版本 |
| Tensor/DataBuffer/shape | ACL/GE | Runtime/设备任务 | 地址、容量、布局、异步生命周期 |
| Device ID/SoC name | Driver/Runtime | ACL/GE 平台能力 | 产品分支和字符串长度 |
| Context/Stream/Event handle | Runtime | ACL、GE Executor、Driver 任务 | 句柄 ABI、归属和销毁顺序 |
| `rtMemPool_t`/Segment metadata | Runtime SOMA | ACL/GE stream-ordered allocation | handle 表示、地址区间、FREE/BUSY/CACHED 状态和异步完成语义 |
| `AicpuPoolCtxArgs`/Soma sub-command | Runtime | AICPU `SomaMemMng` | 字段布局、枚举值、对齐和版本必须一致 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:87-105]` |
| Memory flags/pool attributes | Runtime | HAL/SVM/Driver | HBM/DDR、huge、P2P、module、VA 范围和设备能力 |
| Error code/status | Driver | Runtime → ACL/GE | 映射可能丢失扩展上下文 |
| ioctl 参数结构 | Driver 用户态 | Driver 内核态 | 命令号、布局、位宽和版本 |

GE 图对象职责由架构文档确认 `[ge/docs/zh/design/architecture.md:76-93]`；Runtime 句柄经验证/解包宏跨 C ABI `[runtime/src/runtime/api/api_c.cc:118-153]`；SOMA 的本地 segment 和 AICPU 参数由 Runtime 源码确认；Driver ioctl ABI 见 `esched_ioctl.h`，但 SDK-driver→内核→固件的完整映射仍未确认。

## 资源状态不变量

- SOMA `allocedMap_` 只记录 BUSY 基地址；`cachedSegs_` 和 `freeSegs_` 必须与 Segment 双向链保持一致；`busySize_` 与 `reserveSize_` 分别反映 BUSY 和 BUSY+CACHED 大小 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:107-171]`。
- 普通 device pointer 不应被当成 SOMA pool 地址；非 SOMA `rtMemPoolFreeAsync` 走 HostFunc 和普通 Driver free 路径 `[runtime/src/runtime/api/impl/api_impl_soma.cc:111-195]`。
- Runtime 本地状态、AICPU 操作和 Driver VMM segment 的“提交/完成”不是同一事件；只能在明确 callback/event/synchronize 边界后宣称设备侧完成。

## 变更规则

- 公共结构体字段只能兼容扩展，不能随意重排或改变类型。
- shape/地址/长度变化必须同步参数校验、模型元数据、Runtime 和设备命令。
- 错误码新增需同步底层定义、转换宏、公开头文件和测试。
- handle 内部布局不应暴露；若改变魔数/代际规则，需覆盖旧句柄错误路径。
- 修改内存 flag、pool attribute、AICPU sub-command 或 ioctl 结构时，必须同时核对 Runtime、HAL、SVM/SDK-driver、AICPU 和跨仓测试。
