# M08 接口契约

- `toolsCallbackEnabled`：只读 enable flag，关闭时不 dispatch。
- `toolsIssueCallback`：向注册工具广播参数结构。
- `cudbgApiCheck`：按 flags 依赖闭包检查 module/TLS/device/status/suspended/SM/warp。
- `setGlobalProfMode`：在 profiler mutex 下改变 mode/enable。
- memcheck `CCallocateDevice`/`MemcpyInlineHtoD`：错误需映射为 host/device memory failure。

具体 callback 是否影响主状态需按各 callback ABI 继续核对。
