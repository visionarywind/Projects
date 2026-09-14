# M02 ACL：模块入口

- 对应源码版本：`d26308d3`
- 证据状态：主要 API 和构建边界已确认

ACL 是面向应用的 C/C++ AI 计算接口层，封装设备、Context、Stream、内存、模型、算子和媒体能力 `[acl/README.md:20-41]`。它不是硬件驱动；运行时能力通过 `rt*`/Runtime 接口获得。

## 关键入口

- `aclrtSetDeviceImpl`：ACL 设备选择包装 `[acl/runtime/device.cpp:47-59]`。
- `aclrtResetDeviceImpl`、`aclrtGetDeviceImpl`、`aclrtSynchronizeDeviceImpl`：设备生命周期和同步 `[acl/runtime/device.cpp:80-169]`。
- `aclrtGetDeviceUtilizationRateImpl`：设备资源查询 `[acl/runtime/device.cpp:210-225]`。
- Runtime 仓的 `acl_rt.cpp`：公开 ACL Runtime 符号映射、Hook 和 callback 注册 `[runtime/src/acl/aclrt/acl_rt.cpp:17-104]`。

## 子域

`runtime/`、`model/`、`op/`、`tensor`、`data_transfer`、媒体和轻量 API；具体目录清单需结合版本持续补全。

## 推荐路径

先读 [design.md](design.md)，再读 [interfaces.md](interfaces.md) 和 [call-chains.md](call-chains.md)。

## 限制

构建依赖 Toolkit、第三方库和安装路径；UT 文档注明不支持 root。无匹配 NPU 时不能验证真实设备行为。
