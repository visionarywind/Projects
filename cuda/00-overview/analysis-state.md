# CUDA 项目分析状态

- **更新时间**：2026-09-11
- **源码根**：`/home/mtuser/workspace/cuda`
- **文档根**：`/home/mtuser/workspace/Projects/cuda`
- **总体状态**：M01–M10 模块文档骨架已建立；核心 API→CUI→资源→HAL/提交主线已静态追踪；M04–M07 的 UVM/GPFIFO/marker/module/syscall 深层生命周期已补证，外部后端与生成工具边界仍有明确缺口。

## 版本与仓库证据

| 项目 | 状态 | 说明 |
|---|---|---|
| CUDA API 版本 | 静态确认 | `inc/cuda.h` 的 `CUDA_VERSION=10020` |
| 内部版本字段 | 静态确认 | `common/version.h` 的 NVCUDA 10.2.0 字段 |
| Git commit/branch | 未知 | 源码目录没有 `.git` |
| 外部依赖版本 | 未知 | common、RM/NVRM、compiler/gpgpucomp、firmware 不完整 |

## 模块状态

| 模块 | 深度 | 已覆盖 | 主要缺口 |
|---|---|---|---|
| M01 API/ABI | 深 | wrapper、版本宏、导出/构建关系、错误回滚 | generator 实际执行与符号最终排序 |
| M02 Runtime/Context | 深 | globals、TLS 检查、context 创建、初始化回滚、sticky error 观察点 | TLS 深层、完整 destroy/thread-exit/fork 生命周期 |
| M03 Device/HAL | 中 | remap/过滤/排序、HAL 编译与运行时选择、DMAL 边界 | 具体目标配置、架构实现、RM 末端 |
| M04 Memory/UVA/UVM | 中→深 | memdesc、memobj、suballocation、device/host alloc/free；UVM manager/managed attributes/DAG、stream state、P2P bookkeeping；AL selection、memobj registration rollback、submemblock mapping | UVM fault/migration backend、IPC/external sharing 的完整运行链 |
| M05 Stream/Submit | 深 | stream pool、QMD、channel 初始化、push、GPFIFO/pushbuffer、tracking marker 和 completion reclaim | 各 DMAL backend 的 doorbell/RM 末端、设备消费 |
| M06 Module/Launch | 深 | launch 分支、参数、tracking、setup、ELF/module/JIT、shared ELF、syscall resource、graph instantiate/launch/unload | 架构具体 ABI/QMD 字段、compiler/gpgpucomp 内部 |
| M07 Kernel/Syscall | 中→深 | syscall init/load/destroy、imports/refcount、resource aggregation、launch callback、trap/timeout/reason buffer；trampoline PC binding、per-arch generated cubin chain | 各 syscall 子实现、CNP queue、compiler/asm/cubin 生成链的设备侧细节 |
| M08 Tools/Debug | 中 | callbacks、skip/blocking、debugger checks、memcheck、profiler | 注册/dispatch 全链、attach protocol、后端细节 |
| M09 OpenCL/Interop | 中 | ICD vendor/platform、固定 dispatch、OpenCL globals | enqueue→CUI、对象生命周期、GL/D3D/external interop |
| M10 Tests/Experiments | 中 | nvmake case/binary、dispatcher、DVS runner、sanity/test map | 全量注册、MODS/package/CI、实际执行 |

## 证据质量规则

- “静态确认”仅表示源码文本支持该结论。
- “推断”用于跨文件或动态分派的合理解释，须说明依据。
- “未知”用于源码快照或外部依赖无法证明的内容。
- “未验证”用于未实际运行的构建、GPU、DVS、OpenCL、debugger、profiler 和系统命令。

## 已完成的静态主线

```text
cuInit
 → api wrapper
 → global mutex/init
 → TLS/globals/UVM/UVA/primary memmgr

cuCtxCreate_v2
 → common argument/context check
 → cuiCtxCreate
 → TLS push

cuMemAlloc_v2
 → CUmemdesc
 → memobjAlloc
 → global registration
 → device pointer

cuStreamCreate
 → stream pool
 → QMD/semaphore/UVM setup
 → public handle

cuLaunchKernel
 → function/stream/context validation
 → capture graph node OR normal launch
 → parameter validation/packing
 → syscall + memory tracking
 → HAL launchCheck/ABI setup
 → stream begin push
 → HAL finalize/control
 → QMD tracking
 → stream end push
 → optional synchronization/rollback
```

## 未执行项目

本轮未执行 nvmake、nvcc、DVS、GPU test、`nvidia-smi`、`sudo nvidia-persistenced`、OpenCL runtime、debugger/profiler、外部依赖安装或服务启动。根仓库允许的文档校验为 `git diff --check`，需在文档修改稳定后执行。

## 下一阶段（非运行时）

补齐 M08/M09/M10 的注册、销毁、interop、MODS/CI，并核对跨模块索引；不得把 DMAL/RM/firmware、compiler/gpgpucomp 或 nvcc 生成物写成当前树已验证行为。

