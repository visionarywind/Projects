# 调试指南

- 文档目的：解释 99-roadmap/debugging-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/debugging-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 构建和单测

用 Debug 构建：

```bash
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j$(nproc)
ctest --test-dir build-debug -L main --output-on-failure
```

测试 backend/operator 改动时单独构建并执行 `test-backend-ops`；当前均未运行。[CLAUDE.md:49-65]

## Server

```bash
DEBUG=1 ./tests.sh -s -v -x
DEBUG_EXTERNAL=1 ./tests.sh unit/test_chat_completion.py -v -x
```

前者显示步骤和 server verbose，后者让外部 debugger 启动 server；变量和行为由测试 README 确认。[tools/server/tests/README.md:42-46] [tools/server/tests/README.md:68-95]

## 观察边界

- 请求错误：看 `ex_wrapper` 的 400/500 映射。
- decode 错误：看 ret、KV 清理和 batch 缩小分支。
- context 初始化：看 flash-attn/cache 类型约束。
- backend：看 registry/device enumeration 和 scheduler status。

未提供未经执行的断点地址、性能数字或硬件结论。

---

**页尾：** 调试输出可能含 prompt/model 信息，分享日志前由操作者自行审查。

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
