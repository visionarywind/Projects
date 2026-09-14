# M01 源码地图与接口

## 文件地图

| 文件 | 角色 | 关键符号 |
|---|---|---|
| `inc/cuda.h` | 公开声明、版本和宏 | `CUDA_VERSION`、v2/PTDS/PTSZ 宏 |
| `src/api/apiinit.c` | 初始化 wrapper | `cuapiInit` |
| `src/api/apictx.c` | context wrapper | `cuapiCtxCreate_common` |
| `src/api/apimem.c` | 内存 wrapper | `cuapiMemAlloc_common`、`cuapiMemFree_common` |
| `src/api/apistream.c` | stream wrapper | `cuapiStreamCreate_common` |
| `src/api/apilaunch.c` | launch wrapper | `cuapiLaunchKernelCommon` |
| `src/api/apietbl.c` | API 表相关实现 | 生成 API 接口（需生成链核对） |
| `src/cuda_master.def` | 导出符号 | `cuInit`、`cuMemAlloc_v2` 等 |
| `cuda.nvmk` | 对象/导出构建 | `CUDA_GENERATED_API_INPUTS`、`LIBCUDA_OBJECTS` |

## 接口契约

- wrapper 输入：公开句柄、标量、指针和 ABI 版本；输出：`CUresult` 与已初始化的输出参数。
- CUI 输入：TLS、内部 `CUctx`/`CUIstream`/`CUfunc`，调用者通常需遵循 context lock 约束。
- 错误：参数错误由 wrapper 尽早返回；对象/硬件错误向上传递；异步 context 错误可能由后续初始化检查观察。
- 所有权：wrapper 不拥有成功创建后的对象；成功对象由 context、manager 或 pool 持有，失败路径必须调用对应 CUI detach/free。

## 生成链缺口

静态可见 `CUDA_GENERATED_API_INPUTS = inc/cuda.h` 和 `api/apietbl.c`，但生成工具、生成文件版本以及最终导出排序不在当前目录中完整呈现，标记为未知而非推断为某一实现。
