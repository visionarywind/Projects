# 技术债

- 文档目的：登记**结构性、维护性**欠账（与 [`risk-register.md`](risk-register.md) 的运行期风险互补），每条落到源码证据并给出偿还成本。
- 适用范围：`source/musa` 快照（5.3.0 develop）的构建、生成物、导出契约、测试设施与文档自身。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：已确认（静态核对）；偿还后的效果均**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`risk-register.md`](risk-register.md)
- 后续阅读：[`next-steps.md`](next-steps.md)

## 结论摘要

最重的三笔债都指向同一件事：**关键生成物与关键校验不在本仓库内**。wrapper/参数结构由快照外脚本生成，导出符号由清单单向生成而无反向 diff，测试目录被构建系统引用却不存在。这三笔不还，任何"新增 API / 修改 ABI / 增加测试"的动作都无法在本仓库闭环验证。

---

## 债务清单

| # | 债务 | 性质 | 源码证据 | 影响 | 偿还成本 | 优先级 |
|---|---|---|---|---|---|---|
| D-1 | wrapper 与 `generated_*_meta.h` 的生成脚本不在快照 | 生成链断裂 | [src/driver/mu_wrappers_generated.cpp:1-11]、[src/musa_shared_include/generated_musa_meta.h:1-1] | 新增/改签名的 API 无法重新生成包装，手工改会被 clean build 覆盖 | 低（取回脚本）/ 高（否则无解） | P0 |
| D-2 | 导出符号只由清单单向生成，无反向校验 | 校验缺失 | [src/loader/loader_api_list.inc:1-6]、[cmake/export_symbols.cmake:14-25] | 清单漏一条 = 运行期少一个导出符号，配置期不报错 | 中 | P0 |
| D-3 | `MUSA_BUILD_UT` 指向不存在的 `unittest/` | 测试设施缺失 | [CMakeLists.txt:167-169]、[CMakeLists.txt:146-154] | 打开开关即配置失败；单测层整体空白 | 中（需补目录或改开关） | P0 |
| D-4 | 三个源码目录用 glob/`aux_source_directory` 隐式收集源文件 | 构建不显式 | [src/driver/CMakeLists.txt:3-9]、[src/util/CMakeLists.txt:1-4]、[src/tools/CMakeLists.txt:10-15] | 误放的 `.cpp` 静默参与编译；新增文件不一定触发 CMake 重配 | 低 | P1 |
| D-5 | `export_table.h` 用同一占位值表达所有"未填充" | 语义不可区分 | [src/musa_shared_include/export_table.h:859-866]、[src/driver/mu_entry.cpp:2146-2195] | 调用方无法区分"不支持"与"漏填"，误调直接跳 `0xDEADBEEF` | 中 | P1 |
| D-6 | 未知 Client 分支静默回落到 `muptiDriverTable` | 兼容妥协 | [src/driver/mu_entry.cpp:2296-2303] | 新接入方拿错表，错误被吞掉 | 低 | P1 |
| D-7 | `SplayTree` 析构为空，节点靠调用方手工排空 | 资源语义 | [src/util/utilSplayTree.h:61-64] | 新使用者默认会泄漏全部节点 | 低 | P1 |
| D-8 | `perfStatistics.h` 的 `TimeStatistics` 未被使用 | 死代码 | [src/util/perfStatistics.h:9-28] | 维护噪音；`thread_local` 起点共享问题已登记为风险 | 低 | P2 |
| D-9 | `src/logging.h`/`utils.h` 的 `ToString` 等处于全局命名空间 | 命名污染 | [src/musa/core/logging.h:11-18] | 与第三方同名符号冲突风险 | 中（波及面广） | P2 |
| D-10 | CSV 开关声明与判断拼写不一致 | 配置失效 | [CMakeLists.txt:25-25]、[CMakeLists.txt:134-136] | 按文档打开的选项实际不生效 | 低 | P2 |
| D-11 | loader 仅限 Windows/WSL 且依赖快照外 `SHARED_INCLUDE_PATH` | 构建不可复现 | [src/loader/CMakeLists.txt:5-7]、[src/loader/CMakeLists.txt:15-25] | Linux 主线路径下 loader 结论只能静态推导 | 中 | P1 |
| D-12 | 知识库自身 85 篇文档零源码引用、154 篇引用不足 | 文档债 | 统计见 [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md) | 模板页能通过校验但不承载证据 | 中 | P1 |

---

## 说明与偿还建议

### D-1 生成链断裂（P0）

`mu_wrappers_generated.cpp` 与 `generated_musa_meta.h` 都自称生成物，改动会在下次 clean build 丢失 [src/musa_shared_include/generated_musa_meta.h:1-1]，而生成输入（`muapi.h` 原型 + 参数结构模板）分散在 `src/driver` 与 `src/musa_shared_include` [src/driver/mu_wrappers_generated.cpp:1-11]。偿还方式：把生成脚本纳入仓库或在 README 里写清获取路径；在此之前禁止直接编辑这两个文件——[`feature-development-recipes.md`](feature-development-recipes.md) 的配方 A4 也因此无法闭环。

### D-2 导出契约无反向校验（P0）

`MUSA_LOADER_API_MANIFEST` 只被用来生成 `.def`/`.ver` [cmake/export_symbols.cmake:14-25]，没有任何步骤把 `libmusa.so` 的实际导出与 [src/loader/loader_api_list.inc:1-6] 做 diff。偿还方式：构建后跑一次 `nm -D` 对比，差异非零即失败。这是把"漏登记不会报错"变成"漏登记必报错"的最小改造。

### D-3 测试设施（P0）

`MUSA_BUILD_UT` 开启时先 FetchContent 拉 googletest [CMakeLists.txt:146-154]，再 `add_subdirectory(unittest)` [CMakeLists.txt:167-169]，而本快照没有 `unittest/` 目录；仓库内也没有任何 `*_test.cpp`（`src/hal/m3d/m3d` 私有子模块内的 `add_test` 属于另一套工程）。偿还方式：确认测试代码是否在别处；否则补一个最小 `unittest/CMakeLists.txt` 并把 [`testing-recipes.md`](testing-recipes.md) 的分层用例补进去。**本项未验证。**

### D-4 构建隐式收集（P1）

三个目录各有自己的收集方式 [src/driver/CMakeLists.txt:3-9]、[src/util/CMakeLists.txt:1-4]、[src/tools/CMakeLists.txt:10-15]。工具目录后果最重：每个 `.cpp` 直接变成一个可执行文件。偿还方式：改显式源列表 + `CONFIGURE_DEPENDS`，并约定 `src/tools` 只放工具入口。

### D-5/D-6 导出表语义（P1）

`AccessorHint` 是唯一的"未填充"标记 [src/musa_shared_include/export_table.h:859-866]，填充由 `muapiGetExportTable` 内的 `FillAccessors` 逐槽完成 [src/driver/mu_entry.cpp:2146-2195]；未知 `Client` 还会静默回落 [src/driver/mu_entry.cpp:2296-2303]。偿还方式：给 accessor 结构加"已填充"位或在填充后写入真实空值，并把回落分支改成显式错误。

### D-7/D-8/D-9/D-10 小额但长期

这四项都是"改动面小、拖久了变贵"的类型：析构语义 [src/util/utilSplayTree.h:61-64]、死代码 [src/util/perfStatistics.h:9-28]、全局命名 [src/musa/core/logging.h:11-18]、开关拼写 [CMakeLists.txt:134-136]。建议趁触碰对应文件时顺手偿还，单独开 PR 的收益低于噪音。

### D-11 构建不可复现（P1）

loader 子目录在非 Windows/WSL 环境直接 `FATAL_ERROR` [src/loader/CMakeLists.txt:5-7]，且要求外部 `SHARED_INCLUDE_PATH` [src/loader/CMakeLists.txt:15-25]。这解释了为什么 M11 的所有结论只能是静态推导。偿还方式：把依赖清单写进构建文档，或提供 stub 目录用于配置期校验。

### D-12 文档债（P1）

[`../00-overview/analysis-state.md`](../00-overview/analysis-state.md) 统计显示 203 篇文档中 85 篇零引用、69 篇仅 1–4 处引用。校验器只保证"引用不错"，不保证"有引用"。偿还方式：按 `§12` 的优先级逐篇填充，填充后重跑 `verify_kb.py` 与 `verify_content.py`。

---

## 偿还顺序建议

1. **先还 D-1、D-3**：没有生成脚本和测试目录，后续所有改动都无法验证。
2. **再还 D-2**：一个构建后脚本即可把导出契约变成硬约束。
3. **D-4、D-11 随构建系统改动一起还**。
4. **D-5/D-6 跟着 accessor 使用方一起还**，单改定义收益有限。
5. **D-7–D-10 顺手还**。

所有偿还动作的效果在拿到构建环境前都标记为**静态分析，未验证**。

## 相关文档

- [`risk-register.md`](risk-register.md)
- [`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md)
- [`testing-recipes.md`](testing-recipes.md)
- [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)

## 源码证据摘要

- 生成物：[src/driver/mu_wrappers_generated.cpp:1-11]、[src/musa_shared_include/generated_musa_meta.h:1-1]
- 导出契约：[src/loader/loader_api_list.inc:1-6]、[cmake/export_symbols.cmake:14-25]、[src/driver/mu_entry.cpp:2146-2195]
- 构建与测试：[CMakeLists.txt:146-154]、[CMakeLists.txt:167-169]、[src/driver/CMakeLists.txt:3-9]、[src/tools/CMakeLists.txt:10-15]、[src/loader/CMakeLists.txt:15-25]
- 代码卫生：[src/util/utilSplayTree.h:61-64]、[src/util/perfStatistics.h:9-28]、[src/musa/core/logging.h:11-18]、[CMakeLists.txt:134-136]

## 未解决问题

1. 生成脚本的真实位置与输入格式未知。
2. 测试代码是否存在于其他仓库未知。
3. 上述偿还在本快照均未执行，效果未验证。

## 下一步阅读建议

先按 [`next-steps.md`](next-steps.md) 的 P0 推进 D-1、D-3，再回到本文件逐项销账。
