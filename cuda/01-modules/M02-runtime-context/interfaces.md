# M02 接口契约

- `cuiInitCheckEx`：输入 TLS 和 flags，输出 current context；不创建 context。
- `cuiInitCheckCtx`：检查 state/API version/sticky error；调用者仍负责 context lock。
- `cuiCtxCreate`：成功转移 context 所有权到 `ret`，失败保证 `*ret==NULL`。
- `cuiCtxDestroy`：假定 TLS pop/调用前置条件已满足，负责 finalize/deinitialize 和 primary TSG release。

锁与 TLS 不等价：TLS 选中 context，context mutex 保护其内部资源。
