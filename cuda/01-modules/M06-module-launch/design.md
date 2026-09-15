# M06 设计

- 文档目的：解释 01-modules/M06-module-launch/design.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


M06 将 module/function 的持久镜像状态与每次 launch 的暂态 `CUIlaunchData` 分开。普通 launch 可以立即生成异步提交；capture 则只建立 graph node，后续实例化负责把必要 setup 保存为持久或 node data。

参数 metadata 是 compiler 与 driver ABI 的连接点；HAL 只消费已验证的通用 launch data。

## Graph 设计分层

Graph 的核心取舍是把“描述依赖”与“设备执行计划”分开。capture 期间 `CUIgraph` 只维护节点、双向边、origin stream、事件和 invalidation 状态；不直接提交 kernel。instantiate 先复制为独立 exec graph，再递归 flatten child/conditional graph、把 memset 按 HAL 能力降低为 kernel chain，并将 API 节点 ID 保留用于后续 update 映射（静态确认：[src/cui/cuigraph.c:2320-2437,2534-2627,4366-4429]）。

执行计划有三种后端：

1. **pushbuffer**：异构节点、跨 context、CNP kernel 或不支持 QMD chaining 的边必须经 host push；
2. **QMD chaining**：同 context kernel 的简单串行边可令后继 QMD 直接由前驱完成后启动；
3. **device scheduler**：条件节点、条件体和存在分叉/汇合的 kernel 节点由 device-side graph 的 successor/index/predecessor 状态推进。

`cuiGraphSetupScheduling` 先做拓扑排序，再把可串行部分着色为 scheduling group；它还按硬件 QMD cache 容量决定可缓存 QMD，并插入 scheduler node 衔接无法直接 chaining 的边。这使 graph exec 成为针对当前 context/device 能力的编译结果，而非永远固定的一种提交格式（静态确认：[src/cui/cuigraph.c:2630-2923]）。

资源生命周期也属于设计的一部分：`CUIgraphCtxData` 将每个 context 的 QMD、constant-bank、HAL staging、internal kernel streams、completion marker 和可选 scheduler backing 聚合，使多 context graph 能统一加锁、等待和销毁；launch stream 不在该集合时临时加入锁数组（静态确认：[src/cui/cuigraph.h:218-267]；[src/cui/cuigraph.c:3304-3492,4056-4162]）。

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
