# 设计决策与验证记录

- 文档目的：记录本知识库在分析过程中做出的重大判断——哪些是源码直接证明的事实、哪些是推断、哪些仍未验证——避免后续批次静默推翻前序结论。
- 适用范围：`source/musa` 快照上的全部静态分析结论；运行、构建、硬件相关结论单独标注。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：部分推断（源码事实为已确认，运行期行为未验证）
- 最后更新：2026-10-07
- 前置阅读：[`analysis-state.md`](analysis-state.md)、[`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- 后续阅读：[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)、[`../90-cross-module/change-impact-map.md`](../90-cross-module/change-impact-map.md)

## 结论摘要

本文件记录三类内容：(1) 分析组织决策（模块划分、主 Demo 选择、只读约束、证据分级）；(2) 源码事实判断（`SplayTree::Search` 分支反转、`AccessorHint` 哨兵语义、`ExportTable` 两种分发模式、断言与断言裁剪等），每条都给出最小源码区间；(3) 已知冲突与验证缺口。所有判断基于静态阅读，未执行构建、测试或硬件运行。

## 1. 分析组织决策

| 决策/观察 | 依据 | 状态 |
|---|---|---|
| 使用 M01–M13 稳定模块 ID 而非按目录命名 | 顶层构建按 `add_subdirectory` 组织 util/hal/musa/driver/gdb/tools，与运行时对象边界并不一一对应 [src/CMakeLists.txt:7-15] | 已确认的组织决策 |
| 以 `muInfo` 作为 D01 主 Demo | 工具入口 `main` 是真实的 init → 查设备数 → retain primary context → 打印属性序列 [src/tools/muInfo.cpp:372-388] | 已确认入口，运行未验证 |
| 不执行构建、安装与测试 | `install.sh` 会把库复制进 `/usr/lib/*` 等系统目录，属于需要授权的环境修改 [install.sh:51-70] | 已执行的安全约束 |
| 先写静态调用链，再补动态 trace | 本快照缺 loader 编译依赖（回退路径 `shared_include` 与 `kmdif.h` 缺失即 `FATAL_ERROR`）[src/loader/CMakeLists.txt:14-23] | 当前工作策略 |
| 源码缺陷只分析、不修复 | RCA 仅给出修复建议与回归用例，不改 `source/musa` 任何文件 | 已执行的安全约束 |
| 关键结论同时给符号与行号 | 行号会随源码漂移，符号全名可用于重新定位 | 工作约定 |

## 2. 源码事实判断（重大）

| 判断 | 依据 | 状态 |
|---|---|---|
| `Util::SplayTree::Search` 的两个递归分支方向互换：`node->m_Key < key` 却进左子树 | [src/util/utilSplayTree.h:216-226] | 已确认源码缺陷（静态） |
| `Get` 返回“相等或最近”节点、从不返回空；`Find` 才会把哨兵转成 `nullptr` | [src/util/utilSplayTree.h:86-100] | 已确认（设计注释 + 实现） |
| `MemMgr` 把树查找当“必中”：`Get` 结果直接 `->m_Value`，安全条件只写成断言 | [src/hal/m3d/memMgr.cpp:115-161] | 已确认源码缺陷（静态） |
| `AccessorHint = 0xDEADBEEF` 是“未填充”哨兵，不是“可调用”标记 | [src/musa_shared_include/export_table.h:859-860] | 已确认 |
| 消费方靠比对哨兵判断 hook 是否可用，哨兵与 `nullptr` 都算无效 | [src/driver/muasan/tracepoints.h:11-14] | 已确认 |
| 导出表第一种分发：直接返回驱动侧静态表指针 | [src/driver/mu_entry.cpp:2164-2171] | 已确认 |
| 导出表第二种分发：`FillAccessors` 把函数地址写进调用方缓冲区 | [src/driver/mu_entry.cpp:2146-2156] | 已确认 |
| 填充循环遇源端 `0` 即停止，剩余槽位保持哨兵值，漏填与“不支持”不可区分 | [src/driver/mu_entry.cpp:2146-2156] | 已确认 |
| 未知 `Client` id 走 default 分支静默返回 MUPTI 表，源码自带 `FIXME` | [src/driver/mu_entry.cpp:2300-2304] | 已确认 |
| `MUSA_ASSERT` 在 `NDEBUG` 下展开为 `(void)(EXPR)`，断言不能当运行期保护 | [src/util/utils.h:24-29] | 已确认 |
| `MUSA_ASSERT` 的另一处典型使用是入队前的异步容量断言 | [src/musa/core/stream.cpp:1056-1066] | 已确认 |
| 内存类型校验失败也会改状态：命中条件后主动使 stream capture 失效 | [src/musa/core/memory.cpp:53-60] | 已确认（“失败仍有副作用”的例外） |
| 按线程默认流不随线程退出销毁，只在 context 销毁前统一清理 | [src/musa/core/context.cpp:1255-1262] | 已确认（源码注释 + 注册行为） |
| 池键是五字段位打包，新增字段必须同时调整掩码与位移，受 `static_assert` 约束 | [src/hal/m3d/memMgr.h:96-104] | 已确认 |
| loader 不链接 `util` 目标，只通过头文件与 `musa_loader_resolver` 关联 | [src/loader/CMakeLists.txt:60-63] | 已确认 |
| 新增导出 API 必须同步 loader manifest，同一份清单被包含三次生成表、解析与转发 | [src/loader/loader.cpp:118-124] | 已确认 |
| CSV 开关拼写不一致：README 写 `CSV_UNSUPPORTED`，CMake 判断 `CSV_UNSPPORTED` | [CMakeLists.txt:134-136] | 已确认差异；影响待验证 |
| 命令状态与节点类型枚举都要求“只在尾部追加”，因为被 MUPTI 侧复用 | [src/musa/core/command/command.h:55-70] | 已确认（源码注释） |
| Graph 节点类型是公共 ABI，共 18 个取值，core 侧另写手工的 ToString 分支 | [src/musa_shared_include/musa.h:1964-2000] | 已确认 |

## 3. 命名与范围冲突记录

| 冲突 | 两边证据 | 处理方式 |
|---|---|---|
| 项目自称 `MUSA-Runtime`，CMake 工程名与包名是 `musa_driver` / `musa-driver` | [README.md:1]、[CMakeLists.txt:35] | 以 CMake 产物为准，文档统一称 MUSA Driver，冲突保留在本表 |
| README 选项名 `CSV_UNSUPPORTED` 与 CMake 变量 `CSV_UNSPPORTED` 不一致 | [README.md:34-35]、[CMakeLists.txt:134-136] | 记入风险登记，按 README 操作会静默不生效 |
| `MUSA_BUILD_UT` 分支会 `add_subdirectory(unittest)`，但本快照无该目录 | [CMakeLists.txt:167-169] | 标注为构建缺口，测试覆盖结论一律标“未验证” |
| 单元测试与 `tests/*.cu` 在本快照均不存在，无法核对断言与用例 | —（目录列表检查，无可引文件） | 记为覆盖缺口，不推断测试行为 |

## 4. 验证缺口

- 顶层 cmake 配置是否能在目标机完成：未知；本快照缺 loader 依赖路径 [src/loader/CMakeLists.txt:14-23]。
- `muInfo` 的真实 stdout 与设备数量：未知；仅静态确认入口序列 [src/tools/muInfo.cpp:372-388]。
- `M3D` queue submit、内存池分配的硬件副作用：未展开/未验证，私有子模块不在快照。
- 导出表填充的运行期结果（哪些槽位最终仍是 `AccessorHint`）：未验证，只能静态看到停止条件 [src/driver/mu_entry.cpp:2146-2156]。
- `FetchContent` 拉取 gtest 依赖的私有仓库可达性：未验证 [CMakeLists.txt:146-154]。
- 两条已确认源码缺陷（`Search` 分支反转、`FindPool` 判空缺失）的实际运行期触发频率：静态分析已量化调用面，未在硬件复现。

## 5. 证据分级口径

| 分级 | 含义 | 本文件中的例子 |
|---|---|---|
| 已确认 | 源码行直接证明，且行号经区间核对 | `AccessorHint` 哨兵、`Search` 分支反转 |
| 已确认（静态） | 源码逻辑可判读为缺陷，但未运行复现 | `MemMgr::Free` 的断言裁剪风险 |
| 推断 | 多处证据综合，源码未直接写明 | 锁的覆盖范围、影响面计数 |
| 未验证 | 需要构建、硬件或外部仓库才能确认 | 全部运行期结论、安装与测试结果 |

## 相关文档

- [`analysis-state.md`](analysis-state.md)
- [`evidence-index.md`](evidence-index.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)
- [`../99-roadmap/rca-splaytree-memmgr.md`](../99-roadmap/rca-splaytree-memmgr.md)
- [`../90-cross-module/interface-contracts.md`](../90-cross-module/interface-contracts.md)

## 源码证据摘要

- 缺陷判断：[src/util/utilSplayTree.h:216-226]、[src/hal/m3d/memMgr.cpp:115-161]、[src/util/utils.h:24-29]
- 导出表语义：[src/musa_shared_include/export_table.h:859-860]、[src/driver/mu_entry.cpp:2146-2171]、[src/driver/mu_entry.cpp:2300-2304]
- ABI 与枚举：[src/musa_shared_include/musa.h:1964-2000]、[src/musa/core/command/command.h:55-70]
- 构建与命名：[README.md:1]、[CMakeLists.txt:35]、[CMakeLists.txt:134-136]、[src/loader/CMakeLists.txt:60-63]

## 未解决问题

1. 缺陷的运行期触发条件与频率未验证，需在授权硬件上构造用例。
2. 导出表两类分发模式的实际消费方（profiler/gdb/工具链）是否都做哨兵检查，跨仓库部分未核对。
3. 文档行号会随源码更新漂移，需要在版本变更后重跑引用与内容校验。

## 下一步阅读建议

先读 [`../99-roadmap/rca-splaytree-memmgr.md`](../99-roadmap/rca-splaytree-memmgr.md) 理解第 2 节前两行缺陷的完整链路，再回看 [`../90-cross-module/interface-contracts.md`](../90-cross-module/interface-contracts.md) 中被这些缺陷破坏的契约条目。
