# M01 GE：模块入口

- 文档目的：帮助开发者理解 GE 图编译器与执行器
- 适用范围：`ge/api`、`base`、`graph_metadef`、`compiler`、`parser`、`runtime`
- 对应源码版本：`8ee1b040a`
- 证据状态：已确认主要结构；完整子模块覆盖未完成
- 最后更新：2026-09-10
- 前置阅读：`00-overview/architecture.md`
- 后续阅读：design.md → source-map.md → call-chains.md

## 结论摘要

GE 将前端图或模型文件统一转换为 AscendIR，再经过图优化、算子编译、流/内存规划，生成 Model/OM 并通过 V1 或 V2 执行器运行。它的稳定边界是 GE API、Session、Graph/MetaDef 与 Executor 接口；具体算子定义主要由独立算子仓维护。

## 职责与非职责

负责图构建/表示、模型解析、图编译、优化、模型序列化/加载和图级执行控制；不负责底层设备驱动，也不应把每个算子 Kernel 的实现当作 GE 核心职责。

## 关键入口

- `ge::GEInitialize`：初始化 GE 和 SessionManager `[ge/api/session/client/ge_api.cc:199-239]`。
- `GeSession::Impl`：创建/注册 `InnerSession`，转发 AddGraph、CompileGraph、LoadGraph、RunGraph `[ge/api/session/session/ge_session_impl.cc:34-135]`。
- `ModelV2Executor::Load/Execute/UnLoad`：RT2 模型状态与执行 `[ge/runtime/v2/core/model_v2_executor.cc:201-305]`。
- `ge/api/atc/main.cc`：离线 ATC 命令入口（源码文件存在，具体流程待补）。

## 推荐阅读

1. `docs/zh/design/architecture.md`
2. `api/session/client/ge_api.cc`
3. `api/session/session/ge_session_impl.cc`
4. `graph_metadef/graph` 和 `base/graph`
5. `compiler/graph`、`compiler/graphcompiler`
6. `runtime/v1` 与 `runtime/v2`

## 相关文档

- [design.md](design.md) · [source-map.md](source-map.md) · [interfaces.md](interfaces.md) · [call-chains.md](call-chains.md)
- [testing.md](testing.md) · [development-guide.md](development-guide.md) · [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

`[ge/README.md:10-13]`、`[ge/AGENTS.md:8-19]`、`[ge/docs/zh/design/architecture.md:35-93]`。

## 未解决问题

V1/V2 自动选择条件、Compiler 到 Executor 的所有动态插件加载路径需要按具体模型场景继续追踪。

## 下一步阅读建议

先读 design.md 的生命周期，再根据问题选择 Compiler 或 Executor。
