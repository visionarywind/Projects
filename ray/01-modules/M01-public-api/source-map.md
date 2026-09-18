# M01 源码地图、测试与开发

- 版本：HEAD `cfe4725d23`；Python normal task 代表链静态确认，运行和完整跨语言测试未验证。

## 源码地图

| 文件 | 角色 |
|---|---|
| `python/ray/__init__.py:80-130` | 公共导出、ObjectRef/ID |
| `python/ray/_private/worker.py:1439-1505,2881-3081` | init/get/put/remote/shutdown |
| `python/ray/remote_function.py:90-182,355-574` | RemoteFunction、选项、参数和任务提交 |
| `python/ray/actor.py` | ActorClass/ActorHandle 公共实现边界 |
| `python/ray/_raylet.pyx:3938-4032` | Python-C++ task binding |
| `src/ray/core_worker/core_worker.cc:2056-2135` | TaskSpec、pending task、异步 submit |
| `src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504` | 依赖、排队、lease、PushNormalTask |

## 测试与示例

- 测试主目录：`python/ray/tests`，以及 core/object/actor 相关 C++/Java tests。
- 真实最小示例：`release/hello_world_tests/hello_world.py:1-13`。
- 本轮未执行测试，状态为未验证。

## 修改配方

1. 改公共 API：确认 `__all__`、API policy 和向后兼容。
2. 改任务选项：同步 option utils、Python wrapper、Cython/C++ submit 协议和测试。
3. 改 ObjectRef：评估 `_raylet` ABI、序列化、Object Manager 和跨语言绑定。
4. 改提交链：同时检查 CoreWorker pending task、Raylet lease、ObjectRef/error cleanup。
5. 执行目标包测试和 `git diff --check`；命令结果如实记录。

## 风险与技术债

- wrapper 到 Cython/C++ 的绑定容易隐藏真实副作用。
- 公共 options 会影响调度、runtime_env、观测和重试。
- Python/C++/Java 协议变化的兼容面大。
- Actor、跨语言和动态故障路径需要单独验证。

## 深度审计

|分析对象|入口落地|正常路径|分支|异常|清理|数据生命周期|执行上下文|行级证据|Demo 映射|状态/缺口|
|---|---|---|---|---|---|---|---|---|---|---|
|M01|Python task 代表链已闭合|已完成静态链|client/generator/placement/get 已记录|序列化/task/get 错误已记录|worker/core cleanup 边界已记录|FunctionDescriptor/ObjectRef/temporary refs|driver/worker 已区分|binding/C++ 代表区间|D01|Actor/跨语言与动态未验证|

## 相关文档

[README](README.md) · [项目状态](../../00-overview/analysis-state.md) · [M02](../M02-core-worker/README.md)

## 源码证据摘要

见源码地图与 README。

## 未解决问题

Actor、跨语言、完整 Java/C++ API 映射和动态错误/清理仍需专题验证；normal Python task 提交链已静态确认。

## 下一步阅读建议

读 M02 CoreWorker，再根据 Actor 或跨语言需求进入对应 binding 和测试。
