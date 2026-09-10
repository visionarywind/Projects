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

## ABI 与安全

esched ioctl 头文件中的命令编号、结构体和字段布局是用户态/内核态 ABI；修改必须同步两端并考虑 32/64 位布局。所有来自用户空间的地址、长度、vector 数量和设备 ID 都应在边界校验；`queue_check_vector` 已体现地址/长度及数量检查 `[queue_fops.c:149-175]`。
