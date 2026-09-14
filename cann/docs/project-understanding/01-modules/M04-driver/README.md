# M04 Driver：模块入口

- 对应源码版本：`6e2914c`
- 证据状态：主要目录、HAL/SDK-driver 分层和 HDC/Queue 入口已确认

Driver 为用户态/内核态之间提供设备管理、通信、队列、内存和调度能力，仓库 README 将代码分为 DCMI、HAL 和 SDK-driver `[driver/README.md:9-16]`。

## 关键分层

```text
Runtime / HAL client
  -> ascend_hal
      -> build/bbox/esched/hdc/queue/buff/dms/svm/dpa/trs/comm...
  -> sdk_driver
      -> queue/esched/buff/trsdrv/hdc/vmng/vnic/vpc/kernel_adapt...
  -> device nodes / kernel modules
  -> NPU
```

HAL 构建组成见 `[driver/src/ascend_hal/CMakeLists.txt:9-29]`；SDK-driver 组成见 `[driver/src/sdk_driver/CMakeLists.txt:9-30]`。

## 关键入口

- `halGetSocVersion`：通过设备信息查询 SoC 版本 `[driver/src/ascend_hal/dms/dc/devdrv_manager_adapter.c:16-52]`。
- `drvHdcClientCreate/Destroy`：主机设备通信 client 生命周期 `[driver/src/ascend_hal/hdc/common/hdc_client.c:49-179]`。
- `queue_drv_open/release`：进程 queue context 生命周期 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-95]`。
- `queue_drv_host_init/uninit`：内核侧 HDC session 连接 `[queue_fops.c:97-147]`。

## 内存池专题

普通 `rtMalloc` 下沉到 `halMemAlloc` 后，不是必然直接向设备申请，也不是 Runtime SOMA。Driver 根据产品和请求属性使用 ordinary cache：

- `ascend910B`/`ascend910_93` 编译 V2：heap + 多棵 VA/size/mapped 红黑树；
- `ascend950` 编译 V3：`cache_allocator` + 多个 `ga_range`/`ga_area`；
- 两者都支持按大小 exact/upper-bound 复用、切分、相邻合并和延迟 shrink。

请求是否实际进入 cache 还取决于 size、align、flag、NUMA、设备能力和当前状态。完整设计、Demo、行级走读和验证边界见 [driver-memory-pool-analysis.md](driver-memory-pool-analysis.md)。

## 边界

Driver 用户态源码可确认 HAL、SVM cache、VA/映射元数据和 ioctl 调用边界；闭源内核/固件中的物理页算法、设备完成时机和性能收益仍未验证。
