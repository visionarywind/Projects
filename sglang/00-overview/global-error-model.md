# 全局错误模型

SGLang 的错误不是单一 `raise` 路径，而是按边界分层。

## 1. 输入边界

HTTP schema、参数规范化、tokenizer、上下文长度和 sampling 参数校验可以在请求进入 Scheduler 前失败。此类错误通常对应请求级 HTTP 错误，不应导致 GPU worker 退出。

## 2. 调度边界

KV 不足、请求 abort、停止条件和 batch admission 失败属于请求/批次状态变化。Scheduler 需要释放或回收请求资源，并把结束原因传给输出层。

## 3. 设备/模型边界

模型架构不支持、checkpoint 缺失、dtype/quantization 不兼容、GPU OOM、kernel 错误和 collective hang 属于 worker 初始化或执行错误。它们可能无法只对一个请求隔离。

## 4. 进程边界

Scheduler exception 会记录 traceback，并通过父进程信号/监控机制传播；父进程清理进程树。startup pipe 还承担“初始化成功或失败”的控制信号，ZMQ 长期消息通道承担正常结果/请求流。

## 5. 外部服务边界

HF/ModelScope 下载、远端权重 instance、NCCL/RDMA、Ray、Rust gateway 和对象存储都有各自的超时/连接错误。文档只能在源码确实实现 retry、timeout 或 fallback 时声称存在该行为。

## 6. 阅读和修改规则

修改错误处理前必须回答：错误发生在哪个边界、资源由谁拥有、是否能继续服务、客户端收到什么、其他 rank 是否必须同步。特别是 distributed model loading：单个 rank 先失败而其他 rank 继续等待可能表现为 OOM、timeout 或 monitored barrier 错误，不能只在失败 rank 加日志。
