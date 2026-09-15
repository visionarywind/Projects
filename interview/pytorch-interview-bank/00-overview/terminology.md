# PyTorch 术语表

| 术语 | 含义 | 易混点 |
|---|---|---|
| Tensor | 带 dtype、device、shape 和 stride 的多维数组抽象 | 不等同于连续内存 |
| leaf tensor | 用户创建或被标记为叶子的 autograd 张量 | `requires_grad` 与 leaf 是不同概念 |
| autograd | 基于反向模式自动微分的运行时 | 不等同于完整训练循环 |
| parameter | 注册到 `nn.Module` 的可训练张量 | 普通 Tensor 不会自动进入 `parameters()` |
| buffer | 注册到模块、但通常不参与优化的状态 | 仍可能进入 state dict |
| AMP | 混合精度计算与梯度缩放机制 | autocast 与 GradScaler 职责不同 |
| DDP | 多进程数据并行并同步梯度 | 不等同于模型并行 |
| graph break | 编译捕获无法继续的边界 | 不必然代表结果错误，但可能影响性能 |
| state dict | 可序列化的参数和持久状态映射 | 不等同于完整 Python 对象 |
