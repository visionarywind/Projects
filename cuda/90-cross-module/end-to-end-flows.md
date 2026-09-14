# 端到端流程

## 初始化到完成

```text
cuInit → globals/TLS/managers → context
 → memobj allocation/registration
 → stream/QMD/channel
 → module/function + launchData
 → syscall/HAL setup
 → pushbuffer/channel submit
 → marker/synchronize
 → stream reclaim/context destroy
```

普通 launch 的 push 细节见 `[src/cui/cuilaunch.c:582-817]`；GPFIFO entry 在 tracking semaphore 完成后才推进 GPU get 和 pushbuffer 回收；capture launch 在 API 层形成 graph node 而非立即提交（`[src/api/apilaunch.c:252-286]`）。

## 关键分叉

- API 参数错误在 wrapper 终止。
- capture 与普通 stream 分叉。
- 工具可 skip launch，debugger/profiler 可改变等待策略。
- 异步 stream destroy 分叉为 detached 或 marker 完成后的 free。

所有硬件效果和测试结果未验证。
