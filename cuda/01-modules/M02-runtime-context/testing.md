# M02 测试与验证

- 文档目的：解释 01-modules/M02-runtime-context/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-runtime-context/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 静态覆盖

- `basic_sanity.cu` 通过初始化、设备枚举和 context 操作覆盖 M02 的 globals/TLS/context 主线（静态确认：[tests/cuda_test/basic_sanity.cu:49-100,181-195]）。
- `dispatch_main.cpp` 在测试入口初始化 testutils，并在 `--listGpus` 模式下只展示设备、不执行测试（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。
- context 创建 wrapper 覆盖非法 device、NULL 输出、CUI 创建失败和 TLS push 失败后的 context 销毁（静态确认：[src/api/apictx.c:38-95]）。

## 应覆盖的错误场景

| 场景 | 预期观察 |
|---|---|
| 未初始化时调用需要 context 的 API | `cuiInitCheckEx` 返回初始化错误 |
| TLS 没有 current context | context 检查失败或按 API 契约处理 |
| context sticky error | 后续检查观察并返回 sticky error |
| context 创建后 TLS push 失败 | 新 context 被销毁，不留下半绑定状态 |
| 初始化中途 manager 创建失败 | 按反向顺序释放已初始化 manager |

## 验证状态

本工作区未运行 nvmake、`cuda_test`、GPU、debugger 或任何外部 driver 命令；上述内容是源码静态覆盖，不是执行结果。

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
