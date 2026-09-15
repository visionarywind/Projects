# 跨模块：共享数据与类型

- 文档目的：解释 90-cross-module/shared-data-and-types.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：类别和关键边界已确认；完整 typedef/结构布局和设备侧 ABI 仍待补
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/shared-data-and-types.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## Driver ordinary cache 类型边界

普通内存 flags 从 Runtime 传入 HAL/SVM 后，V2 由 `devmm_node_data` 的 `va/size/total/flag/advise` 和多棵树承接，V3 由 cache flag、`ga_range`/`ga_area` 和地址/大小索引承接。修改 page type、P2P、Huge Page、align 或 module ID 时，必须同步核对 Runtime policy、HAL flag 转换、V2/V3 cache 分类及 shrink 语义；不能将这些字段映射成 SOMA Segment 属性。


- SOMA `allocedMap_` 只记录 BUSY 基地址；`cachedSegs_` 和 `freeSegs_` 必须与 Segment 双向链保持一致；`busySize_` 与 `reserveSize_` 分别反映 BUSY 和 BUSY+CACHED 大小 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:107-171]`。
- 普通 device pointer 不应被当成 SOMA pool 地址；非 SOMA `rtMemPoolFreeAsync` 走 HostFunc 和普通 Driver free 路径 `[runtime/src/runtime/api/impl/api_impl_soma.cc:111-195]`。
- Runtime 本地状态、AICPU 操作和 Driver VMM segment 的“提交/完成”不是同一事件；只能在明确 callback/event/synchronize 边界后宣称设备侧完成。

## 变更规则

- 公共结构体字段只能兼容扩展，不能随意重排或改变类型。
- shape/地址/长度变化必须同步参数校验、模型元数据、Runtime 和设备命令。
- 错误码新增需同步底层定义、转换宏、公开头文件和测试。
- handle 内部布局不应暴露；若改变魔数/代际规则，需覆盖旧句柄错误路径。
- 修改内存 flag、pool attribute、AICPU sub-command 或 ioctl 结构时，必须同时核对 Runtime、HAL、SVM/SDK-driver、AICPU 和跨仓测试。

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
