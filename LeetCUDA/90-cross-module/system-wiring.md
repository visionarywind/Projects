# 跨模块系统接线图

- 文档目的：解释 90-cross-module/system-wiring.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/system-wiring.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 依赖类型

| 依赖类型 | 示例 | 是否统一 runtime |
|---|---|---|
| 编译/链接 | PyTorch extension loader、CUDAExtension、nvcc、CUTLASS/cudnn headers | 否 |
| 静态源码 | `notes-v2.cu` include `common/base/sgemv/sgemm/hgemm/flash_attn` | 仅 Interview 内 |
| 运行时控制 | Python script、PyBind、CUDA launch、standalone CLI | 各模块独立 |
| 数据 | PyTorch tensor、raw CUDA pointer、host/device buffer | 契约不统一 |
| 生命周期 | PyTorch allocator、CUDA allocation、events/streams/handles/TMA map | 由入口决定 |

## 2. 真实接线

```mermaid
flowchart LR
  PY[Python entry] --> LOAD[torch cpp_extension.load]
  LOAD --> PB[PyBind boundary]
  PB --> K[CUDA kernel]
  K --> OUT[Tensor/output]
  OUT --> REF[reference/check]
  SH[build.sh --arch] --> BIN[notes-v2 binary]
  BIN --> CLI[phase/test/bench CLI]
  CLI --> K2[standalone CUDA paths]
  K2 --> RES[check/sync/D2H/free]
  CUT[third-party CUTLASS/cudnn headers] -.build boundary.-> LOAD
  CUT -.build boundary.-> BIN
```

节点对应 `kernels/*` 的真实入口或外部子模块边界；虚线不表示运行时调用。

## 3. D01/D02 关系

D01 代表 Python/PyTorch ownership 和 dynamic extension；D02 代表 standalone explicit ownership 和 architecture-specific binary。两者共享 CUDA execution concepts，但没有共享的统一 runtime 或调用关系。

## 4. 配置传递

- `TORCH_CUDA_ARCH_LIST`/PyTorch loader：影响普通 extension 编译目标；
- HGEMM stage/swizzle/variant：影响模板、SMEM 和 grid；
- FlashAttention device macro/head dimension：影响 sources/dispatch/tile；
- Interview `--arch`：影响 gencode、宏、库和输出 binary。

配置影响应分别沿 build-time、compile-time、run-time 追踪，不能只记录 CLI 名字。

## 5. 当前状态

系统 wiring 的入口和代表终点已完成；所有小算子和每个 Tensor Core 变体未形成统一 call graph，状态为部分完成。GPU 执行未验证。

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
