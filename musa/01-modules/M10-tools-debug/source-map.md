# 工具与调试接口：源码地图

- 文档目的：tools debug：源码文件/目录到文档章节的映射。
- 适用范围：M10 模块的《工具与调试接口：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 层 | 路径 | 关注点 |
|---|---|---|
| 工具入口 | `src/tools/muInfo.cpp`、`src/tools/CMakeLists.txt` | `muInfo` 主流程、设备属性查询、构建规则 |
| GDB 客户端 | `src/gdb/musaGdb.{h,cpp}`、`interface.{h,cpp}`、`hooks.h` | `dlopen libmusa.so`、FIFO 消息、accessor 绑定 |
| 驱动观测 | `src/driver/mupti/{hooks,tracepoints}.{h,cpp}` | MUPTI hook 存储、tracepoint 门控与回调 |
| 驱动观测 | `src/driver/muasan/{hooks,tracepoints}.{h,cpp}` | MUASAN hook 存储、kernel/memory tracepoint |
| 驱动观测 | `src/driver/mugdb/{hooks,tracepoints}.{h,cpp}` | GDB 驱动侧存储、调试器加载与 trap 处理 |
| 分发入口 | `src/driver/mu_entry.cpp` | `muapiGetExportTable`、静态导出表、`FillAccessors` |
| 公共契约 | `src/musa_shared_include/export_table.h` | `Client` 枚举、`ExportTable` 布局、`AccessorHint` |

## 关键文件与锚点

| 文件 | 锚点 | 说明 |
|---|---|---|
| `src/tools/muInfo.cpp` | [src/tools/muInfo.cpp:9-20] | `checkMuErrors` 宏：首次错误即打印并 `exit` |
| `src/tools/muInfo.cpp` | [src/tools/muInfo.cpp:119-274] | `GetDeviceProps` 局部静态缓存，批量属性采集 |
| `src/tools/muInfo.cpp` | [src/tools/muInfo.cpp:372-388] | `main`：`muInit` → 逐设备 primary ctx → 打印 |
| `src/gdb/musaGdb.cpp` | [src/gdb/musaGdb.cpp:58-78] | `GetDriverImportTable`：`dlopen("libmusa.so")` 取表 |
| `src/gdb/interface.cpp` | [src/gdb/interface.cpp:38-41] | `InitGdbDriverHooks` 注入 kernel 回调 |
| `src/gdb/interface.cpp` | [src/gdb/interface.cpp:43-70] | `InitGdbDriverAccessors` 绑定 26 个 accessor |
| `src/driver/mu_entry.cpp` | [src/driver/mu_entry.cpp:2146-2155] | `FillAccessors` 逐槽填充循环 |
| `src/driver/mu_entry.cpp` | [src/driver/mu_entry.cpp:2164-2304] | 按 `Client` 分发：静态表/拷贝/默认回退 |

## 证据锚点

[src/tools/CMakeLists.txt:10-18] [src/tools/muInfo.cpp:119-388] [src/gdb/musaGdb.cpp:58-136] [src/driver/mu_entry.cpp:2164-2304] [src/driver/mupti/hooks.cpp:3-16]

## 搜索建议

```bash
rg -n "MUpti::Enter|MUpti::Register|MUasan::|MUgdb::" src/musa/core -g "*.cpp"
rg -n "case Client::" src/driver/mu_entry.cpp
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（4 处，例：[src/tools/muInfo.cpp:9-20]）
- `src/driver/mu_entry.cpp`（3 处，例：[src/driver/mu_entry.cpp:2146-2155]）
- `src/gdb/musaGdb.cpp`（2 处，例：[src/gdb/musaGdb.cpp:58-78]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
