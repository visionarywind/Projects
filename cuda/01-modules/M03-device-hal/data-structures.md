# M03 数据结构

## 核心对象

| 对象 | 作用 | 生命周期/所有权 |
|---|---|---|
| `CUdev` | 对外 device ordinal 对应的设备对象 | 由 device manager 建立并由全局设备表持有 |
| `CUdevstate` | 设备能力、架构和计数状态 | 随 `CUdev` 初始化/销毁；具体字段由 HAL 填充 |
| `CUhal` | 通用操作到架构实现的函数表 | 由架构初始化选择并挂在 device 上 |
| `DMAL` 状态 | RM/MRM/WDDM/amodel 后端句柄 | 由平台后端创建，失败时由对应 `Deinit` 回滚 |

## 选择关系

`devmgr` 的 remap table 将用户 ordinal 映射到物理设备；`CUdev.state.ccArch` 再选择 HAL。编译宏决定候选架构函数是否存在，因此设备枚举结果、运行时架构和最终 binary 是三个不同层次（静态确认：[src/cui/devmgr.c:39-121,175-199,201-277]；[src/cui/hal/hal.c:35-79]）。

## 并发边界

设备表排序/筛选属于初始化或设备管理路径；channel 初始化另受 context/manager 锁保护。具体锁顺序和后端句柄字段需结合外部 common/RM 树确认，当前标为未知。
