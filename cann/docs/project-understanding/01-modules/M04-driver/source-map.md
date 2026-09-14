# M04 Driver 源码地图

- 证据状态：主要目录已确认；不是全量符号清单

| 路径 | 职责 | 关键证据 |
|---|---|---|
| `src/ascend_hal/` | 用户态 HAL 组件 | `[driver/src/ascend_hal/CMakeLists.txt:9-29]` |
| `src/ascend_hal/dms/dc/devdrv_manager_adapter.c` | 设备管理适配、SoC 查询 | `[16-52]` |
| `src/ascend_hal/hdc/common/hdc_client.c` | HDC client/session | `[20-179]` |
| `src/sdk_driver/` | SDK/设备适配实现 | `[driver/src/sdk_driver/CMakeLists.txt:9-30]` |
| `src/sdk_driver/queue/host/queue_fops.c` | queue file operations、HDC 连接 | `[62-175]` |
| `src/sdk_driver/esched/command/ioctl/esched_ioctl.h` | 调度 ioctl ABI | 命令和结构体定义 |
| `pkg_inc/` | 公共包头文件 | `driver/README.md:32-115` |
| `test/` | Driver 测试 | `driver/CMakeLists.txt:12-14` |
| `examples/`、`docs/` | 示例与设计资料 | `driver/README.md:32-115` |

| `src/ascend_hal/svm/CMakeLists.txt` | 按 `PRODUCT` 选择 V2/V3 SVM 实现 | `[11-15]` |
| `src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c` | V2 heap、mapped/unmapped node、split/merge/shrink | `[540-1391]` |
| `src/ascend_hal/svm/v2/devmm/devmm_rbtree/devmm_rbtree.c` | V2 VA/size/mapped 多树索引和 exact/upper-bound 查询 | `[104-179,188-316]` |
| `src/ascend_hal/svm/v3/assign/malloc_mng/malloc_mng.c` | V3 cache/normal 分流和 `is_from_cache` 生命周期 | `[394-527]` |
| `src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c` | V3 cache 条件、range 扩展、free、shrink、BUSY recycle | `[133-355]` |
| `src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c` | V3 range/area、地址/大小索引、split/merge/recycle | `[19-668]` |
| `src/ascend_hal/svm/v3/assign/normal_malloc/normal_malloc.c` | V3 normal VA allocate/populate/depopulate/free | `[127-167]` |


HAL 包含 build、bbox、esched、hdc、queue、buff、dmc、pbl、dms、svm、dpa、mmpa、trs、comm 等；Ascend910B 额外包含 RoCE。SDK-driver 还包含 vmng、vnic、vpc、kernel_adapt、fms、vascend、ts_agent 等。
