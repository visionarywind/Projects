# M01 源码地图与接口

- 文档目的：解释 01-modules/M01-api-abi/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
