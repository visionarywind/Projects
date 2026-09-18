# M06 集群启动、Autoscaler、Runtime Env 与 Job

- 版本：HEAD `cfe4725d23`；静态代表链已补充；动态执行与测试仍未验证。

## 结论摘要

本模块把 `ray.init`/CLI 启动、节点扩缩容、worker runtime environment 和 job submission 归入同一“运行环境编排”边界；它们跨越 `python/ray/scripts`、`autoscaler`、`runtime_env`、`job_submission` 和 `_private/services`，不能由单一目录代表。[已确认目录；组合边界为推断]

## 入口与边界

- `ray.init`：连接已有集群或启动本地 runtime。[`worker.py:1439-1505`]
- `ray start/up`：CLI 与集群配置入口，待精确函数证据。
- runtime env：任务/Actor options 在 `RemoteFunction` 初始化/调用时解析和序列化。[`remote_function.py:137-155,319-332`]
- autoscaler/job：集群容量和远程作业控制，需独立生命周期分析。

## 深度审计

| 对象 | 入口 | 正常 | 分支 | 异常 | 清理 | 数据 | 上下文 | 行证据 | Demo | 状态 |
|---|---|---|---|---|---|---|---|---|---|---|
| M06 | init 已定位 | 部分 | 未完成 | 未完成 | 未完成 | 部分 | Driver/cluster | 部分 | D01 auto-init | 静态深化完成，动态未验证 |

## 相关文档
[运行时模型](../../00-overview/runtime-model.md) · [配置影响](../../90-cross-module/configuration-impact-map.md)

## 源码证据摘要
`python/ray/_private/worker.py:1439-1505`；`python/ray/remote_function.py:137-155,319-332`。

## 未解决问题
CLI、NodeProvider/autoscaler v2、runtime env agent、job server 和 shutdown 需逐符号追踪。

## 下一步阅读建议
从 init 的本地启动分支进入 `_private/services`，再看 autoscaler/runtime_env。
