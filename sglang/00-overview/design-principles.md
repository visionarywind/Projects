# 设计原则

本文把设计原则分为源码直接显示的事实和从事实推导出的阅读规则。

## 1. 配置先解析，再投影

`ServerArgs` 接收原始输入，resolution pipeline 计算派生决定，runtime config bags 才是业务进程读取的有效配置。**已确认**：`source-doc/02-request-flow/05-配置解析与运行时上下文.md` 记录了 `_raw_input`、declaration stash、`resolve_once` 和 `publish` 的源码链。

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
