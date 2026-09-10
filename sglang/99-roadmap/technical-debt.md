# 技术债务

## 文档债务

- M01/M02/M11-M18 尚未达到模块最低完成标准；
- HTTP/OpenAI、multimodal、disaggregation、Rust gateway 尚未与普通 Engine 达到同等深度；
- D01 已有静态端到端文章，但真实模型、GPU、权重和 stdout 未验证；
- 部分源码行号会随 checkout 变化，需要在版本更新后复核。

## 源码理解债务

- M08 allocator 的具体实现、eviction 和 HiCache transfer 待补；
- M09 各 attention backend、capture 初始化和 buffer ownership 待补；
- M10 具体 sampler kernel 和 grammar 编译缓存待补；
- M07 group 的真实 forward 消费点需继续补证；
- M06 专用 loader、weight mapping 和量化后处理需继续补证。

## 验证债务

完整 pre-commit、Mintlify、Rust workspace、GPU 模型服务、TP/PP、NCCL/RCCL/HCCL、CUDA Graph 和 registered suites 均未在本批执行。
