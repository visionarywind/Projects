# CUDA Driver 具体架构设计

- 文档目的：解释 `source/cuda` 中 API、CUI、Memory、Stream/Channel、HAL、DMAL 和 Graph 之间的职责边界、对象所有权与异步完成模型。
- 适用范围：源码快照的 host-side 静态设计；RM/NVRM、firmware、compiler 和 GPU 设备执行属于外部边界。
- 证据状态：架构分层、函数表、对象关系、锁和资源生命周期为静态确认；未执行构建、GPU、DVS、OpenCL、debugger 或 profiler。
- 对应源码版本：见 [`00-overview/analysis-state.md`](../00-overview/analysis-state.md)。

## 1. 总体分层

```text
CUDA application / test
  ↓ public ABI
inc/cuda*.h + src/api
  ↓ validated arguments and handles
CUI runtime
  ├─ TLS / globals / Context
  ├─ memory objects and VA managers
  ├─ streams / channels / markers
  ├─ modules / functions / launches / graphs
  └─ tools / UVM / syscall coordination
  ↓ architecture contract
CUdev->hal
  ↓ platform contract
CUdev->dmal
  ↓ external dependency
RM / NVRM / WDDM / MRM / firmware / GPU
```

分层不是完全线性调用：Memory、Stream、Module 和 Tools 都由 Context 关联；launch 同时横跨这些对象，并在提交前将它们的引用收集到 tracking state。

## 2. 关键对象和所有权

| 对象 | 主要职责 | 典型所有者/关联者 | 异步生命周期依据 |
|---|---|---|---|
| `CUdev` | 设备能力、HAL 和 DMAL 分派 | globals/device manager | device/platform teardown |
| `CUctx` | Context 状态、锁和 Context 级 manager | device / primary-context state | Context work/markers |
| `CUmemobj` | 用户分配的逻辑内存对象 | Context memory manager | memory tracking |
| `CUmemblock` | backing、映射或池化分配单元 | memobj/memory manager | backing references/marker |
| `CUIstream` | 用户 stream 语义、QMD、marker、capture | stream pool / Context | completion marker |
| `CUnvchannel` | engine 提交和 GPFIFO 后端 | channel manager / Context | channel flush/completion |
| `CUqmd` | 设备 kernel 描述 | stream/graph QMD pool | QMD tracking |
| module/function | code、符号和 launch metadata | Context module registry | outstanding launches |
| graph exec | flattened graph、schedule 和专用资源 | graph API / Context | graph markers |

核心原则：

```text
host object lifetime != device use lifetime
```

API 删除或 detach 只改变 host-side 可见性；只要 tracking marker 尚未完成，backing、QMD、pushbuffer 或 code 仍可能被异步命令引用。

## 3. `CUdev->hal` 函数表设计

`InitDeviceArch` 根据 `device->state.ccArch` 选择架构初始化函数。编译时，`cuda.nvmk` 使用 `NVCFG_GLOBAL_ARCH_*` 选择可用实现；运行时，当前 device 再选择实际函数表。

典型 HAL 契约：

```text
launchCheck
launchInitialize
launchFinalize
launchPatchForGraph
launchControl
encodeQmd
encodeAbiConstBankGridParams
setGpfifoEntry
writeGPPut
pushPcas
launchMakePcasPacket
semaphore acquire/release
cache/TLB/trap/debug
```

调用者只依赖统一的输入和结果约定：

```text
CUI launch state + CUfunc + QMD
  → HAL validates and encodes architecture fields
  → CUI emits the result into pushbuffer
```

HAL 不拥有高层 Context、Module 或用户 handle；它负责将统一 launch state 映射到具体硬件 ABI。Ampere 中按 `classCompute` 选择不同 QMD 编码版本，说明架构选择和 compute-class ABI 需要分开记录。

## 4. DMAL 后端设计

DMAL 是平台提交和资源后端抽象。CUI 负责统一的 stream、QMD、marker 和提交顺序；DMAL 负责平台特有的 channel、memory、cache、fence 和同步实现。

```text
channelInit
  → device->dmal.channelInitDmal
  → channel->dmal.Init

channelFlushUnitFlushGPFIFO
  → channel->dmal.GpfifoAdvanceGpuPut
```

RM 路径中，`channelGpfifoAdvanceGpuPutRM`：

1. 遍历待发布的 GPFIFO entry；
2. 对 host pushbuffer 和 sysmem GPFIFO 做 cache flush；
3. 在特定平台配置下同步 generation register；
4. 执行 store fence；
5. 通过 `hal.writeGPPut` 发布新的生产者位置；
6. 执行第二次 store fence。

这是一种“提交发布”契约，不是完成契约。完成仍需通过 backend 支持的 semaphore、marker 或其他状态传播路径观察。

平台比较：

| 层次 | CUI 统一逻辑 | HAL 架构差异 | DMAL 平台差异 |
|---|---|---|---|
| launch | 参数和资源准备 | ABI/QMD 编码 | 通常不参与 |
| QMD | 分配、复用、tracking | 字段布局 | 通常不参与 |
| semaphore | 逻辑值和引用 | packet 编码 | 平台同步对象 |
| GPFIFO | entry 生命周期 | entry/packet 格式 | cache flush、publish、RM/WDDM 调用 |
| completion | marker 状态机 | 硬件语义 | 查询/推进 backend 状态 |
| memory | memobj/memblock | 地址属性编码 | physical mapping、cache 操作 |

RM、MRM、WDDM、MPS 和 AModel 不能假设拥有完全相同的提交或同步能力；例如某些 backend 对 syncpoint 或 external semaphore 返回 not supported。

## 5. Stream、Channel、QMD、GPFIFO 的组合

四者职责不同：

```text
Stream
  = 用户看到的顺序和同步语义
Channel
  = 向设备 engine 提交命令的资源
QMD
  = 一次 kernel 的设备描述
Pushbuffer
  = 实际命令和 packet 的 host/device 可见存储
GPFIFO
  = 指向 pushbuffer/QMD 工作的提交环
```

一次普通 launch 的组合关系：

```text
CUIstream
  → select/reuse CUqmd
  → streamBeginPush obtains CUnvCurrent/CUnvchannel
  → encode QMD and launch packets
  → streamEndPush creates GPFIFO entry
  → DMAL publishes GPPut
  → marker/tracking later permits reuse
```

因此不能把 `cuStream` 直接等同于硬件 queue，也不能把 `GPPut` 写入等同于 kernel completion。

## 6. Memory tracking 和异步所有权

launch 根据 backend 需求建立 `CUmemTrackList`。被追踪的对象可能包括 function memory、texture/sampler、local memory、constant bank、vidmem heap 和 syscall memory。

```text
collect references
  → channelTrackMem / channelTrackQMD
  → publish command
  → completion marker
  → release/reuse tracked objects
```

这个模型解决了两个独立问题：

- API 层是否允许用户删除对象；
- GPU 是否仍然拥有对 backing 的异步引用。

释放 API 对象不应跳过第二个问题。UVM DAG、stream marker 和 graph completion 还可能增加额外的完成依赖。

## 7. Syscall/trampoline 架构

Host-side syscall 层负责把 module/function 的 syscall imports、reason buffer、trampoline 和 per-launch resource 聚合到 launch state：

```text
cuiSyscallInit
  → reason buffer
  → trampoline initialization
  → module imports/refcounts
  → cuiSyscallOnKernelLaunch
  → launch memory tracking
  → cuiSyscallDestroy
```

设备侧则依赖构建系统生成的 syscall code、架构汇编、cubin/bin2c 和 firmware/CNP 机制。`cudaSyscalls.nvmk` 能证明构建输入和架构条件，不能单独证明设备入口已在实际 GPU 上执行。

## 8. Graph scheduler 设计

Graph 有两个不同阶段：

```text
capture
  = 把 API 操作保存成 host graph node

instantiate
  = clone/flatten/validate，并建立可重复执行的资源计划
```

instantiate 的资源计划包括：

- node 到 Context 的归属；
- cycle/conditional 验证；
- scheduling group 和 device-node index；
- per-context kernel node/QMD/constant-bank 计数；
- 跨 Context lock 集合；
- graph stream、marker 和 QMD semaphore pool；
- 条件节点、scheduler backing 和 device-visible metadata。

launch 时，scheduling group 可以通过一个 push sequence 批量初始化和发布多个 kernel node。memory tracking 仍在 launch 时建立，因为 Context 中可追踪对象可能在 instantiate 后变化。

## 9. 锁模型

| 锁 | 保护范围 | 使用注意 |
|---|---|---|
| `globals.initMutex` | 全局初始化/初始化状态 | 保证 `cuiInit` 幂等 |
| Context mutex | Context manager、module、launch 状态 | API/CUI 主串行化边界 |
| `streamPool->mutex` | active/detached/free 列表和状态转移 | `cuiStreamCreate_UnderLock`、detach、reclaim |
| stream marker mutex | marker、barrier、stream 完成状态 | 与异步完成查询配合 |
| graph capture lock | graph 节点和 capture 关系 | 多 stream capture 需要协调 |
| 多 Context lock 集合 | graph instantiate/跨 Context launch | 统一获取，避免只锁当前 Context |

锁不能代替异步完成。持有 Context lock 只保证 host-side 状态一致，不表示设备已经消费或完成命令。

## 10. 错误和 teardown 设计

错误按发生层次分为：

```text
validation error
  → no device submission

host setup/submission error
  → rollback push/setup/tracking state
  → preserve sticky error

device/firmware error
  → marker/trap/timeout/reason-buffer path
  → report after submitted work is observed
```

teardown 应遵循反向依赖：

```text
stop new API use
  → synchronize or retain pending markers
  → detach streams / destroy graph execution resources
  → release module/function/syscall resources
  → release memory tracking/backing
  → destroy channels and backend state
  → deinitialize Context
  → release primary Context
  → destroy globals/platform managers
```

实际某些路径可以 deferred reclaim，而不是同步等待；文档应写明“逻辑 detach”和“物理 reclaim”是两个阶段。

## 11. 证据边界

### 静态确认

- `src/api` 到 `src/cui` 的入口关系；
- `CUdev->hal` 的架构分派和函数表；
- stream pool、QMD、pushbuffer、GPFIFO 和 marker 的 host-side 关系；
- DMAL backend 的调用边界；
- graph instantiate 的 flatten、lock 和资源登记。

### 当前不可由本树证明

- RM 内核驱动如何解释 GPPut；
- firmware/GPU 如何消费 GPFIFO、QMD 和 trampoline；
- compiler/gpgpucomp 生成 cubin 的内部规则；
- UVM fault/migration 的设备行为；
- 实际 GPU 上的延迟、错误注入和回收时序。

## 12. 进一步验证

拥有完整 parent NVIDIA tree、构建工具和硬件时，验证顺序应为：

1. 按 `source/cuda/CLAUDE.md` 使用 parent `nvmake` 构建；
2. 运行最小 init/context/memory/stream/kernel 测试；
3. 观察 launch、GPFIFO、marker 和 error path 的调试日志；
4. 单独验证 graph、syscall、UVM 和工具旁路；
5. 将运行结果与本页静态调用链分开记录。

在这些条件具备前，不能把构建配方、测试注册或源码中存在的 backend 函数写成运行验证结果。
