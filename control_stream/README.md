# MUSA Graph 控制流分析

本目录沉淀 MUSA Driver Graph 的控制流实现分析，重点回答 Graph 如何表达和执行：

- 普通 DAG 依赖如何建立、拓扑化并转换为硬件同步；
- stream capture 如何生成 Graph；
- GraphExec 如何把节点组织成 submission cluster；
- child graph 如何递归执行；
- conditional graph 的 IF、WHILE、SWITCH 如何在运行时选择或重复提交子图；
- 条件变量如何在 GPU 与 Driver 之间传递；
- 最终命令如何进入 UniversalManager 和 HAL。

## 文档

- [MUSA Graph 控制流实现分析](./musa-graph-control-flow-analysis.md)

## 分析边界

分析基于远程源码仓库：

```text
shanfeng@10.20.34.9:/home/shanfeng/workspace/linux-ddk/musa
```

本地文档只读分析远程源码，不修改远程工作树，不包含构建或硬件测试结果。远程工作树在调研时已有未提交改动，文档对这些改动与 Graph 核心逻辑作了区分。

WHILE 的首次执行语义、GraphExec2 对 conditional graph 的完整支持，以及 HAL 更底层的 cache/coherence 细节，仍应通过对应测试或硬件环境进一步确认。
