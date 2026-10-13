# Module 与 Kernel 执行：调用链

- 模块：M07 Module / Kernel
- 文档目的：module kernel：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M07 模块的《Module 与 Kernel 执行：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：加载、符号解析、launch 组装和 DispatchCommand 构建/提交均已追到源码；M3D 内部（`CmdDispatch`/`Submit` 的硬件行为）属于运行验证边界。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## CHAIN-M07-MODULE-LOAD：模块与函数句柄

```text
muapiModuleLoad(module, fname)                                [src/driver/mu_module.cpp:87-120]
  ├─ InitPlatform()
  ├─ fname/file 检查，读文件到 image buffer
  └─ muapiModuleLoadData(module, image.get())                 [src/driver/mu_module.cpp:122-146]
     ├─ TlsCtxTop() 取 current context
     ├─ pContext->CreateModule(&pModule)                      [src/musa/core/context.cpp:1280-1286]
     ├─ pModule->LoadFatBinary(image)                         [src/musa/core/module.cpp:20-54]
     │  ├─ 填 Hal::LibraryCreateInfo（MusaFatBin, lazyLoad/dumpDeviceCode/dumpKernelAssembly）
     │  └─ HAL CreateLibrary -> m_pHalLibrary
     └─ LoadFatBinary 失败 -> pContext->DestroyModule(pModule)

muapiModuleGetFunction(hfunc, hmod, name)                     [src/driver/mu_module.cpp:157-178]
  └─ pModule->GetFunction(&pFunction, name)                   [src/musa/core/module.cpp:56-73]
     ├─ FindSymbolByName(name) 触发惰性 LoadImage             [src/musa/core/module.cpp:235-256]
     ├─ 检查 SYMBOL_KIND_KERNEL
     └─ pFunction->Init()（懒创建 HAL kernel）                 [src/musa/core/symbol.cpp:434-452]
```

说明：`LoadImage` 用 `std::call_once(m_IsModuleLoaded)` 保证只解析一次；eager 模式下由 `LoadFatBinary` 直接调用。[src/musa/core/module.cpp:44-45] [src/musa/core/module.cpp:187-233]

## CHAIN-M07-LAUNCH：`muapiLaunchKernel` 到 GraphKernelNode

```text
muapiLaunchKernel(f, gridDim..., blockDim..., sharedMemBytes, hStream, kernelParams, extra)
                                                                                    [src/driver/mu_module.cpp:231-268]
  ├─ f == nullptr -> MUSA_ERROR_INVALID_VALUE
  ├─ 填充 MUSA_KERNEL_NODE_PARAMS（grid/block/sharedMem/kernelParams/extra）
  └─ Context::GeneralLaunchKernel(TlsCtxTop(), hStream, nodeParams, ..., launchBlocking)
                                                                                    [src/musa/core/context.cpp:631-677]
     ├─ InfoStream(ctx, hStream) 解析 stream 句柄
     ├─ 校验 parentCtx / capture 状态 / green context
     ├─ CreateKernelNodeOnStream(nodeParams, ...)              [src/musa/core/context.cpp:2240-2267]
     │  └─ GraphKernelNode::Create + UpdateParams              [src/musa/core/node/graphKernelNode.cpp:57-131]
     │     ├─ grid/block/cluster 校验
     │     └─ func->CreateState(sharedMemBytes, kernelParams, extra, blockSize) -> Hal::KernelState
     │                                                        [src/musa/core/symbol.cpp:169-266]
     ├─ capture active  -> Stream::CaptureNode                 [src/musa/core/stream.cpp:673-694]
     └─ 非 capture      -> Stream::CmdLaunchKernel             [src/musa/core/stream.cpp:1568-1582]
```

## CHAIN-M07-DISPATCH：DispatchCommand 构建 cmd buffer

```text
Stream::CmdLaunchKernel(pGraphNode, blocking)                 [src/musa/core/stream.cpp:1568-1582]
  -> CreatePooledDispatchCommand(stream, pGraphNode, pfm...)  [src/musa/core/command/dispatchCommand.cpp:1347-1360]
  -> ResolveDependencyAndQueueCommand -> QueueCommand/AsyncSubmit
  -> DispatchCommand::Build(mergingList)                      [src/musa/core/command/dispatchCommand.cpp:78-300]
     ├─ InitPfm / BeginPfm / BeginPcSampling / timestamp(TopOfPipe)
     ├─ Command::Build(mergingList) 解析依赖为 semaphore wait
     ├─ (hasDpCe) SetupGpuCopyJobResources                     [src/musa/core/command/dispatchCommand.cpp:1055-1100]
     ├─ UpdateImplicitResources(pHalCmdBuffer)                 [src/musa/core/command/dispatchCommand.cpp:773-1053]
     │  └─ 按 KernelResourceMapping(Implicit) 更新 KernelState：
     │     workgroup size/count、core number、spill/print/cluster 等
     ├─ CmdBindKernel(pKernel->Hal())                          [src/hal/m3d/cmdBuffer.cpp:230-236]
     ├─ CmdBindKernelState(m_pHalKernelState)                  [src/hal/m3d/cmdBuffer.cpp:238-240]
     ├─ CmdSetLLCPersistcyWindow / (可选) CmdBindSpillMemoryRange / CmdSetMultiCoreMode
     └─ pHalCmdBuffer->CmdDispatch(dispatch)                   [src/hal/m3d/cmdBuffer.cpp:347-380]
```

## CHAIN-M07-HAL-M3D：CmdBuffer 与 Queue

```text
DispatchCommand::Submit                                      [src/musa/core/command/dispatchCommand.cpp:302-347]
  ├─ ResolveSubmitSignal / EndPfm / EndPcSampling
  ├─ pHalCmdBuffer->End()                                    [src/hal/m3d/cmdBuffer.cpp:199-211]
  ├─ MUgdb::KernelLoadingProcess / MUasan::KernelLoadingProcess
  └─ SubmitToQueue(stream->GetHalQueue(cdm), submitInfo)
     -> Hal::M3d::Queue::Submit                              [src/hal/m3d/queue.cpp:182-355]
        -> m_M3dQueue->Submit(m3dSubmitInfo)
```

## 缺口

1. `Function::CreateState` 之后的 `Hal::KernelState` 内部 resource buffer 布局由 HAL 私有实现决定，仅见接口。[src/hal/halKernel.h:144-169]
2. `UpdateImplicitResources` 中 printf、spill、const/global memory 的资源分配已逐行文档化，但 M3D 侧执行细节（`CmdDispatch` 之后的调度）属于运行验证边界。[src/musa/core/command/dispatchCommand.cpp:773-1053]
3. `IM3d::ICmdBuffer::CmdDispatch` 和 `IM3d::IQueue::Submit` 的内部实现仍属于 M3D 子模块/运行验证边界。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/command/dispatchCommand.cpp`（6 处，例：[src/musa/core/command/dispatchCommand.cpp:1347-1360]）
- `src/musa/core/module.cpp`（5 处，例：[src/musa/core/module.cpp:20-54]）
- `src/driver/mu_module.cpp`（4 处，例：[src/driver/mu_module.cpp:87-120]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
