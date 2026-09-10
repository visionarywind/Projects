# D01-S01 构建与运行

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
