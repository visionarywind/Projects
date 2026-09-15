# M01 GE 源码地图

- 文档目的：把 GE 目录映射到职责、入口和测试
- 适用范围：主要源码路径
- 对应源码版本：`8ee1b040a`
- 证据状态：已确认目录和关键文件；不是全量符号清单
- 最后更新：2026-09-10
- 前置阅读：README.md
- 后续阅读：line-level-analysis.md
## 结论摘要

本页聚焦 01-modules/M01-ge/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 路径 | 类型 | 作用 | 重要性 |
|---|---|---|---|
| `api/session/client/ge_api.cc` | API 实现 | GE 初始化、Session 管理、异步回调 | 高 |
| `api/session/session/ge_session_impl.cc` | Session 实现 | `InnerSession` 生命周期和 API 转发 | 高 |
| `api/atc/main.cc`、`main_impl.cc` | CLI | ATC 离线编译入口 | 高 |
| `graph_metadef/graph` | 图结构 | Graph/Node/Anchor 等元数据实现 | 高 |
| `base/graph`、`base/common` | 基础层 | 图与公共工具 | 高 |
| `compiler/graph` | 编译优化 | Pass、内存/流、图变换 | 高 |
| `compiler/engines` | 引擎 | 算子引擎和分区 | 高 |
| `compiler/graphcompiler` | 编译器 | 编译目标和引擎构建组织 | 高 |
| `runtime/v1` | 执行器 | 传统/静态模型执行 | 高 |
| `runtime/v2/core` | RT2 | ExecuteGraph、V2 模型执行 | 高 |
| `parser/parser` | 解析器 | ONNX/PB 等模型解析 | 中 |
| `tests/ge`、`tests/graph_metadef`、`tests/parser` | 测试 | 单元/系统验证 | 高 |

## 关键行级入口

- `GEInitialize`: `[ge/api/session/client/ge_api.cc:199-239]`
- Session 构造/析构: `[ge/api/session/session/ge_session_impl.cc:34-71]`
- V2 Load/Unload: `[ge/runtime/v2/core/model_v2_executor.cc:201-257]`
- V2 Execute: `[ge/runtime/v2/core/model_v2_executor.cc:260-305]`

## 相关文档

- [line-level-analysis.md](line-level-analysis.md)

## 源码证据摘要

目录职责来自 `[ge/AGENTS.md:8-19]`，构建子目录来自 `ge/CMakeLists.txt` 和各层 CMakeLists。

## 未解决问题

全量库符号、生成的 protobuf 文件、闭源算子实现不在本地图中展开。

## 下一步阅读建议

以 `GEInitialize` 和 `ModelV2Executor::Execute` 为两个切入点。
