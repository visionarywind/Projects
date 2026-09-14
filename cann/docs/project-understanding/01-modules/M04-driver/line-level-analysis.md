# M04 Driver 行级分析

- 证据状态：关键函数已确认

## `halGetSocVersion`

1. 在功能宏启用时进入 SoC 查询路径；检查输出指针、长度和设备 ID `[driver/src/ascend_hal/dms/dc/devdrv_manager_adapter.c:16-30]`。
2. 调用 `halGetDeviceInfo` 获取设备信息，再调用 `drvGetDevInfo` 读取 Driver 信息 `[31-42]`。
3. 使用 `strncpy_s` 复制 SoC 版本，返回 Driver 错误码 `[43-52]`。

## `drvHdcClientCreate`

参数检查后分配 client 与 session 数组，初始化 magic、服务类型、超时、状态和 mutex；任一步失败都应走清理路径 `[driver/src/ascend_hal/hdc/common/hdc_client.c:20-111]`。

## `drvHdcClientDestroy`

验证 client/magic 后加锁；活动 session 存在时返回 busy，避免释放仍被使用的底层资源；无活动 session 才销毁 PCIe client 和内存 `[114-179]`。

## Queue

open 将 TGID 关联到 queue context，release 解除关联并释放；host init/uninit 负责 HDC kernel session 的连接和关闭 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-147]`。

## SVM cache 行级走读

### V2

- `devmm_alloc_mem` 按 `heap->chunk_size` 对齐请求，在 `heap_rw_lock` 读锁和 `tree_lock` 内选择 node `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1095-1137]`。
- 不超过 `need_cache_thres` 时先查 mapped cache；查找按 exact/upper-bound，未命中再查 idle size `[devmm_virt_com_heap.c:1036-1084]` `[devmm_rbtree.c:104-138]`。
- mapped node 大于请求时切分并把剩余部分插回 mapped tree；unmapped node 命中则调用 map operation `[devmm_virt_com_heap.c:688-821,917-978]`。
- free 合并相邻 mapped node；超过 shrink 条件时找完整 node，底层释放失败则恢复 tree 和统计 `[devmm_virt_com_heap.c:540-686,1151-1278]`。

### V3

- malloc manager 先依据 NUMA、flag、cache allocator、size 和 align 判定是否进 cache `[driver/src/ascend_hal/svm/v3/assign/malloc_mng/malloc_mng.c:394-399,492-517]`。
- `gen_allocator` 的 `size_area_tree` 先 exact 再 upper-bound；命中后切分 area，free 时按地址找到邻居并合并 `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:163-177,219-295,600-668]`。
- cache miss 通过 normal malloc 获取新 range，加入失败回滚；完整 idle range 才回收，底层 BUSY 时进入 recycle segment `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:192-252]`。

