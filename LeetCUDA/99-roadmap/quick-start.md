# 快速上手

- 对应源码版本：`main` / `0983c65`。
- 状态：命令来自仓库脚本/文档，未在本环境执行。
- 最后更新：2026-09-10

## 先确认环境

```bash
python3 - <<'PY'
import torch
print(torch.__version__, torch.version.cuda, torch.cuda.is_available())
if torch.cuda.is_available(): print(torch.cuda.get_device_name())
PY
nvcc --version
```

未验证：以上命令只用于环境检查，不代表仓库安装成功。

## 最小阅读和运行顺序

1. 读根 README 和 [M08](../01-modules/M08-pytorch-extension/README.md)。
2. 读 elementwise 的 `.py`/`.cu`，理解 `load()`、binding、kernel。
3. 在有 CUDA 环境时执行：

```bash
cd kernels/elementwise
TORCH_CUDA_ARCH_LIST=Ada python3 elementwise.py
```

4. 再读 NMS 并执行其 correctness 脚本。
5. 最后按 GPU 架构尝试 `kernels/interview/build.sh --arch <arch>`。

## 构建入口

- 普通扩展：各目录 Python script。
- HGEMM wheel：`python3 setup.py bdist_wheel`。
- Interview：`./build.sh --arch sm_89`。
- 代码格式：`pre-commit run --all-files`。

## 失败分类

编译失败先查 nvcc、headers、架构和 ABI；运行失败先缩小输入并执行 correctness；性能异常先确认 warmup/synchronize、shape 和 GPU。不要把 README 历史 benchmark 当作本机结果。

## 证据

`[README.md:53-105]`；`[kernels/elementwise/elementwise.py:9-24]`；`[kernels/interview/build.sh:29-43]`。
