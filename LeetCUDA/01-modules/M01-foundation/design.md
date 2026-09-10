# M01 设计

M01 的设计是让 interview 入口把公共 CUDA 辅助与具体 benchmark 分开：`common.cuh` 聚合平台/指令辅助，`base.cuh` 聚合可讲解基础原语，上层阶段头文件再被 `notes-v2.cu` include。该层是 interview 内部复用层，不是全仓库公共 ABI。

## 关键取舍

- 以单一编译单元和可读注释换取快速学习入口。
- 以架构宏隔离 WGMMA/TMA 等非通用指令。
- 以显式 shared-memory/synchronization 代码展示 CUDA 生命周期。

证据：`[kernels/interview/README.md:6-16]`、`[kernels/interview/notes-v2.cu:1-20]`。
