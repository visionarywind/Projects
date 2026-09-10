# M06 运行示例与修改练习

## 示例 A：静态追踪一个 stage kernel

命令（未验证）：

```bash
cd kernels/hgemm
python3 hgemm.py --M 128 --N 128 --K 128 --enable-mma --warmup 10 --iters 100
```

阅读预期：脚本选择 MMA tag，扩展加载对应 source，kernel 经过 shared stage 和 HMMA 写回 C；实际是否成功取决于 CUDA/PyTorch/GPU 环境，不能把命令当作测试通过。

## 示例 B：对比矩阵尺寸

优先组合 `128^3`、`130×129×131` 和不同 `stages`。第一组观察整 tile，第二组专门暴露尾 tile/stride 契约。运行结果、误差和 TFLOPS 必须单独记录。

## 修改练习

1. 修改 `A_PAD/B_PAD` 后重新计算动态 shared memory，并检查 bank conflict。
2. 禁用 `BLOCK_SWIZZLE`，比较同一 GPU 上的 L2/时间变化。
3. 把 F16 accumulator 与 F32/reference 对拍，记录误差而非只看 TFLOPS。
4. 在 launch 后加入明确 CUDA error check，验证错误边界。

上述练习均未在当前环境执行。
