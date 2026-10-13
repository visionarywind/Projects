# 工具与调试接口：设计分析

- 文档目的：tools debug：设计意图、职责边界与关键取舍。
- 适用范围：M10 模块的《工具与调试接口：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

M10 的核心设计问题是：**观测/调试方如何拿到驱动内部状态而不依赖公共 API 演进**。答案是 `muGetExportTable`：消费方以 `Client` 枚举 id 换取一张由 accessor 函数指针组成的导出表。该设计把“表布局”钉死在公共头文件（M13）[src/musa_shared_include/export_table.h:1954-1964]，把“填充”钉在驱动侧单点 `muapiGetExportTable` [src/driver/mu_entry.cpp:1255]。

工具与调试的职责被拆成三层：

- `muInfo` 这类工具走标准 `mu*` API，不依赖导出表 [src/tools/muInfo.cpp:372-388]；
- MUPTI/MUASAN/GDB 走导出表拿到 accessor/hook 控制器 [src/driver/mu_entry.cpp:2164-2304]；
- 驱动侧 hook 存储用 release/acquire 原子标志做门控 [src/driver/mupti/hooks.cpp:9-12]。

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 导出表两种分发模式（静态指针/拷贝） | 高频消费方免拷贝；低频方免生命周期纠缠 | 所有权语义不对称，静态表误释放即破坏进程 | 已确认（`interfaces.md` 模式表） |
| GDB 直接 `dlopen libmusa.so` | 取真实驱动符号，绕过 loader 转发 | 与 M11 loader 路径解析不一致，可能取到不同副本 | 已确认 [src/gdb/musaGdb.cpp:58-78] |
| `AccessorHint` 哨兵预填充 | 未填充槽位可被识别 | 误判即跳 `0xDEADBEEF`，且与“支持但为空”不可区分 | 已确认 [src/musa_shared_include/export_table.h:859-860] |
| tracepoint 内联 + `ready` 门控 | 关闭时零调用开销 | hook 注册晚于命令执行时数据丢失 | 已确认 [src/driver/mupti/tracepoints.h:9-11] |
| 未知 client 回退 MUPTI 表 | 旧版 MUPTI 兼容 | 拼错 id 静默拿错表布局 | 已确认（`FIXME` 标注）[src/driver/mu_entry.cpp:2300-2303] |
| `muInfo` 单错误即退出 | 实现简单、输出确定 | 多设备排障需反复运行 | 已确认 [src/tools/muInfo.cpp:9-20] |

## 不变量

1. 消费方使用任何 accessor 前必须确认其不等于 `AccessorHint`；否则跳到 `0xDEADBEEF`。[src/musa_shared_include/export_table.h:859-860]
2. 静态表族指针（`muptiDriverTable`/`gdbTable` 等）由驱动所有，进程期有效，消费方不得释放。[src/driver/mu_entry.cpp:1471-1494]、[src/driver/mu_entry.cpp:2024-2037]
3. 拷贝模式（`Driver`/`Tools`/`Injection`/`ToolsMuapi`）的目标表内存必须由调用方持有并预置 `AccessorHint`。[src/driver/mu_entry.cpp:2146-2299]
4. hook 回调只在 `ready == true` 且函数指针有效时被调用。[src/driver/muasan/tracepoints.h:12-14]

以上第 1、2、4 项有源码校验路径支持；第 3 项依赖调用方遵守头文件约定，属开发约束而非源码强制。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_entry.cpp`（6 处，例：[src/driver/mu_entry.cpp:1255]）
- `src/musa_shared_include/export_table.h`（3 处，例：[src/musa_shared_include/export_table.h:1954-1964]）
- `src/tools/muInfo.cpp`（2 处，例：[src/tools/muInfo.cpp:372-388]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
