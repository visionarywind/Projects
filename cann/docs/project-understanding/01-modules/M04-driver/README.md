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

## 边界

Driver 不负责 GE 图优化或 ACL 高层模型接口；Runtime/ACL 通过 ABI 和设备协议使用其能力。具体 ioctl、设备节点和内核版本兼容矩阵待补。
