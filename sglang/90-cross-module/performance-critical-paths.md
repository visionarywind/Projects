# 性能关键路径

- 文档目的：定位普通请求路径中影响吞吐、首 token 延迟和显存压力的跨模块环节。
- 对应源码版本：`78be4b50af88e9ea72d75b4c3a3e42b7297d2501`
- 证据状态：部分完成
- 最后更新：2026-09-10

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

**已确认**：scheduler 代码在 planning、forward 和 result processing 阶段分别记录/使用 batch 与时间状态；但单凭静态源码不能推出具体吞吐。[`python/sglang/srt/managers/scheduler.py:3499-3795`][`python/sglang/srt/managers/scheduler.py:4199-4382`]

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
