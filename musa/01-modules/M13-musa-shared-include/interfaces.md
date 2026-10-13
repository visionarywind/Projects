# 公共 API 头文件层：接口

- 文档目的：musa shared include：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M13 模块的《公共 API 头文件层：接口》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## API 版本

| 项 | 值/位置 | 证据 |
|---|---|---|
| `MUSA_VERSION` | `50300`（MAJOR×10000 + MINOR×100 + PATCH，即 5.3.0） | [src/musa_shared_include/musa.h:212] |
| 版本管理文档组 | `\defgroup MUSA_VERSION` | [src/musa_shared_include/musa.h:5746]-[src/musa_shared_include/musa.h:5781] |
| 运行期版本查询 | `muDriverGetVersion(int* driverVersion)` | [src/musa_shared_include/musa.h:27240] 附近 |

## `MUSAAPI` 调用约定

| 平台 | 定义 | 证据 |
|---|---|---|
| Windows | `__stdcall` | [src/musa_shared_include/musa.h:5647] |
| 其他 | 空 | [src/musa_shared_include/musa.h:5649] |

GCC 下支持 `__MUSA_API_PUSH_VISIBILITY_DEFAULT` 配合 `#pragma GCC visibility push(default)` [src/musa_shared_include/musa.h:5640-5645]。

## 两套 API 命名族

| 族 | 命名 | 示例 | 证据 |
|---|---|---|---|
| driver API（`mu*`） | 直接导出，无前缀 | `muInit` [src/musa_shared_include/musa.h:5741]、`muStreamCreate` [src/musa_shared_include/musa.h:16618] | [src/musa_shared_include/musa.h] |
| 兼容 API（`muapi*`） | `muapi` + 原名 | `muapiMemUnmap`、`muapiMemSetAccess` | [src/musa_shared_include/export_table.h:1938-1940] |

`muapi*` 兼容层由 `src/driver/mu_wrappers_generated.cpp` 生成，其导出通过 M10 的 `ToolsMuapi::ExportTable` 暴露，M11 loader 不参与该族。

## API 版本化别名机制

`musa.h` 大量使用 `#define 旧名 新名_vN` 形式，把旧符号映射到带版本后缀的真实实现。典型段落 [src/musa_shared_include/musa.h:41-108]：

```c
#define muDeviceTotalMem                    muDeviceTotalMem_v2
#define muCtxCreate                         muCtxCreate_v2
#define muMemAlloc                          muMemAlloc_v2
#define muGetProcAddress                    muGetProcAddress_v2
#define muGraphInstantiate                  muGraphInstantiateWithFlags
```

带 PTDS 变体的则经 `__MUSA_API_PTDS` / `__MUSA_API_PTSZ` 包装 [src/musa_shared_include/musa.h:37-38]：

```c
#define muMemcpyHtoD      __MUSA_API_PTDS(muMemcpyHtoD_v2)
#define muMemcpyHtoDAsync __MUSA_API_PTSZ(muMemcpyHtoDAsync_v2)
```

`muCtxCreate` 有 `_v2`、`_v3`、`_v4` 三代并存 [src/musa_shared_include/musa.h:43-45]；`muTexRefSetAddress` 映射到 `_v3` [src/musa_shared_include/musa.h:83]，其旧版通过末尾 `#undef` 保留声明 [src/musa_shared_include/musa.h:28845]。

### PTDS（Per-Thread Default Stream）

| 条件 | 效果 | 证据 |
|---|---|---|
| 定义 `__MUSA_API_VERSION_INTERNAL` 或 `MUSA_API_PER_THREAD_DEFAULT_STREAM` | `__MUSA_API_PTDS(api)` → `api##_ptds`；`__MUSA_API_PTSZ(api)` → `api##_ptsz` | [src/musa_shared_include/musa.h:33-39] |
| 否则 | 宏为空展开，符号名不变 | [src/musa_shared_include/musa.h:36-38] |

PTDS 生效时，`muMemcpy`、`muMemcpyAsync`、`muMemcpyPeer`、`muMemcpyPeerAsync`、`muMemcpy3DPeer`、`muMemcpy3DPeerAsync`、`muMemPrefetchAsync` 等会被重定向到 `_ptds`/`_ptsz` 变体 [src/musa_shared_include/musa.h:122-131]。

对应的 inline 包装在文件末尾 [src/musa_shared_include/musa.h:29105-29119]，例如：

```c
static inline MUresult muGetProcAddress_v2_ptsz(const char *symbol, void **funcPtr,
    int driverVersion, muuint64_t flags, MUdriverProcAddressQueryResult *symbolStatus) {
    ...
    return muGetProcAddress_v2(symbol, funcPtr, driverVersion, flags, symbolStatus);
}
#define muGetProcAddress_v2 muGetProcAddress_v2_ptsz
```

主声明在 [src/musa_shared_include/musa.h:27237]；末尾的 `#undef muGetProcAddress` [src/musa_shared_include/musa.h:28838] 撤销了开头的别名，避免递归。

## 动态符号查询

唯一公开的符号查询入口：

| 项 | 内容 | 证据 |
|---|---|---|
| 声明 | `MUresult MUSAAPI muGetProcAddress(const char* symbol, void** pfn, int musaVersion, muuint64_t flags, MUdriverProcAddressQueryResult* symbolStatus)` | [src/musa_shared_include/musa.h:27237] |
| 别名 | `muGetProcAddress` → `muGetProcAddress_v2` | [src/musa_shared_include/musa.h:108] |
| PTDS 变体 | `muGetProcAddress_v2_ptsz` | [src/musa_shared_include/musa.h:29111] |
| flags 说明 | 搜索选项标志 | [src/musa_shared_include/musa.h:2502] |
| 状态说明 | 搜索结果状态标志 | [src/musa_shared_include/musa.h:2511] |
| 函数指针类型 | `muGetProcAddress_v2_fn` | [src/musa_shared_include/export_table.h:701] |

`musaVersion` 参数对应 `MUSA_VERSION`，即驱动只返回不高于该版本的实现。

## 废弃标记

`__MUSA_DEPRECATED` 定义按平台分四路 [src/musa_shared_include/musa.h:18-26]：

| 条件 | 定义 | 证据 |
|---|---|---|
| `__MUSA_API_VERSION_INTERNAL` 或 `__DOXYGEN_ONLY__` 或 `MUSA_ENABLE_DEPRECATED` | 空 | [src/musa_shared_include/musa.h:19-20] |
| `_MSC_VER` | `__declspec(deprecated)` | [src/musa_shared_include/musa.h:21-22] |
| `__GNUC__` | `__attribute__((deprecated))` | [src/musa_shared_include/musa.h:23-24] |
| 其他 | 空 | [src/musa_shared_include/musa.h:26] |

全头文件共 54 处使用。典型：

| API | 证据 |
|---|---|
| `muDeviceGetProperties` | [src/musa_shared_include/musa.h:6560] |
| `muDeviceComputeCapability` | [src/musa_shared_include/musa.h:6594] |
| `muCtxAttach` | [src/musa_shared_include/musa.h:8165] |
| `muCtxDetach` | [src/musa_shared_include/musa.h:8201] |

## 平台条件编译

| 条件 | 位置 | 效果 |
|---|---|---|
| `_WIN32` | [src/musa_shared_include/musa.h:5646] | `MUSAAPI` = `__stdcall` |
| `_WIN32` | [src/musa_shared_include/export_table.h:17-22] | 额外包含 `dxgi.h`、`d3d11.h`、`musaD3D11.h` |
| `MUSA_FORCE_API_VERSION` | [src/musa_shared_include/musa.h:29-31] | 直接 `#error`，该宏已废弃 |
| `_WIN64` / `__LP64__` | [src/musa_shared_include/musa.h:222-227] | `MUdeviceptr` 为 64 位，否则 32 位 |

## 子系统入口

| 子系统 | 主头 | 关键符号 | 证据 |
|---|---|---|---|
| MUPTI driver | `mupti/mupti_driver_cbid.h` | 回调 ID 定义 | [src/musa_shared_include/export_table.h:13] |
| MUPTI runtime | `mupti/mupti_runtime_cbid.h` | 回调 ID 定义 | [src/musa_shared_include/export_table.h:14] |
| Raytracing | `raytracing/raytracing.h` | `muInternalBvhDescriptorEncode` 等 | [src/musa_shared_include/export_table.h:1677]、[src/musa_shared_include/export_table.h:1678] |
| VDPAU | `vdpau/vdpau.h` | 视频互操作（4987 行） | 本文件 |
| GDB | `mugdb/bphandler.h`、`protocol.h`、`pack.h` | `MUgdb::ExportTable` | [src/musa_shared_include/export_table.h:1760] |
| MUASAN | — | `MUasan::ExportTable` | [src/musa_shared_include/export_table.h:1830] |
| OpenGL | `ogl/GL/gl.h`、`glcorearb.h`、`glew.h`、`glext.h` | GL 兼容层 | 本目录 |
| D3D11 | `musaD3D11.h` | `muD3D11GetDevice` 等 | [src/musa_shared_include/musaD3D11.h] |

M11 loader 的 Windows 专属清单 [src/loader/windows_api_list.inc:2-4] 声明的正是 `musaD3D11.h` 中的三个入口。

## 相关文档

- [`README.md`](README.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/musa_shared_include/musa.h`（38 处，例：[src/musa_shared_include/musa.h:212]）
- `src/musa_shared_include/export_table.h`（9 处，例：[src/musa_shared_include/export_table.h:1938-1940]）
- `src/musa_shared_include/musaD3D11.h`（1 处，例：[src/musa_shared_include/musaD3D11.h]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
