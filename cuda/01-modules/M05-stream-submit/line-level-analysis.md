# M05 行级分析

- `[src/api/apistream.c:43-109]`：检查 flags、锁 context、创建 stream、注册 QMD semaphore pool，失败 detach。
- `[src/cui/cuistream.c:1741-1877]`：pool stream、QMD、UVM prefetch semaphore、CPU semaphore、public handle 和 UVM registration。
- `[src/cui/channel.c:43-173]`：DMAL Init、tracking/cross-engine semaphore、manager 插入与 cleanup。
- `[src/cui/cuistream.c:1926-1986]`：marker 完成后 detached stream reclaim。
- `[src/cui/cuilaunch.c:584-724]`：begin push、HAL finalize/control、QMD tracking、end push。
- `[src/cui/gpfifo.c:313-416,437-490]`：entry 编码、tracking completion 和 pushbuffer get reclaim。
- `[src/cui/gpfifo.c:716-796]`：flush unit 封存 entry、推进 gpuPut 和 DMAL advance 边界。
- `[src/cui/pushbuffer.c:22-99,181-235]`：put/get 环形游标、对齐和回收条件。
- `[src/cui/marker.c:655-813,1008-1140]`：异构 marker entry、flush、blocking wait 和完成检查。
