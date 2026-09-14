# M01 实现：公开入口到内部对象

## ABI 兼容

`inc/cuda.h` 将 `cuDeviceTotalMem`、`cuCtxCreate`、`cuMemAlloc`、`cuMemFree` 等旧名映射到 `_v2` 版本，同时根据 `__CUDA_API_VERSION_INTERNAL` 或 per-thread default stream 宏生成 PTDS/PTSZ 名称（静态确认：[inc/cuda.h:76-170]）。内部头文件进一步把公开 CUDA 类型和内部对象前向声明集中起来（静态确认：[src/cuda_types.h:29-34,36-163]）。

## Wrapper 模式

以初始化为例，`cuapiInit` 先处理平台特例，再初始化全局 CriticalSections、验证 Flags 必须为零，最后调用 `cuiInit(CUI_API_CUDA)`，错误原样返回（静态确认：[src/api/apiinit.c:19-47]）。

以内存为例，common wrapper 将输出指针预置为零，取得 TLS 并执行 `cuiInitCheckEx`，检查 stream capture 和参数，构造 `CUmemdesc`，在 context lock 下调用 `memobjAlloc`，随后做全局登记和工具通知（静态确认：[src/api/apimem.c:52-118]）。

以 launch 为例，wrapper 验证 function、stream、grid，并确保 function 与 stream 属于同一 context；capturing stream 进入 graph node 分支，普通 stream 进入 `cuiLaunchKernel_nonreentrant`（静态确认：[src/api/apilaunch.c:212-301]）。

## 导出与构建

`cuda.nvmk` 从 `inc/cuda.h` 生成 API 输入，列出 `src/api` 和 `src/cui` 源文件，并把 `src/cuda_master.def` 纳入导出文件源；Linux 目标名为 `libcuda.so.1.1`（静态确认：[cuda.nvmk:348-376,380-492,1695-1704,1713-1817]）。生成器的实际执行程序和最终符号排序在当前源码边界内未确认。

## 错误语义

wrapper 不能把内部失败统一改成 SUCCESS；例如 `cuapiMemAlloc_common` 在全局注册失败时显式锁 context、释放 `memobj` 后返回原错误（静态确认：[src/api/apimem.c:100-111]）。这类局部回滚是 ABI 层和资源层之间的重要边界。
