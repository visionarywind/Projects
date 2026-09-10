# M03 测试

以 torch 的 `sum`、`softmax`、`layer_norm` 和 `rms_norm` 为 reference，覆盖短行、长行、非整 warp、全相等值、极大/极小值、NaN/Inf、不同 dtype 和空输入。对 softmax 重点检查有限性和误差；对 norm 同时检查均值/方差与输出。

以上测试建议未在本环境执行；具体脚本参数需按目录核对。
