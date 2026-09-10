# 开发实践：快速上手

- 证据状态：步骤依据仓库文档和静态分析；命令未在本环境执行

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
