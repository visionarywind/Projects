# M01 GE 接口

- 文档目的：说明 GE API/Session/Executor 的契约
- 适用范围：公开和主要内部接口
- 对应源码版本：`47020afc8`
- 证据状态：主要状态与错误语义已确认；完整 API 列表待补
- 最后更新：2026-09-10
- 前置阅读：README.md
- 后续阅读：call-chains.md

## 主要接口

| 接口 | 前置条件 | 行为/后置条件 | 错误 |
|---|---|---|---|
| `ge::GEInitialize(options)` | options key 不为空 | 初始化 GE、必要时 DFlow、创建 SessionManager | 返回 `Status`；非法 key 失败 `[ge/api/session/client/ge_api.cc:199-239]` |
| `GeSession::Impl::AddGraph` | `inner_session_ != nullptr` | 将图加入 Session | 空 Session 返回 `FAILED` `[ge/api/session/session/ge_session_impl.cc:85-89]` |
| `CompileGraph` | 图已添加 | 编译图并保存编译状态 | 依赖 Compiler/算子包 |
| `LoadGraph` | 图已编译 | 将模型资源加载到执行器/设备 | 依赖 Runtime/设备 |
| `RunGraph/RunGraphAsync` | 图已加载 | 同步或异步执行并返回 Tensor | 异步错误由回调和日志报告 |
| `ModelV2Executor::Execute` | state=Loaded；输入输出数量匹配 | 指定参数并执行 ExecuteGraph | 参数/状态/Runtime 错误 `[ge/runtime/v2/core/model_v2_executor.cc:260-305]` |

## 调用者责任

调用者负责先初始化、保持 Session/Stream/输入输出生命周期、按编译模型提供正确 shape 和数量，并在异步执行完成前保持回调相关资源有效。线程安全和跨线程 Session 使用边界需以具体接口文档为准。

## 兼容性

GE API、MetaDef、Runtime 头文件、模型格式和插件必须与目标 CANN 版本匹配；不要仅替换单个仓库 master。

## 相关文档

- [design.md](design.md) · [call-chains.md](call-chains.md)

## 源码证据摘要

见表格。

## 未解决问题

需要逐个公共头文件补充参数、ABI、线程安全和版本废弃策略。

## 下一步阅读建议

开发 API 前先看 ACL/Runtime 对同一资源的所有权约定。
