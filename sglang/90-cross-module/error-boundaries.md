# 错误边界

- 文档目的：区分输入、调度、设备、进程和输出阶段的错误处理与资源清理。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/error-boundaries.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 错误分层

| 阶段 | 典型错误 | 清理/传播 |
|---|---|---|
| tokenize 前 | 输入结构、参数或 tokenizer 错误 | 未 dispatch 的本地 state 可直接释放 |
| dispatch | socket/VMM/序列化失败 | 取消临时 VMM 资源，保留异常语义 |
| scheduler admission | KV/token budget 不足 | 等待、chunk、retract 或请求级 abort |
| worker forward | CUDA/model/device 错误 | scheduler process error path，通知 parent |
| output | rid 不存在、断连或 abort | 跳过/抛错并删除或终止对应 state |
| shutdown | graceful 或异常退出 | 正常释放 host resources，wedged GPU 避免阻塞清理 |

**已确认**：dispatch 前后清理语义不同；`retract_decode` 优先尝试恢复请求，无法恢复时设置 abort；`run_scheduler_process` 负责异常通知和进程组处理。[`python/sglang/srt/managers/tokenizer_manager.py:836-845`][`python/sglang/srt/managers/schedule_batch.py:1-2845`][`python/sglang/srt/managers/scheduler.py:1-4005`]

## 调试问题顺序

1. `rid_to_state` 是否创建且仍存在？
2. 请求是否已标记 dispatched？
3. scheduler receiver 是否收到对象？
4. waiting queue 是否因 budget/prefix/priority 拒绝？
5. batch 是否进入 worker，forward mode 是什么？
6. result 是否回到正确 rid？
7. abort 后 scheduler 和本地 state 是否都清理？

## 未验证范围

没有 GPU、模型和真实网络断连运行；测试路径仅作源码地图，不能声称通过。

## 相关文档

- [端到端流程](end-to-end-flows.md)
- [共享数据与类型](shared-data-and-types.md)
- [M03 Tokenizer 与请求状态](../01-modules/M03-tokenizer-request-state/README.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
