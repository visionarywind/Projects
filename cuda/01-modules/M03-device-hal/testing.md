# M03 测试与验证

## 源码测试入口

- `basic_sanity.cu` 遍历 `getCudaGpus()`，创建 context 并核对 context device 与枚举对象一致（静态确认：[tests/cuda_test/basic_sanity.cu:67-100,181-195]）。
- dispatcher 的 `--listGpus` 展示 bus order、ordinal 和过滤状态，并通过 `dontRunTests` 停止执行；实际运行还需 `--forceRun`（静态确认：[tests/cuda_test/dispatch_main.cpp:22-50]）。

## 覆盖矩阵

| 场景 | 静态覆盖 | 运行状态 |
|---|---|---|
| 多 GPU/ordinal | `basic_sanity`、dispatcher | 未验证 |
| 设备过滤/排序 | dispatcher 与 devmgr 逻辑 | 未验证 |
| 不支持架构 | HAL default 分支 | 未验证 |
| DMAL 初始化失败 | `channelInit` 回滚逻辑 | 未验证 |

本知识库未执行 nvmake、GPU 测试或 `nvidia-smi`；表中“覆盖”只表示源码存在对应路径。
