# M03 数据结构

- 文档目的：解释 01-modules/M03-device-hal/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M03-device-hal/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 核心对象

| 对象 | 作用 | 生命周期/所有权 |
|---|---|---|
| `CUdev` | 对外 device ordinal 对应的设备对象 | 由 device manager 建立并由全局设备表持有 |
| `CUdevstate` | 设备能力、架构和计数状态 | 随 `CUdev` 初始化/销毁；具体字段由 HAL 填充 |
| `CUhal` | 通用操作到架构实现的函数表 | 由架构初始化选择并挂在 device 上 |
| `DMAL` 状态 | RM/MRM/WDDM/amodel 后端句柄 | 由平台后端创建，失败时由对应 `Deinit` 回滚 |

## 选择关系

`devmgr` 的 remap table 将用户 ordinal 映射到物理设备；`CUdev.state.ccArch` 再选择 HAL。编译宏决定候选架构函数是否存在，因此设备枚举结果、运行时架构和最终 binary 是三个不同层次（静态确认：[src/cui/devmgr.c:39-121,175-199,201-277]；[src/cui/hal/hal.c:35-79]）。

## 并发边界

设备表排序/筛选属于初始化或设备管理路径；channel 初始化另受 context/manager 锁保护。具体锁顺序和后端句柄字段需结合外部 common/RM 树确认，当前标为未知。

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
