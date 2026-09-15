# CUDA 项目分析状态

- 文档目的：解释 00-overview/analysis-state.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/analysis-state.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **更新时间**：2026-09-14
- **源码根**：`source/cuda`
- **文档根**：`/home/mtuser/workspace/Projects/cuda`
- **总体状态**：M01–M10 模块文档骨架已建立；核心 API→CUI→资源→HAL/提交主线已静态追踪；M04–M07 的 UVM/GPFIFO/marker/module/syscall 深层生命周期以及 M08–M10 的工具、OpenCL 对象/interop、测试聚合语义已补证，外部后端与生成工具边界仍有明确缺口。

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
| M04 Memory/UVA/UVM | 深（静态） | memdesc、memobj、memblock、descriptor-compatible suballocator、best-fit/split/coalesce、device/host alloc/free；UVM manager/managed attributes/DAG、stream state、P2P bookkeeping；AL selection、memobj registration rollback、submemblock mapping | UVM fault/migration backend、IPC/external sharing 的完整运行链、DMAL 物理实现和运行时碎片验证 |
| M05 Stream/Submit | 深 | stream pool、QMD、channel 初始化、push、GPFIFO/pushbuffer、tracking marker 和 completion reclaim | 各 DMAL backend 的 doorbell/RM 末端、设备消费 |
| M06 Module/Launch | 深（静态） | launch 分支、参数、tracking、setup、ELF/module/JIT、shared ELF、syscall resource、graph capture/instantiate/launch/update/destroy、clone/flatten、memset lowering、QMD chaining/pushbuffer/device scheduler、per-context 资源 reverse path | 架构具体 ABI/QMD 字段、compiler/gpgpucomp 内部、Graph 专用运行时测试和失败疑点复现 |
| M07 Kernel/Syscall | 中→深 | syscall init/load/destroy、imports/refcount、resource aggregation、launch callback、trap/timeout/reason buffer；trampoline PC binding、per-arch generated cubin chain | 各 syscall 子实现、CNP queue、compiler/asm/cubin 生成链的设备侧细节 |
| M08 Tools/Debug | 中→深（host-side） | callbacks、skip/blocking、debugger checks/shared state、memcheck device table、profiler mode/perfmon | 外部 attach protocol、helper/backend、DRS/runtime client |
| M09 OpenCL/Interop | 中→深（host-side） | ICD/vendor、固定 dispatch、public/internal object tree、context destroy、event marker aggregation、GL registration、external memobj | enqueue worker 全链、GL/D3D fence/RM、handle close、运行验证 |
| M10 Tests/Experiments | 中→深（构建/聚合） | binary/case registration、device-link、dispatcher、runner pipe/timeout/no-result/failure/waived、DVS package | 全量 case、MODS/CI、硬件执行 |

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

## 资源管理专题审查结果

### GPU 显存池化

- **已确认**：现行通用池化是 `CUmemblock` 内的 `CUsuballocatorRadixTree`，以 descriptor compatibility 分组，按 free-region size best-fit，split 后在 free 时 coalesce（`src/cui/memobj.c:265-375`；`src/cui/suballocator.c:163-220,343-405`）。
- **已确认**：新 block 的物理/平台资源由 `DMAL.memblockAlloc` 申请，释放由 `DMAL.memblockFree` 完成；`CUheap` 只做 VA reservation/lookup（`src/cui/memblock.c:471-562,682-740`；`src/cui/heap.c:24-35`）。
- **遗留疑点**：`CUmembins`/`membin` 仍在头文件和测试计划中，但当前树未发现初始化/使用路径；不能把 5-bin 策略写成已启用（`src/cuda_mem.h:173-182`；`src/cui/memmgr.h:21-23,85-86`）。

### CUDA Graph

- **已确认**：instantiate 为每个 context 分配 QMD、constant-bank、HAL staging、internal stream/marker，并在 device-node 场景创建 host/device scheduler backing（`src/cui/cuigraph.c:1835-1933,2054-2141`）。
- **已确认**：launch 使用 UVM DAG running、completion marker、拓扑依赖和 memory tracking；destroy 反向 detach stream、释放 QMD/constant-bank/marker/scheduler memobj（`src/cui/cuigraph.c:3495-3575,4056-4162,1035-1205`）。
- **静态疑点**：instantiate QMD pool 注册失败和 launch 中途失败的 stream 恢复路径分别存在潜在悬挂锁数组/临时 stream 未恢复问题，尚未运行验证（`src/cui/cuigraph.c:3457-3492,4116-4157`）。

## 未执行项目

本轮未执行 nvmake、nvcc、DVS、GPU test、`nvidia-smi`、`sudo nvidia-persistenced`、OpenCL runtime、debugger/profiler、外部依赖安装或服务启动。根仓库允许的文档校验为 `git diff --check`，需在文档修改稳定后执行。
## 本轮一致性验收

+- 已扫描本项目 185 个知识库 Markdown：统一元信息、结论摘要和四段页尾均存在且顺序一致。
- 根 README 已链接总览、模块、Demo、关联和实践层全部文档；相对 Markdown 链接未发现断链。
- 可解析的源码引用已检查文件存在性与行号数值边界；该检查不替代符号语义复核或动态执行。
- 本轮未执行构建、GPU/NPU、模型、网络、NCCL/HCCL、benchmark 或多节点测试，相关行为继续标记为未验证。


## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 关键分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| M04 suballocator/memblock | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | memmgr lock/DMAL 边界已确认 | 已完成 | D01 仅基础 memobj | 部分完成：DMAL 末端、运行测试 |
| M06 Graph/capture | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | capture lock、多 context lock、stream marker | 已完成 | 无专用 Graph Demo | 部分完成：失败疑点和设备执行 |

## project-prompt 合规状态

总览、模块、关联层和实践层目录已按提示词建立；模块/Demo 审计表、源码证据和链接检查已补齐。缺失的关联主题因当前源码/外部依赖边界保留为“不适用或待验证”，未伪造实现。

## 下一阶段（非运行时）

M08–M10 的 host-side 生命周期和测试聚合语义已补齐；下一步优先为 M04 suballocator 和 M06 Graph 增加 nvmake/GPU 运行验证及失败注入，再回读模块注册表、跨模块页面和总入口，检查链接/证据等级一致性。不得把 DMAL/RM/firmware、compiler/gpgpucomp 或 nvcc 生成物写成当前树已验证行为。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
