# 跨模块：配置影响地图

- 证据状态：构建配置和已见运行时配置已确认；完整发布配置矩阵未知

| 配置/环境 | 影响模块 | 主要影响 |
|---|---|---|
| `ASCEND_INSTALL_PATH` | GE/ACL/Runtime | 头文件、库、工具链和安装依赖 `[ge/CMakeLists.txt:49-68]` |
| `CANN_3RD_LIB_PATH` | GE/ACL/Runtime | 第三方库查找和链接 `[ge/CMakeLists.txt:49-68]` |
| `ENABLE_OPEN_SRC` | ACL | Runtime、HAL、MetaDef、DFX 等依赖查找 `[acl/CMakeLists.txt:110-126]` |
| `DRIVER` / `DRIVER_COMPAT` | Driver | 完整 HAL/SDK 或兼容 HAL 构建 `[driver/src/CMakeLists.txt:9-30]` |
| `ENABLE_TEST` / UT/COV/ASAN | 各仓 | 测试源、mock、诊断和编译器插桩 |
| SoC/产品宏（如 Ascend910B） | GE/Runtime/Driver | 组件集合、核心数、RoCE/DVPP 等产品能力 |
| ACL 初始化配置字符串 | Runtime ACL impl | 配置读取、SoC/平台缓存 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:153-180]` |
| Toolkit/Driver/Firmware 版本 | 全链路 | ABI、模型加载、设备运行和错误行为 |

## 排查顺序

1. 固定四仓 commit、Toolkit、Driver/Firmware 和 SoC。
2. 记录 CMake 选项、安装路径和动态库搜索路径。
3. 再看模型、shape、stream、资源限制和 DFX 配置。
4. 任何配置改变都重新跑接口、集成和设备回归。
