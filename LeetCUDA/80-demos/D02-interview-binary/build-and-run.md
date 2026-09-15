# D02-S01 构建与运行

- 文档目的：解释 80-demos/D02-interview-binary/build-and-run.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D02-interview-binary/build-and-run.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 构建命令（未验证）

```bash
cd kernels/interview
./build.sh --arch sm_89
```

其他源码支持的选择包括 `sm_86`、`sm_90a`、`sm_120a` 和 `all`；`--clean` 清理输出。[kernels/interview/build.sh:45-126]。脚本使用 `/usr/local/cuda/bin/nvcc`、C++20、CUTLASS/cudnn-frontend include，并按架构设置宏/链接参数。[kernels/interview/build.sh:13-81,134-185]。

## 运行命令（未验证）

实际可用 flag 应先执行：

```bash
./notes_v2_sm89.bin --help
```

README/根 README 中的 benchmark 示例为：

```bash
./notes_v2_sm120a.bin --bench --mnk 4096,4096,4096 --bhnd 1,32,16384,128
```

该命令要求对应 binary 已构建、GPU 架构匹配、库和输入满足约束；当前未执行。

## 构建产物

- `notes-v2.cu` 先编译为 `notes-v2.o`；
- 再链接为架构后缀 binary；
- `--arch all` 依次构建支持的架构；
- 不同 binary 的 gencode/宏决定可用 WGMMA/TMA/CuTe 路径，不能交叉替代。

证据：[kernels/interview/build.sh:134-185]。

## 环境记录模板

运行前保存：

```text
GPU:
Driver:
CUDA toolkit:
nvcc:
CUTLASS submodule commit:
cudnn-frontend submodule commit:
arch binary:
```

## 当前阻断

本知识库没有安装依赖、更新子模块、调用 nvcc 或启动 binary；因此没有真实编译/运行结果。

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
