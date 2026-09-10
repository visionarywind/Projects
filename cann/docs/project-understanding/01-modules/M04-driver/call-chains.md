# M04 Driver 调用链

- 证据状态：关键链已确认；用户态 HAL 到具体内核设备命令部分未知

## SoC 查询

```text
halGetSocVersion(devId, out, len)
  -> validate output/length/device
  -> halGetDeviceInfo
  -> drvGetDevInfo
  -> strncpy_s(out, soc version)
  -> drvError_t
```

证据：`driver/src/ascend_hal/dms/dc/devdrv_manager_adapter.c:16-52`。

## HDC client 生命周期

```text
drvHdcClientCreate
  -> parameter checker
  -> allocate client/session arrays
  -> initialize magic/config/state/mutex
  -> return client

drvHdcClientDestroy
  -> validate magic
  -> lock
  -> active session? --yes--> DRV_ERROR_CLIENT_BUSY
  -> destroy PCIe client
  -> free arrays/client
```

证据：`driver/src/ascend_hal/hdc/common/hdc_client.c:20-179`。

## Queue 文件生命周期

```text
open
  -> TGID queue context
  -> add process info
  -> file->private_data
release
  -> delete process info
  -> free context
  -> private_data = NULL
```

证据：`driver/src/sdk_driver/queue/host/queue_fops.c:62-95`。

## HDC 连接

`queue_drv_host_init` 检查重复连接，调用 `hdcdrv_kernel_connect` 并保存 session；uninit 调用 close 并清理 `[queue_fops.c:97-147]`。
