# M05 实现：stream pool 到 channel

- 文档目的：解释 01-modules/M05-stream-submit/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-stream-submit/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Stream flags 与策略

API wrapper 先拒绝未知 flags 和互斥 scheduling flags，再把 public flags 转成 CUI flags，进入 common create（静态确认：[src/api/apistream.c:43-64,113-136]）。CUI 根据 null/barrier/non-blocking/defer-QMD/no-bank-pipeline 等 flags 设置内部行为（静态确认：[src/cui/cuistream.c:1750-1755,1811-1837]）。priority 在设备支持时 clamp 到 context 允许范围，否则回退为 0（静态确认：[src/cui/cuistream.c:1840-1856]）。

## 锁和创建

API 在 context lock 下调用 `cuiStreamCreate`；CUI 再取得 stream pool mutex 调用 `_UnderLock`。QMD semaphore pool 的 portable 登记刻意在 API 释放 context lock 后进行，以避免锁序问题；该步骤失败进入 `cuiStreamDetach`（静态确认：[src/api/apistream.c:83-109]）。

## Channel

`channelInit` 复制创建参数、建立 progress condition variable、加入 flush unit，再调用 `channel->dmal.channelInitDmal` 和 `channel->dmal.Init`。backend 成功后分配 tracking semaphore、cross-engine semaphore，把 channel 插入 manager，并按跨 channel flush 情况决定使用自己的 push mutex 还是 channel[0] 的 mutex（静态确认：[src/cui/channel.c:43-157]）。失败路径调用 DMAL Deinit、释放 semaphores 和 condition variable（静态确认：[src/cui/channel.c:159-173]）。

## QMD 与循环复用

每个 stream 可持有 `NUM_QMD_PER_STREAM` 个 QMD；launch 每次取 `nextQmdIndex` 并取模递增，避免同步使用同一个 QMD（静态确认：[src/cui/cuistream.c:1956-1986]）。具体 QMD 内容由 M06/M03 的 launch/HAL 逻辑填充。

## GPFIFO 与 pushbuffer 提交

GPFIFO 为每个 channel 保存 `cpuPut/gpuPut/gpuGet` 环形索引、CPU entry 数组和 GPFIFO memobj；同时创建 system/optional video pushbuffer。GPFIFO 和 pushbuffer 都通过 `memobjAlloc` 创建，分别使用 40-bit device VA 与 driver-owned 类型；任一分配失败都按 pushbuffer、GPFIFO memobj、entry 数组的反向顺序清理（静态确认：[src/cui/gpfifo.c:80-207]）。pushbuffer 的 `put/get` 是环形字节游标，Kepler/Maxwell 还按硬件 latency-buffer workaround 对 segment 做 128-byte 对齐（静态确认：[src/cui/pushbuffer.c:22-99,181-235]）。

一次 push 先检查 pushbuffer 空间和 GPFIFO padding entry；`channelGetPutPointerGPFIFO` 为当前 CPU entry 预留 pushbuffer 区间，method 写入后 `channelSetPutPointerGPFIFO` 更新 entry length。flush 时 `gpfifoFlushGpfifoEntry` 调用架构 `setGpfifoEntry`，把 pushbuffer device address、offset、length 和 wait bit 编码到 GPFIFO entry，并记录 tracking semaphore 的结束值（静态确认：[src/cui/gpfifo.c:313-416,576-714]）。

flush unit 遍历同一 flush unit 的 channel，封存当前 CPU entry、推进 `cpuPut`，将已封存 entry 标记为 flushed，更新 `gpuPut`，最后把 flush item 交给 `channelFlushUnit->channelHead->dmal.GpfifoAdvanceGpuPut`；这个 DMAL 调用是 host GPFIFO 到 RM/设备消费的边界，源码当前未展开其具体 doorbell/firmware 行为（静态确认：[src/cui/gpfifo.c:716-796]）。

## 完成判定

`gpfifoAdvanceGpuGet` 先从 channel tracking semaphore 读取最近完成值，再按环形顺序推进 `gpuGet`；只有 entry 的 `trackSemValEnd` 已完成时才同步推进关联 pushbuffer 的 get 游标（静态确认：[src/cui/gpfifo.c:437-490]）。marker 将 channel tracking、pending QMD、CPU/UVM semaphore 等多种 entry 的状态合并为最小状态；只有所有 entry 达到完成条件才报告 `CU_CTX_MARKER_COMPLETED_BY_GPU`，并在完成时检查 channel/ECC 错误（静态确认：[src/cui/marker.c:420-541]）。

## QMD semaphore

QMD pool 在 device memory 中分配固定大小的 QMD 数组，并为每个 QMD 分配 semaphore；QMD 的 semaphore pool 在 graph/stream 使用前登记为 portable，释放时先移除 active QMD 再回 free stack（静态确认：[src/cui/qmd.c:43-159,161-212,240-260]）。具体 QMD 字段编码仍由架构 HAL 的 launch finalize/patch 负责。

## Detach/reclaim

detach 注销 UVM、释放 public handle、释放 constant-bank pipeline、abandon UVM/CPU semaphore、释放 QMD，并置为 DETACHED；reclaim 查询 marker，只有 `CU_CTX_MARKER_COMPLETED_BY_GPU` 才移动到 free list，并清空 channel/ID/marker 状态（静态确认：[src/cui/cuistream.c:1926-1954,2004-2088]）。

## Marker 聚合与等待

marker entry 可以代表 channel tracking semaphore、pending QMD、CPU/UVM semaphore、cross-context semaphore 等不同完成源。状态检查按 entry 类型判定；`ctxMarkerSetMax` 在容量不足时先删除已完成项、扩容，再合并冗余 entry。`ctxMarkerWait` 先进行不刷 channel 的快速检查；blocking behavior 下可推送 awaken，再 flush 必需的 channel/QMD，尝试 blocking wait，失败为 not-ready 时退化为 spin/yield，并在需要时进入 syscall/RM trap service。等待前后都有 tools notification，因此同步 API 不是单纯轮询一个 GPU flag（静态确认：[src/cui/marker.c:约 300-541,约 880-1060]）。

cross-context release/acquire 通过 marker entry 保存 remote channel/semaphore 关系；它把 context 间依赖纳入统一完成模型，但 IPC event 明确由另一套路径处理。当前源码能确认 host 侧聚合、flush 与 reclaim 顺序，不能确认 RM/firmware 写入完成值的最终时序（静态确认：[src/cui/marker.c:约 600-850]；外部边界为未知）。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
