# M08 - 构建、测试与 CI

- 文档目的：解释 01-modules/M08-build-tests-ci/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-build-tests-ci/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


> 状态：首版构建/测试入口分析。

## 构建编排

顶层 CMake 先加入 ggml、src、vendor，再按选项加入 common/tests/examples/tools/app/mtmd。[CMakeLists.txt:204-258] CTest helper 用 `llama_build` 创建 executable，用 `llama_test` 注册测试和 label，fixtures 用于生成 dummy models 后供 state tests 使用。[tests/CMakeLists.txt:1-117] [tests/CMakeLists.txt:201-257]

## 常用验证

```bash
cmake -B build
cmake --build build --config Release -j$(nproc)
ctest --test-dir build -L main --output-on-failure
cmake --build build --target test-backend-ops -j$(nproc)
```

## 验证记录

- `cmake -B build -DLLAMA_BUILD_UI=OFF && cmake --build build --target llama-server llama-cli -j$(nproc)`：通过（CPU/x86_64）。
- `ctest --test-dir build -N`：已执行，仅列出测试；因测试 target 尚未构建，输出包含找不到 executable 的 CTest 提示，未执行测试用例。
- `python3 -m unittest discover ./gguf-py -v`：失败，`tests.test_gguf_reader_validation` 导入时缺少 `pytest`（`ModuleNotFoundError`）。
- server pytest、UI tests、`test-backend-ops`：未执行。

上述结果更新了“命令来源已确认”和“实际执行结果”的区别。

## CI 边界

CPU CI 常关闭 `GGML_NATIVE` 并开启 fatal warnings；backend CI 需要匹配硬件；server CI 先构建 server 再运行 `tools/server/tests/tests.sh`。完整 self-hosted 命令见 `ci/README.md`，不是无依赖的单机 smoke test。

## 修改影响

CMake option 会改变 target、链接库和运行时 backend registry；测试 fixture/label 改动可能影响顺序和模型下载。不要未经批准在 `tests/*` 新增测试文件。[AGENTS.md:83-86]

---

**页尾：** 测试“已注册”不代表测试“已通过”。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M08-build-tests-ci/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M08-build-tests-ci/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
