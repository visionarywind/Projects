# D01 数据与状态轨迹

| 阶段 | 关键状态 |
|---|---|
| init | globals、TLS、device table |
| context | current `CUctx`、module/function 链表 |
| launch | grid/block、packed params、QMD、syscall imports |
| submit | compute channel、push counters、marker |
| completion | synchronization 与 kernel 输出 |

以上是源码结构映射；未执行测试，因此没有实际数值或硬件结论。
