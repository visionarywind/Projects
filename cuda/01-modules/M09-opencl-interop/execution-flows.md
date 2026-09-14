# M09 执行流程

```text
OpenCL call → ICD initialize/vendor enumerate → platform/device dispatch
→ CLI globals/context/object → CUI mem/stream/event → HAL/RM
```

初始化失败清除 platform/dispatch table 并允许后续重试；loader vendor add 临时释放查询数组和初始库句柄，保留 vendor 节点需要的 handle。
