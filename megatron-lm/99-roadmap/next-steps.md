# 后续路线

## 下一批

- 精化 M01 forward/postprocess 和 sharded_state_dict。
- 深入 M02 `forward_backward_pipelining_with_interleaving`。
- 深入 M05 `save_checkpoint/load_checkpoint`。
- 选择并分析 M06 offline inference Demo。

## 验证批

- 运行 `git diff --check`。
- 在具备 GPU 的上游环境运行 D01。
- 运行 parallel_state、dataset、model、checkpoint focused unit tests。

## 完成标准

每个模块至少具备：README、implementation、execution flow、interfaces、data structures/source map、测试/风险说明，并且 analysis-state 记录剩余未验证项。