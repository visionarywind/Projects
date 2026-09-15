# D01-S01 构建与运行

- 文档目的：解释 80-demos/D01-nms-python/build-and-run.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-nms-python/build-and-run.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 真实入口

`kernels/nms/nms.py` 在模块加载时调用 `torch.utils.cpp_extension.load`，源文件为 `nms.cu`，并传递 CUDA/C++17 flags。[kernels/nms/nms.py:8-23]。因此第一次运行可能触发编译/cache；这不是预编译 wheel。

## 2. 命令（未验证）

```bash
cd kernels/nms
python3 nms.py
```

脚本会执行固定 regression、随机 correctness sweep，并在满足条件时进入 benchmark；实际分支以文件当前 `__main__` 为准。[kernels/nms/nms.py:87-136]。当前环境未执行，不能宣称 build 或 correctness 成功。

## 3. 执行前检查

```bash
python3 - <<'PY'
import torch
print(torch.__version__)
print(torch.cuda.is_available())
if torch.cuda.is_available():
    print(torch.cuda.get_device_name())
PY
```

此检查命令也未执行。需要另行记录 driver、CUDA toolkit、torchvision 和可用显存。

## 4. 构建产物与所有权

动态 extension 的编译产物进入 PyTorch extension cache；脚本只持有 Python module 引用。boxes/scores/输出由 PyTorch allocator 管理，`nms.cu` 使用 `data_ptr` 借用其 device storage。[kernels/nms/nms.cu:126-192]。没有源码证据表明本 Demo 手工释放 CUDA allocation。

## 5. 最小输入（未验证）

```python
boxes = torch.tensor([
    [0., 0., 10., 10.],
    [1., 1.,  9.,  9.],
    [20., 20., 30., 30.],
    [21., 21., 29., 29.],
    [40., 40., 50., 50.],
    [60., 60., 70., 70.],
], device="cuda", dtype=torch.float32)
scores = torch.tensor([.9, .8, .7, .6, .5, .4], device="cuda")
# lib.nms(boxes, scores, 0.5)
```

这是用于复现路径的输入，不是记录过的输出；连续坐标 IoU 和排序 ties 必须以实际调用结果为准。

## 6. 注意事项

- CPU tensor 会触发 device check；
- boxes 形状不是 `(N,4)` 或 scores 不是 `(N,)` 会触发检查；
- 空输入路径已在 binding 中显式返回；
- launch error 的显式检查不应假设存在，必要时用 `CUDA_LAUNCH_BLOCKING=1` 或 sanitizer 进行独立诊断。

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
