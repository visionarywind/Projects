# M07 构建与 CI 行级审计

- 文档目的：定位构建和持续集成的实际副作用。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：配置静态确认，运行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M07 README](README.md)
- 后续阅读：[CI 矩阵](ci-matrix.md)
## 结论摘要

本页聚焦 01-modules/M07-packaging-and-ci/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 入口 | 作用 | 证据 |
|---|---|---|
| `CMakeLists.txt` | interface target、安装头、导出 config | `CMakeLists.txt:1-16` |
| `build/makefile` | tests/benchmarks 默认和对象链接 | `build/makefile:28-50` |
| `tests/*/makefile` | run 包装目标 | 各测试目录 |
| `.github/workflows/ci.yml` | native/RISC-V 构建运行 | workflow 文件 |
| `concurrentqueueConfig.cmake.in` | 安装后 target include | config template |

## 修改影响

改变公共头文件需要同时考虑 CMake 安装和 legacy Make 编译；改变 C API `.cpp` 需要 Make 的对象链接和 C API unit tests；改变编译 flags 需要 native 与 RISC-V 条件宏复核。

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
