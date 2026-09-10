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

## 复杂度

参数检查、状态判断和单个资源操作通常 O(1)；session/vector 数量相关的遍历为 O(n)。设备命令和 DMA 延迟取决于硬件与队列状态。
