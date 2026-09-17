# Demo 注册表

- 版本：HEAD `cfe4725d23`；静态候选，均未运行。

| ID | Demo | 入口 | 覆盖模块 | 选择理由 | 状态 |
|---|---|---|---|---|---|
| D01 | Hello World remote task | `release/hello_world_tests/hello_world.py:1-13` | M01-M04/M06 | 最小真实 task/ObjectRef/get 主线 | 部分完成：静态链 |
| D02 | Data representative pipeline | `python/ray/data/examples`/docs 中选择 | M01-M03/M08 | 覆盖 Dataset execution | 未开始，待选具体脚本 |
| D03 | Serve deployment | `python/ray/serve` examples/docs 中选择 | M01/M06/M07/M12 | 覆盖 Actor/HTTP/control plane | 未开始，待选具体脚本 |

## 选择规则

Demo 必须来自当前 checkout，能追踪构建、入口、参数、调用链、数据状态、异常和清理；“文档里有命令”不等于本环境运行成功。

## 相关文档
[D01](D01-hello-world/README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`release/hello_world_tests/hello_world.py:1-13`。

## 未解决问题
D02/D03 需在后续批次依据真实脚本、依赖成本和测试可执行性最终选择。

## 下一步阅读建议
先完成 D01 动态验证和完整底层链，再扩展到 Data/Serve。
