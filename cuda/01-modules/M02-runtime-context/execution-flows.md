# M02 执行流程

```text
cuInit → cuiInit → TLS → globals → memglobals → platform → UVM/UVA → heap → primary memmgr
```

失败沿相反顺序销毁。context 路径为：参数规范化→`cuiCtxAllocate`→`cuiCtxInitialize`→TLS push；销毁为 finalize→deinitialize→必要时 release primary TSG。异步 channel 错误进入 sticky error，在后续检查点返回。

证据：[src/cui/cuiinit.c:3060-3208]、[src/cui/cuictx.c:240-405]。
