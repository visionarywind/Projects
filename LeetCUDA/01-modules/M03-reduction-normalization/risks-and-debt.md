# M03 风险与债务

- warp/block reduction 的同步范围错误会造成非确定性。
- softmax 未做稳定化可能溢出；低精度 accumulator 会放大误差。
- row 长度和尾部 lane 的 mask 需要明确。
- 不同 norm 实现对 epsilon、affine 参数和 layout 的契约可能不同。

应建立逐算子输入矩阵和误差阈值；当前没有统一规范。
