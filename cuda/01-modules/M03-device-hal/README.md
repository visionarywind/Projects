# M03 Device/HAL：设备发现、平台与架构分派

- 文档目的：解释 01-modules/M03-device-hal/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-device-hal/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **职责**：发现和筛选设备、维护 `CUdev` 状态、按 compute capability 选择架构 HAL，并把通用操作交给 RM/MRM/WDDM/amodel 等 DMAL。
- **证据状态**：设备重排和 HAL 静态分派已确认；具体运行时平台配置未知。
- **源码范围**：`src/cui/devmgr.c`、`device.c`、`cuidevice.c`、`dmal/`、`hal/`。

## 设备选择

`devmgr.c` 维护 remap table，校验范围和重复项，并可依据 PCI bus ID 或计算能力排序；还处理 emulation、Windows NVAPI、MPS 和环境筛选（静态确认：[src/cui/devmgr.c:39-121,175-199,201-277]）。因此 `CUdevice` ordinal 不应被简单理解成物理 PCI 顺序。

## HAL 选择

`InitDeviceArch` 按 `device->state.ccArch` switch；Kepler、Maxwell、Pascal、Volta、Turing、Ampere case 只有在对应 `NVCFG(GLOBAL_ARCH_*)` 编译宏启用时才存在（静态确认：[src/cui/hal/hal.c:8-79]）。`isSupportedComputeClass` 和 `bigPageSizeForArch` 采用同样编译时过滤（静态确认：[src/cui/hal/hal.c:82-157]）。

## 外部边界

DMAL 目录包含 RM、MRM、WDDM、amodel、MPS 等平台实现；当前文档确认其文件和接口存在，但没有外部 RM/NVRM 源码，不能将 `dmal.Init` 的最终内核行为写成已确认。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M03-device-hal/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M03-device-hal` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

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
