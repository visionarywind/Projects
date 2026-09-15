# M04 Driver 行级分析

- 文档目的：解释 01-modules/M04-driver/line-level-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：关键函数已确认
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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
