# D01 CUDA 测试主线

- 文档目的：解释 80-demos/D01-cuda-test-memory-stream/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-cuda-test-memory-stream/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 目的

以 `basic_sanity.cu` 串起 Runtime 设置、内部 context/function 查找、kernel launch、CNP expected、stream push 检查和结果校验。

## 静态步骤

1. 枚举可见 GPU 并建立 context（静态确认：[tests/cuda_test/basic_sanity.cu:49-100]）。
2. 从 module/function 结构找到 `gpuIncrement`，检查 launchable 属性（静态确认：[tests/cuda_test/basic_sanity.cu:127-163]）。
3. 使用 `<<<1,1>>>` 发起 kernel，按设备 CNP capability 计算 expected，并检查 launch/push 状态（静态确认：[tests/cuda_test/basic_sanity.cu:169-237]）。
4. 将结果与同步/资源状态关联；任何实际结果均需在有 GPU 的测试日志中确认。

## 状态

源码路径已静态阅读；nvcc、nvmake、cuda_test 和 GPU 均未运行。

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D01-cuda-test-memory-stream/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `80-demos/D01-cuda-test-memory-stream` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
