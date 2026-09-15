# 跨模块：变更影响地图

- 文档目的：解释 90-cross-module/change-impact-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：影响关系基于构建和调用边界；需按具体符号复核
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 90-cross-module/change-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 变更 | 直接模块 | 必查模块/风险 |
|---|---|---|
| Driver ordinary cache V2/V3、threshold 或 flag | Runtime memory policy / Driver SVM | `halMemAlloc`、产品 CMake、V2 heap trees/V3 range-area、free/shrink 回滚、SoC 矩阵和性能测试 |
| GE Graph/shape/模型格式 | GE | ACL 模型 API、Runtime Execute、算子包、Driver 设备能力 |
| GE Executor stream/event 资源 | GE | Runtime 生命周期、Driver queue/esched、异步测试 |
| ACL 公共 API/错误码 | ACL | Runtime 导出表、GE 调用者、应用 ABI、文档/测试 |
| Runtime C API/handle | Runtime | ACL 包装、GE Executor、Driver HAL、所有客户端 |
| KernelMemoryPool 策略/池大小 | Runtime | Kernel/Program load/free、Driver memory、碎片与资源限制测试 |
| SOMA Segment/重用/trim | Runtime | ACL stream-ordered API、AICPU `SomaMemMng`、HAL/SVM、异步失败回滚和统计测试 |
| 普通 memory flags/policy | Runtime | NpuDriver、HAL/SVM、SoC 产品分支、P2P/huge/Host 回归 |
| Runtime scheduler/memory | Runtime | Stream/Event、DFX、Driver queue、性能/并发回归 |
| HAL 函数/结构体 | Driver | Runtime 链接、SDK-driver、内核 ioctl、所有产品分支 |
| SOMA ioctl/VMM 协议 | Driver | Runtime async config、SVM V2/V3、AICPU/固件、free 失败恢复 |
| esched ioctl ABI | Driver | 用户态 SDK、内核模块、固件/设备调度 |
| SoC 条件编译/设备信息 | Driver/Runtime | ACL 平台缓存、GE 编译目标、产品测试矩阵 |

## 资源变更评估顺序

1. 先确认 API/wrapper、Runtime 内部对象和真正产生副作用的实现，而不是只改公共声明。
2. 对内存变更区分 KernelMemoryPool、SOMA 和普通 device/Host memory，分别检查 ownership、锁、异步完成和失败回滚。
3. 检查 CMake target、安装库、导出表、HAL/SDK-driver ABI、AICPU 参数和动态依赖。
4. 更新错误映射、生命周期文档、DFX 字段和测试矩阵。
5. 分别运行无设备 UT、跨仓集成和真实设备回归；未运行或缺设备时只记录静态证据和未验证边界。
6. 更新 `analysis-state.md` 及证据索引。

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
