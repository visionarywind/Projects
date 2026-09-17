# M01 源码地图、测试与开发

- 版本：HEAD `cfe4725d23`；静态分析。

## 源码地图

| 文件 | 角色 |
|---|---|
| `python/ray/__init__.py` | 公共导出、ObjectRef/ID |
| `python/ray/_private/worker.py` | init/get/put/remote/shutdown |
| `python/ray/remote_function.py` | RemoteFunction 与任务提交参数整理 |
| `python/ray/actor.py` | ActorClass/ActorHandle |
| `python/ray/_raylet` | Python-C++ runtime 扩展边界 |

## 测试与示例

- 测试主目录：`python/ray/tests`，具体 task/object/actor 测试需按目标符号检索。
- 真实最小示例：`release/hello_world_tests/hello_world.py:1-13`。
- 本轮未执行测试，状态为未验证。

## 修改配方

1. 改公共 API：先确认 `__all__`/导出和 API policy。
2. 改任务选项：同步 `ray_option_utils`、Python wrapper、C++ submit 协议和测试。
3. 改 ObjectRef：同时评估 `_raylet` ABI、序列化、Object Manager 和跨语言绑定。
4. 每次修改执行目标包测试和 `git diff --check`；命令结果如实记录。

## 风险与技术债

- wrapper 到 Cython/C++ 的绑定容易隐藏真实副作用。
- 公共 options 会影响调度、runtime_env、观测和重试。
- Python/C++/Java 协议变化的兼容面大。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M01 | 已完成 Python task 代表链 | 已完成 | 部分完成 | 部分完成 | 未完成 | 部分完成 | 部分完成 | 已完成代表区间 | D01 | 部分完成 |

## 相关文档
[README](README.md) · [项目状态](../../00-overview/analysis-state.md)

## 源码证据摘要
见源码地图与 README。

## 未解决问题
Actor、跨语言、C++ binding 和完整测试映射待补。

## 下一步阅读建议
读 M02 CoreWorker。
