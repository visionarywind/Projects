# M04 Driver 设计

- 证据状态：组件分层和资源生命周期已确认；完整内核实现路径部分未知

## HAL 与 SDK-driver

`driver/src/CMakeLists.txt` 将正式构建拆成 `ascend_hal`、`sdk_driver` 和可选 `custom`；`DRIVER_COMPAT` 只构建 HAL `[driver/src/CMakeLists.txt:9-30]`。这形成兼容层与完整驱动包的边界。

- **HAL**：向上提供相对稳定的设备管理、通信、队列、调度和内存接口。
- **SDK-driver**：提供更贴近设备/内核的 queue、HDC、VM、网络、kernel adaptation 和平台适配。
- **内核/设备**：处理 ioctl、设备节点、DMA、命令队列和硬件状态。

## 生命周期设计

HDC client 创建时分配 client/session 数组、初始化 magic、服务类型、超时、session 状态和 mutex `[hdc_client.c:49-111]`。销毁前加锁检查活动 session；有活动 session 返回 `DRV_ERROR_CLIENT_BUSY`，否则关闭底层 client 并释放内存 `[114-179]`。

Queue 打开时按当前进程 TGID 创建 context 并放入 `file->private_data`，release 时删除进程信息并释放 context `[queue_fops.c:62-95]`。这表明 queue 资源与文件描述符/进程生命周期绑定。

## 普通内存 cache 设计

Driver ordinary cache 与 SOMA pool 是两个不同 API 家族。普通 `rtMalloc` 进入 HAL 后，malloc manager 按请求属性选择 cache 或 normal allocator；cache 负责保留 backing、切分空闲区和延迟释放。

```text
V2：heap -> alloced/idle_va/idle_size/idle_mapped 多棵树 -> heap operation
V3：cache_allocator -> ga_inst -> 多个 ga_range -> ga_area
```

V2 的 mapped cache 可直接复用已有映射，unmapped node 仍需 map；V3 将 normal allocator 返回的不连续 range 统一交给 gen allocator。两者都以完整 backing/node/range 空闲作为 shrink 前提，但具体阈值、锁和错误回滚不同。详见 [driver-memory-pool-analysis.md](driver-memory-pool-analysis.md)。

## 已确认与未知

已确认：HAL 到用户态 SVM/cache 的调用边界、产品构建选择、树/区间元数据和主要锁。未知或未验证：SDK-driver 以下的完整 ioctl/内核/固件路径、物理页算法、BUSY 完成时机和真实性能收益。

## ABI 与安全

esched ioctl 头文件中的命令编号、结构体和字段布局是用户态/内核态 ABI；修改必须同步两端并考虑 32/64 位布局。所有来自用户空间的地址、长度、vector 数量和设备 ID 都应在边界校验；`queue_check_vector` 已体现地址/长度及数量检查 `[queue_fops.c:149-175]`。
