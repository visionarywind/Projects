# 设计与文档决策记录

- 版本：`source/ray` HEAD `cfe4725d23`。

## 决策

| ID | 决策 | 理由 | 状态 |
|---|---|---|---|
| D-001 | 以 Git HEAD 而非 `_version.py` 的 `3.0.0.dev0` 作为证据锚点 | 后者是构建占位，不能代表正式发行版 | 已确认 |
| D-002 | 用 14 个职责模块而不是逐目录模块 | Ray 多目录共同实现 runtime，目录机械切分会丢失边界 | 推断并采用 |
| D-003 | Core 五模块优先于 AI libraries 深审 | 所有上层库复用任务、对象、调度和控制面 | 已采用 |
| D-004 | `release/hello_world_tests/hello_world.py` 作为首个 Demo | 最小真实脚本覆盖 decorator、submit、ObjectRef 和 get | 已确认候选、未运行 |
| D-005 | 不分析 thirdparty/生成物 | 遵循 project-prompt；仅登记其构建/ABI影响 | 已采用 |
| D-006 | 将未执行的命令统一标为未验证 | 防止将文档命令误写成成功结果 | 已采用 |

## 相关文档
[分析状态](analysis-state.md) · [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要
`python/ray/_version.py:1-2`；`README.rst:17-47`；`release/hello_world_tests/hello_world.py:1-13`。

## 未解决问题
是否需要将 Ray Client、LLM、KubeRay/release 拆成独立模块，取决于后续源码深度和用户重点。

## 下一步阅读建议
在新增模块前先更新本页和模块注册表，保持 ID 稳定。
