# 工具与调试接口：风险与技术债

- 文档目的：tools debug：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M10 模块的《工具与调试接口：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


以下条目均定位到具体行号。**只记录，不修改源码。**

## 严重

### 1. 未知 client id 静默返回 MUPTI 表

```cpp
default:
    /* FIXME(Zhangjian.Zhao): backward compatible for current MUPTI, return INVALID_VALUE when MUPTI updated. */
    *ppExportTable = &muptiDriverTable;
    break;
```

[src/driver/mu_entry.cpp:2300-2303]。任何拼错的 client id 都会拿到 MUPTI 导出表而非报错。调用方若随后按错误的表布局解释内存，是难以定位的内存损坏。注释本身已标注这是待修的临时措施。

### 2. `AccessorHint` 语义反直觉，误判即跳野地址

`0xDEADBEEF` [src/musa_shared_include/export_table.h:859] 表示**待填充**而非"可调用"，但两者都会导致跳转到 `0xDEADBEEF`，修复方向相反。652 处 accessor 成员 [src/musa_shared_include/export_table.h:860] 全部依赖该隐式契约。

### 3. `FillAccessors` 遇源端 0 即中止，其后槽位静默不填

```cpp
while (!stop && *pDst == AccessorHint && *pSrc != 0) {   // src/driver/mu_entry.cpp:2150
```

[src/driver/mu_entry.cpp:2146-2155]。驱动侧 accessor 列表出现一个空洞（0），该分组后续全部不被填充；调用方只看到 `AccessorHint`，无法区分"不支持"与"漏填"。另有两处边界：`pDst`/`pSrc` 为空即置 `stop` 并中止**所有剩余分组**（[src/driver/mu_entry.cpp:2147-2149]）；调用方预置非 `AccessorHint` 值会产生部分填充。

## 高危

### 4. `MUpti::DriverExportTable` 前 12 个成员被 ABI 锁定

[src/musa_shared_include/export_table.h:1152]、[src/musa_shared_include/export_table.h:1164] 的 legacy 注释界定了不可重排区间。对应静态表 [src/driver/mu_entry.cpp:1471-1494] 必须与头文件严格同序，否则旧 profiler 静默错位。

### 5. 静态表族的所有权未在接口上表达

返回指针的五个分支（`MUpti`、`Runtime`、`MUgdb`、`MUasan`、`Photon`）交出驱动静态表的地址。若调用方误判为自有的表并释放，会破坏进程内驱动状态。接口无任何所有权标注。

### 6. GDB 客户端绕过 loader 直接 `dlopen`

[src/gdb/musaGdb.cpp:60] 用 `Util::Library musaLib("libmusa.so")` 直接加载驱动，与 M11 loader 的路径解析机制（DriverStore 包 ID 比对）不同。两条路径可能定位到不同副本。

## 中危

### 7. `Client` 枚举无版本字段

[src/musa_shared_include/export_table.h:1955]-[src/musa_shared_include/export_table.h:1963] 只有 9 个枚举值，无版本号、无能力位。消费方无法判断某 accessor 分组在新旧驱动间是否语义一致，只能靠填充哨兵（见本文件风险 2）兜底。

### 8. `muInfo` 错误处理即退出，无法批量诊断

`checkMuErrors` [src/tools/muInfo.cpp:9-20] 首次失败即打印并 `exit(EXIT_FAILURE)`。一次运行只能暴露一个错误，多设备环境下排障需反复运行。

### 9. `muInfo` 属性缓存使首个设备承担全部 API 调用

`GetDeviceProps` 的函数内 static [src/tools/muInfo.cpp:119-274] 在首次调用时完成全部设备采集。失败若发生在该 lambda 内，`checkMuErrors` 会直接退出；缓存建立后失败点难以定位到具体字段。

## 低危

| 问题 | 证据 |
|---|---|
| loader 阶段错误原因不在 `muInfo` 输出中体现 | [src/loader/loader.cpp:58-71]、[src/tools/muInfo.cpp:9-20] |
| `probe.cpp`（唯一 loader 诊断工具）不参与构建 | [src/loader/CMakeLists.txt:74-93] |
| `src/tools/CMakeLists.txt` 只构建 `muInfo`，三类工具表在本地无消费方 | [src/tools/CMakeLists.txt:10-18] |

## 未解决问题

1. `Driver`、`Tools`、`Injection`、`ToolsMuapi` 四张表的消费方不在本快照，无法核对它们是否都做了 `AccessorHint` 检查。
2. 驱动侧各 accessor 源列表是否含 0 空洞未逐项核对。
3. `src/hal/m3d/m3d` 私有子模块内的链路仍未展开。
4. `gdbTable`（约 23 组）与 `asanTable`（约 18 组）的 accessor 未逐项展开。

## 相关文档

- [`call-chains.md`](call-chains.md)
- [`interfaces.md`](interfaces.md)
- [`../M13-musa-shared-include/data-structures.md`](../M13-musa-shared-include/data-structures.md)
- [`../M11-loader/call-chains.md`](../M11-loader/call-chains.md)

## 源码证据摘要

- `src/musa_shared_include/export_table.h`（6 处，例：[src/musa_shared_include/export_table.h:859]）
- `src/driver/mu_entry.cpp`（4 处，例：[src/driver/mu_entry.cpp:2300-2303]）
- `src/tools/muInfo.cpp`（3 处，例：[src/tools/muInfo.cpp:9-20]）

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
