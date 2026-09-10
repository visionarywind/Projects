# M01 GE 行级分析

- 证据状态：关键入口已确认；Compiler 内部细节待补

## 初始化和 Session

- `GEInitialize` 将选项转换后调用 `GEInitializeV2`，异构场景额外初始化 DFlow，最后创建 `SessionManager` `[ge/api/session/client/ge_api.cc:199-239]`。
- `GeSession::Impl` 获取 Session ID，创建 `InnerSession`，调用 `Initialize` 并注册到 `GeSessionRegistry` `[ge/api/session/session/ge_session_impl.cc:34-57]`。
- 析构时注销 Registry、调用 `Finalize` 并清理 Session ID `[ge/api/session/session/ge_session_impl.cc:59-71]`。

## V2 Load

`Load` 先检查状态，加载 init graph，初始化 Runtime variable manager，安排常量/权重/模型描述参数，执行并卸载 init graph，随后加载主图并设置 `kLoaded` `[ge/runtime/v2/core/model_v2_executor.cc:201-230]`。这使初始化副作用和主图可执行状态有明确边界。

## V2 Execute

`Execute` 先检查 `kLoaded`、输入输出数量和空指针，再指定 IO、占用 Stream 资源、设置确定性配置并执行图；资源绑定由 RAII guard 在返回路径解除 `[ge/runtime/v2/core/model_v2_executor.cc:260-305]`。

## V2 UnLoad/同步

`UnLoad` 销毁默认 Stream、卸载主图、执行 de-init graph 并恢复 `kInit` `[ge/runtime/v2/core/model_v2_executor.cc:233-257]`。`ExecuteSync` 创建默认 Stream，调用 Execute 后按超时同步 `[307-318]`。

复杂度：输入输出检查和参数指定随 IO 数量线性；模型执行复杂度由 ExecuteGraph 和设备任务决定。
