# M03 调用链、源码地图与风险

## 设备链

```text
cuDeviceGet / cuDeviceGetCount
 → API device wrapper
 → globals.devices / remap table
 → cuiDevice* 查询
```

设备初始化阶段：

```text
globalsInit / device discovery
 → CUdev.state.ccArch
 → InitDeviceArch
 → deviceInitArchKepler/…/Ampere
 → CUdev->hal function table
```

`InitDeviceArch` 的架构候选及编译条件有直接证据：[src/cui/hal/hal.c:35-79]；当前构建具体宏值未知。

## 文件地图

| 文件/目录 | 角色 |
|---|---|
| `src/cui/devmgr.c` | 设备表、过滤、排序、UUID/NVAPI/MPS |
| `src/cui/device.c` | 通用 device 状态与操作 |
| `src/cui/cuidevice.c` | Driver API 对接的设备查询 |
| `src/cui/hal/hal.c` | 架构分派 |
| `src/cui/hal/*` | 架构具体 device/launch/mem/QMD |
| `src/cui/dmal/common_*` | DMAL 通用连接 |
| `src/cui/dmal/rm/*` | RM backend |
| `src/cui/dmal/mrm/*` | MRM backend |
| `src/cui/dmal/wddm/*` | WDDM backend |
| `src/cui/dmal/amod/*` | amodel backend |

## 风险

- 架构代码由编译宏裁剪；未启用的 case 在二进制中不存在，静态阅读不能代表目标构建。
- ordinal 重排和 `CUDA_VISIBLE_DEVICES`/MPS 筛选可能改变设备索引；测试不能硬编码物理顺序。
- DMAL 到外部 RM/NVRM 是未完成链路，需匹配外部树和构建日志验证。
