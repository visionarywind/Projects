# M01 GE 调用链与行级分析

- 文档目的：追踪 GE 初始化、Session 和 RT2 模型执行
- 适用范围：关键路径
- 对应源码版本：`47020afc8`
- 证据状态：已确认关键代码；跨仓 Runtime 调用部分推断
- 最后更新：2026-09-10
- 前置阅读：design.md
- 后续阅读：`90-cross-module/cross-module-call-chains.md`

## 初始化/Session 链

```text
GEInitialize(options)
  -> GEInitializeV2(options)
  -> (heterogeneous) DFlowInitializeInner(options)
  -> MakeShared<SessionManager>()
  -> GeSession::Impl
      -> MakeShared<InnerSession>
      -> InnerSession::Initialize
      -> GeSessionRegistry::Register
```

证据：`GEInitialize` `[ge/api/session/client/ge_api.cc:199-239]`；Session 构造 `[ge/api/session/session/ge_session_impl.cc:34-57]`。

## RT2 Load/Execute/Unload 链

```text
ModelV2Executor::Load
  -> init executor Load
  -> InitRtVarManager / ArrangeModelLoadArg
  -> SpecifyInputs / SpecifyArgsInputs
  -> init executor Execute
  -> init executor UnLoad
  -> main graph Load
  -> state = Loaded
ModelV2Executor::Execute
  -> state/input/output checks
  -> SpecifyInputs/Outputs
  -> OccupyStreamResource
  -> ExecuteGraph
  -> unbind stream resource guard
ModelV2Executor::UnLoad
  -> destroy default stream
  -> main graph UnLoad
  -> de-init graph Load/Execute/UnLoad
  -> state = Init
```

证据：`[ge/runtime/v2/core/model_v2_executor.cc:201-257,260-305]`。

## 行级逻辑要点

- Load 阶段以状态机防止重复加载；初始化图承载运行时变量、权重和模型描述输入 `[201-230]`。
- Execute 阶段先做数量和空指针校验，再将用户 Tensor 写入执行数据；`CheckIoReuseAddrs` 防止不允许的输入输出地址复用 `[260-278]`。
- 资源限制通过 ACL API 绑定当前执行线程，guard 保证返回路径解除绑定 `[80-100,284-297]`。
- ExecuteSync 创建默认 Stream，调用 Execute 后超时同步 `[307-318]`。

## 复杂度与风险

输入/输出校验为 O(n)，指定参数为 O(n)；实际模型执行复杂度由 ExecuteGraph/设备任务决定。关键风险是状态调用顺序、异步 Tensor/回调生命周期、跨仓 ABI 和设备错误码转换。

## 相关文档

- [testing.md](testing.md) · [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

见上述调用链。

## 未解决问题

V1 传统执行器完整调用链及 Runtime 具体符号尚未追踪。

## 下一步阅读建议

用 GDB/日志从 `GEInitialize`、`ModelV2Executor::Load`、`Execute` 设断点。
