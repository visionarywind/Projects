# M05 调用链和同步边界

## Stream create

```text
cuStreamCreate
 → cuapiStreamCreate / VerifyFlags              [src/api/apistream.c:113-136]
 → cuapiStreamCreate_common                     [src/api/apistream.c:66-109]
 → cuiInitCheck
 → cuiStreamCreate
 → pool mutex / cuiStreamCreate_UnderLock
 → pool stream + QMD + semaphores + UVM register
 → release context lock
 → qmdRegisterSemaphorePoolIfNeeded
 → public handle
```

## Channel create

```text
context initialization
 → channel manager
 → channelInit
 → dmal.channelInitDmal
 → channel->dmal.Init
 → tracking/cross-engine semaphores
 → manager.channels[] + push mutex
```

## Push

```text
launch/memcpy
 → stream's channelOfLastWork
 → channelBeginPushInternal
 → channel lock / submission lock (MPS if needed)
 → pushbuffer methods / GPFIFO
 → channelEndPushInternal
 → tracking marker/semaphore
```

`channelBeginPushInternal` 在 MPS subcontext client 且未跳过标志时取得 submission lock，再锁 channel 并保持锁返回给调用者（静态确认：[src/cui/channel.c:247-260]）。

## GPFIFO 末端

```text
channelGetPutPointerGPFIFO
 → pushbufferStartPush
 → host method writes
 → channelSetPutPointerGPFIFO
 → gpfifoFlushGpfifoEntry
 → hal.setGpfifoEntry
 → channelFlushUnitFlushGPFIFO
 → dmal.GpfifoAdvanceGpuPut
 → tracking semaphore updates gpuGet
```

GPFIFO entry 的 device address/length/wait 被 HAL 编码；DMAL `GpfifoAdvanceGpuPut` 是当前源码可见的最后提交调用，RM/firmware/doorbell 消费未能从本树静态闭合（静态确认：[src/cui/gpfifo.c:591-714,716-796]）。

## Completion/reclaim

```text
GPU/backend progress
 → marker status
 → cuiStreamPoolMoveDetachedToFree
 → ctxMarkerGetStatus(NO_CHANNEL_FLUSH)
 → COMPLETED_BY_GPU
 → cuiStreamReclaimDetached
```

这证明 host 侧对象回收由完成信号驱动，但当前证据不能证明 firmware 对 marker 的最终写入顺序。

## Marker wait 与跨 context 依赖

```text
ctxMarkerWait
 → fast status check (NO_CHANNEL_FLUSH)
 → optional push awaken
 → flush channel/QMD entries
 → blocking wait attempt
 → spin/yield or RM interrupt service
 → completion by heterogeneous entries
```

marker 可合并 tracking、pending QMD、CPU/UVM 和 cross-context semaphore；`ctxMarkerSetMax` 会清理已完成项、扩容并合并冗余项。跨 context release/acquire 仍落入 marker entry 聚合，但 IPC event 不走 UVM DAG；RM/firmware 对完成值的写入时序是当前源码外部边界（静态确认：[src/cui/marker.c:约 300-541,600-1060]）。
