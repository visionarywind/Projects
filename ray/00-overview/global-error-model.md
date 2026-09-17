# 全局错误模型

- 文档目的：建立错误来源、边界和恢复的统一地图。
- 版本：HEAD `cfe4725d23`；静态部分确认，运行时恢复未验证。
- 前置：[运行时模型](runtime-model.md)。后续：[错误边界](../90-cross-module/error-boundaries.md)。

## 结论摘要

Ray 的错误至少跨越输入/序列化、提交/资源、用户代码、对象可用性、worker/node 故障、控制面和部署环境七类边界。公共 `ray.get` 可能把远端执行异常或对象不可用表现给 Driver，但重试、重建和清理由 Core/调度/对象层共同决定。[推断；`worker.py` get 入口、Core 目录和官方 fault-tolerance 文档存在]

| 边界 | 典型错误 | 传播方向 | 当前证据 |
|---|---|---|---|
| API输入 | 参数、资源选项非法 | API→调用方异常 | 部分确认 |
| 序列化 | closure/参数不能序列化 | wrapper/CoreWorker→Driver | 待深审 |
| 调度 | 资源不足、节点不可用 | Raylet→task status→get | 待深审 |
| 用户代码 | task/Actor exception | Worker→ObjectRef/error→get | 公共模型确认 |
| 对象 | lost/spill/pull failure | Object Manager→CoreWorker→get | 待深审 |
| 控制面 | GCS/RPC timeout/disconnect | RPC client→runtime | 待深审 |
| 环境 | dependency/runtime_env/image failure | startup/worker→job | 待深审 |

## 恢复原则

只把“有 fault-tolerance 文档/配置”写成能力存在，不把它等同于每条路径都自动重试。具体 retry policy、超时、幂等性和清理必须由对应模块源码和测试证明。

## 相关文档
[错误边界](../90-cross-module/error-boundaries.md) · [风险登记](../99-roadmap/risk-register.md)

## 源码证据摘要
`python/ray/_private/worker.py:2851+`；`doc/source/ray-core/internals/rpc-fault-tolerance.rst`；`doc/source/ray-core/fault_tolerance/`；`src/ray/rpc/`。

## 未解决问题
需补充各异常类型的真实类名、重试计数、清理动作和测试用例。

## 下一步阅读建议
从 D01 故意制造 task exception，再回读 M02/M03。
