# D01 失败路径

- 文档目的：解释 80-demos/D01-cuda-test-memory-stream/failure-paths.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-cuda-test-memory-stream/failure-paths.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 失败点 | 源码行为 |
|---|---|
| 非零 init flags | wrapper 返回 `CUDA_ERROR_INVALID_VALUE` |
| 无效 device/context | API 参数或 handle 检查返回错误 |
| memobj 登记失败 | 释放局部 memobj 并返回原错误 |
| CNP 不支持 | 测试按 capability 调整 expected，不证明 launch 成功 |
| syscall/HAL/push 失败 | 结束 push，必要时回滚 UVM running，发送 launch end |
| GPU 未完成时 stream destroy | public handle 失效，内部 stream 留在 detached，marker 完成后回收 |

证据：[src/api/apiinit.c:19-47]、[src/api/apimem.c:100-111]、[src/cui/cuilaunch.c:779-817]、[src/cui/cuistream.c:1926-1954,2004-2088]。运行未验证。

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
