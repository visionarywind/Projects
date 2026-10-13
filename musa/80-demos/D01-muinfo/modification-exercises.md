# D01：修改练习

- 文档目的：在 D01（muInfo）上做四档由浅入深的修改练习，每档给出改动落点、验证方式与红线。
- 适用范围：`source/musa` 快照的 `src/tools/muInfo.cpp`（387 行，本轮统计以 387 行为准）。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：已确认（行号静态核对）；**所有练习均未执行**。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`execution-trace.md`](execution-trace.md)
- 后续阅读：[`../../99-roadmap/testing-recipes.md`](../../99-roadmap/testing-recipes.md)

## 结论摘要

D01 全部代码集中在两个函数：`GetDeviceProps` 采集、`printDeviceProp` 打印，主循环只做调用与 `muInit`/`muDevicePrimaryCtxRetain` 收尾 [src/tools/muInfo.cpp:372-387]。所以四档练习的共同模式是：**改在 `muInfo.cpp` 内部，不碰 `src/driver` 与 `src/musa`**。`src/tools` 由 `GLOB_RECURSE` 收集源文件 [src/tools/CMakeLists.txt:10-15]，改完重编即可，不需要改清单。

---

## 练习 1（低风险）：给已有属性增加输出字段

**目标**：在逐设备打印里增加一个 `MU_DEVICE_ATTRIBUTE_*` 字段，保持错误处理风格不变。

**改法**：
1. 在采集侧 `GetDeviceProps` 里，照着 `MU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK` 的三行写法新增一次属性查询 [src/tools/muInfo.cpp:133-139]——注意它把错误统一交给 `checkMuErrors` 处理，不要改成裸 `if`。
2. `DeviceProp` 结构体补字段（结构体从 22 行开始，采集函数从 119 行开始，两处必须同时改）[src/tools/muInfo.cpp:22-117]、[src/tools/muInfo.cpp:120-124]。
3. 在 `printDeviceProp` 增加一行对齐输出，与其他字段格式一致 [src/tools/muInfo.cpp:283-370]。

**验收**：无设备、单设备、多设备三种情况下打印列不串行；错误时仍走 `checkMuErrors` 记录并跳过该字段 [src/tools/muInfo.cpp:9-20]。**静态分析，未验证。**

**风险**：打印列宽变化会让现有 diff 对比失效，新增字段建议追加在行尾而不是插入中间。

---

## 练习 2（错误处理）：查询失败记 device id 后继续

**目标**：把"属性查询失败即中断"改成"记录 device id + 错误码，继续其余设备"。

**改法**：
1. 先确认工具预期：`GetDeviceProps` 当前在首个错误处就返回，`main` 只打印 `muInit` 结果与设备信息 [src/tools/muInfo.cpp:372-387]。若改成部分结果，主函数必须能区分"全失败"与"部分失败"。
2. `checkMuErrors` 目前只判断返回码 [src/tools/muInfo.cpp:9-20]；扩展时把 `deviceId` 一并打印，并把返回策略改为"记录 + 继续"。
3. 结构化日志的推荐落法：在 `GetDeviceProps` 的循环体内把 `checkMuErrors` 的返回值暂存，循环结束后统一返回首个错误码，这样下游既有部分结果又能看到失败原因。

**验收**：模拟第 2 台设备查询失败（可通过传入非法属性值或 mock），输出应含 `deviceId` 且其余设备信息完整；全失败时退出码非零。**未验证。**

**先决条件**：确认工具是否允许部分结果——这是本练习的唯一阻塞项，改代码之前先查 [`failure-paths.md`](failure-paths.md) 与调用方约定。

---

## 练习 3（可观测性）：增加 API 阶段标签

**目标**：在打印输出里区分"采集阶段"与"打印阶段"的耗时或阶段标签，**不改任何 Driver API**。

**改法**：
1. 阶段划分只用 `muInfo.cpp` 自己的代码：init → `GetDeviceProps` → peer 矩阵（`muDeviceCanAccessPeer` 两重循环）→ `muMemGetInfo_v2` → `printDeviceProp` [src/tools/muInfo.cpp:341-359]、[src/tools/muInfo.cpp:363-369]。
2. 用本地计时结构包住每个阶段，**不要**为打点去改 `src/driver`——那会让 D01 失去"最小查询 Demo"的定位。
3. 输出建议走 `MUSA_LOG` 的位掩码开关，别无条件打印（开关读取与缓存语义见 [`../../99-roadmap/performance-guide.md`](../../99-roadmap/performance-guide.md)）。**不要在默认输出里引入新的 API 调用次数。**

**验收**：关开关时输出与改前逐字节一致；开开关时能看出每个阶段的归属。**未验证。**

---

## 练习 4（回归）：三环境验证矩阵

| 环境 | 期望 |
|---|---|
| 无设备 | 在 `muInit` 或首个设备调用处干净失败，错误码来自 `checkMuErrors`，不崩不卡 [src/tools/muInfo.cpp:9-20] |
| 单设备 | 属性、peer 自环矩阵、memInfo 全部打印；`muDevicePrimaryCtxRetain` 正常 [src/tools/muInfo.cpp:372-387] |
| 多设备 | peer 矩阵维度正确，某台失败不影响其余设备输出 |

硬件环境未提供时**只能准备测试方案，不能宣称通过**；本快照没有任何已执行的 D01 运行记录。

---

## 红线

1. 不要把 D01 改成隐式调用 kernel / 内存分配，否则它就不再是最小查询 Demo。
2. 不要修改 `src/driver`、`src/musa` 以配合本练习；需要它们配合时，改用 [`../../99-roadmap/feature-development-recipes.md`](../../99-roadmap/feature-development-recipes.md) 的正式配方。
3. 不要删除 `checkMuErrors` 统一检查——`muInfo.cpp` 的错误出口只有这一处。

## 相关文档

- [`README.md`](README.md)、[`execution-trace.md`](execution-trace.md)、[`failure-paths.md`](failure-paths.md)
- [`../../01-modules/M10-tools-debug/README.md`](../../01-modules/M10-tools-debug/README.md)
- [`../../99-roadmap/feature-development-recipes.md`](../../99-roadmap/feature-development-recipes.md)

## 源码证据摘要

- 错误出口：[src/tools/muInfo.cpp:9-20] `checkMuErrors`
- 采集：[src/tools/muInfo.cpp:119-274] `GetDeviceProps`、[src/tools/muInfo.cpp:133-139] 属性查询范式
- 打印：[src/tools/muInfo.cpp:283-370] `printDeviceProp`、[src/tools/muInfo.cpp:341-359] peer 矩阵、[src/tools/muInfo.cpp:363-369] memInfo
- 主流程：[src/tools/muInfo.cpp:372-387]
- 构建：[src/tools/CMakeLists.txt:10-15] `GLOB_RECURSE`

## 未解决问题

1. 工具是否允许部分结果，需要向原作者或调用方确认。
2. 四个练习全部未编译、未运行。
3. `MUSA_LOG` 开关在 muInfo 进程内是否已被初始化，未验证。

## 下一步阅读建议

先做练习 1，用它验证你的构建环境是否可用，再进入练习 2；练习 2 的先决条件没确认之前不要动错误处理策略。
