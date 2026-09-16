# 跨模块运行时轨迹

## 初始化/查询

```text
muInfo::main
 -> muInit(0)
 -> muapiInit
 -> Platform::Init (call_once)
 -> Hal::CreatePlatform
 -> visible HAL devices
 -> Musa::Device
 -> muDeviceGetCount/Get/Attribute
 -> stdout
```

证据：[src/tools/muInfo.cpp:372-388] [src/driver/mu_context.cpp:121-133] [src/musa/core/platform.cpp:84-140]。

## 内存

```text
muapiMemAlloc_v2
 -> TlsCtxTop
 -> Context::CreateMemory
 -> Memory::Init
 -> GeneralAlloc
 -> Hal::IDevice::GetMemMgr()->Allocate/CreateMemory
 -> MemoryTracker::TrackMemory
 -> MUdeviceptr
```

证据：[src/driver/mu_memory.cpp:271-320] [src/musa/core/context.cpp:1037-1081] [src/musa/core/memory.cpp:470-513]。

## Kernel/异步

```text
muapiLaunchKernel
 -> Context::GeneralLaunchKernel
 -> Stream::CmdLaunchKernel
 -> ResolveDependencyAndQueueCommand
 -> Stream::QueueCommand
 -> AsyncSubmit
 -> Command::Submit
```

证据：[src/driver/mu_module.cpp:232-285] [src/musa/core/context.cpp:625-673] [src/musa/core/stream.cpp:1048-1220]。
