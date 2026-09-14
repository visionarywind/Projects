# M05 代表性例子

- `cuStreamCreate` 成功后 public handle 与内部 stream 双向关联；后续 destroy 先使 handle 不可用，不能立即释放仍被 GPU 使用的 QMD。
- launch 选取 `stream->qmds[nextQmdIndex]`，通过 marker 防止复用仍在执行的 QMD。
- `basic_sanity:kernel` 检查 `beginPushCount` 和 per-thread stream 的 compute channel，证明测试意图覆盖提交入口但不证明真实硬件完成。
