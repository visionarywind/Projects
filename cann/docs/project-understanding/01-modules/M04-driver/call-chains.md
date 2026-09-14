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

## 普通 rtMalloc 的 Driver cache

```text
Runtime NpuDriver::DevMemAlloc
  -> halMemAlloc
  -> V2: devmm_alloc_proc -> heap list -> mapped/size tree -> node split/map
  -> V3: svm_mem_malloc -> malloc manager -> cache_malloc
       -> svm_ga_alloc -> size_area_tree -> area split
  -> 返回 VA
```

cache miss 的后半段不同：V2 必要时创建 heap 或建立 mapping；V3 计算扩展大小，调用 `svm_normal_malloc` 获取 backing，`svm_ga_add_range` 成功后重试。证据与 free/shrink 链见 [driver-memory-pool-analysis.md](driver-memory-pool-analysis.md)。

## 产品选择

`driver/src/ascend_hal/svm/CMakeLists.txt` 按 `PRODUCT` 选择 V2/V3：`ascend910B` 使用 V2，`ascend950` 使用 V3；`build.sh` 将 `ascend910b` 和 `ascend910_93` 映射到 `ascend910B`。源码规则不能替代当前构建产物或设备验证。

## HDC 连接

`queue_drv_host_init` 检查重复连接，调用 `hdcdrv_kernel_connect` 并保存 session；uninit 调用 close 并清理 `[queue_fops.c:97-147]`。
