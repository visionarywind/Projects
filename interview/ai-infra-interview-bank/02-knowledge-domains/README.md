# AI Infra 知识域详解

12 个知识域覆盖 AI 训练/推理基础设施全栈。每篇包含概念定义、机制拆解、关键参数/公式、常见误区与本仓库证据引用，并与 `01-question-bank/` 中对应题目互链。

| 域 | 范围 | 题目数 |
|---|---|---|
| [K01](K01.md) 计算硬件与加速器 | SM/warp 执行模型、roofline、HBM、Tensor Core、拓扑 | 7 |
| [K02](K02.md) 驱动、运行时与设备管理 | context、stream、event、异步提交、kernel launch 链、Graph | 8 |
| [K03](K03.md) 显存、主机内存与存储 | 显存池化、生命周期、pinned/pageable、UVM、KV cache | 8 |
| [K04](K04.md) PCIe、NVLink、RDMA 与网络 | 互联拓扑、带宽层级、集合通信路径、GPUDirect | 6 |
| [K05](K05.md) 容器、Kubernetes 与节点 | GPU 容器化组件栈、device-plugin、MPS/MIG 隔离 | 5 |
| [K06](K06.md) 调度、资源隔离与多租户 | Ray lease/spillback、continuous batching、gang scheduling、公平性 | 6 |
| [K07](K07.md) 数据管道与数据血缘 | 数据加载、prefetch、shuffle、样本管道瓶颈 | 5 |
| [K08](K08.md) 分布式训练与容错 | DP/TP/PP/SP/EP、1F1B、checkpoint、worker 容错、故障域 | 8 |
| [K09](K09.md) 推理服务与弹性伸缩 | prefill/decode、KV cache、continuous batching、PD 分离、autoscaling | 7 |
| [K10](K10.md) 性能、容量与可观测性 | profiling 工具链、MFU、尾延迟、benchmark 计时、容量规划 | 8 |
| [K11](K11.md) 可靠性、安全与供应链 | 错误边界、故障模型、隔离恢复、资源安全 | 4 |
| [K12](K12.md) MLOps、发布与平台演进 | 灰度发布、版本兼容、回滚、平台化演进 | 4 |

总计 76 道题目（部分题目跨域交叉引用）。

## 阅读路径

建议按依赖顺序：K01 → K02 → K03 → K04 → K05 → K06 → K07 → K08 → K09 → K10 → K11 → K12。K01/K02/K03 是硬件与运行时基座，K08/K09 是训练与推理两个主场景，K10/K11/K12 横跨两者。

## 证据说明

每篇引用的 `path:line` 来自本仓库源码分析文档（`cuda/`、`musa/`、`cann/`、`megatron-lm/`、`sglang/`、`ray/`、`insights/` 等）。薄弱域（K05 K8s 版本行为、K04 RoCE 细节）标 `[待验证]`，不编造。
