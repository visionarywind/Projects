# M11 Triton、CUTLASS 与 Profiling

- 文档目的：定位非主线示例、第三方模板和性能分析资料的边界。
- 适用范围：`kernels/openai-triton/`, `kernels/cutlass/`, `kernels/nvidia-nsight/`, `third-party/`。
- 对应源码版本：`4513b31`。
- 证据状态：目录和用途已确认；示例逐个行为未验证。
- 最后更新：2026-09-10
- 前置阅读：[dependency-map](../../00-overview/dependency-map.md)
- 后续阅读：[性能路径](../../90-cross-module/performance-critical-paths.md)

## 结论摘要

M11 是比较和工具材料集合：OpenAI Triton 示例展示另一种 GPU DSL；`kernels/cutlass` 包含 CuTe/CUTLASS 示例；`nvidia-nsight` 保存 profiling kernel/说明；`third-party` 是 git submodule，不属于本项目实现。它们可帮助解释 M06/M07 的布局与性能，但不是统一依赖层。

## 边界

只在明确被 setup.py/makefile/include 使用时追踪第三方影响；不复制第三方源码。profiling 输出、缓存和生成物不纳入知识库。

## 测试建议

每个 Triton/CuTe example 单独检查版本、GPU capability 和 reference output；Nsight 依赖真实 GPU，文档命令标为未验证，不能把历史 profile 当当前数据。

## 相关文档

- [design.md](design.md)
- [source-map.md](source-map.md)
- [testing.md](testing.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `.gitmodules:1-5`：CUTLASS/cudnn-frontend。
- `kernels/cutlass/`、`kernels/openai-triton/`、`kernels/nvidia-nsight/` 文件清单。

## 未解决问题

第三方 submodule checkout 版本和各 DSL 的环境支持矩阵未确认。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M11-triton-cutlass-profiling/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
