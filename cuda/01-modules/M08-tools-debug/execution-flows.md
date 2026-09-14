# M08 执行流程

```text
工具注册/enable → runtime event callback
→ 可选 debugger attach / profiler setup / memcheck patch
→ launch/memory/context event
→ callback/report/extra synchronization
→ detach/finalize cleanup
```

launch callback 的 begin/end 必须成对；memcheck 的 host table 下载和 device error entry 需在 context/stream 生命周期内有效。
