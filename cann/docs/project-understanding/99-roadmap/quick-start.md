# 开发实践：快速上手

- 文档目的：解释 99-roadmap/quick-start.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：步骤依据仓库文档和静态分析；命令未在本环境执行
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 先确认环境

1. 固定四仓 commit：见 `00-overview/analysis-state.md`。
2. 准备匹配 CANN Toolkit、第三方库、Driver、Firmware 和 NPU。
3. 设置 `ASCEND_INSTALL_PATH`、`CANN_3RD_LIB_PATH`，确认动态库搜索路径。
4. 无硬件时先限制目标为源码阅读、编译或 mock/UT。

## 推荐顺序

```text
读总览 → 选择模块 → 读接口/调用链 → 改代码 → 写 UT
→ 构建 → 无设备测试 → 设备集成测试 → 记录证据
```

## 构建入口

- GE：`bash build.sh` 或 `--ge_compiler/--ge_executor/--dflow`。
- ACL：`bash build.sh --ascend_install_path=... --ascend_3rd_lib_path=...`。
- Runtime：`bash install_deps.sh`、`python3 download_3rd_party.py`、`bash build.sh`。
- Driver：按顶层 CMake 配置 `src/`，需要测试时启用 `ENABLE_TEST`。

这些是仓库文档入口，不代表当前环境已成功运行。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
