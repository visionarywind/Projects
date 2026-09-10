# 测试配方

- 对应源码版本：`main` / `0983c65`。
- 状态：GPU 命令未在本环境执行。
- 最后更新：2026-09-10

## 正确性矩阵

| 层 | 最小测试 | 扩展测试 |
|---|---|---|
| elementwise | 0/1/非 pack 长度、多个 dtype | 二维 K 分支、非 contiguous |
| reduction/norm | 短行、极值、NaN/Inf | 大 row、多个 axis/dtype |
| GEMM | M/N/K=1、非 tile 倍数 | 多架构、stages/layout |
| attention | B/H/N/D 小组合 | D、架构宏、reference variants |
| NMS | 空、6-box regression、ties | N×seed×threshold、超大 N |
| interview | 每个 phase smoke | all arch + benchmark |

## 参考实现

优先使用 torch、torchvision、cuBLAS 或 CPU reference；记录语义差异。NMS CPU `+1` 面积与 GPU 连续坐标不同，不能未经调整直接判定失败。[kernels/nms/nms.cc:4-40]；[kernels/nms/nms.cu:36-57]

## 性能测试

warmup 后同步，重复固定次数，记录均值/分位数和 shape；不要把首次编译时间混入 kernel 时间。HGEMM benchmark 已显式使用同步。[kernels/hgemm/hgemm.py:255-282]

## 安全性/工具

推荐 `compute-sanitizer --tool memcheck`、`racecheck` 和 Nsight；这些在当前环境均未验证。
