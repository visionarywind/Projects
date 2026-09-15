# M05 数据结构、源码地图与审计

- 文档目的：解释 01-modules/M05-stream-submit/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-stream-submit/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 关键对象

| 对象 | 所有者/用途 |
|---|---|
| `CUIstream` | stream pool；保存 context、public handle、QMD、semaphore、last-work channel、marker、capture 状态 |
| `CUIstreamPool` | active/detached/free list 和 pool mutex |
| `CUnvchannel` | channel manager；保存 DMAL backend、GPFIFO、push lock、tracking/cross-engine semaphore |
| `CUqmd` | 每 stream 的 launch descriptor 槽位 |
| `CUctxMarker` | 异步完成/等待/回收判定 |
| `CUsema` | CPU、UVM、tracking 或跨 engine 同步载荷 |
| `CUgpfifo` | 环形 GPFIFO entry、CPU/GPU put/get 和 pushbuffer 关联 |
| `CUpushbuffer` | host/device method 字节环形区间及 put/get 游标 |

前向声明见：[src/cuda_types.h:79-97]。

GPFIFO entry 的 `trackSemValStart/End` 把字节 pushbuffer 生命周期连接到 tracking semaphore；只有 GPU 完成对应值后，`gpuGet` 和 pushbuffer get 才推进（静态确认：[src/cui/gpfifo.c:437-490]）。

## 状态推进

- `channelGetPutPointerGPFIFO` 从当前 CPU entry 取得 pushbuffer 区间；method 写入后 `channelSetPutPointerGPFIFO` 更新 entry length。
- `gpfifoFlushGpfifoEntry` 将 pushbuffer 地址、offset、length 和 wait bit 编码进 entry，并记录 tracking semaphore 区间。
- flush unit 推进 `cpuPut`/`gpuPut`，通过 DMAL `GpfifoAdvanceGpuPut` 将封存 entry 交给更低层；完成值满足 `trackSemValEnd` 后 `gpfifoAdvanceGpuGet` 才释放 pushbuffer 空间。
- marker 用 entry 数组聚合多种完成来源，等待路径可以触发 flush 和 blocking wait。

因此 put/get 游标不是独立的环形 buffer 指针，而是由“提交封存—tracking 完成—空间回收”三段状态机共同约束。

## 源码地图

- `cuistream.c`：stream policy、pool、QMD、detach/reclaim。
- `stream_manager.c`：context stream 集合和默认/per-thread stream。
- `channel.c`：channel 初始化、锁、push begin/end、flush/reset。
- `channel_manager.c`：按 context 管理 channel。
- `marker.c`/`semaphore.c`：完成和同步原语。
- `gpfifo.c`/`pushbuffer.c`/`qmd.c`：设备命令描述与队列。
- `task.c`：提交任务/资源追踪的通用载体。

## 行级审计

- `[src/cui/cuistream.c:1741-1877]` 创建资源和失败清理。
- `[src/cui/cuistream.c:1880-1894]` pool mutex 包装。
- `[src/cui/cuistream.c:1926-1954]` marker 驱动 detached reclaim。
- `[src/cui/cuistream.c:1973-1986]` QMD round-robin。
- `[src/cui/channel.c:43-173]` DMAL/backend/channel 资源。
- `[src/cui/channel.c:247-260]` MPS submission lock 和 channel lock。

## 未完成

host 侧 pushbuffer/GPFIFO/marker 生命周期已完成静态主线；仍需继续读取各 DMAL backend/HAL 的 doorbell、RM/WDDM 提交和设备消费，以闭合 firmware 完成值的来源与时序。

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
