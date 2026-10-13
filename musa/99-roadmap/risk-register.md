# 风险登记

- 文档目的：风险登记册：条目、证据、状态与验证方法。
- 适用范围：99-roadmap《风险登记》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- 后续阅读：[`reading-guide.md`](reading-guide.md)、[`risk-register.md`](risk-register.md)


| 等级 | 风险 | 当前状态 | 下一步 |
|---|---|---|---|
| 严重 | `Util::SplayTree::Search` 递归方向反转（`utilSplayTree.h:218-221`），`Find` 对**已存在**的键也 miss | 已确认源码缺陷，根因与影响面已量化 | 已完成：见 `99-roadmap/rca-splaytree-memmgr.md`，修分支方向 |
| 严重 | `MemMgr::FindPool` 无条件解引用 `Find` 结果（`memMgr.cpp:43`），`Find` miss 返回 `nullptr`，调用即崩 | 已确认源码缺陷 | 已完成 RCA：判空 + 修 `Search` |
| 严重 | `MemMgr::Free` 在 release 下可能错池/空指针：`Get` 缺失键返回"最近节点"（`memMgr.cpp:158-160`），断言被裁剪 | 已确认源码缺陷 | 已完成 RCA：`Free` 需可失败出口，禁止裸解引用 |
| 中 | `MemMgr::GetPool`/`Allocate:116` 用 `Get`（返回"最近节点"）做精确键查找，无判空 | 已确认源码缺陷 | `Allocate` 靠 `memMgr.cpp:117-121` 字段比对兜底，`GetPool` 无兜底；建议改用 `Find`+判空 |
| 阻断 | M11 loader 在本快照无法配置编译：缺 `shared_include`、缺 `hwdefs/mtgpu_asics.h` 正确路径 | 已确认 | 查上游构建脚本确定 include 路径 |
| 严重 | `FillAccessors` 遇源端 0 即中止填充，该分组后续槽位静默不填，调用方无法区分"不支持"与"漏填" | 已确认 | 核对 M02 各 accessor 源列表是否含 0 空洞 |
| 高 | 未知 `Client` id 静默返回 MUPTI 导出表（`mu_entry.cpp:2300-2303`，源码自带 `FIXME`） | 已确认 | 待 MUPTI 升级后改为返回 `MUSA_ERROR_INVALID_VALUE` |
| 高 | `AccessorHint = 0xDEADBEEF` 是填充哨兵而非"可调用"标记，语义与直觉相反 | 已确认 | 已完成：链路与两种分发模式已写入 `M10-tools-debug/call-chains.md` |
| 高 | 返回静态表族的五个分支未在接口上标注所有权，调用方误释放会破坏驱动状态 | 已确认 | 补充接口注释 |
| 高 | `MUpti::DriverExportTable` 前 12 个成员被 legacy profiler ABI 锁定，不能重排 | 已确认 | 建立导出表符号快照回归 |
| 高 | loader 导出清单无一致性检查：`loader_api_list.inc` 漏一条即静默少一个导出符号 | 已确认 | 与 `libmusa.so` 实际导出做符号表 diff |
| 高 | loader 错误吞掉：损坏的 MUSA 适配器与非 MUSA 硬件返回码不可区分 | 已确认 | 引入区分错误码或诊断日志 |
| 高 | `musa_loader` 不链接 `util` 目标，仅因头文件 header-only 才能链接 | 已确认 | 向 `utilLibrary.h` 加任何非 inline 定义都会破坏 loader 链接 |
| 高 | 异步资源和析构顺序 | 部分确认 | command 状态/回收审计 |
| 高 | HAL/M3D 私有实现未完整追踪 | 已知缺口 | 补接口到子模块行级链 |
| 高 | `SplayTree` 析构为空实现，泄漏全部节点 | 已确认 | HAL 靠手工排空绕过，其他使用者会泄漏 |
| 中 | `Executor::QueueTask` 在关闭后返回无效 `std::future`，调用 `.get()` 抛 `broken_promise` | 已确认 | 改为返回错误 future 或显式失败对象 |
| 中 | `Library::LoadPath` 在 Windows 忽略 `LoadMode`；`Load` 把未终止的 `string_view::data()` 交给加载函数 | 已确认 | Windows 分支补 `LoadMode` 映射 |
| 中 | `Log::Setting::Get()` 的 `std::stoul` 可抛异常，异常会逃逸到任意 API 调用路径 | 已确认 | 改用不抛异常的解析 |
| 中 | `IsPowerOfTwo(0)` 返回 `true`，影响 `IsMultipleOf` 与 `RoundUpToNextPowerOfTwo` | 已确认 | 加 `val != 0` 守卫 |
| 中 | `probe.cpp` 完全不参与构建，loader 层无诊断手段 | 已确认 | 恢复构建目标或补独立诊断路径 |
| 中 | `TimeStatistics` 嵌套使用共享同一 `static thread_local` 起始时刻 | 已确认 | 改为实例成员 |
| 中 | CSV 开关拼写差异：`CSV_UNSUPPORTED` 声明 vs `CSV_UNSPPORTED` 判断，且引用缺失的 `tool/code_reason.py` | 已确认 | 配置矩阵验证 |
| 中 | `MUSA_BUILD_UT` 会 `add_subdirectory(unittest)`，但本快照无该目录 | 已确认 | 构建可用性验证 |
| 中 | `MUSA_FORCE_API_VERSION` 是硬 `#error`，旧应用编译失败且无迁移指引 | 已确认 | 补迁移说明 |
| 中 | 文档行号随源码漂移 | 持续风险 | 版本锚点和定期重扫 |
| 低 | `util` 用 `aux_source_directory` glob，新 `.cpp` 静默纳入编译 | 已确认 | 改显式源文件列表 |
| 低 | `perfStatistics.{h,cpp}` 与 `SteadyClock` 为死代码 | 已确认 | 评估删除 |
| 低 | `ObjectPool::Return` 无校验，重复归还会使 `mAllocated` 下溢 | 已确认 | 加断言或计数上界 |
| 低 | `ParseCoreDumpTemplate` 的 `%h` 每次调用都做 WSAStartup/WSACleanup | 已确认 | 进程内只初始化一次 |
| 低 | `muMemcpy` 兼容包装在同步传入 stream 后又调 `muDeviceSynchronize()`，默认流下可能死锁 | 已确认源码缺陷 | 交由驱动源码负责人判断，本次不改源码 |
| 低 | `logging.h`/`utils.h` 污染全局命名空间（`fatal`、`ToString`、`MUSA_ASSERT`） | 已确认 | 逐步迁入命名空间 |
| 低 | 安装写系统目录 | 已确认 | 仅授权执行 |

## 证据

- SplayTree：[src/util/utilSplayTree.h:216-226]、[src/util/utilSplayTree.h:64-64]、[src/util/utilSplayTree.h:97-100]、[src/hal/m3d/memMgr.cpp:38-43]
- loader：[src/loader/CMakeLists.txt:16-17]、[src/loader/CMakeLists.txt:45]、[src/loader/driver_resolver.cpp:220]、[src/loader/driver_resolver.cpp:247]、[src/loader/CMakeLists.txt:74-93]
- ExportTable：[src/musa_shared_include/export_table.h:859-860]、[src/musa_shared_include/export_table.h:1151-1172]
- util 设施：[src/util/utilExecutor.h:53-62]、[src/util/utilLibrary.h:45-68]、[src/util/utilLog.h:95-98]、[src/util/utilMath.h:17-19]、[src/util/perfStatistics.h:26]
- 构建：[CMakeLists.txt:25]、[CMakeLists.txt:134]、[CMakeLists.txt:146-154]、[CMakeLists.txt:167-169]

## 相关文档

- [`risk-register.md`](risk-register.md)
- [`reading-guide.md`](reading-guide.md)
- [`technical-debt.md`](technical-debt.md)
- [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)

## 源码证据摘要

- `src/util/utilSplayTree.h`（3 处，例：[src/util/utilSplayTree.h:216-226]）
- `src/loader/CMakeLists.txt`（3 处，例：[src/loader/CMakeLists.txt:16-17]）
- `src/loader/driver_resolver.cpp`（2 处，例：[src/loader/driver_resolver.cpp:220]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`reading-guide.md`](reading-guide.md)；阅读 [`risk-register.md`](risk-register.md)。
