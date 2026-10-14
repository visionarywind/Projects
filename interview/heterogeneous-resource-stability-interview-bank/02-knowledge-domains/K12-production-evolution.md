# K12：生产架构、发布迁移、灾备与前沿演进

## 边界

本知识域讨论异构资源栈在生产环境中的演进治理：多设备、多版本、多节点下的灰度发布、兼容性、迁移、回滚和灾备。

讨论对象是“版本、资源、状态一起变的治理过程”，不是单次运行时机制（那属于 K01–K11）。四仓独立发布带来的跨仓版本配套代价、排空/释放/恢复顺序、兼容矩阵、发布门禁和 RTO/RPO，是本域的事实边界。

特定厂商的热升级能力、reset 语义和 CRIU/检查点行为必须绑定目标平台与版本。文档确认了独立发布、ABI 同步代价、HDC busy、cache shrink 与 Context teardown 顺序，但真实驱动/固件升级与灾备切换属于 `[待验证]`。

## 核心机制

### 1. 四仓独立发布带来的约束

`[已确认]` CANN 的四仓可独立构建和发布，优势是图优化与硬件资源管理解耦、独立发布、多语言接入；代价是跨仓版本配套和 ABI 兼容要求高，错误码、头文件和二进制符号需要同步，且没有设备时只能做有限验证。[cann/00-overview/architecture.md:51-54]

`[已确认]` 设计原则要求头文件、共享库、模型格式、设备固件和工具链来自兼容的 CANN 版本；异步 API 调用者必须保持输入、输出、回调和 stream/event 有效直到完成通知；不能把无硬件环境的编译成功等同于设备运行成功。[cann/00-overview/design-principles.md:22-31]

因此“发布”在异构栈上不是一个仓库的动作，而是版本矩阵的一次整体对齐。

### 2. 版本与配置矩阵

`[已确认]` 配置影响地图把关键维度列在一起：`ASCEND_INSTALL_PATH`、`CANN_3RD_LIB_PATH`、`ENABLE_OPEN_SRC`、`DRIVER/DRIVER_COMPAT`、测试与插桩选项、SoC/产品宏、ACL 初始化配置字符串，以及 Toolkit/Driver/Firmware 版本。[cann/90-cross-module/configuration-impact-map.md:15-24]

`[已确认]` 变更影响地图列出典型联动：Driver cache flag 变更触达 Runtime memory policy 与 SVM；Runtime C API/handle 变更触达 ACL/GE/Driver；HAL 函数/结构体变更触达 SDK-driver 与内核 ioctl；esched ioctl ABI 变更必须同步用户态与内核态。[cann/90-cross-module/change-impact-map.md:15-29]

```text
version_matrix = { GE, ACL, Runtime, Driver, Toolkit, Firmware, SoC }
deploy requires: version_matrix_consistent(version_matrix)
```

### 3. 发布/迁移状态机

把发布、迁移、升级统一成一个显式状态机：

```text
RUNNING --stop new access--> DRAINING
DRAINING --pending_tasks==0 and active_sessions==0--> DRAINED
DRAINED --release deps--> RELEASED
RELEASED --upgrade/migrate--> RESTARTED
任意阶段 --失败--> ROLLBACK
```

`[已确认]` Driver 侧对排空有明确要求：HDC client 必须没有活动 session 才能销毁，否则返回 `DRV_ERROR_CLIENT_BUSY`。[cann/90-cross-module/cross-module-call-chains.md:42-52]

`[已确认]` Cache 在设备反初始化前先执行 `svm_cache_shrink`，再销毁 allocator；初始化失败会反向销毁已创建的 flag。[cann/90-cross-module/pooling-and-resource-management.md:67-71]

所以 `DRAINING -> DRAINED` 的判据是 `pending_tasks == 0 and active_sessions == 0`，而不是“等服务重启”。

### 4. 释放顺序与依赖逆序

`[已确认]` Context 销毁顺序为“停止新访问 → 清理 Model → flush/清理 Stream → 清除线程绑定 → 等待引用耗尽 → 移除 Context Registry → 删除 Context”。[cann/90-cross-module/memory-and-resource-lifecycle.md:49-55]

`[已确认]` 释放链为 `GE Executor UnLoad -> main graph UnLoad/de-init graph -> Runtime stream/event/model -> Driver queue/HDC close -> client/context free`。[cann/90-cross-module/cross-module-call-chains.md:42-52]

这是依赖图的逆拓扑序，不是简单创建逆序：先停新访问，再等未完成工作，再清子资源，最后删被依赖对象。

### 5. 状态恢复与灾备

`[已确认]` Driver V3 的 CRIU reset/restore 通过 recycle range 重新登记；`cache_recycle_seg` 记录“异步释放但暂不能立即复用”的 VA 段，释放时先删树再转 normal free，按设备清空用于 reset/卸载。[cann/90-cross-module/pooling-and-resource-management.md:75]

```text
recovery requires: recycle/registry 状态先登记，再映射/分配
```

顺序错误会导致恢复后地址冲突或泄漏。灾备还要定义 RTO/RPO、切换粒度和回滚路径，并确认驱动/固件在目标节点可配套。

### 6. 发布门禁与验证分层

`[已确认]` 跨仓变更的评估顺序要求：先确认真正产生副作用的实现，再检查 CMake target / 导出表 / HAL ABI，更新错误映射与文档，最后分别运行无设备 UT、跨仓集成和真实设备回归；未运行或缺设备时只记录静态证据和未验证边界。[cann/90-cross-module/change-impact-map.md:31-38]

```text
gate_no_device : 静态分析 + UT + 跨仓集成（边界明确为未验证）
gate_device    : 真实设备回归（多设备、故障恢复、驱动/固件匹配）
```

`[已确认]` 测试配方强调在报告中写明命令、环境、实际输出和未执行项，不以文档中的命令代替验证。[cann/99-roadmap/testing-recipes.md:25-27]

### 7. 灰度、回滚与爆炸半径

```text
canary   : 小流量/少节点先验证版本矩阵与资源排空
blue-green: 新旧并存，切换点即排空点，失败可切回
rollback : 保留可回滚旧版本；ROOLBACK 状态复用 DRAINING/DRAINED 判据
```

`[推断]` 灰度与回滚都依赖“版本 + 资源 + 状态一起动”的原则：只切版本而不排空会残留 session/pending，只排空而不对齐版本会在重启后 ABI 不匹配。

### 8. 失败模式

- 跳过排空直接重启：残留 session 或 pending 任务导致升级失败。
- 版本不同步发布：跨仓 ABI 不匹配，运行期错误难定位。
- 恢复顺序错误：recycle/registry 未先登记，出现地址冲突或泄漏。
- 假设热升级无影响：驱动/固件升级可能要求排空和 reset。
- 单环境成功即发布：未覆盖其他 SoC/驱动/固件组合。

### 9. 兼容矩阵与 ABI 治理

`[已确认]` 变更影响地图要求先确认真正产生副作用的实现，再检查 CMake target、安装库、导出表、HAL/SDK-driver ABI、AICPU 参数和动态依赖，最后同步错误映射、文档与测试矩阵。[cann/90-cross-module/change-impact-map.md:31-38]

`[已确认]` Driver 设计明确 esched ioctl 头文件中的命令编号、结构体和字段布局是用户态/内核态 ABI，修改必须同步两端并考虑 32/64 位布局。[cann/01-modules/M04-driver/design.md:43-45]

```text
compatibility_matrix[GE][ACL][Runtime][Driver][Firmware][SoC] = {supported, deprecated, unsupported}
ABI 变更清单: 头文件 | 导出符号 | 结构体布局 | ioctl 32/64 位 | 模型格式
```

治理动作是把“哪些组合被支持”显式化，而不是让运行期去发现不兼容。

### 10. 渐进发布策略

`[推断]` 三种渐进策略共享同一排空前提，差异在切换点和回退成本：

```text
滚动(rolling) : 逐节点替换，容量不降；滚动期间的版本混跑要能容忍旧协议
蓝绿(blue-green): 新旧并存，切换点即排空点；回退是切回旧环境
金丝雀(canary): 少量节点/流量先验证矩阵与排空；验证通过再扩量
```

关键约束：任何策略都不能绕过 `pending_tasks == 0 and active_sessions == 0` 的排空判据；混跑期间必须保证新旧版本的错误码与数据结构可共存。

### 11. 灾备指标与切换粒度

`[推断]` 灾备要显式定义以下量，否则“能切换”无法验收：

```text
RTO : 从故障到服务恢复的目标时间
RPO : 可接受的数据丢失窗口（对设备状态/aICPU 命令要对账）
switch_granularity : 切换单位（进程/节点/CSV/fabric 链路/整集群）
reconcile : 切换后对 recycle/registry/pool 状态的对账步骤
```

`[已确认]` CRIU reset/restore 依赖 recycle range 重新登记，说明切换粒度越细，越需要精确的状态对账。[cann/90-cross-module/pooling-and-resource-management.md:75]

### 12. 验证门禁与回滚演练

`[待验证]` 演练设计：

| 输入 | 步骤 | 期望 | oracle |
|---|---|---|---|
| 版本矩阵 + 活动 session + pending 任务 | 排空→释放→升级→恢复 | 排空后才释放，恢复后无地址冲突 | 终态合法、矩阵一致、资源计数归零 |
| 可注入升级失败 | 各阶段注入失败 | 触发 `ROLLBACK` 并退回旧版本 | 退后资源计数归零、无半排空 |
| 无设备环境 | 运行静态/UT/集成 | 明确标记未验证边界 | 不以编译成功代替设备验证 |

失败判据：出现半排空、版本错配或恢复后地址冲突，测试失败。真实驱动/固件热升级、CRIU restore 和灾备切换需在目标环境执行。

## 常见失效条件

- 把单环境成功当作可发布，把热升级和 reset 当作无影响操作。
- 只更新一个仓库，未同步错误码、头文件和二进制符号。
- 迁移前不排空：`pending_tasks != 0` 或仍有活动 session。
- 恢复时先映射/分配再登记 recycle/registry。
- 无回滚路径或未保留旧版本，失败后无法退回。
- 发布门禁不区分“无设备静态验证”和“真实设备回归”。

## 关键不变量

```text
I1  deploy 前 version_matrix 一致（含 Toolkit/Driver/Firmware/SoC）
I2  释放按依赖逆拓扑序：停新访问 -> 等 pending -> 清子资源 -> 删父对象
I3  迁移/升级前 pending_tasks == 0 且 active_sessions == 0
I4  恢复先登记 recycle/registry，再映射/分配
I5  失败路径存在可执行 ROLLBACK 且旧版本可用
I6  发布结论区分已在设备验证与仅静态验证
```

## 与其他知识域的连接

- **K02 设备能力与兼容**：版本矩阵和拓扑快照是发布门禁的输入。
- **K09 错误与恢复**：设备级自愈和故障域是灾备切换的基础单元。
- **K10 可观测性**：发布/迁移需要可回滚判据和观测门禁，K10 提供证据链。
- **K11 性能与容量**：迁移和灰度要考虑容量预留与水位，避免切换时过载。
- **K03 生命周期**：排空、释放和恢复顺序直接复用 Context/Stream/HDC 生命周期规则。
- **K04 内存与池**：cache shrink、recycle range 和 registry 是状态恢复的关键对象。

## 题目索引

本域对应题目（按 `知识域：` 字段确认）：`HRS-B013`、`HRS-I012`、`HRS-A012`、`HRS-A015`、`HRS-E012`、`HRS-E014`。

- `HRS-B013` 概念题：发布、迁移和灾备时异构资源栈的演进约束。
- `HRS-I012` 分析题：生产架构、发布迁移、灾备与演进路径的设计。
- `HRS-A012` 设计题：灰度迁移与安全回滚。
- `HRS-A015` 设计题：版本兼容与运行时契约治理。
- `HRS-E012` 高级题：平台化演进与兼容迁移战略。
- `HRS-E014` 高级题：多代际、多厂商异构环境治理。

## 证据入口

- `[已确认]` `cann/00-overview/architecture.md:51-54`：四仓独立发布的优势与跨仓 ABI/版本配套代价。
- `[已确认]` `cann/00-overview/design-principles.md:22-31`：版本配套、异步有效期与“编译成功 ≠ 设备运行成功”。
- `[已确认]` `cann/90-cross-module/configuration-impact-map.md:15-32`：配置/版本维度与排查顺序。
- `[已确认]` `cann/90-cross-module/change-impact-map.md:15-38`：变更联动与资源变更评估顺序、验证分层。
- `[已确认]` `cann/90-cross-module/cross-module-call-chains.md:42-52`：HDC client 无活动 session 才能销毁与释放链。
- `[已确认]` `cann/90-cross-module/pooling-and-resource-management.md:67-71,75`：cache shrink、CRIU recycle range 与按设备清空。
- `[已确认]` `cann/90-cross-module/memory-and-resource-lifecycle.md:49-55`：Context 销毁的资源顺序。
- `[已确认]` `cann/99-roadmap/testing-recipes.md:14-27`：最小回归矩阵与执行声明。
- `[推断]`：发布/迁移状态机、灰度与回滚原则由上述证据推导；`[待验证]`：真实驱动/固件热升级、CRIU restore 与灾备切换。

## 学习提示

先读 `HRS-B013` 建立“版本、资源、状态一起动”的约束，再用 `HRS-I012` 写出发布/迁移/灾备路径。重点掌握 `HRS-A012`/`HRS-A015`：如何设计排空判据、回滚路径和兼容契约。最后用 `HRS-E012`/`HRS-E014` 检验平台化演进与多代际治理：发布门禁要明确哪些已在设备验证、哪些仅静态验证，并保证任何失败都能沿依赖逆序安全回退。
