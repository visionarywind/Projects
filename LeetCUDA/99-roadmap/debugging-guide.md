# 调试指南

- 对应源码版本：`main` / `0983c65`。
- 状态：命令为建议流程，未在本环境执行。
- 最后更新：2026-09-10

## 分层排查

1. **环境**：确认 Python/PyTorch/CUDA/nvcc/GPU 与目标 SM。
2. **编译**：确认 include、库、C++ ABI、extension cache。
3. **输入**：打印 dtype/device/shape/stride/contiguous，缩小到最小 case。
4. **运行**：加入同步并检查 launch/runtime error。
5. **正确性**：与 torch/torchvision/reference 对拍，记录 max error 和索引顺序。
6. **内存/竞态**：使用 `compute-sanitizer`（未验证）。
7. **性能**：最后再使用 Nsight，固定 warmup/iters/同步点。

## 专项检查

- elementwise：pack 对齐、尾部、二维 K 分支。
- GEMM：非 tile 倍数、K-loop 最后 tile、shared barrier。
- HGEMM/FA：目标架构、动态 shared memory、layout 和 accumulator。
- NMS：同分数 stable sort、mask 初始化、原始 index mapping、N=0。

## 证据

`[kernels/interview/notes-v2.cu:82-97]` 展示显式资源检查；`[kernels/nms/nms.py:87-121]` 展示随机 correctness 范围；`[kernels/hgemm/hgemm.py:210-328]` 展示同步 benchmark。

## 报告格式

记录 commit、GPU、driver、CUDA、PyTorch、命令、输入 shape、预期、实际、是否同步、是否 sanitizer。把源码推断、历史输出、本机实测分栏。
