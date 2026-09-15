# M01 实现：公开入口到内部对象

- 文档目的：解释 01-modules/M01-api-abi/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## ABI 兼容

`inc/cuda.h` 将 `cuDeviceTotalMem`、`cuCtxCreate`、`cuMemAlloc`、`cuMemFree` 等旧名映射到 `_v2` 版本，同时根据 `__CUDA_API_VERSION_INTERNAL` 或 per-thread default stream 宏生成 PTDS/PTSZ 名称（静态确认：[inc/cuda.h:76-170]）。内部头文件进一步把公开 CUDA 类型和内部对象前向声明集中起来（静态确认：[src/cuda_types.h:29-34,36-163]）。

## Wrapper 模式

以初始化为例，`cuapiInit` 先处理平台特例，再初始化全局 CriticalSections、验证 Flags 必须为零，最后调用 `cuiInit(CUI_API_CUDA)`，错误原样返回（静态确认：[src/api/apiinit.c:19-47]）。

以内存为例，common wrapper 将输出指针预置为零，取得 TLS 并执行 `cuiInitCheckEx`，检查 stream capture 和参数，构造 `CUmemdesc`，在 context lock 下调用 `memobjAlloc`，随后做全局登记和工具通知（静态确认：[src/api/apimem.c:52-118]）。

以 launch 为例，wrapper 验证 function、stream、grid，并确保 function 与 stream 属于同一 context；capturing stream 进入 graph node 分支，普通 stream 进入 `cuiLaunchKernel_nonreentrant`（静态确认：[src/api/apilaunch.c:212-301]）。

## 导出与构建

`cuda.nvmk` 从 `inc/cuda.h` 生成 API 输入，列出 `src/api` 和 `src/cui` 源文件，并把 `src/cuda_master.def` 纳入导出文件源；Linux 目标名为 `libcuda.so.1.1`（静态确认：[cuda.nvmk:348-376,380-492,1695-1704,1713-1817]）。生成器的实际执行程序和最终符号排序在当前源码边界内未确认。

## 错误语义

wrapper 不能把内部失败统一改成 SUCCESS；例如 `cuapiMemAlloc_common` 在全局注册失败时显式锁 context、释放 `memobj` 后返回原错误（静态确认：[src/api/apimem.c:100-111]）。这类局部回滚是 ABI 层和资源层之间的重要边界。

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
