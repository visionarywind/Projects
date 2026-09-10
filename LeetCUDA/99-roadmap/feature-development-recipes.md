# 功能开发配方

- 对应源码版本：`main` / `0983c65`。
- 最后更新：2026-09-10

## 新增普通 kernel

1. 复制 `kernels/elementwise/` 的最小 Python + `.cu` 结构。
2. 明确 dtype/device/shape/stride/contiguous、空输入和尾部契约。
3. 在 binding 中检查输入并导出符号。
4. 写 torch reference 和小尺寸 correctness。
5. 加 warmup、同步、计时，再写 README。
6. 运行 pre-commit（未验证）。

## 修改布局或 tile

先画 global/shared/register 数据流；保留原版本作为 correctness baseline；覆盖非倍数尺寸、不同架构和 sanitizer；只有 correctness 通过后才比较性能。

## 修改 PyTorch API

同步更新 `.cu/.cc` 导出、Python 参数、示例、错误信息和测试。不要在异步 kernel 完成前保存裸指针或让输入 tensor 生命周期结束。

## 修改 Interview 公共头文件

列出所有 include 消费者，按每个 `--arch` 编译，并运行至少一个 correctness phase；公共宏的改动可能影响多个阶段。

## 证据

`[CONTRIBUTE.md:5-31]`；`[kernels/elementwise/elementwise.py:9-66]`；`[kernels/nms/nms.cu:126-189]`。
