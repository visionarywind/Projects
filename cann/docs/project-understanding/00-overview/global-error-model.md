# 总览：全局错误模型

- 文档目的：解释 00-overview/global-error-model.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：错误分层模式已确认；完整错误码表和所有转换规则待补
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/global-error-model.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 错误流

```text
设备/驱动错误
  -> Runtime rtError_t / 扩展错误码
  -> ACL aclError 或 GE graphStatus/Status
  -> 日志、回调或调用方返回值
```

Runtime C API 使用统一宏验证句柄和返回错误 `[runtime/src/runtime/api/api_c.cc:118-153]`。ACL 设备包装在 Runtime 失败时使用 `ACL_GET_ERRCODE_RTS` 进行映射 `[acl/runtime/device.cpp:47-59]`。GE 异步回调在失败时记录错误并把 GERT Tensor 转为 GE Tensor `[ge/api/session/client/ge_api.cc:87-101]`。

## 内存 cache 错误边界

Driver ordinary cache 的 miss 不一定是设备物理内存耗尽：V3 首次 `svm_ga_alloc` 的 `DRV_ERROR_OUT_OF_MEMORY` 可先表示已有 cache range 没有适配 area，随后尝试扩展；V2 则可能寻找其他 heap 或建立 mapping。扩展、映射、free 和 shrink 失败都必须保留设备、VA、size、flag、heap/range 和原始 Driver 错误上下文。[../01-modules/M04-driver/driver-memory-pool-analysis.md](../01-modules/M04-driver/driver-memory-pool-analysis.md)


| 类别 | 示例 | 调用方动作 |
|---|---|---|
| 参数错误 | 空指针、数量不匹配、非法设备 ID | 修正参数，不重试同一请求 |
| 状态错误 | 未初始化、Executor 非 Loaded、活动 session 仍存在 | 按生命周期补齐或释放 |
| 资源错误 | 内存、Stream、Queue、HDC 资源不足 | 记录上下文，按策略清理/重试 |
| 设备/驱动错误 | 设备不可用、IOCTL/HDC 失败 | 检查驱动、固件、设备日志 |
| 编译/模型错误 | 图不合法、shape/算子不支持 | 回到模型和编译配置排查 |
| 异步错误 | callback 返回失败、任务执行失败 | 等待完成通知并保留原始错误上下文 |

## 诊断原则

- 优先保存最底层错误码、设备 ID、Context/Stream、模型/Graph 标识和调用阶段。
- 不以 `ACL_SUCCESS` 或 `GRAPH_SUCCESS` 推断设备任务已完成；异步路径必须同步或等待事件。
- 日志和 DFX 能力属于旁路诊断设施，不改变资源所有权。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

需要从公共头文件和错误管理实现生成 Runtime→ACL、Runtime→GE、Driver→HAL 的完整映射表；当前文档不假设未核实的数值错误码。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
