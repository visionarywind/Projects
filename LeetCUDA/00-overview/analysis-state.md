# 分析状态、覆盖范围与断点

- 文档目的：支持大仓库分批分析和增量维护。
- 对应源码版本：`main` / `4513b3114de21140c846171042515e17bb907e8a`。
- 最后更新：2026-09-15
- 当前批次：阶段 0-5（盘点、模块划分、总览层、模块层、跨模块层、实践层，以及 M06/M07 代表实现和 D01/D02 Demo 深度初稿）；本轮新增池化与资源管理专题。

## 已完成文档

- 根入口和阅读导航。
- 总览：项目定位、架构、设计原则、运行模型、数据流、依赖、构建部署、错误模型、术语、证据索引、决策记录。
- 模块注册表及 M01-M11 的 README、设计、源码地图、接口/数据结构、调用链、图示、部分行级分析、示例、测试、开发指南、风险债务。
- 跨模块：端到端流程、调用链、共享数据、配置影响、错误边界、变更影响、性能路径。
- 实践：快速上手、阅读路径、调试、功能配方、测试配方、性能指南、风险登记、技术债务、下一步。
- M06 深度代表：staged MMA 的 128×128 tile、dsmem、cp.async/ldmatrix/HMMA、register double buffer、collective store、dynamic SMEM 和 sm90 未测试声明。
- M07 深度代表：tiling-QKV 的 `[B,H,N,D]` offset、Q/K/V shared alias、QKᵀ、online softmax、P@V、O rescale/normalize/store、D/stage/整 tile dispatch。
- D01/D02 Demo：已建立真实源码步骤、数据/资源生命周期、失败路径、调试方法、修改练习和模块双向映射；均未运行验证。
- 关联层补充：已建立 system wiring/interface contracts，并将 M06/M07 pipeline 和 Demo 步骤加入跨模块索引。

## 已确认关键事实

1. 当前目标版本是 `main` 的 `4513b31`。
2. 项目是 CUDA/PyTorch 学习与 benchmark 集合，不是统一生产 runtime。
3. 普通模块多采用 `torch.utils.cpp_extension.load`；HGEMM/FlashAttention 支持 setup/build 变体；interview 用 `nvcc` 二进制。
4. 两个 git submodule 是 CUTLASS 与 cudnn-frontend，`.gitmodules:1-6` 已核对。
5. NMS 最近修复了 keep race、未初始化读和索引语义；当前实现包含两阶段 mask/resolve 和 stable sort/index mapping。
6. interview 的公共层次为 `common.cuh` → `base.cuh` → `sgemv/sgemm/hgemm/flash_attn` → `notes-v2.cu`。
7. benchmark 通过 warmup 和 CUDA synchronize 后计时，但没有全仓库统一 harness。

## 覆盖范围

| 维度 | 当前状态 | 缺口 |
|---|---|---|
| 目录覆盖 | 已归类顶层 `kernels`、`others`、`slides`、`third-party` | 第三方内部未分析；docs/slides 仅记录边界 |
| 模块覆盖 | 11 个稳定职责域，并有模块级文档骨架 | M06/M07 代表实现已深挖；split/CuTe/WGMMA 和小算子未逐文件完整证据 |
| 入口覆盖 | Python、setup.py、makefile、build.sh、pre-commit、主要 CLI | D01/D02 已形成端到端步骤；无统一全仓库入口 |
| 流程覆盖 | 单算子、GEMM、attention、NMS、interview 正常/主要异常 | Demo 运行未验证；更多边界尺寸和清理路径需实测 |
| 符号覆盖 | elementwise/NMS/SGEMM/benchmark 代表符号；interview 总入口；M06/M07 代表 kernel | 深层 Tensor Core 变体、小算子仍需增量 |
| 测试覆盖 | README 中的 Python correctness/benchmark 与 interview tests 已说明 | D01/D02 运行、GPU CI、覆盖率数字未知 |
| 证据覆盖 | 总览、M06/M07 代表实现、D01/D02 关键结论均有路径/行号 | 源码变更后行号需维护；部分推断需逐文件核验 |
| 图示覆盖 | 总体、生命周期、数据流、错误、跨模块、M06 pipeline、M07 online softmax 和 Demo 图已生成 | 各深层变体的精确时序图可继续补充 |
| 开发场景 | 新 kernel、行为/API/配置/修 bug/性能/测试配方已生成 | 各小模块的专属配方可细化 |
| 链接状态 | 知识库内部相对链接已建立并检查过 | 外部/源码证据链接不是 Markdown 文件链接 |

## 未解决问题

- 当前目标 GPU、CUDA/PyTorch/driver 版本和是否允许执行构建测试未由用户另行指定；文档按源码推断并将命令标为未验证。
- 各 kernel 的非 contiguous、空输入、尾部 tile、NaN、stream 和错误检查契约不统一。
- 子模块具体 commit、生成物和外部库版本需要目标环境确认。
- 全仓库是否应引入统一 CMake/pytest/CI 尚无源码决策。
- M06/M07 的 split/CuTe/WGMMA/其它变体和 M02-M05 小算子逐文件符号级证据仍不完整。
- D01/D02 的动态构建、GPU correctness、sanitizer、benchmark 和 profile 尚未执行；无法填写真实输出或性能数字。
- Python/PyTorch tensor、interview host/device buffer、stream/event/handle/workspace/TMA descriptor、shared-memory 生命周期和“无统一 Graph/CPU pool”边界已按 `4513b31` 复核；相关运行行为仍未验证。

## 下一批起点

若继续深挖：

1. 读取本文件、`module-registry.md` 和 `evidence-index.md`，先按 D01/D02 步骤维护行号。
2. 补 M06/M07 split/CuTe/WGMMA/TMA 变体的实际实例化、layout、resource 和架构条件。
3. 补 M02-M05 小算子的公共 launcher 模式与边界差异。
4. 用脚本检查相对链接、Demo 双向映射和证据行号漂移。
5. 如有 CUDA GPU 且获得执行授权，记录环境后执行 D01 fixed/random、D02 单架构 smoke，再执行 HGEMM/FlashAttention correctness、sanitizer 和 profile。
6. 不把“文档步骤已建立”更新为“运行已验证”，除非保留真实命令、版本和输出。

## 维护规则

源码文件变更后，至少更新：对应模块 README/source-map/line-level/testing、受影响的 90-cross-module 文件、`evidence-index.md` 和本状态文件。不要复制整段源码；优先更新符号、行号、证据状态和未解决问题。

## 相关文档

- [module-registry.md](../01-modules/module-registry.md)
- [evidence-index.md](evidence-index.md)
- [../99-roadmap/next-steps.md](../99-roadmap/next-steps.md)

## 下一步阅读建议

从 M08、M09 入手可最快复现实验；从 M06、M07 入手可深入性能关键路径。
