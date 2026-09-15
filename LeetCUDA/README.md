# LeetCUDA 项目理解知识库

- 文档目的：解释 README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](README.md)。
- 后续阅读：[分析状态](00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 生成范围：以当前源码 checkout `main` 分支提交 `4513b3114de21140c846171042515e17bb907e8a` 为目标版本。
- 项目名称：LeetCUDA
- 文档语言：中文
- 证据规则：正文中的源码引用均使用 `[相对路径:起始行-结束行]`；行号以目标提交为准。
- 分析状态：当前批次完成总览、模块地图、核心运行/构建说明和实践入口；M06/M07 代表 kernel 与 D01/D02 Demo 已补充深度源码追踪，其他变体和真实 GPU 运行仍部分覆盖。

## 5 分钟快速理解

LeetCUDA 是一个以 CUDA C++ 和 PyTorch 为载体的学习/实验仓库：每个小型 kernel 通常由 `.cu` 实现、Python 脚本通过 `torch.utils.cpp_extension.load` 即时编译加载、再与 PyTorch 或 torchvision 参考实现做正确性/性能对比。项目的学习主线从逐元素操作、归约和归一化，逐步进入 GEMV/GEMM、Tensor Core（WMMA/MMA/CuTe/WGMMA）和 FlashAttention；另有 interview 单文件整合版、Triton/CUTLASS 示例、Nsight 资料和 NMS 等专题。

最短阅读路径：

1. [总览：项目定位](00-overview/project-overview.md)
2. [总览：架构](00-overview/architecture.md)
3. [模块注册表](01-modules/module-registry.md)
4. [PyTorch 扩展路径](01-modules/M08-pytorch-extension/README.md)
5. [面试整合入口](01-modules/M09-interview-benchmark/README.md)
6. [快速上手](99-roadmap/quick-start.md)

## 源码版本与证据边界

- 已确认版本：`main` / `4513b31`（`notes: add SGEMM TF32 (WMMA m16n16k8) kernel and bench (#520)`）。
- 远端仓库信息：README 声明项目地址为 <https://github.com/xlite-dev/LeetCUDA>。
- 目标平台：Linux + NVIDIA CUDA GPU + CUDA Toolkit；不同模块的最低版本不同，不能把单一版本要求视为全仓库契约。
- 本环境未执行 GPU 构建、安装依赖或 benchmark；所有运行结果均标为仓库文档中的历史示例，不能视为本机验证。
- `third-party/cutlass` 与 `third-party/cudnn-frontend` 是子模块；知识库只记录它们对构建/行为的边界影响，不分析第三方内部实现。

## 知识库目录

```text
LeetCUDA/
├── README.md
├── 00-overview/
├── 01-modules/
├── 80-demos/
├── 90-cross-module/
└── 99-roadmap/
```

## 总览层

- [项目定位、能力与边界](00-overview/project-overview.md)
- [总体架构与模块关系](00-overview/architecture.md)
- [设计原则、取舍与稳定契约](00-overview/design-principles.md)
- [启动、运行与资源生命周期](00-overview/runtime-model.md)
- [全局数据流与控制流](00-overview/global-data-flow.md)
- [内部/外部依赖地图](00-overview/dependency-map.md)
- [构建、运行、测试与部署](00-overview/build-and-deploy.md)
- [错误模型与恢复边界](00-overview/global-error-model.md)
- [术语表](00-overview/glossary.md)
- [源码证据索引](00-overview/evidence-index.md)
- [决策与冲突记录](00-overview/decision-log.md)
- [分析状态与覆盖范围](00-overview/analysis-state.md)
- [池化与资源管理专题](90-cross-module/pooling-and-resource-management.md)

## 真实 Demo 层

- [Demo 注册表](80-demos/demo-registry.md)
- [D01 NMS Python](80-demos/D01-nms-python/README.md)：动态 PyTorch extension、两阶段 mask/resolve、原始索引恢复
- [D02 Interview binary](80-demos/D02-interview-binary/README.md)：多架构构建、CLI、显式 CUDA 资源和 benchmark

| Demo | M08 | M09 | M10 | M06/M07 | 运行状态 |
|---|---:|---:|---:|---:|---|
| D01 NMS Python | ✓ | — | ✓ | — | 源码已追踪，运行未验证 |
| D02 Interview binary | — | ✓ | — | ✓ | 源码已追踪，构建/运行未验证 |

## 模块层

| ID | 模块 | 主要源码 | 说明 |
|---|---|---|---|
| M01 | [基础公共层](01-modules/M01-foundation/README.md) | `kernels/interview/common.cuh`, `base.cuh` | 公共 CUDA 类型、宏、基础原语和学习框架 |
| M02 | [逐元素算子](01-modules/M02-elementwise/README.md) | `kernels/elementwise/`、激活目录 | 标量/向量化逐元素计算与绑定模式 |
| M03 | [归约与归一化](01-modules/M03-reduction-normalization/README.md) | `reduce/`, `softmax/`, `layer-norm/`, `rms-norm/` | warp/block reduce、safe/online softmax、Norm |
| M04 | [索引与位置算子](01-modules/M04-indexing-position/README.md) | `embedding/`, `histogram/`, `mat-transpose/`, `rope/` | 索引访问、布局变换、统计与位置编码 |
| M05 | [GEMV/SGEMM](01-modules/M05-gemv-sgemm/README.md) | `sgemv/`, `hgemv/`, `sgemm/` | CUDA Core 矩阵/向量乘及 tiling/pipeline |
| M06 | [HGEMM/Tensor Core](01-modules/M06-hgemm-tensorcore/README.md) | `hgemm/`, `swizzle/`, `ws-hgemm/` | WMMA、MMA、CuTe、WGMMA 和共享内存优化 |
| M07 | [FlashAttention](01-modules/M07-flash-attention/README.md) | `flash-attn/` | MMA/CuTe/TMA、分块 online softmax 和 Q/K/V 复用 |
| M08 | [PyTorch 扩展边界](01-modules/M08-pytorch-extension/README.md) | 各模块 `.py`, `pybind/*.cc`, `setup.py` | 即时扩展、打包扩展、张量契约和 benchmark 驱动 |
| M09 | [Interview 整合与 benchmark](01-modules/M09-interview-benchmark/README.md) | `kernels/interview/` | 单一 `notes-v2.cu` 入口、CLI、测试与多架构构建 |
| M10 | [NMS 专题](01-modules/M10-nms/README.md) | `kernels/nms/` | 两阶段 GPU NMS、CPU 参考和回归对拍 |
| M11 | [Triton/CUTLASS/Profiling](01-modules/M11-triton-cutlass-profiling/README.md) | `openai-triton/`, `cutlass/`, `nvidia-nsight/` | 外部 DSL、CuTe DSL 和性能分析材料 |

## 三条最重要的端到端流程

1. **单算子学习流程**：`<op>.py` 生成 CUDA 张量 → `load()` 编译 `<op>.cu` → PyBind 暴露函数 → CUDA kernel 写回输出 → Python 与 torch 结果/耗时比较。详见 [跨模块流程](90-cross-module/end-to-end-flows.md)。
2. **HGEMM 流程**：`hgemm.py` 解析矩阵/算法选项 → `tools/utils.py` 组织 sources/flags 并加载扩展 → 选定 CUDA Core/WMMA/MMA/CuTe kernel → 预热、同步、计时、计算 TFLOPS → 可选绘图。详见 [M06](01-modules/M06-hgemm-tensorcore/README.md)。
3. **Interview 二进制流程**：`build.sh` 按 SM 架构编译/链接 `notes-v2.cu` → CLI 选择 test/bench → `notes-v2.cu` 调用各阶段 kernel、参考实现和 CUDA 错误检查 → 输出误差/TFLOPS。详见 [M09](01-modules/M09-interview-benchmark/README.md)。

## 构建与运行入口

| 目标 | 入口 | 备注 |
|---|---|---|
| 单个普通 kernel | `cd kernels/<name>; python3 <name>.py` | 即时编译；需要 PyTorch/CUDA |
| FlashAttention | `python3 kernels/flash-attn/flash_attn_mma.py --D 64` | 需要 flash-attn；硬件相关 |
| Toy-HGEMM 包 | `cd kernels/hgemm; python3 setup.py bdist_wheel` | 可选 CUTLASS 子模块 |
| Interview | `cd kernels/interview; ./build.sh --arch sm_89` | 直接使用 `/usr/local/cuda/bin/nvcc` |
| 代码质量 | `pre-commit run --all-files` | 配置见 `.pre-commit-config.yaml` |

未验证：本机没有执行上述 GPU 依赖流程，因此不承诺当前环境可直接成功运行。

## 按角色推荐阅读

- 初级 CUDA 开发者：M01 → M02 → M03 → M05 → M06。
- PyTorch/C++ 扩展开发者：M08 → 任一简单算子 M02 → M10 → M06。
- 性能工程师：M05 → M06 → M07 → M11 → `90-cross-module/performance-critical-paths.md`。
- 面试准备者：M09 的 `common.cuh` → `base.cuh` → `sgemv.cuh` → `sgemm.cuh` → `hgemm.cuh` → `flash_attn.cuh`。

## 状态、缺口与维护

当前文档覆盖仓库的主要职责域、所有 kernel 一级目录、主要构建/测试入口和三条核心流程；深层实现按高价值代表（elementwise、NMS、SGEMM、HGEMM、FlashAttention、interview）建立证据，其余小算子采用共享模式归纳。未解决问题、未验证命令、覆盖维度和增量维护规则集中在 [analysis-state.md](00-overview/analysis-state.md)。

## 文档元数据（规范补充）

- 文档目的：说明 `README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 全文档索引

### 00-overview
- [00-overview/analysis-state.md](00-overview/analysis-state.md)
- [00-overview/architecture.md](00-overview/architecture.md)
- [00-overview/build-and-deploy.md](00-overview/build-and-deploy.md)
- [00-overview/decision-log.md](00-overview/decision-log.md)
- [00-overview/dependency-map.md](00-overview/dependency-map.md)
- [00-overview/design-principles.md](00-overview/design-principles.md)
- [00-overview/evidence-index.md](00-overview/evidence-index.md)
- [00-overview/global-data-flow.md](00-overview/global-data-flow.md)
- [00-overview/global-error-model.md](00-overview/global-error-model.md)
- [00-overview/glossary.md](00-overview/glossary.md)
- [00-overview/project-overview.md](00-overview/project-overview.md)
- [00-overview/runtime-model.md](00-overview/runtime-model.md)

### 01-modules
- [01-modules/M01-foundation/README.md](01-modules/M01-foundation/README.md)
- [01-modules/M01-foundation/call-chains.md](01-modules/M01-foundation/call-chains.md)
- [01-modules/M01-foundation/data-structures.md](01-modules/M01-foundation/data-structures.md)
- [01-modules/M01-foundation/design.md](01-modules/M01-foundation/design.md)
- [01-modules/M01-foundation/diagrams.md](01-modules/M01-foundation/diagrams.md)
- [01-modules/M01-foundation/examples.md](01-modules/M01-foundation/examples.md)
- [01-modules/M01-foundation/interfaces.md](01-modules/M01-foundation/interfaces.md)
- [01-modules/M01-foundation/line-level-analysis.md](01-modules/M01-foundation/line-level-analysis.md)
- [01-modules/M01-foundation/risks-and-debt.md](01-modules/M01-foundation/risks-and-debt.md)
- [01-modules/M01-foundation/source-map.md](01-modules/M01-foundation/source-map.md)
- [01-modules/M01-foundation/testing.md](01-modules/M01-foundation/testing.md)
- [01-modules/M02-elementwise/README.md](01-modules/M02-elementwise/README.md)
- [01-modules/M02-elementwise/call-chains.md](01-modules/M02-elementwise/call-chains.md)
- [01-modules/M02-elementwise/data-structures.md](01-modules/M02-elementwise/data-structures.md)
- [01-modules/M02-elementwise/design.md](01-modules/M02-elementwise/design.md)
- [01-modules/M02-elementwise/development-guide.md](01-modules/M02-elementwise/development-guide.md)
- [01-modules/M02-elementwise/diagrams.md](01-modules/M02-elementwise/diagrams.md)
- [01-modules/M02-elementwise/examples.md](01-modules/M02-elementwise/examples.md)
- [01-modules/M02-elementwise/execution-flows.md](01-modules/M02-elementwise/execution-flows.md)
- [01-modules/M02-elementwise/implementation.md](01-modules/M02-elementwise/implementation.md)
- [01-modules/M02-elementwise/interfaces.md](01-modules/M02-elementwise/interfaces.md)
- [01-modules/M02-elementwise/line-level-analysis.md](01-modules/M02-elementwise/line-level-analysis.md)
- [01-modules/M02-elementwise/risks-and-debt.md](01-modules/M02-elementwise/risks-and-debt.md)
- [01-modules/M02-elementwise/source-map.md](01-modules/M02-elementwise/source-map.md)
- [01-modules/M02-elementwise/testing.md](01-modules/M02-elementwise/testing.md)
- [01-modules/M03-reduction-normalization/README.md](01-modules/M03-reduction-normalization/README.md)
- [01-modules/M03-reduction-normalization/call-chains.md](01-modules/M03-reduction-normalization/call-chains.md)
- [01-modules/M03-reduction-normalization/data-structures.md](01-modules/M03-reduction-normalization/data-structures.md)
- [01-modules/M03-reduction-normalization/design.md](01-modules/M03-reduction-normalization/design.md)
- [01-modules/M03-reduction-normalization/diagrams.md](01-modules/M03-reduction-normalization/diagrams.md)
- [01-modules/M03-reduction-normalization/risks-and-debt.md](01-modules/M03-reduction-normalization/risks-and-debt.md)
- [01-modules/M03-reduction-normalization/testing.md](01-modules/M03-reduction-normalization/testing.md)
- [01-modules/M04-indexing-position/README.md](01-modules/M04-indexing-position/README.md)
- [01-modules/M04-indexing-position/development-guide.md](01-modules/M04-indexing-position/development-guide.md)
- [01-modules/M04-indexing-position/diagrams.md](01-modules/M04-indexing-position/diagrams.md)
- [01-modules/M04-indexing-position/interfaces.md](01-modules/M04-indexing-position/interfaces.md)
- [01-modules/M04-indexing-position/source-map.md](01-modules/M04-indexing-position/source-map.md)
- [01-modules/M04-indexing-position/testing.md](01-modules/M04-indexing-position/testing.md)
- [01-modules/M05-gemv-sgemm/README.md](01-modules/M05-gemv-sgemm/README.md)
- [01-modules/M05-gemv-sgemm/call-chains.md](01-modules/M05-gemv-sgemm/call-chains.md)
- [01-modules/M05-gemv-sgemm/design.md](01-modules/M05-gemv-sgemm/design.md)
- [01-modules/M05-gemv-sgemm/diagrams.md](01-modules/M05-gemv-sgemm/diagrams.md)
- [01-modules/M05-gemv-sgemm/performance.md](01-modules/M05-gemv-sgemm/performance.md)
- [01-modules/M05-gemv-sgemm/source-map.md](01-modules/M05-gemv-sgemm/source-map.md)
- [01-modules/M05-gemv-sgemm/testing.md](01-modules/M05-gemv-sgemm/testing.md)
- [01-modules/M06-hgemm-tensorcore/README.md](01-modules/M06-hgemm-tensorcore/README.md)
- [01-modules/M06-hgemm-tensorcore/call-chains.md](01-modules/M06-hgemm-tensorcore/call-chains.md)
- [01-modules/M06-hgemm-tensorcore/data-structures.md](01-modules/M06-hgemm-tensorcore/data-structures.md)
- [01-modules/M06-hgemm-tensorcore/design.md](01-modules/M06-hgemm-tensorcore/design.md)
- [01-modules/M06-hgemm-tensorcore/development-guide.md](01-modules/M06-hgemm-tensorcore/development-guide.md)
- [01-modules/M06-hgemm-tensorcore/diagrams.md](01-modules/M06-hgemm-tensorcore/diagrams.md)
- [01-modules/M06-hgemm-tensorcore/examples.md](01-modules/M06-hgemm-tensorcore/examples.md)
- [01-modules/M06-hgemm-tensorcore/execution-flows.md](01-modules/M06-hgemm-tensorcore/execution-flows.md)
- [01-modules/M06-hgemm-tensorcore/glossary.md](01-modules/M06-hgemm-tensorcore/glossary.md)
- [01-modules/M06-hgemm-tensorcore/implementation.md](01-modules/M06-hgemm-tensorcore/implementation.md)
- [01-modules/M06-hgemm-tensorcore/interfaces.md](01-modules/M06-hgemm-tensorcore/interfaces.md)
- [01-modules/M06-hgemm-tensorcore/line-level-analysis.md](01-modules/M06-hgemm-tensorcore/line-level-analysis.md)
- [01-modules/M06-hgemm-tensorcore/performance.md](01-modules/M06-hgemm-tensorcore/performance.md)
- [01-modules/M06-hgemm-tensorcore/risks-and-debt.md](01-modules/M06-hgemm-tensorcore/risks-and-debt.md)
- [01-modules/M06-hgemm-tensorcore/source-map.md](01-modules/M06-hgemm-tensorcore/source-map.md)
- [01-modules/M06-hgemm-tensorcore/testing.md](01-modules/M06-hgemm-tensorcore/testing.md)
- [01-modules/M06-hgemm-tensorcore/visual-guide.md](01-modules/M06-hgemm-tensorcore/visual-guide.md)
- [01-modules/M07-flash-attention/README.md](01-modules/M07-flash-attention/README.md)
- [01-modules/M07-flash-attention/call-chains.md](01-modules/M07-flash-attention/call-chains.md)
- [01-modules/M07-flash-attention/data-structures.md](01-modules/M07-flash-attention/data-structures.md)
- [01-modules/M07-flash-attention/design.md](01-modules/M07-flash-attention/design.md)
- [01-modules/M07-flash-attention/development-guide.md](01-modules/M07-flash-attention/development-guide.md)
- [01-modules/M07-flash-attention/diagrams.md](01-modules/M07-flash-attention/diagrams.md)
- [01-modules/M07-flash-attention/examples.md](01-modules/M07-flash-attention/examples.md)
- [01-modules/M07-flash-attention/execution-flows.md](01-modules/M07-flash-attention/execution-flows.md)
- [01-modules/M07-flash-attention/implementation.md](01-modules/M07-flash-attention/implementation.md)
- [01-modules/M07-flash-attention/interfaces.md](01-modules/M07-flash-attention/interfaces.md)
- [01-modules/M07-flash-attention/line-level-analysis.md](01-modules/M07-flash-attention/line-level-analysis.md)
- [01-modules/M07-flash-attention/performance.md](01-modules/M07-flash-attention/performance.md)
- [01-modules/M07-flash-attention/risks-and-debt.md](01-modules/M07-flash-attention/risks-and-debt.md)
- [01-modules/M07-flash-attention/source-map.md](01-modules/M07-flash-attention/source-map.md)
- [01-modules/M07-flash-attention/testing.md](01-modules/M07-flash-attention/testing.md)
- [01-modules/M07-flash-attention/visual-guide.md](01-modules/M07-flash-attention/visual-guide.md)
- [01-modules/M08-pytorch-extension/README.md](01-modules/M08-pytorch-extension/README.md)
- [01-modules/M08-pytorch-extension/design.md](01-modules/M08-pytorch-extension/design.md)
- [01-modules/M08-pytorch-extension/development-guide.md](01-modules/M08-pytorch-extension/development-guide.md)
- [01-modules/M08-pytorch-extension/diagrams.md](01-modules/M08-pytorch-extension/diagrams.md)
- [01-modules/M08-pytorch-extension/interfaces.md](01-modules/M08-pytorch-extension/interfaces.md)
- [01-modules/M08-pytorch-extension/risks-and-debt.md](01-modules/M08-pytorch-extension/risks-and-debt.md)
- [01-modules/M08-pytorch-extension/runtime.md](01-modules/M08-pytorch-extension/runtime.md)
- [01-modules/M08-pytorch-extension/testing.md](01-modules/M08-pytorch-extension/testing.md)
- [01-modules/M09-interview-benchmark/README.md](01-modules/M09-interview-benchmark/README.md)
- [01-modules/M09-interview-benchmark/call-chains.md](01-modules/M09-interview-benchmark/call-chains.md)
- [01-modules/M09-interview-benchmark/design.md](01-modules/M09-interview-benchmark/design.md)
- [01-modules/M09-interview-benchmark/development-guide.md](01-modules/M09-interview-benchmark/development-guide.md)
- [01-modules/M09-interview-benchmark/diagrams.md](01-modules/M09-interview-benchmark/diagrams.md)
- [01-modules/M09-interview-benchmark/interfaces.md](01-modules/M09-interview-benchmark/interfaces.md)
- [01-modules/M09-interview-benchmark/risks-and-debt.md](01-modules/M09-interview-benchmark/risks-and-debt.md)
- [01-modules/M09-interview-benchmark/source-map.md](01-modules/M09-interview-benchmark/source-map.md)
- [01-modules/M09-interview-benchmark/testing.md](01-modules/M09-interview-benchmark/testing.md)
- [01-modules/M10-nms/README.md](01-modules/M10-nms/README.md)
- [01-modules/M10-nms/call-chains.md](01-modules/M10-nms/call-chains.md)
- [01-modules/M10-nms/design.md](01-modules/M10-nms/design.md)
- [01-modules/M10-nms/diagrams.md](01-modules/M10-nms/diagrams.md)
- [01-modules/M10-nms/interfaces.md](01-modules/M10-nms/interfaces.md)
- [01-modules/M10-nms/risks-and-debt.md](01-modules/M10-nms/risks-and-debt.md)
- [01-modules/M10-nms/source-map.md](01-modules/M10-nms/source-map.md)
- [01-modules/M10-nms/testing.md](01-modules/M10-nms/testing.md)
- [01-modules/M11-triton-cutlass-profiling/README.md](01-modules/M11-triton-cutlass-profiling/README.md)
- [01-modules/M11-triton-cutlass-profiling/design.md](01-modules/M11-triton-cutlass-profiling/design.md)
- [01-modules/M11-triton-cutlass-profiling/diagrams.md](01-modules/M11-triton-cutlass-profiling/diagrams.md)
- [01-modules/M11-triton-cutlass-profiling/risks-and-debt.md](01-modules/M11-triton-cutlass-profiling/risks-and-debt.md)
- [01-modules/M11-triton-cutlass-profiling/source-map.md](01-modules/M11-triton-cutlass-profiling/source-map.md)
- [01-modules/M11-triton-cutlass-profiling/testing.md](01-modules/M11-triton-cutlass-profiling/testing.md)
- [01-modules/module-registry.md](01-modules/module-registry.md)

### 80-demos
- [80-demos/D01-nms-python/README.md](80-demos/D01-nms-python/README.md)
- [80-demos/D01-nms-python/build-and-run.md](80-demos/D01-nms-python/build-and-run.md)
- [80-demos/D01-nms-python/data-and-state-trace.md](80-demos/D01-nms-python/data-and-state-trace.md)
- [80-demos/D01-nms-python/debug-walkthrough.md](80-demos/D01-nms-python/debug-walkthrough.md)
- [80-demos/D01-nms-python/execution-trace.md](80-demos/D01-nms-python/execution-trace.md)
- [80-demos/D01-nms-python/failure-paths.md](80-demos/D01-nms-python/failure-paths.md)
- [80-demos/D01-nms-python/modification-exercises.md](80-demos/D01-nms-python/modification-exercises.md)
- [80-demos/D02-interview-binary/README.md](80-demos/D02-interview-binary/README.md)
- [80-demos/D02-interview-binary/build-and-run.md](80-demos/D02-interview-binary/build-and-run.md)
- [80-demos/D02-interview-binary/data-and-state-trace.md](80-demos/D02-interview-binary/data-and-state-trace.md)
- [80-demos/D02-interview-binary/debug-walkthrough.md](80-demos/D02-interview-binary/debug-walkthrough.md)
- [80-demos/D02-interview-binary/execution-trace.md](80-demos/D02-interview-binary/execution-trace.md)
- [80-demos/D02-interview-binary/failure-paths.md](80-demos/D02-interview-binary/failure-paths.md)
- [80-demos/D02-interview-binary/modification-exercises.md](80-demos/D02-interview-binary/modification-exercises.md)
- [80-demos/demo-registry.md](80-demos/demo-registry.md)

### 90-cross-module
- [90-cross-module/change-impact-map.md](90-cross-module/change-impact-map.md)
- [90-cross-module/configuration-impact-map.md](90-cross-module/configuration-impact-map.md)
- [90-cross-module/cross-module-call-chains.md](90-cross-module/cross-module-call-chains.md)
- [90-cross-module/end-to-end-flows.md](90-cross-module/end-to-end-flows.md)
- [90-cross-module/error-boundaries.md](90-cross-module/error-boundaries.md)
- [90-cross-module/interface-contracts.md](90-cross-module/interface-contracts.md)
- [90-cross-module/performance-critical-paths.md](90-cross-module/performance-critical-paths.md)
- [90-cross-module/pooling-and-resource-management.md](90-cross-module/pooling-and-resource-management.md)
- [90-cross-module/runtime-trace.md](90-cross-module/runtime-trace.md)
- [90-cross-module/shared-data-and-types.md](90-cross-module/shared-data-and-types.md)
- [90-cross-module/system-wiring.md](90-cross-module/system-wiring.md)

### 99-roadmap
- [99-roadmap/debugging-guide.md](99-roadmap/debugging-guide.md)
- [99-roadmap/feature-development-recipes.md](99-roadmap/feature-development-recipes.md)
- [99-roadmap/next-steps.md](99-roadmap/next-steps.md)
- [99-roadmap/performance-guide.md](99-roadmap/performance-guide.md)
- [99-roadmap/quick-start.md](99-roadmap/quick-start.md)
- [99-roadmap/reading-guide.md](99-roadmap/reading-guide.md)
- [99-roadmap/risk-register.md](99-roadmap/risk-register.md)
- [99-roadmap/technical-debt.md](99-roadmap/technical-debt.md)
- [99-roadmap/testing-recipes.md](99-roadmap/testing-recipes.md)

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档

- [分析状态](00-overview/analysis-state.md)
- [快速上手](99-roadmap/quick-start.md)

## 源码证据摘要

- [README.md:15-16]：项目定位、CUDA kernel/LLM notes、HGEMM/FlashAttention 主线。
- [README.md:53-105]：Interview 构建和 benchmark 命令。
- [CONTRIBUTE.md:3-31]：学习目标、pre-commit 和新增 kernel 流程。

## 未解决问题

- 当前仓库是否为某一 GPU/CI 的强制支持矩阵，源码未定义统一答案。
- 各独立 kernel 对非 contiguous 输入、边界尺寸和错误检查的完整契约需要逐模块实验确认。

## 下一步阅读建议

先读 [architecture.md](00-overview/architecture.md)，然后按目标选择模块；修改代码前阅读对应模块的 `development-guide.md` 与 `testing.md`。
