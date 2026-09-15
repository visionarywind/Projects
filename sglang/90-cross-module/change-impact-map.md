# 修改影响图

- 文档目的：把常见源码修改映射到必须联查的模块、状态和测试。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/change-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 影响矩阵

| 修改对象 | 直接影响 | 必须联查 |
|---|---|---|
| 输入/IPC 字段 | tokenize、序列化、scheduler dispatcher | M03/M04/M15、协议测试 |
| admission budget | waiting/running、KV 分配、chunking | M04/M08/M09、scheduler tests |
| `ScheduleBatch` 字段 | forward input、overlap 生命周期 | M04/M05/M09 |
| result processor | token、finish、KV、输出对齐 | M03/M04/M05/M08/M10 |
| rank/group 初始化 | GPU placement、collective、模型层 | M06/M07、multi-GPU tests |
| loader/weight mapping | 参数拥有者、量化后处理 | M05/M06/M14 |
| multimodal placeholder/feature | token budget、embedding splice、position/graph metadata | M02/M03/M04/M05/M09/M12 |
| KV transfer/staging | cache layout、admission、请求完成时机 | M04/M08/M13/M15、transfer backend |
| quant/MoE/LoRA metadata | packed weights、expert routing、adapter slot | M05/M06/M07/M14/M16 |
| platform/kernel dispatch | dtype/layout、graph eligibility、fallback | M05/M09/M14/M16/M18 |
| Rust/router/gateway schema | stream、retry、worker routing、cancellation | M02/M04/M08/M13/M15/M17 |
| suite/filter/benchmark config | 实际覆盖率、CI 分片、性能结论 | M18、受影响模块 |
| shutdown/error path | child、socket、device 资源 | M03/M04/M15/M17 |

## 变更前问题

- 状态改变发生在哪个进程？
- 是否有 device tensor 或 allocator 所有权？
- overlap 是否仍持有旧 batch snapshot？
- 失败时本地 state、scheduler `Req` 和 KV 是否一致清理？
- 是否有真实 registered test 覆盖正常和异常两类？

**未验证**：矩阵是基于静态源码边界整理，尚未运行完整测试。

## 相关文档

- [配置影响图](configuration-impact-map.md)
- [错误边界](error-boundaries.md)
- [M04 Scheduler](../01-modules/M04-scheduler-batching/README.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
