# M06 运行示例与修改练习

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/examples.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
