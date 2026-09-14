# D01 调试 walkthrough

1. 先看 dispatcher 的 GPU ordinal/过滤输出（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。
2. 若初始化失败，沿 `cuapiInit`→`cuiInit` 检查 globals、TLS、UVM/UVA 和 primary memmgr。
3. 若 kernel 失败，检查 function/stream 是否同一 context、参数 metadata、CNP capability 和 launch syscall setup。
4. 若提交失败，按 `streamBeginPush`、HAL finalize/control、`streamEndPush` 和 marker 状态定位。
5. 工具启用时额外检查 callback begin/end、debugger blocking、profiler wait 和 memcheck allocation。

这是静态调试路线；没有实际失败日志。
