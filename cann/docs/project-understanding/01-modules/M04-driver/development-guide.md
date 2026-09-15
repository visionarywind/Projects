# M04 Driver 开发指南

- 文档目的：解释 01-modules/M04-driver/development-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 修改前

- 确认变更属于 HAL、SDK-driver 还是内核接口。
- 查 `pkg_inc`、ioctl 头文件、设备节点和目标 SoC 的条件编译。
- 列出资源所有者、锁、DMA 映射、session 和异常清理路径。
- 评估 Runtime/ACL 依赖的 ABI 和错误码影响。

## 修改 SVM/cache 前

1. 先确认目标产品编译的是 V2 还是 V3，不能只看目录名或运行时日志猜测。
2. V2 变更同时检查 heap list、VA/size/mapped 多棵树、`total/size` 不变量和释放失败回滚。
3. V3 变更同时检查 cache flag、`ga_range/ga_area` 双索引、range 完整 idle 判断和 recycle segment。
4. 普通 `rtMalloc` cache 与 SOMA pool 分开评审；size/align/flag/NUMA 改动要同步 Runtime HAL ABI。
5. 修改底层 free、ioctl 或结构体时，保留原始错误、设备、VA、size、flag 和回滚日志。


- 保持用户态/内核态结构体布局和命令号兼容。
- 所有用户指针、长度、数量和设备 ID 先校验；避免整数溢出。
- 在 HDC/queue 生命周期中使用既有锁和状态检查，避免 active session 下释放。
- 不把平台特定代码无条件扩展到其他 SoC。

## 修改后

先运行静态检查和无硬件 UT，再在匹配 Driver/Firmware/NPU 上执行设备测试；记录内核日志、设备 ID、命令和错误码。任何未执行项应明确标注。

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
