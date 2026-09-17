# 设计原则

- 目的：从源码结构提炼 Ray 的核心设计约束；设计意图不足时标为推断。
- 版本：HEAD `cfe4725d23`。证据状态：部分确认。
- 前置：[架构](architecture.md)。后续：[依赖地图](dependency-map.md)。

## 结论摘要

1. **统一抽象而非统一实现**：tasks、actors、objects 是公共抽象，Data/Train/Tune/RLlib/Serve 在其上建立不同语义。[已确认：`README.rst:24-36`]
2. **语言入口与 C++ runtime 分层**：Python 导出和 wrapper 与 CoreWorker/Raylet/GCS 分离，便于多语言共享 runtime。[已确认：`AGENTS.md:56-62`]
3. **控制面与对象数据面分离**：GCS/RPC/Raylet 处理控制与调度，对象管理器处理值与可用性。[推断，待逐 RPC 验证]
4. **引用和不可变对象**：公共对象抽象使用 ObjectRef 指代可跨集群访问的不可变值。[已确认：`README.rst:32-36`]
5. **本地到集群一致入口**：`ray.init` 可连接已有集群或启动本地实例，API 尽量保持一致。[已确认：`python/ray/_private/worker.py:1439-1505`]
6. **故障与观测是一等边界**：仓库有独立 dashboard、observability、stats、认证及 fault-tolerance 文档/测试；不把错误处理视为 wrapper 的附属功能。[已确认]

## 取舍

- 通用任务图与 Actor 模型提高表达力，但进程、RPC、序列化和对象生命周期增加调试成本。
- 多语言和多部署模式带来稳定接口要求，也扩大构建与兼容矩阵。
- 分布式引用弱化数据位置，但性能和故障定位必须追踪 ownership、locality、spill 和重建路径。

## 修改影响

公共 ID/ObjectRef、任务描述、protobuf、资源语义和 worker 生命周期属于高影响契约；修改时需同时评估 Python/C++/Java、序列化、调度、容错和测试。

## 相关文档
[决策记录](decision-log.md) · [修改影响](../90-cross-module/change-impact-map.md)

## 源码证据摘要
`README.rst:17-47`；`python/ray/__init__.py:80-130`；`src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
“为什么”层面的历史动机需结合白皮书和提交历史，不应仅由目录推断。

## 下一步阅读建议
阅读运行时模型，再以 D01 对照这些原则。
