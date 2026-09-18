# D03 Ray Serve DeploymentHandle

- 来源：`source/ray/doc/source/serve/doc_code/key_concepts.py:2-20`。
- 证据状态：脚本、Serve API 和 handle 调用已静态确认；未运行。

## 选择理由

这是当前 checkout 内不依赖模型下载的最小 Serve 示例：定义一个部署、`serve.run` 返回 `DeploymentHandle`，调用 `handle.remote().result()` 取得字符串。它覆盖 M12 的 controller/deployment/replica/handle 链，同时能连接 M01/M02 的 Ray actor/object 语义。

## 源码步骤

1. `@serve.deployment` 把 `MyFirstDeployment` 注册为部署定义；`bind("Hello world!")` 形成 application DAG。[`key_concepts.py:3-17`]
2. `serve.run(my_first_deployment)` 进入 `python/ray/serve/api.py:952-1010`，通过 controller client 提交 application；controller 的 `deploy_applications` 在 `python/ray/serve/_private/controller.py:1139-1196` 更新 deployment state。[静态确认]
3. controller 创建/更新 replica actors；`DeploymentState` 管理 replica 状态，router 将 handle 请求选路到可用 replica。[`deployment_state.py:2952-5924`; `router.py:538-1660`]
4. `handle.remote()` 生成异步请求，replica 调用 `__call__` 返回 `Hello world!`；`.result()` 等待 handle response。[脚本确认入口，跨进程动态行为未验证]
5. 示例没有显式调用 `serve.shutdown()`；Serve 资源的清理取决于进程/应用生命周期，未验证。

## 数据与状态

| 数据/状态 | 作用 |
|---|---|
| Deployment/Application | controller 的声明和目标状态 |
| Replica actor | 持有 `msg`，执行用户 callable |
| DeploymentHandle | driver 到 Serve deployment 的异步请求句柄 |
| request/response | router、replica、ObjectRef/异步结果之间的边界 |

## 分支、异常、清理

- replica 启动或健康检查失败时，deployment state 可重试或标记不健康；具体选择取决于配置和状态机。
- 用户 callable 抛错时，异常沿 handle response 返回；本示例未显式捕获。
- 多副本、滚动更新、autoscaling 和 HTTP proxy 不在此最小脚本中。
- 动态启动、实际日志、端口占用、replica 创建顺序和 shutdown 未验证。

## 源码映射

- M12 API：`python/ray/serve/api.py:82-166,902-1010`。
- M12 control/data plane：`controller.py:134-1988`、`deployment_state.py:2952-5924`、`router.py:538-1660`、`replica.py:1080-3637`。
- M01/M02：Serve replica/handle 底层仍使用 Ray actor/task/object 运行时；具体动态调用链未执行。

## 运行状态

没有安装 Serve 依赖、启动 HTTP server 或执行脚本；任何返回值、进程拓扑和清理日志均保持未验证。
