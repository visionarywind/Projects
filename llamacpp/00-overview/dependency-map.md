# 依赖地图

- 文档目的：解释 00-overview/dependency-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/llamacpp HEAD 093a2f86c（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/dependency-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 构建依赖

```text
ggml -> llama -> common -> {tests, examples, tools}
                         -> app
server-context -> llama-server-impl -> llama-server
mtmd -> llama + ggml
ui assets -> llama-server-impl
```

**已确认：** 顶层 CMake 的条件 `add_subdirectory` 给出上述大致顺序。[CMakeLists.txt:210-258] **已确认：** server impl 链接 context、UI、httplib；server executable 链接 impl。[tools/server/CMakeLists.txt:36-67] **已确认：** mtmd 链接 ggml、llama，并明确不链接 llama-common。[tools/mtmd/CMakeLists.txt:83-115] 

## 依赖类型

| 类型 | 例子 | 是否等于链接依赖 |
|---|---|---|
| 编译/链接 | CMake target、vendor、线程库 | 是 |
| 源码 | `LLM_ARCH_*`、模型类、模板文件 | 否 |
| 运行时 | backend registry、设备、HTTP queue | 否 |
| 数据 | GGUF、词表、cache、模型文件 | 否 |
| 生命周期 | backend -> model -> context -> task/result | 否 |

## 解耦意图

- M01 不知道上层请求语义，只执行 tensor graph。
- M02 对外提供稳定 C API，隐藏 model/context 实现。
- M03 把应用语义（模板、grammar、下载）留在 runtime 外。
- M04 以 task/result 隔离 HTTP 与推理线程。
- M06 不依赖 common，便于单独打包。[tools/mtmd/CMakeLists.txt:131-163]

---

**页尾：** 具体第三方版本以各自 CMake、package 和 requirements 为准，本文不重复供应商清单。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
