# 项目总览

- 文档目的：建立 Ray 的边界、目标和源码导航。
- 适用范围：`source/ray` 当前 checkout。
- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：已完成静态盘点；动态行为未验证。
- 最后更新：2026-09-17
- 前置阅读：[README](../README.md)。
- 后续阅读：[架构](architecture.md)。

## 结论摘要

Ray 统一提供可从单机扩展到集群的 Python/AI 分布式运行时：Core 提供 tasks、actors、objects，AI libraries 在其上提供 Data、Train、Tune、RLlib、Serve 能力。[已确认：`README.rst:17-47`]

## 项目边界

- **包含**：Driver API、Python runtime、C++ CoreWorker/Raylet/Object Manager/GCS、RPC/protobuf、AI libraries、部署/观测/测试基础设施。
- **不等同于**：单一调度器、单一数据库或某个模型训练框架；AI libraries 的业务语义与 Core 的执行语义分层。
- **外部边界**：用户进程、操作系统进程/网络、云/Kubernetes、用户函数和模型/数据源。

## 关键入口

| 入口 | 位置 | 作用 |
|---|---|---|
| `ray.init` | `python/ray/_private/worker.py:1439-1505` | 解析地址、启动或连接 runtime |
| 公共导出 | `python/ray/__init__.py:80-130` | 导出 `init/get/put/remote/shutdown` 等 |
| `ray.remote` | `python/ray/_private/worker.py:3686-3785` | 创建远程函数/Actor 包装 |
| RemoteFunction | `python/ray/remote_function.py:41`、`_remote` 约 358 起 | 形成任务提交路径 |
| CoreWorker 初始化 | `src/ray/core_worker/core_worker_process.cc:231-285` | 建立 Raylet 客户端并注册 worker |
| CoreWorker 生命周期 | `src/ray/core_worker/core_worker.cc:312-363,585-628` | 初始化、运行期状态和关闭 |

## 当前覆盖

这是大型仓库的第一轮知识库，核心入口、模块地图和代表 Demo 已建立；各模块实现卡片和行级调用链按批次补齐。任何“未验证”都不会写成运行成功。

## 相关文档
- [架构](architecture.md) · [运行时模型](runtime-model.md) · [分析状态](analysis-state.md)

## 源码证据摘要
- [`README.rst:17-47`](../../source/ray/README.rst#L17-L47)
- [`python/ray/__init__.py:80-130`](../../source/ray/python/ray/__init__.py#L80-L130)

## 未解决问题
动态进程拓扑、跨节点对象传输、故障恢复和性能数字需要运行日志或更深入的源码追踪。

## 下一步阅读建议
先读架构，再从 M01、M02 和 D01 追踪一条任务链。
