# M04 Driver 数据结构与资源

- 证据状态：关键资源布局和所有权已确认；完整内部结构未知

## HDC client

创建函数分配 client 和 session 数组，并记录 magic、服务类型、超时、session 状态、锁等字段 `[driver/src/ascend_hal/hdc/common/hdc_client.c:49-111]`。client 是 session 的拥有者；销毁必须晚于所有 session。

## Queue context

`queue_drv_open` 使用当前进程 TGID 建立上下文，保存于 `file->private_data`；`queue_drv_release` 负责移除进程记录和释放上下文 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-95]`。其典型所有权是 file descriptor → queue context → 进程/设备队列。

## Vector/DMA 参数

`queue_check_vector` 验证 vector 数量、每项地址与长度的一致性 `[queue_fops.c:149-175]`。这类参数可能进一步用于 DMA 或 HDC 传输，必须避免溢出、越界和生命周期失效；具体 pin/map/unmap 代码待继续追踪。

## 调度 ioctl

`esched_ioctl.h` 定义 CPU ID、进程组、事件优先级、等待/提交事件和表项增删等结构，是跨用户态/内核态的数据契约。字段布局、兼容版本和安全拷贝规则应与实现配套审查。
