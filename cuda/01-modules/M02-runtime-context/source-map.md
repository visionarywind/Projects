# M02 源码地图、接口和数据

- 文档目的：解释 01-modules/M02-runtime-context/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-runtime-context/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 文件 | 责任 | 证据 |
|---|---|---|
| `src/cui/cuiinit.c` | globals、初始化检查、回滚 | `cuiInitCheckEx`、`cuiInitInternal` |
| `src/cui/cuictx.c` | context 参数、分配、初始化、销毁 | `cuiCtxCreateParamsInit`、`cuiCtxCreate` |
| `src/cui/cuitls.c` | TLS/current context | 由 API 和 CUI 多处调用，深层字段仍待整理 |
| `src/cui/cuierror.c` | context/设备错误状态 | sticky error 的被调用方；完整产生点未穷尽 |
| `src/cui/cuimutex.c` | 内部锁封装 | 锁顺序需要跨文件核对 |
| `src/cuda_types.h` | 对象前向声明 | `CUItlsThreadData`、`CUctx` 等 `[src/cuda_types.h:36-43]` |

## 核心字段

- `globals.initialized`、`globals.initMutex`：进程级状态和初始化串行化。
- `CUctx.persistentState.state/device/isPrimary/threadCtxStackRefCount`：context 生命周期、归属设备和 TLS 引用（静态确认：[src/cui/cuictx.c:387-405]）。
- `CUctx.apiVersion`：区分 legacy v3010 与 v3020 wrapper 语义（静态确认：[src/cui/cuiinit.c:3010-3025]）。

## 接口契约

调用者需提供已初始化 TLS；要求 context 的 API 必须接受 current context 或显式处理无 context 结果。context 创建成功后由 context manager/线程栈持有，销毁前必须完成同步和对象清理；具体 `cuiCtxDeinitialize` 释放清单需继续读取。

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
