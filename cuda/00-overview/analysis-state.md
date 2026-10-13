# CUDA 项目分析状态

- 文档目的：解释 00-overview/analysis-state.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`source/cuda` 快照（仅含 `src/`；无 `.git`，提交未知；文件时间戳 2022-02-22）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-10-05
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/analysis-state.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **更新时间**：2026-10-05
- **源码根**：`source/cuda`（当前检出仅含 `src/`）
- **文档根**：本 `README.md` 所在仓库的 `cuda/`
- **总体状态**：M01–M10 模块文档骨架已建立；核心 API→CUI→资源→HAL/提交主线已静态追踪；M04–M07 的 UVM/GPFIFO/marker/module/syscall 深层生命周期以及 M08–M09 的工具、OpenCL 对象/interop 语义已补证；跨模块 Kernel Launch 端到端调用链和具体架构设计文档已补充。**M10 与 D01 的入口源码在当前快照外，已改记为受阻**；外部后端与生成工具边界仍有明确缺口。**D02（cuGetExportTable 导出表机制）为补选快照内唯一可复核的公开入口链，已建立，可覆盖 M01/M08/M09/M10 的导出-消费者-测试体链路。**

## 快照边界（本轮只读确认）

当前 `source/cuda` 只包含 `src/` 一个子树。以下被正文引用的路径在本检出中**不存在**，相关结论一律不可复验：

| 缺失路径 | 被引用为 | 影响文档数 |
|---|---|---|
| `inc/cuda.h` | 公开 API/ABI 声明、版本宏 | 14 |
| `common/version.h` | 内部版本字段 | 5 |
| `cuda.nvmk`、`cudaSyscalls.nvmk`、`tests/**/*.nvmk` | 构建与源文件清单 | 45 |
| `tests/**`（`basic_sanity.cu`、`dispatch_main.cpp`、`unittest_*.cpp` 等） | D01 Demo 与 M10 测试入口 | 32 |

快照内可复核的替换入口：`cuiGetExportTable` → `g_etblMap` → `g_etbl*` 导出表链路（`src/cui/cuietbl.c`、`src/api/apietbl.c`），已在 D02（[cuGetExportTable（D02）](../80-demos/D02-cugetexporttable/README.md)）完成静态解剖；`src/etbl/` 的 123 个 `eti*` 测试体经该链注册。
| `mods/Makefile`、`import/r*` | MODS 构建、历史依赖快照 | 3 |

处置原则见 [决策与冲突记录](decision-log.md) 的“快照边界冲突”。快照内 `src/` 的行号引用已抽样复核为准确（例如 `src/api/apiinit.c:18-47`、`src/api/apimem.c:89-117,128-136`）。

## 版本与仓库证据

| 项目 | 状态 | 说明 |
|---|---|---|
| 快照内容 | 已确认 | 仅 `src/`；`source/src.tar.gz` 清单一致（2906 项） |
| 快照时间 | 已确认 | `src/` 下 2387 个文件时间戳一致为 2022-02-22 |
| Git commit/branch | 未知 | 源码目录没有 `.git`，且 `/source/` 被 `.gitignore` 整体忽略 |
| CUDA API 版本 | 推断（数值当前不可复验） | 快照外 `inc/cuda.h` 记为 `CUDA_VERSION=10020`；`src/` 内 `10020` 零命中（已排除 `.cubins.c` 十六进制数据） |
| 版本宏存在的间接证据 | 推断 | 快照内 [src/cui/cuiinit.c:3064-3067] `cuiInitInternal` 对 `CUDA_VERSION`、`NVCUDA_VER_MAJOR/MINOR` 做 `ct_assert`，说明这些宏由缺失的 `cuda.h` 提供，但不提供其数值 |
| 内部版本字段 | 推断（当前不可复验） | 依据 `common/version.h`；该文件不在快照内 |
| 外部依赖版本 | 未知 | common、RM/NVRM、compiler/gpgpucomp、firmware 不完整 |

> 2026-10-05 修正：本页此前把版本字段记为“静态确认”，并把页头写成“source/cuda HEAD 39d4a83”。前者依赖快照外文件，后者误引了本 Projects 仓库的提交号，均已按上表更正。

## 模块状态

| 模块 | 深度 | 已覆盖 | 主要缺口 |
|---|---|---|---|
| M01 API/ABI | 深（快照外证据已降级） | wrapper、参数校验、错误回滚（`src/api/` 内）；`inc/cuda.h` 版本宏与 `cuda.nvmk` 导出关系已降级为历史结论 | generator 实际执行与符号最终排序；`inc/cuda.h`、`cuda.nvmk` 不在快照内，ABI/版本宏与导出关系不可复验 |
| M02 Runtime/Context | 深 | globals、TLS 检查、context 创建、初始化回滚、sticky error 观察点 | TLS 深层、完整 destroy/thread-exit/fork 生命周期 |
| M03 Device/HAL | 中 | remap/过滤/排序、HAL 编译与运行时选择、DMAL 边界 | 具体目标配置、架构实现、RM 末端 |
| M04 Memory/UVA/UVM | 深（静态） | memdesc、memobj、memblock、descriptor-compatible suballocator、best-fit/split/coalesce、device/host alloc/free；UVM manager/managed attributes/DAG、stream state、P2P bookkeeping；AL selection、memobj registration rollback、submemblock mapping | UVM fault/migration backend、IPC/external sharing 的完整运行链、DMAL 物理实现和运行时碎片验证 |
| M05 Stream/Submit | 深 | stream pool、QMD、channel 初始化、push、GPFIFO/pushbuffer、tracking marker 和 completion reclaim | 各 DMAL backend 的 doorbell/RM 末端、设备消费 |
| M06 Module/Launch | 深（静态） | launch 分支、参数、tracking、setup、ELF/module/JIT、shared ELF、syscall resource、graph capture/instantiate/launch/update/destroy、clone/flatten、memset lowering、QMD chaining/pushbuffer/device scheduler、per-context 资源 reverse path | 架构具体 ABI/QMD 字段、compiler/gpgpucomp 内部、Graph 专用运行时测试和失败疑点复现 |
| M07 Kernel/Syscall | 中→深 | syscall init/load/destroy、imports/refcount、resource aggregation、launch callback、trap/timeout/reason buffer；trampoline PC binding、per-arch generated cubin chain | 各 syscall 子实现、CNP queue、compiler/asm/cubin 生成链的设备侧细节 |
| M08 Tools/Debug | 中→深（host-side） | callbacks、skip/blocking、debugger checks/shared state、memcheck device table、profiler mode/perfmon | 外部 attach protocol、helper/backend、DRS/runtime client |
| M09 OpenCL/Interop | 中→深（host-side） | ICD/vendor、固定 dispatch、public/internal object tree、context destroy、event marker aggregation、GL registration、external memobj | enqueue worker 全链、GL/D3D fence/RM、handle close、运行验证 |
| M10 Tests/Experiments | 受阻（整个模块源码在快照外） | 仅保留历史静态阅读结论：binary/case registration、device-link、dispatcher、runner pipe/timeout/no-result/failure/waived、DVS package | `tests/`、`mods/`、`experiments/` 均不在快照内，不可复读也不可重跑；全量 case、MODS/CI、硬件执行；快照内可见验证代码仅 `src/etbl/`、`src/drs/` |

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

- **端到端调用链**：已完成普通 `cuLaunchKernel` 的静态主线文档，覆盖初始化、Context、Memory、Stream/Channel、Module、Syscall、HAL/QMD、Pushbuffer/GPFIFO、DMAL/RM、completion 和 reclaim；GPU/RM/firmware 末端仍未验证。
- **具体架构设计**：已完成 API/CUI/HAL/DMAL 分层、`CUdev->hal` 函数表、对象所有权、锁、异步 tracking、Graph scheduler 和 teardown 的静态设计说明。


本轮未执行 nvmake、nvcc、DVS、GPU test、`nvidia-smi`、`sudo nvidia-persistenced`、OpenCL runtime、debugger/profiler、外部依赖安装或服务启动。根仓库允许的文档校验为 `git diff --check`，需在文档修改稳定后执行。
## 本轮一致性验收（2026-10-05）

本轮以 `source/cuda` 当前检出为基准，复核知识库一致性，结果如下：

| 检查项 | 方法 | 结果 |
|---|---|---|
| 内部链接 | 遍历 188 个 Markdown，校验 1550 条相对链接 | 0 断链 |
| 源码引用可解析性 | 提取 85 个带行号的 `path:lines` 引用（142 处） | 引用的行号**均在文件范围内**，无越界 |
| 引用文件存在性 | 同上 | 13 个路径缺失，全部属于快照外的 `inc/`、`common/`、`*.nvmk`、`tests/` |
| 行号语义抽样 | 逐行读取 `src/api/apiinit.c:18-47`、`src/api/apimem.c:89-117,128-136` | 与文档描述一致（`cuapiInit`→`cuiInit`、`cuapiMemAlloc_v2`→`cuapiMemAlloc_common`→`memobjAlloc`） |
| 模块注册表目录 | 核对 M01–M10 列出的目录/文件 | 修正 M06 的 `apigraph.c`/`apistream.c` 归属（实为 `src/api/`） |
| 版本标识 | 比对 `source/cuda/.git` 与 Projects 提交历史 | 发现并撤销 170 处错误 HEAD 声明 |
| 陈旧绝对路径 | 检索 `/home/...`、`/Users/...` | 修正 1 处（`/home/mtuser/workspace/Projects/cuda`） |

未执行构建、GPU/NPU、模型、网络、NCCL/HCCL、benchmark 或多节点测试，相关行为继续标记为未验证。

本轮修改均为文档层修正，未触碰 `source/cuda` 任何源码。

**D02 复核（2026-10-05 续）**：新增 `80-demos/D02-cugetexporttable/`（README、执行轨迹、数据与状态轨迹、失败路径、调试走查、修改练习），为快照内唯一可复核的公开入口链补充行级证据。额外的静态核查结论：`src/cui/cuietbl.c` 的 `g_etblMap`（`:521-603`，75 项）与 72 个 `cuda_etbl/*.h` 引用、`g_etblCudaGraphs` 表体（`src/etbl/cuda_graphs/cuda_graphs.c:389-430`，含 26 个 NULL 槽与 12 个函数槽）均已定位；其中 3 个函数槽（`:425/:426/:428`）与 `cuapiGraphAddConditionalNode` 等符号在快照全树 0 命中，判定为快照不完整的静态证据；`cuGetExportTable` 文档承诺 `CUDA_ERROR_NOT_FOUND` 而 `cuiGetExportTable` 返回 `CUDA_ERROR_INVALID_VALUE`（`apietbl.c:36,45` vs `cuietbl.c:647`），为源码自身的文档/实现偏差，已记入 failure-paths。


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
