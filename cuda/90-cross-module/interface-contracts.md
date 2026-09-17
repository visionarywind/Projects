# 跨模块接口契约

- 文档目的：定义 API、CUI、Memory、Stream/Channel、HAL、DMAL 和 Syscall 之间可由源码追踪的接口契约。
- 适用范围：`source/cuda` host-side 静态实现；设备/firmware 契约只记录边界，不伪造细节。
- 证据状态：静态确认；未执行构建、GPU、DVS、OpenCL、debugger 或 profiler。
- 前置阅读：[端到端 Kernel Launch](end-to-end-kernel-launch.md)、[具体架构设计](architecture-design.md)。

## 1. API → CUI

### 输入

- 用户参数和 flags；
- public handle；
- 当前线程 TLS Context；
- API 版本和兼容入口。

### CUI 必须保证

- 检查参数和 handle；
- 将 public handle 解析为内部对象；
- 检查 Context、Function、Stream 的归属一致性；
- 不把尚未验证的用户输入直接交给 HAL/DMAL；
- 失败时返回 CUDA error，不产生未定义的设备提交。

### 输出

成功只表示 host-side 请求已接受或命令已发布，除非 API 明确是同步操作；不默认表示 GPU 已完成。

证据：`src/api/apiinit.c`、`apictx.c`、`apimem.c`、`apistream.c`、`apilaunch.c`。

## 2. CUI → Context/TLS

`CUctx` 提供：

- current Context 和 TLS 关联；
- Context mutex；
- device、HAL、DMAL 关联；
- memory/stream/channel/module/UVM/syscall managers；
- sticky error 和工具状态。

契约：Context lock 保护 host-side 状态一致性，但不代表 GPU completion；Context destroy 必须处理其未完成的异步资源。

证据：`src/cui/cuiinit.c`、`cuictx.c`、`cuitls.c`。

## 3. CUI → Memory

### 输入

```text
CUmemdesc + size/alignment + Context + allocation flags
```

### 正常结果

```text
memobjAlloc
  → CUmemobj
  → CUmemblock/backing
  → VA/mapping
  → device pointer
```

### 契约

- allocation 必须进入可查找/可登记的对象结构；
- descriptor compatibility 决定 suballocator 区域是否可复用；
- split/coalesce 不得破坏仍存活的 submemblock；
- 异步 launch 引用由 tracking marker 保持；
- 释放可以分为逻辑移除和物理 backing reclaim 两阶段。

证据：`src/cui/memobj.c`、`memblock.c`、`memmgr.c`、`suballocator.c`、`heap.c`。

## 4. CUI → Stream/Channel

### Stream 输入

```text
Context + stream flags + priority
```

### Stream 保证

- 从 stream pool 获取或建立 `CUIstream`；
- 分配所需 QMD、CPU/UVM semaphore 和 marker；
- 维护 null/barrier/capture/priority 语义；
- destroy 时先 detach，再由 marker 完成驱动 reclaim。

### Channel 输入

```text
channel type + Context + backend-specific init params
```

### Channel 保证

- `channelInit` 经 `device->dmal.channelInitDmal` 进入选定平台；
- 建立 tracking/cross-engine 同步资源；
- GPFIFO flush 只在 entry 和 pushbuffer 状态有效时发布；
- backend 不得伪造 GPU completion。

证据：`src/cui/cuistream.c`、`channel.c`、`gpfifo.c`、`dmal/*`。

## 5. CUI → Module/Function

Module 提供 ELF/JIT、code、constant、symbol 和 syscall import 的 Context 级所有权；Function 提供参数布局、grid/launch metadata、register/shared-memory 信息。

契约：

- Function 必须属于当前可用的 module/Context；
- launch 前 code、constant、syscall resources 必须可引用；
- module unload 不得绕过 outstanding launch 的异步引用；
- JIT/compiler 细节属于外部或条件编译边界。

证据：`src/cui/cuimod.c`、`cuifunc.c`、`cuielf*.c`、`cuijitlink.c`。

## 6. CUI → HAL

### 输入

- `CUdev` 和 `CUdev->hal`；
- `CUfunc`、launchData、grid/block/shared-memory 参数；
- QMD 和 constant-bank 状态；
- architecture/class-specific device state。

### HAL 保证

- `launchCheck` 检查架构约束；
- `launchInitialize`/`launchFinalize` 填充硬件相关状态；
- `encodeQmd` 和 ABI encoder 只编码架构字段；
- `launchControl` 生成控制 packet；
- `setGpfifoEntry`/`writeGPPut` 遵守提交格式。

### 非契约内容

HAL 不拥有用户 handle、Context manager 或 module 生命周期；当前树也不能证明设备已消费编码结果。

证据：`src/cui/hal/hal.c`、`src/cui/hal/<arch>/*`、`ampere_devstate.c`。

## 7. CUI → DMAL

DMAL 接收 Channel、memory object、flush item 和平台 flags，负责：

- channel/backend 初始化和 teardown；
- memory/platform mapping；
- pushbuffer/GPFIFO cache 操作；
- fence、doorbell/GPPut 或平台等价发布；
- backend 能力差异和 not-supported 返回。

CUI 不应假定 RM、MRM、WDDM、MPS 共享同样的 syncpoint、external semaphore 或 scheduler 能力。

RM 静态证据：`src/cui/dmal/rm/rm_channel.c:2828-2891`。此处 `writeGPPut` 代表 host-side 提交发布，不代表设备执行完成。

## 8. CUI → Syscall

### 初始化契约

```text
cuiSyscallInit
  → reason buffer
  → trampoline
  → imports/refcounts
```

### Launch 契约

```text
cuiSyscallOnKernelLaunch
  → 校验/聚合 per-launch syscall resources
  → 纳入 memory tracking
  → 与 function/module entry 关联
```

### 销毁契约

```text
cuiSyscallDestroy
  → 解除 imports
  → 减少 refcount
  → 释放 trampoline/reason buffer
```

`cudaSyscalls.nvmk`、assembler、cubin/bin2c、CNP、firmware 和设备入口是外部边界，不能仅凭 host-side 函数确认设备执行。

## 9. CUI → Graph

Graph capture 的契约是“记录节点，不提交命令”；instantiate 的契约是“验证并建立可重复 execution plan”；launch 的契约是“按依赖/scheduling group 生成实际提交”。

instantiate 可以分配：

- per-context QMD/constant-bank；
- graph internal streams/markers；
- scheduler host/device backing；
- Context lock set；
- QMD semaphore pools。

Graph 的 destroy 必须遵守 completion marker 和 tracking；不把 host graph node free 当成设备资源释放完成。

证据：`src/cui/cuigraph.c`。

## 10. Completion 契约

提交完成不等于设备执行完成。抽象完成链为：

```text
DMAL/RM publish
  → RM/firmware/GPU boundary
  → tracking semaphore/marker
  → gpfifoAdvanceGpuGet / marker query
  → release pushbuffer and tracked objects
```

同步 API 可等待该状态链，但如果 wait 失败或超时，必须保留未完成资源，不能提前转入 free pool。

## 11. 错误契约

| 错误类别 | 发生位置 | 资源规则 |
|---|---|---|
| validation | API/CUI 入口 | 不发布新命令 |
| setup | syscall/HAL/QMD | 释放临时状态 |
| push | stream/pushbuffer | 关闭部分 push，不能宣称完成 |
| submit | DMAL/RM | sticky error，不能伪造 marker |
| device | trap/timeout/reason buffer | 通过异步错误观察点传播 |
| destroy | Context/stream/graph | defer reclaim 至 marker 完成 |

## 12. 关联文档

- [Kernel Launch 端到端调用链](end-to-end-kernel-launch.md)
- [具体架构设计](architecture-design.md)
- [跨模块调用链审计](cross-module-call-chains.md)
- [运行时轨迹](runtime-trace.md)
