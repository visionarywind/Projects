# 快速上手

- 文档目的：解释 99-roadmap/quick-start.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 状态：命令来自仓库脚本/文档，未在本环境执行。

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

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
