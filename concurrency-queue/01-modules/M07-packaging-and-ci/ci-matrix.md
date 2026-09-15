# M07 CI 矩阵

- 文档目的：记录 CI 已声明的环境，而不是把它扩展成未存在的发布流程。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：workflow 静态确认；当前工作站未执行。
- 最后更新：2026-09-10
- 前置阅读：[M07 README](README.md)
- 后续阅读：[构建总览](../../00-overview/build-and-deploy.md)
## 结论摘要

本页聚焦 01-modules/M07-packaging-and-ci/ci-matrix.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 维度 | CI 路径 | 备注 |
|---|---|---|
| native | GNU build + `./bin/unittests --disable-prompt --iterations 1` | 单元 smoke |
| RISC-V | cross compile + `qemu-riscv64-static -L /usr/riscv64-linux-gnu` | 同类 unit smoke |
| fuzz | 未确认 CI 直接运行 | 不能据此声称覆盖 |
| benchmark | 未确认 CI 直接运行 | 需手动构建/运行 |
| CMake install | 配置文件存在；CI 是否运行需 workflow 证据 | 未扩大结论 |

CI 通过只说明该 workflow 中的步骤通过，不说明所有平台、场景或性能目标均通过。

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
