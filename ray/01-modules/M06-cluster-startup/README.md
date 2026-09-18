# M06 集群启动、Autoscaler、Runtime Env 与 Job

- 版本：HEAD `cfe4725d23`；静态代表链已补充；动态执行与测试仍未验证。

## 结论摘要

本模块把 `ray.init`/CLI 启动、节点扩缩容、worker runtime environment 和 job submission 归入同一“运行环境编排”边界；它们跨越 `python/ray/scripts`、`autoscaler`、`runtime_env`、`job_submission` 和 `_private/services`，不能由单一目录代表。[已确认目录；组合边界为推断]

## 入口与边界

- `ray.init`：连接已有集群或启动本地 runtime。[`worker.py:1439-1505`]
- `ray start/up`：CLI 与集群配置入口，`scripts.py:start` 与 `submit` 的代表区间为 `800-1318`、`1976-2095`。
- runtime env：任务/Actor options 在 `RemoteFunction` 初始化/调用时解析和序列化。[`remote_function.py:137-155,319-332`]
- autoscaler/job：`ResourceDemandScheduler` 根据 NodeState/resource demand 计算供需；`RuntimeEnvAgent` 管理环境设置与引用，`JobManager` 管理 job 状态。[`scheduler.py:905-2204`; `runtime_env_agent.py:316-773`; `job_manager.py:57-703`]

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 数据 | 上下文 | 行证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| M06 | init、CLI、scheduler、agent、job manager 已定位 | 启动→连接→runtime env/job→task 静态链已记录 | auto-init、cache、扩缩容、重连已记录 | 连接、安装、节点、job failure 已记录 | shutdown、job stop、cache reference 已记录 | cluster/node/resource/env/job state | Driver/head/agent/autoscaler | 代表区间已列 | D01/D02 | 动态未验证 |

## 相关文档
[运行时模型](../../00-overview/runtime-model.md) · [配置影响](../../90-cross-module/configuration-impact-map.md)

## 源码证据摘要
`python/ray/_private/worker.py:1439-1505`；`python/ray/remote_function.py:137-155,319-332`。

## 未解决问题
CLI、NodeProvider/autoscaler v2、runtime env agent、job server 和 shutdown 需逐符号追踪。

## 下一步阅读建议
从 init 的本地启动分支进入 `_private/services`，再看 autoscaler/runtime_env。
