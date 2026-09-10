# 错误边界

- 文档目的：区分输入、调度、设备、进程和输出阶段的错误处理与资源清理。
- 对应源码版本：`f1a512c51c73ab660cf41e1af3110c7c11e3b600`
- 证据状态：部分完成
- 最后更新：2026-09-10

## 错误分层

| 阶段 | 典型错误 | 清理/传播 |
|---|---|---|
| tokenize 前 | 输入结构、参数或 tokenizer 错误 | 未 dispatch 的本地 state 可直接释放 |
| dispatch | socket/VMM/序列化失败 | 取消临时 VMM 资源，保留异常语义 |
| scheduler admission | KV/token budget 不足 | 等待、chunk、retract 或请求级 abort |
| worker forward | CUDA/model/device 错误 | scheduler process error path，通知 parent |
| output | rid 不存在、断连或 abort | 跳过/抛错并删除或终止对应 state |
| shutdown | graceful 或异常退出 | 正常释放 host resources，wedged GPU 避免阻塞清理 |

**已确认**：dispatch 前后清理语义不同；`retract_decode` 优先尝试恢复请求，无法恢复时设置 abort；`run_scheduler_process` 负责异常通知和进程组处理。[`python/sglang/srt/managers/tokenizer_manager.py:836-845`][`python/sglang/srt/managers/schedule_batch.py:3076-3159`][`python/sglang/srt/managers/scheduler.py:5744-5833`]

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
