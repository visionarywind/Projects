# 依赖地图

- 文档目的：记录真实文件依赖和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：构建入口和主要 include 已确认；第三方库内部不展开。
- 最后更新：2026-09-10
- 前置阅读：[总体架构](architecture.md)
- 后续阅读：[模块注册表](../01-modules/module-registry.md)
## 结论摘要

本页聚焦 00-overview/dependency-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


```mermaid
flowchart TB
  H[concurrentqueue.h] --> B[blockingconcurrentqueue.h]
  S[lightweightsemaphore.h] --> B
  H --> C1[c_api/concurrentqueue.cpp]
  B --> C2[c_api/blockingconcurrentqueue.cpp]
  C0[c_api/concurrentqueue.h] --> C1
  C0 --> C2
  H --> U[tests/unittests/unittests.cpp]
  B --> U
  C0 --> U
  H --> F[tests/fuzztests/fuzztests.cpp]
  H --> K[benchmarks/benchmarks.cpp]
  B --> K
  M[build/makefile] --> U
  M --> F
  M --> K
  CM[CMakeLists.txt] --> Install[安装/导出]
```

## 维护边界

| 变更 | 直接影响 | 必须复核 |
|---|---|---|
| 原子协议/producer/block | M01 | M05 unit、Relacy/CDSChecker、目标架构 |
| 等待计数或 signal | M02/M03 | timed/bulk tests、平台分支 |
| C handle 签名 | M04 | C++ 实现、C API tests、ABI 文档 |
| Make/CMake/CI | M07 | native、RISC-V、安装导出 |
| benchmark adapter | M06 | benchmark 编译和结果解析 |

## 构建依赖

根 `CMakeLists.txt` 仅生成 `INTERFACE` target；legacy `build/makefile` 才定义测试和 benchmark 的编译链接。因而“CMake 配置成功”不等价于“测试可执行文件已生成”。详见 [build-and-deploy](build-and-deploy.md)。

## 未知

此图没有把 Boost、dlib、TBB、Relacy 和 CDSChecker 的内部调用展开，因为它们是仓库外部或 vendored 验证依赖；仅记录项目对其构建/运行入口的依赖。

## 下一步

修改公共头文件后，使用 [变更影响图](../90-cross-module/change-impact-map.md) 而不是只编译单个示例。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
