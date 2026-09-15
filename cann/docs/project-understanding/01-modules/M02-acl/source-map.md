# M02 ACL 源码地图

- 文档目的：解释 01-modules/M02-acl/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`50be4c922`
- 证据状态：主要路径已确认，非全量清单
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 路径 | 职责 | 证据/重点 |
|---|---|---|
| `runtime/device.cpp` | 设备选择、重置、查询、同步 | `aclrtSetDeviceImpl` `[acl/runtime/device.cpp:47-59]` |
| `runtime/context.cpp` | Context API | 构建源文件 `[acl/CMakeLists.txt:167-190]` |
| `runtime/stream.cpp` | Stream API | 同上 |
| `runtime/event.cpp` | Event API | 同上 |
| `runtime/memory.cpp`、`allocator.cpp` | 内存分配和释放 | 同上 |
| `runtime/model_ri.cpp` | Runtime model interface | 同上 |
| `runtime/data_buffer.cpp` | 数据 buffer 句柄 | 同上 |
| `runtime/callback.cpp` | 异步回调 | 同上 |
| `runtime/kernel.cpp` | Kernel/算子启动 | 同上 |
| `CMakeLists.txt` | 依赖、源文件和打包 | `[acl/CMakeLists.txt:110-190]` |
| `tests/` | ACL UT 和测试脚本 | `tests/run_test.sh`（仓库文档） |

Runtime 仓还包含公开 ACL Runtime 符号表和 Hook 入口 `[runtime/src/acl/aclrt/acl_rt.cpp:17-104]`；两处代码的最终打包归属需用安装包清单确认。

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
