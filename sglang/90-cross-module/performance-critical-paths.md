# 性能关键路径

- 文档目的：定位普通请求路径中影响吞吐、首 token 延迟和显存压力的跨模块环节。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/performance-critical-paths.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 路径

```text
request tokenize/IPC
  -> prefix hit and admission
  -> prefill token budget
  -> KV allocation
  -> decode batch size
  -> worker forward/sample
  -> output copy/detokenize/IPC
```

## 关键杠杆

- prefix cache 命中减少 prefill，但会受 page alignment 和 cache policy 影响；
- `PrefillAdder` 的 token/page 预算决定批次边界；
- continuous batching 在 decode 与新 prefill 之间折中；
- overlap 可以交错 CPU result processing 与下一次 forward，但引入 snapshot、stream/event 和 WAR 约束；
- TP/PP/EP collective 和 device backend 决定跨卡开销；
- output streaming、copy-to-CPU 和 detokenization 会影响尾延迟。

**已确认**：scheduler 代码在 planning、forward 和 result processing 阶段分别记录/使用 batch 与时间状态；但单凭静态源码不能推出具体吞吐。[`python/sglang/srt/managers/scheduler.py:3499-3795`][`python/sglang/srt/managers/scheduler.py:1-4005`]

## 调优顺序

1. 先确认模型、GPU、并行配置和 batch workload；
2. 观察 prefill/decode token 数、KV capacity 和 retraction；
3. 再比较 prefix hit、overlap、CUDA Graph 和输出路径；
4. 用同一 workload 的基准结果验证改动，而不是只看单次请求。

**未验证**：未运行 benchmark、profiling 或 GPU kernel trace。

## 相关文档

- [系统 wiring](system-wiring.md)
- [配置影响图](configuration-impact-map.md)
- [M04 Scheduler](../01-modules/M04-scheduler-batching/README.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
