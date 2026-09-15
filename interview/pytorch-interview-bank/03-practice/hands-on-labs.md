# PyTorch 实践实验

实验只给出方案，不声称已执行。建议按题库四级路线逐步绑定题目；所有 GPU、通信、压测和真实部署结果仍标为 `[待验证]`。

1. **Tensor 契约实验**：固定 shape、dtype、device 和非连续切片，断言结果、stride 与 `.contiguous()` 的差异；结果标为 `[待验证]`。
2. **Autograd 数值实验**：对一个小函数比较解析梯度、autograd 梯度和有限差分；记录误差范围。
3. **AMP 对照实验**：固定模型、数据、seed 和 warmup，比较 FP32、autocast 和梯度缩放的 loss、吞吐、峰值显存和溢出次数。
4. **DDP 最小实验**：两个进程执行相同模型，验证参数初始化、梯度同步、rank 日志和退出清理。
5. **Compile 实验**：比较 eager 与 `torch.compile` 的首轮编译时间、稳态延迟、graph break 和结果一致性。
