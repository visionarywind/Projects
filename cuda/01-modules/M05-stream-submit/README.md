# M05 Stream/Submit：流、通道、QMD 与完成信号

- **职责**：管理 stream pool、public handle、QMD、channel、pushbuffer、marker/semaphore 和异步 detach/reclaim。
- **证据状态**：stream 创建、channel、QMD、pushbuffer/GPFIFO、tracking marker 和 detach→free 的 host 侧生命周期已静态确认；DMAL/RM/firmware 设备消费未闭合。
- **核心文件**：`cuistream.c`、`stream_manager.c`、`channel.c`、`channel_manager.c`、`marker.c`、`semaphore.c`、`task.c`、`gpfifo.c`、`pushbuffer.c`、`qmd.c`。

## 两阶段生命周期

```text
FREE pool → ACTIVE stream → DETACHED (GPU may still use) → FREE pool
```

public handle 在 detach 时立即失效，但 CUI stream 对象要等 marker 报告 GPU 完成后才回收（静态确认：[src/cui/cuistream.c:1897-1954,2004-2088]）。

## 创建资源

`cuiStreamCreate_UnderLock` 可分配 public handle、从 pool 取 stream、分配 QMD、UVM prefetch semaphore、CPU semaphore、marker mutex、UVM registration，并更新 priority、stream ID 和 tools 通知（静态确认：[src/cui/cuistream.c:1741-1877]）。

## 提交边界

channel 由 channel manager 创建；`channelInit` 选择 DMAL、调用 backend `Init`，分配 tracking/cross-engine semaphore 并建立 push mutex（静态确认：[src/cui/channel.c:43-173]）。Kernel/ memcpy 等上层代码在 channel lock 下 begin push，最终由 channel/pushbuffer/gpfifo 和 backend 提交；具体硬件消费不能从当前读到的 CUI 代码单独证明。
