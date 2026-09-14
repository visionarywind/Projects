# M07 执行流程

```text
compiler/generated kernel metadata
 → module/function syscall imports
 → cuiSyscallOnKernelLaunch
 → syscall function memobj tracking
 → architecture SASS/QMD/ABI generation
 → launch control/push
 → device service/child launch
```

CNP child launch 的完成与父 stream、queue 和 context destroy 相关；不能仅在 host API 返回时释放 syscall 资源。
