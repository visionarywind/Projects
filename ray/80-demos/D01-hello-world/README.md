# D01 Ray Hello World

- 文档目的：用仓库内真实最小 Demo 贯穿 `@ray.remote`、任务提交、ObjectRef 和 `ray.get`。
- 适用范围：`source/ray/release/hello_world_tests/hello_world.py`。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：脚本和静态执行链已确认；未运行，输出和进程行为未验证。
- 前置阅读：[M01 公共 API](../../01-modules/M01-public-api/README.md)。后续：[执行轨迹](execution-trace.md)。

## 结论摘要

脚本定义 `hello_world` 为远程函数，`main` 调用 `hello_world.remote()` 获得 ObjectRef，再用 `ray.get` 取回字符串并打印；源码真实入口只有 13 行。[已确认：`release/hello_world_tests/hello_world.py:1-13`]

## 源码步骤

| 步骤 | 源码 | 结果 | 模块 |
|---|---|---|---|
| D01-01 | 1 | import ray，加载公共导出 | M01 |
| D01-02 | 3-5 | `@ray.remote` 创建 RemoteFunction | M01 |
| D01-03 | 8-9 | 调用 `.remote()` 提交任务，得到 ref | M01/M02/M04 |
| D01-04 | 9 | `ray.get(ref)` 获取并打印结果 | M01/M03 |
| D01-05 | 12-13 | 作为脚本入口运行 `main` | M01/M06 |

## 模块映射

```text
D01-02 → ray.remote / RemoteFunction
D01-03 → RemoteFunction._remote / CoreWorker.submit_task / Raylet（后两者静态链待补）
D01-04 → worker.get / get_objects / Object Manager
```

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| D01 hello world | 已完成脚本入口 | 静态完成 | 未完成 | 未完成 | 未完成 | 部分完成 | 未运行 | 已完成 1-13 | 映射 M01-M04 | 部分完成：运行和底层链未验证 |

## 相关文档
[构建运行](build-and-run.md) · [源码轨迹](execution-trace.md) · [失败路径](failure-paths.md) · [修改练习](modification-exercises.md)

## 源码证据摘要
`source/ray/release/hello_world_tests/hello_world.py:1-13`。

## 未解决问题
本环境是否已有 Ray 构建产物、最小 runtime 是否可启动、输出和 shutdown 日志是什么，均未验证。

## 下一步阅读建议
先读 execution-trace，再按 M01→M02→M03/M04 反查。
