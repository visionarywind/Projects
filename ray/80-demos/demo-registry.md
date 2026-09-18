# Demo 注册表

- 版本：HEAD `cfe4725d23`；三个 Demo 均已静态选择和追踪，均未运行。

| ID | Demo | 入口 | 覆盖模块 | 选择理由 | 状态 |
|---|---|---|---|---|---|
| D01 | Hello World remote task | `release/hello_world_tests/hello_world.py:1-13` | M01-M05/M06 | 最小真实 task/ObjectRef/get 主线，并已追到 Cython/C++ normal task 提交 | 静态完成，未运行 |
| D02 | Data + Tune resource allocation | `doc/source/data/doc_code/key_concepts.py:4-24` | M01-M04/M06/M08/M10 | 无模型依赖，覆盖 Dataset lazy execution、Tune trial 和资源留空 | 静态完成，未运行 |
| D03 | Serve DeploymentHandle hello | `doc/source/serve/doc_code/key_concepts.py:2-20` | M01/M02/M06/M12 | 无模型依赖，覆盖 deployment bind、controller、replica、router、handle | 静态完成，未运行 |

## 选择规则

Demo 必须来自当前 checkout，能追踪构建、入口、参数、调用链、数据状态、异常和清理；“文档里有命令”不等于本环境运行成功。

## 相关文档
[D01](D01-hello-world/README.md) · [D02](D02-data-tune-resource/README.md) · [D03](D03-serve-handle/README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`release/hello_world_tests/hello_world.py:1-13`；`doc/source/data/doc_code/key_concepts.py:4-24`；`doc/source/serve/doc_code/key_concepts.py:2-20`。

## 未解决问题
需要在合适的 upstream Python 环境中运行三个脚本，记录真实 stdout、进程、日志、异常和 shutdown；当前不宣称动态成功。

## 下一步阅读建议
先读 D01 的 normal task 链，再读 D02 的资源/执行边界和 D03 的 Serve state machine。
