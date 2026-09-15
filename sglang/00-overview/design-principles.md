# 设计原则

- 文档目的：解释 00-overview/design-principles.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/design-principles.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


本文把设计原则分为源码直接显示的事实和从事实推导出的阅读规则。

## 1. 配置先解析，再投影

`ServerArgs` 接收原始输入，resolution pipeline 计算派生决定，runtime config bags 才是业务进程读取的有效配置。**已确认**：`02-request-flow/05-配置解析与运行时上下文.md` 记录了 `_raw_input`、declaration stash、`resolve_once` 和 `publish` 的源码链。

阅读规则：看到一个配置字段时，先找它的定义、解析 handler 和 runtime bag 消费者，不要假设 CLI 字段就是运行时最终值。

## 2. 共享资源由 Scheduler 编排

并发请求共享 GPU、KV cache、通信组和输出通道。Scheduler 用批次和 admission 把它们组织起来；ModelRunner 不负责决定哪些请求进入下一轮。**已确认**：`Scheduler` 的主循环先 ingest/select，再运行 batch，再处理结果。

## 3. 热路径减少动态决策

配置驱动的选项在启动期解析，执行期使用已建立的 backend、buffer 和 batch metadata。**推断**：这同时减少 Python 分支和设备同步；具体收益指标未知，不能在此文档编造 benchmark 数字。

## 4. 通过小协议替换实现

模型 loader、attention backend、sampling、量化方法和部署后端以共同接口接入。主线代码选择实现，细节在实现类中完成。阅读时必须继续追踪到真正写参数、访问 checkpoint、发 collective 或写输出的位置。

## 5. 进程边界显式化

ZMQ 用于长期消息流，startup pipe 用于初始化 ready/错误传播，配置通过 runtime context 发布。跨边界对象必须可序列化或使用 shared memory/专门包装。

## 6. 快照必须保持稳定

overlap scheduler 会保存批次引用；因此 `ScheduleBatch` 快照相关字段不能随意原地变更。仓库规则明确要求用重新绑定替代对快照字段的 in-place mutation。

## 7. 设备并行是多种正交拓扑

TP、PP、attention DP/CP/DCP、MoE DP/EP/TP 由不同 rank group 表达。不能把“global rank”“GPU id”和某一种并行 rank 当作同义词。

## 8. 证据优先

文档结论必须能回到当前源码路径和符号。未执行命令标记为“未验证”；不使用远程 release、旧文档或函数名猜测替代本地实现。

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
