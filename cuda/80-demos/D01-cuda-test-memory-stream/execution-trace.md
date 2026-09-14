# D01 执行轨迹

```text
basic_sanity
 → Runtime/device setup
 → context/module/function lookup
 → gpuIncrement<<<1,1>>>
 → cuLaunchKernel
 → parameter/setup/syscall/CNP
 → stream push/channel
 → synchronize/result check
```

驱动侧证据：[src/api/apilaunch.c:212-301]、[src/cui/cuilaunch.c:229-338,582-817]。执行结果未验证。
