# Ray 源码级项目理解知识库

- 文档目的：从 `source/ray` 当前源码建立可导航、可追溯的 Ray 架构、实现、调用链和开发知识库。
- 适用范围：Ray 单仓库中的 Python API/AI libraries、C++ Core、Java/C++ bindings、构建测试与部署脚本；第三方库、生成物和缓存不展开。
- 对应源码版本：`source/ray` HEAD `cfe4725d23`（2026-09-17，提交 `[Data] Make distribution metric export directly testable (#65750)`）。
- 证据状态：第一轮静态分析；关键入口与代表性链路已确认，构建、运行、GPU、多节点和网络行为未验证。
- 最后更新：2026-09-17
- 前置阅读：无。
- 后续阅读：[分析状态](00-overview/analysis-state.md)、[项目总览](00-overview/project-overview.md)。

## 结论摘要

Ray 是以 C++ 分布式 Core runtime 为底座、由 Python/C++/Java API 和 Data、Train、Tune、RLlib、Serve 等库组成的统一 AI/Python 扩展框架。应用通过 `ray.init` 建立 driver 与本地/集群运行时的连接，通过 `@ray.remote`、`.remote()` 和 `ray.get()` 将函数、Actor 与不可变对象映射到 CoreWorker、Raylet、Object Manager、GCS/RPC 等组件；这是由入口源码和官方仓库定位直接确认的事实。[`README.rst:17-47`][`python/ray/__init__.py:80-130`][`python/ray/_private/worker.py:1439-1505`]

本知识库按“总览→模块→Demo→跨模块→实践”组织。Ray 源码规模大（静态盘点约 4,831 个 Python、491 个 C++、473 个头文件、360 个 Java、58 个 proto 文件），因此将核心运行时与主要库列为稳定模块，专用云厂商适配、第三方和生成代码以边界说明为主。覆盖是分层增量的，不把文件数量等同于实现覆盖率。

## 五分钟理解

```text
用户 Python/Java/C++ 程序
        │ ray.init / @ray.remote / .remote / ray.get
        ▼
M01 公共 API与句柄 ──序列化/提交──> M02 CoreWorker
        │                              │ Raylet RPC / IPC
        │                              ▼
        │                      M04 Raylet 调度与资源
        │                              │
        │              ┌───────────────┴───────────────┐
        ▼              ▼                               ▼
M03 Object Store  M05 GCS/RPC/发布订阅          Worker 执行用户代码
        │              │                               │
        └──────────────┴───────────────> ObjectRef / 结果
                         │
       M06 启动与运行环境 · M07 观测/安全 · M08-M13 AI库与扩展
```

- 实线表示源码中可追踪的调用、RPC 或数据关系；虚线（如未在图中绘出）表示推断或运行时选择。
- Driver、Worker、Raylet、GCS 的具体进程拓扑随 `ray.init` 地址和部署模式变化；图示是共同骨架，不是单机进程清单。[`python/ray/_private/worker.py:1439-1505`][`src/ray/core_worker/core_worker_process.cc:231-285`]

## 推荐阅读顺序

1. [项目总览](00-overview/project-overview.md) → [总体架构](00-overview/architecture.md) → [运行时模型](00-overview/runtime-model.md)。
2. [模块注册表](01-modules/module-registry.md)，再读 M01→M02→M03→M04→M05。
3. [D01 Hello World](80-demos/D01-hello-world/README.md)，把 API 入口落到执行链。
4. [端到端流程](90-cross-module/end-to-end-flows.md)、[错误边界](90-cross-module/error-boundaries.md)。
5. [快速上手](99-roadmap/quick-start.md)、[调试指南](99-roadmap/debugging-guide.md) 和目标模块的开发指南。

## 三条关键端到端流程

1. **启动**：`ray.init` 解析地址和配置，连接已有集群或启动本地 Ray 进程，再初始化 driver 的 CoreWorker；静态确认，真实进程启动未验证。
2. **任务**：`@ray.remote` 创建 `RemoteFunction`，`.remote()` 形成任务提交请求，经 CoreWorker/Raylet 调度，由 Worker 执行并把返回值放入对象存储，`ray.get` 解析 ObjectRef；入口与代表实现已确认，跨进程动态细节未验证。
3. **AI 库**：Data/Train/Tune/RLlib/Serve 调用 Core API 或 runtime_env/调度/观测能力；各库有独立状态和测试边界，不应把它们误当成 C++ Core 的同一实现。

## 证据与验证约定

- 代码证据使用 `[仓库相对路径:起始行-结束行]`；行号以当前 checkout 的已读取源码为准，源码变化后须复核。
- `已确认` 表示源码、构建文件、测试或命令直接证明；`推断` 表示由多个证据推导；`未知` 表示需要实验或额外源码。
- 未执行的命令一律标记“未验证”。本轮没有执行构建、pytest、Ray 集群、模型下载、GPU/多节点通信或 Demo。

## 文档索引

- 总览：[项目总览](00-overview/project-overview.md) · [架构](00-overview/architecture.md) · [设计原则](00-overview/design-principles.md) · [运行时](00-overview/runtime-model.md) · [全局数据流](00-overview/global-data-flow.md) · [依赖](00-overview/dependency-map.md) · [构建部署](00-overview/build-and-deploy.md) · [错误模型](00-overview/global-error-model.md) · [术语](00-overview/glossary.md) · [证据索引](00-overview/evidence-index.md) · [决策](00-overview/decision-log.md) · [状态](00-overview/analysis-state.md)
- 模块：[注册表](01-modules/module-registry.md)
- 实践：[快速上手](99-roadmap/quick-start.md) · [阅读路线](99-roadmap/reading-guide.md) · [调试](99-roadmap/debugging-guide.md) · [风险](99-roadmap/risk-register.md) · [后续步骤](99-roadmap/next-steps.md)
- Demo：[D01 Hello World](80-demos/D01-hello-world/README.md)

## 相关文档

- `source/ray/README.rst`：项目官方定位与公共抽象。
- `source/ray/doc/source/ray-core/internals.rst`：官方 Core internals 目录。
- `source/ray/AGENTS.md`：上游贡献和验证约束。

## 源码证据摘要

[`README.rst:17-47`]、[`python/ray/__init__.py:80-130`]、[`python/ray/_private/worker.py:1439-1505`]、[`python/ray/remote_function.py:41-358`]、[`src/ray/core_worker/core_worker_process.cc:231-285`]。

## 未解决问题

- 当前环境未验证 Ray wheel/build、最小集群、真实 Demo 输出和关闭路径。
- 需要在后续批次补充每个核心模块的精确符号链、异常路径、测试映射和动态证据。
- 进程拓扑、调度策略和对象跨节点传输的具体运行时选择不能仅由公共 API 页面确定。

## 下一步阅读建议

先读 [分析状态](00-overview/analysis-state.md)，然后沿 [模块注册表](01-modules/module-registry.md) 进入 Core 五模块和 D01；需要开发时再读 [构建部署](00-overview/build-and-deploy.md)。
