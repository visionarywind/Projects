# Module 与 Kernel 执行：调用链

- 模块：M07 Module / Kernel
- 证据状态：kernel launch 已追到 GraphKernelNode、DispatchCommand、HAL CmdBuffer、M3D CmdDispatch 和 Queue::Submit；模块加载到 HAL kernel metadata 的细节仍需继续展开。

## CHAIN-M07-MODULE-LOAD：模块与函数句柄

```text
muModuleLoad(module, fname)                                [src/driver/mu_module.cpp:88-121]
  ├─ InitPlatform()
  ├─ 读取文件到 image buffer
  └─ muapiModuleLoadData(module, image.get())              [src/driver/mu_module.cpp:123-148]
     ├─ TlsCtxTop()
     └─ pContext->CreateModule(...)
        -> Module::LoadFatBinary                          [src/musa/core/module.cpp:20-55]
           -> Module::LoadImage                           [src/musa/core/module.cpp:187-234]

muModuleGetFunction(hfunc, hmod, name)                     [src/driver/mu_module.cpp:158-...]
  -> Module::GetFunction                                  [src/musa/core/module.cpp:56-73]
```

说明：本链只定位到模块对象和函数查找入口；fat binary 解析、HAL kernel/state 创建和 metadata 字段来源需要继续在 `module.cpp`、HAL `library/kernel` 文件中展开。

## CHAIN-M07-LAUNCH：`muLaunchKernel` 到 GraphKernelNode

```text
muLaunchKernel(...)
  -> muapiLaunchKernel                                    [src/driver/mu_module.cpp:232-272]
     ├─ InitPlatform()
     ├─ ICast<Musa::IFunction>(f)
     ├─ KernelReplayHandler(...)
     ├─ 填充 MUSA_KERNEL_NODE_PARAMS
     └─ Context::GeneralLaunchKernel(TlsCtxTop(), hStream, nodeParams, ..., launchBlocking)
        -> Context::GeneralLaunchKernel                   [src/musa/core/context.cpp:625-671]
           ├─ InfoStream(ctx, hStream)
           ├─ 校验 stream parent context / green context / capture 状态
           ├─ CreateKernelNode(nodeParams, ...)
           │  -> new GraphKernelNode + UpdateParams        [src/musa/core/context.cpp:2192-2218]
           ├─ capture active -> Stream::CaptureNode
           └─ 非 capture -> Stream::CmdLaunchKernel
```

## CHAIN-M07-DISPATCH：DispatchCommand 构建 cmd buffer

```text
Stream::CmdLaunchKernel(pGraphNode, blocking)
  -> std::make_shared<DispatchCommand>(stream, GraphNode, pfm...) [src/musa/core/stream.cpp:1568-1582]
  -> Context::ResolveDependencyAndQueueCommand
  -> Stream::QueueCommand / AsyncSubmit
  -> DispatchCommand::Build                                [src/musa/core/command/dispatchCommand.cpp:68-260]
     ├─ GetHalCmdBuffer(true)
     ├─ BeginPfm / timestamp / implicit resource update
     ├─ Command::Build(mergingList) 解析依赖为 semaphore wait
     ├─ pHalCmdBuffer->CmdBindKernel
     ├─ pHalCmdBuffer->CmdBindKernelState
     ├─ pHalCmdBuffer->CmdSetLLCPersistcyWindow
     ├─ 必要时 CmdBindSpillMemoryRange / CmdSetMultiCoreMode
     └─ pHalCmdBuffer->CmdDispatch(dispatch)
```

## CHAIN-M07-HAL-M3D：CmdBuffer 和 Queue

```text
DispatchCommand::Build
  -> Hal::M3d::CmdBuffer::CmdBindKernel                   [src/hal/m3d/cmdBuffer.cpp:224-227]
     -> m_M3dCmdBuffer->CmdBindPipeline(Compute, pipeline)
  -> Hal::M3d::CmdBuffer::CmdDispatch                     [src/hal/m3d/cmdBuffer.cpp:338-364]
     ├─ GenerateComputeRuntimeData
     ├─ 转换 workgroupCount/workgroupSize/blockClusterSize
     ├─ 设置 dynamicSharedSize
     └─ m_M3dCmdBuffer->CmdDispatch(dispatchArgs)

DispatchCommand::Submit                                   [src/musa/core/command/dispatchCommand.cpp:263-312]
  ├─ ResolveSubmitSignal
  ├─ pHalCmdBuffer->End()
  │  -> Hal::M3d::CmdBuffer::End
  │     -> m_M3dCmdBuffer->End()                          [src/hal/m3d/cmdBuffer.cpp:193-204]
  ├─ MUgdb/MUasan KernelLoadingProcess
  └─ SubmitToQueue(stream->GetHalQueue(CDM), submitInfo)
     -> Hal::M3d::Queue::Submit
        -> m_M3dQueue->Submit(m3dSubmitInfo)              [src/hal/m3d/queue.cpp:178-355]
```

## 缺口

1. `Function`/`KernelResource`/`KernelState` 的创建和参数布局需要专项分析。
2. `DispatchCommand::UpdateImplicitResources` 中 printf、spill、const/global memory 的资源分配尚未逐行文档化。
3. `IM3d::ICmdBuffer::CmdDispatch` 和 `IM3d::IQueue::Submit` 的内部实现仍属于 M3D 子模块/运行验证边界。
