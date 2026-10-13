# 平台与设备管理：风险与技术债

- 文档目的：platform device：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M03 模块的《平台与设备管理：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 风险 | 影响 | 证据/状态 | 建议 |
|---|---|---|---|
| 函数内静态对象单例 | 析构发生在进程退出 static 析构期，HAL 清理顺序与用户代码释放顺序互相依赖 | 已确认结构 [src/musa/core/platform.cpp:14-17] | 保证所有引用先释放，再触达 ~Platform |
| 设备可见性筛选的 break 语义 | 输入 index 列表含越界/重复 index 时 `break` 提前退出，可能得到空集 -> 误报无设备 | 源码 TODO 未定 [src/musa/core/platform.cpp:69-80] | 明确非法/重复 index 处理并补测试 |
| 惰性初始化失败 | `LaterInit` 失败后 `GetDevice` 返回 nullptr，后续 API 需逐层处理空 device | 已确认 [src/musa/core/platform.cpp:141-150] | 失败时记录日志并给出可诊断信息 |
| HAL 初始化失败路径 | `Hal::CreatePlatform` 失败仅打日志，其余设备构造被跳过；`m_pHalPlatform` 保持空 | 已确认 [src/musa/core/platform.cpp:98-100] | 校验失败后 m_pHalPlatform 是否被安全置空 |
| 资源泄漏窗口 | 用户上下文若未销毁，只能在 `ReleaseUnfreedMemories` 时被解登记，内存可能在退出前一直占用 | 已确认 [src/musa/core/platform.cpp:582-600] | 运行时尽早检测未释放内存 |
| 进程退出清理顺序 | ~Platform 等待设备 -> 释放未回收内存 -> 删 pool -> 销毁 HAL；顺序错会导致 use-after-free | 已确认 [src/musa/core/platform.cpp:610-656] | 保持现有顺序，修改析构前画依赖图 |
| Device 析构顺序 | ~Device 先删用户 context 再删 primary context，copy manager 依赖 primary context | 已确认 [src/musa/core/device.cpp:718-748] | 保持 `m_CpyMgrs` 先置空再 reset primary ctx |
| ABI/导出变化 | 外部程序加载失败 | M02 已确认敏感 | 增加符号回归 |
| 构建环境依赖 | 本地无法复现 | CI/子模块已确认 | 固化依赖和 artifact |

本模块具体风险按其入口和资源类型排序；未执行的测试不标为通过。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（6 处，例：[src/musa/core/platform.cpp:14-17]）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:718-748]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 上表风险的运行期复现与修复验证均未执行。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
