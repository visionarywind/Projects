# 修改影响图

- 文档目的：把常见源码修改映射到必须联查的模块、状态和测试。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10

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
