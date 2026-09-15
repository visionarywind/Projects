# 测试配方

- 文档目的：解释 99-roadmap/testing-recipes.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/testing-recipes.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 静态核对 binary 在 case 前定义，确认 `.cu` device-link 和内部 fatbinary 宏。
2. 为每个改动覆盖成功、参数错误、context/资源错误和清理。
3. 运行环境具备后先 `--listGpus`，再使用明确的 case/模式执行。
4. 阅读 stdout/stderr 和 DVS summary，不把 waived 当 pass；零结果按 runner 失败。

## 资源管理专项

- **Suballocator**：准备同一 descriptor 的小于 generic block size 的两次分配，核对 memblock serial 是否复用；释放中间对象后检查 free region split/coalesce 和 `memblockGetInfo().frag`；再覆盖 `noSuballoc`、固定地址、sharing、descriptor 不兼容和 DMAL failure rollback。
- **Graph**：准备 capture success、invalidated/unjoined、multi-context instantiate、重复 launch、destroy-after-launch 和 exec update topology/function/memory-type failure。核对 QMD、constant-bank、internal stream、completion marker、scheduler backing 和 UVM running 的创建/回收成对性。
- 当前上述场景均未执行；`tests/cuda_test/tests.nvmk` 也没有专用 CUDA Graph/capture case，不能用 channel queued-dependency graph 测试代替。

本知识库未执行任何 nvmake、DVS、GPU 或系统权限命令。

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
