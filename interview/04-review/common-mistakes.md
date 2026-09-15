# 常见错误与反例

## 训练

- 把 global batch、microbatch、gradient accumulation 和 DP degree 混为一谈，导致 loss scaling 或有效 token 口径错误。
- 把 TP 的参数切分误当作 DP 的副本；忽略 collective 顺序和 layout。
- 只保存模型权重而丢失 optimizer、scheduler、数据迭代器和 RNG，恢复后训练轨迹改变。
- 只看低精度吞吐，不比较 FP32/BF16 reference、溢出、梯度和最终质量。
- 将 MoE token dispatch 的容量丢弃当作普通 padding，掩盖 token 数不守恒。

## 推理

- 把 TTFT 当成纯 prefill 时间，忽略排队、tokenize 和 IPC。
- 把 KV slot/page 当成可立即复用的整数，忽略异步 stream、引用和 ownership。
- 把 prefix cache 命中等同于字符串前缀，忽略模型、tokenizer、adapter 和采样上下文。
- 把 CUDA Graph 适用性当作全局开关，忽略 shape、地址、metadata 和 fallback 清理。
- 把 speculative decoding 的接受 token 当作无条件正确，忽略 verify 语义和拒绝路径。

## 共同反例

- 进程退出或 ready 返回不代表所有 rank、请求和资源都完成。
- 文档中的命令、设计方案或 benchmark 数字不代表本工作区已执行。
- 平均吞吐提升不能证明 p99、错误率、质量和成本同时改善。
