# 设计决策、冲突与取舍记录

- 文档目的：记录源码/文档中能观察到的决策，避免把推断写成事实。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`4513b31`。
- 证据状态：决策事实已确认，动机部分为推断。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/decision-log.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 决策记录

| ID | 决策 | 证据 | 收益 | 代价/边界 |
|---|---|---|---|---|
| D01 | 以教学和可读性优先于生产最优 | `CONTRIBUTE.md:2-3` | 适合逐步学习和 PR | 不能等价替代官方库 |
| D02 | 普通示例采用 PyTorch `load()` | `elementwise.py:9-24` | 修改后可快速编译运行 | 首次编译/缓存/ABI 敏感 |
| D03 | HGEMM 同时保留 WMMA/MMA/CuTe/cublas 基线 | `hgemm.py:58-148`、README | 可量化优化增量 | 大量接口/编译组合 |
| D04 | Interview 使用单一 `notes-v2.cu` 和架构 build script | `interview/README.md:6-16`、`build.sh` | 复习和发布方便 | 编译单元大、硬件耦合 |
| D05 | NMS 用两阶段 mask/resolve | `nms.cu:9-109` | phase 1 无写冲突，phase 2 有序决策 | resolve 是单 block 顺序瓶颈 |
| D06 | 用 stable score sort 并映射原始索引 | `nms.cu:152-189` | 与 torchvision 输出语义一致 | 需要额外排序/CPU 映射 |
| D07 | 用 padding/swizzle 处理 shared-memory bank conflict | `hgemm/README.md:195-204` | 改善访存 | layout、硬件、调参耦合 |

## 冲突/差异

- README 的部分 benchmark 使用历史硬件/版本；不能与当前 build flags 或其他机器的结果直接比较。
- `kernels/nms/nms.cc` 使用 `+1` 像素面积，而 GPU `nms.cu` 使用连续坐标面积；NMS README 已明确这是已知语义差异。[kernels/nms/README.md:14-16]
- 根 README 的示例架构列表与 `build.sh` 当前 `VALID_ARCHS` 可能随提交演进；本知识库以当前脚本源码为准，并在使用前检查脚本。

## 未解决决策

- 是否应引入仓库级统一构建系统/依赖锁定。
- 是否将普通脚本的 import-time benchmark 改成 `if __name__ == "__main__"`。
- 是否为所有 extension 统一输入检查、stream 与错误检查。

## 相关文档

- [design-principles.md](design-principles.md)
- [analysis-state.md](analysis-state.md)

## 源码证据摘要

见上表及 [evidence-index.md](evidence-index.md)。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议

涉及行为修改时先判断是稳定契约还是教学实现细节，再更新对应决策记录。
