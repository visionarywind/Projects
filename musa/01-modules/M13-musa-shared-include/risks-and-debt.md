# 公共 API 头文件层：风险与技术债

- 文档目的：musa shared include：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M13 模块的《公共 API 头文件层：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


本模块是**声明型**代码，风险集中在 ABI 契约与宏正确性，而非运行时逻辑。以下均定位到具体行号。**只记录，不修改源码。**

## 严重

### 1. `AccessorHint` 是隐式契约，误判方向与直觉相反

```cpp
constexpr uintptr_t AccessorHint = 0xDEADBEEF;                                 // src/musa_shared_include/export_table.h:859
#define ADDMEMBER(type, name) type name = reinterpret_cast<type>(AccessorHint)  // src/musa_shared_include/export_table.h:860
```

全文件 652 处 `ADDMEMBER` 都以该魔数初始化函数指针。**这不是"未初始化的野值"，而是驱动侧填充循环的终止哨兵**：驱动在 `muapiGetExportTable` 中逐槽拷贝，遇到目标仍为 `AccessorHint` 就继续、遇到非 `AccessorHint` 就停（[src/driver/mu_entry.cpp:2146-2155]）。

因此 `0xDEADBEEF` 出现在某槽位表示**该槽位待填充**，而非"可以直接调用"。两种情况都会跳转到 `0xDEADBEEF`，但修复方向完全相反。

### 2. `FillAccessors` 遇到源端 0 即中止，其后所有槽位静默不填

```cpp
while (!stop && *pDst == AccessorHint && *pSrc != 0) {   // src/driver/mu_entry.cpp:2150
    *pDst = *pSrc;
    pDst++;
    pSrc++;
}
```

驱动侧 accessor 列表中若出现一个空洞（0），**该分组后续所有 accessor 都不会被填充**，而调用方只看到 `AccessorHint`，无法区分"该功能不支持"与"驱动忘了填"。

同一循环另有两个边界：`pDst`/`pSrc` 任一为空即置 `stop`，中止**所有剩余分组**的填充（[src/driver/mu_entry.cpp:2147-2149]）；若调用方预先把某槽设为非 `AccessorHint` 值，循环在此停止，产生部分填充。

### 3. `MUpti::DriverExportTable` 前 12 个成员被 ABI 锁定

[src/musa_shared_include/export_table.h:1152] 与 [src/musa_shared_include/export_table.h:1164] 的 `legacy interface starts` / `legacy interface ends` 注释表明该区间内的成员顺序被旧版 profiler 依赖。**任何插入、删除或重排都会静默改变 ABI**，且只在接上旧 profiler 时才暴露。

对应的静态表定义在 [src/driver/mu_entry.cpp:1471-1494]，共 21 个分组指针，末位聚合初始化（[src/driver/mu_entry.cpp:1493]），必须与头文件严格同序。

## 高危

### 4. `musa.h` 单体头 29280 行，性能与可维护性风险

`musa.h` [src/musa_shared_include/musa.h:1-29280] 自包含（仅含 `<stdlib.h>` [src/musa_shared_include/musa.h:9] 与 `<stdint.h>` [src/musa_shared_include/musa.h:14]），任何应用包含它都要解析 29280 行声明。`vdpau/vdpau.h` 另有 4987 行。

### 5. `muCtxCreate` 三代版本并存

`muCtxCreate_v2`、`_v3`、`_v4` 同时声明 [src/musa_shared_include/musa.h:43-45]。默认别名指向 `_v2` [src/musa_shared_include/musa.h:43]，但 `_v3`/`_v4` 无别名，应用需显式调用。三者语义差异未在本头文件中说明。

### 6. `muTexRefSetAddress` 别名指向 `_v3`，但 `_v2` 形态另行声明

[src/musa_shared_include/musa.h:83] 定义 `muTexRefSetAddress` → `muTexRefSetAddress2D_v3`，而 [src/musa_shared_include/musa.h:28845] 又单独声明了 `MUresult MUSAAPI muTexRefSetAddress(...)`。同名两形态并存，取决于是否启用 `MUSA_ENABLE_DEPRECATED` [src/musa_shared_include/musa.h:18]，是易错点。

### 7. `MUSA_FORCE_API_VERSION` 是硬 `#error` 而非弃用警告

[src/musa_shared_include/musa.h:29-31] 直接 `#error "MUSA_FORCE_API_VERSION is no longer supported."`。仍在使用该宏的旧应用会编译失败而非降级——行为本身合理（宏已失效），但缺少迁移指引。

## 中危

### 7. 宏别名与末尾 `#undef` 必须严格配对

文件开头定义约 90 条别名 [src/musa_shared_include/musa.h:41-131]，末尾逐一 `#undef` [src/musa_shared_include/musa.h:28830-28852]。新增 API 时若只改一处，会导致重定义错误或递归展开。`muGetProcAddress` 的 inline 变体 [src/musa_shared_include/musa.h:29119] 甚至在 `#undef` 之后再 `#define` 一次同名宏，顺序敏感。

### 8. PTDS 宏改变符号名，且不可见

定义 `MUSA_API_PER_THREAD_DEFAULT_STREAM` 后，`muMemcpy` 等约 7 个符号被重定向到 `_ptds`/`_ptsz` 变体 [src/musa_shared_include/musa.h:122-131]。**`nm` 查到的符号名与源码写的不同**，排障时容易误判符号缺失。`__MUSA_API_VERSION_INTERNAL` 是内部宏 [src/musa_shared_include/musa.h:33]，外部误设同样触发。

### 9. 生成文件不可编辑且无一致性校验

[src/musa_shared_include/generated_musa_meta.h:1] 明确 "Any changes you make will be lost during the next clean build"。生成脚本不在本快照内，无法验证 657+446 个 `*_params_st` 与 `musa.h` 原型的字段顺序是否一致——而 MUPTI 参数拦截**依赖严格一致**。

### 10. `MUdeviceptr` 宽度随平台切换

[src/musa_shared_include/musa.h:222-227]：64 位平台是 `unsigned long long`，否则 `unsigned int`。应用侧若用 `intptr_t` 或硬编码宽度做跨平台序列化会截断。`MUdevice` 则是 `int` [src/musa_shared_include/musa.h:229]，与 CUDA 的 `struct CUdevice_st*` 语义不同。

## 低危

| 问题 | 证据 |
|---|---|
| 54 处 `__MUSA_DEPRECATED` API 仍在头文件中，应用可继续调用 | [src/musa_shared_include/musa.h:6560]、[src/musa_shared_include/musa.h:6594]、[src/musa_shared_include/musa.h:8165]、[src/musa_shared_include/musa.h:8201] 等 |
| `MUdeviceptr_v2`、`MUdevice_v1`、`MUtexObject_v1`、`MUsurfObject_v1` 保留旧名但无文档说明迁移路径 | [src/musa_shared_include/musa.h:222]-[src/musa_shared_include/musa.h:246] |
| `Musa::GraphExec` 与 `GraphExec2` 并存前置声明 | [src/musa_shared_include/export_table.h:33]、[src/musa_shared_include/export_table.h:34] |
| `export_table.h` 包含 `<string>`、`<list>`、`<fstream>`、`<algorithm>`、`<atomic>`，其中 `<fstream>` 与 accessor 表无关 | [src/musa_shared_include/export_table.h:3-8] |
| `ogl/` 下 7 个 OpenGL 头与 `glew.h`、`glcorearb.h` 并存，存在互相覆盖风险 | `ogl/GL/` 目录 |
| Windows 专属 `dxgi.h`/`d3d11.h` 在头文件顶层引入，污染所有 Windows 编译单元 | [src/musa_shared_include/export_table.h:17-22] |

## 已解决

- 8 个 `ExportTable` 的填充方已定位：`muapiGetExportTable` [src/driver/mu_entry.cpp:1255]，分派 switch 在 [src/driver/mu_entry.cpp:2164-2304]，客户端枚举 `Client` 逐值定义在 [src/musa_shared_include/export_table.h:1955]-[src/musa_shared_include/export_table.h:1963]。

## 未解决问题

1. `generated_*_meta.h` 的生成脚本不在本快照，参数结构与原型一致性无法验证。
2. `muCtxCreate_v3`/`_v4` 相对 `_v2` 的语义差异未确认。
3. `vdpau/vdpau.h`（4987 行）未逐项分析。
4. `Client` 枚举的实际消费者仅确认调试器一项：client id 取 [src/gdb/musaGdb.cpp:68]。
   另三类（工具表、兼容 API 表、注入表）的调用方不在本快照——`src/tools` 只构建 `muInfo`，而 `muInfo` 不使用导出表。

## 相关文档

- [`README.md`](README.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/musa_shared_include/musa.h`（22 处，例：[src/musa_shared_include/musa.h:1-29280]）
- `src/musa_shared_include/export_table.h`（8 处，例：[src/musa_shared_include/export_table.h:1152]）
- `src/driver/mu_entry.cpp`（6 处，例：[src/driver/mu_entry.cpp:2146-2155]）

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
