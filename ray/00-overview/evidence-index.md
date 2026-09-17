# 源码证据索引

- 版本：`source/ray` HEAD `cfe4725d23`（2026-09-17）。证据状态：首轮索引。

| 主题 | 证据 |
|---|---|
| 项目定位与 Core 抽象 | `README.rst:17-47` |
| 公共导出 | `python/ray/__init__.py:80-130` |
| 初始化与地址解析 | `python/ray/_private/worker.py:1439-1505` |
| shutdown | `python/ray/_private/worker.py:2070+` |
| get | `python/ray/_private/worker.py:2851+` |
| remote overloads | `python/ray/_private/worker.py:3686-3785` |
| RemoteFunction | `python/ray/remote_function.py:41,358+` |
| Actor | `python/ray/actor.py:944,1079,1232,1717,1883` |
| CoreWorker 进程初始化 | `src/ray/core_worker/core_worker_process.cc:231-285` |
| CoreWorker 生命周期 | `src/ray/core_worker/core_worker.cc:312-363,585-628` |
| Demo | `release/hello_world_tests/hello_world.py:1-13` |
| 构建/贡献边界 | `AGENTS.md:56-92` |
| Core internals 官方目录 | `doc/source/ray-core/internals.rst:5-23` |

## 使用规则

索引只登记已定位入口；“+”表示本轮只确认函数起点/代表区间，后续应收敛到精确结束行。旧文档和目标源码冲突时，以当前源码为准。

## 相关文档
[分析状态](analysis-state.md) · [项目总览](project-overview.md)

## 源码证据摘要
见上表。

## 未解决问题
待补充 protobuf RPC 方法、对象所有权、Raylet 调度核心函数和 AI library 代表路径。

## 下一步阅读建议
按表中顺序从公共入口下钻到 CoreWorker。
