# Driver API 导出层：调用链

- 模块：M02 Driver API 导出层
- 文档目的：driver api：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M02 模块的《Driver API 导出层：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：init、设备查询、Context/Stream/Memory/Module/Launch 主链、`muapiGetProcAddress`、`muapiGetExportTable` 分派已静态确认；`Musa::Context::GeneralMemcpy`/`GeneralLaunchKernel` 的 Core 内部展开未在本模块逐行跟踪。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## CHAIN-M02-INIT：显式初始化

```text
muInit(Flags)                                            [src/driver/mu_wrappers_generated.cpp:216-254]
  -> ApiTrace 记录 + g_InjectionManager.Init()
  -> tools 回调未启用或 skipDriverImpl 未置位时调用 muapiInit
     -> muapiInit(flags)                                 [src/driver/mu_context.cpp:121-133]
        ├─ flags != 0 -> MUSA_ERROR_INVALID_VALUE
        └─ Musa::CreatePlatform(nullptr)
           -> Platform::Get().Init()                     [src/musa/core/lib.cpp:14-18]
```

注意：绝大多数非 init API 不会自动创建平台，而是调用 `InitPlatform()` 检查平台是否已存在；TLS 数据已销毁返回 `MUSA_ERROR_INVALID_CONTEXT`、入口未登记返回 `MUSA_ERROR_NOT_SUPPORTED`、`Musa::GetPlatform` 失败返回 `MUSA_ERROR_NOT_INITIALIZED`。[src/driver/internal.h:307-321]

## CHAIN-M02-DEVICE-QUERY：设备句柄和属性查询

```text
muDeviceGet(&device, ordinal)
  -> muapiDeviceGet                                        [src/driver/mu_device.cpp:14-37]
     -> InitPlatform()
     -> Platform::Get().GetIDeviceView(ordinal)           [src/musa/core/platform.cpp:378-385]
     -> pDevice->GetId()（越界则 *device=-1 并 INVALID_DEVICE）

muDeviceGetAttribute(&pi, attrib, dev)
  -> muapiDeviceGetAttribute                              [src/driver/mu_device.cpp:71-94]
     -> GetDeviceCount()==0 -> MUSA_ERROR_NO_DEVICE
     -> GetIDeviceView(dev)
     -> Device::GetAttribute(pi, attrib)                  [src/musa/core/device.cpp:41-48]
        -> switch(attrib) 读 Hal::DeviceProperties 字段（首段已确认）
```

`GetIDeviceView` 只是按 seqID 返回 `m_Devices[seqID]`，不触发 `LaterInit`；`Platform::GetDevice` 才会调用 `LaterInit`。[src/musa/core/platform.cpp:378-385] [src/musa/core/platform.cpp:141-149]

## CHAIN-M02-CONTEXT-TLS：当前 Context 与 primary ctx

```text
muDevicePrimaryCtxRetain(&ctx, dev)
  -> muapiDevicePrimaryCtxRetain                          [src/driver/mu_context.cpp:472-489]
     -> Platform::Get().GetDevice(dev)                    [src/musa/core/platform.cpp:141-149]
        -> Device::LaterInit() if needed
     -> pDevice->GetPrimaryContext()
     -> pContext->Retain()

muCtxSetCurrent(ctx)
  -> muapiCtxSetCurrent                                   [src/driver/mu_context.cpp:280-296]
     -> ctx==nullptr ? TlsCtxPop()
     -> ctx!=TlsCtxTop() 时 TlsCtxPop + TlsCtxPush(musaContext)

muCtxCreate_v2(pctx, flags, dev)
  -> muapiCtxCreate_v2                                    [src/driver/mu_context.cpp:156-182]
     -> pDevice->CreateContext(&pContext, flags)
     -> TlsCtxPush + Platform::LockedAddCtx                [src/musa/core/platform.cpp:436-441]
```

## CHAIN-M02-EXPORT-ACCESSOR：内部访问表

`muapiGetProcAddress` 用静态 `DriverEntryPointTable s_EntryPointTable`：`Find(symbol)` 命中后按 `FindBestMatch(musaVersion, flags)` 选择带 `_ptsz` 或普通实现；查不到符号时 `symbolStatus` 保持 `MU_GET_PROC_ADDRESS_SYMBOL_NOT_FOUND`。[src/driver/mu_entry.cpp:1222-1253] 表构造集中在 `DriverEntryPointTable::DriverEntryPointTable`，符号到实现全部登记为版本 40300（`muInit`、`muGetExportTable` 等在 [src/driver/internal.cpp:175-414]）。

`muapiGetExportTable` 把 `MUpti::InternalAccessors`（`muapiInit`、`muapiDriverGetVersion`、`muapiDeviceGetCount`、`muapiCtxCreate`、`muapiCtxDestroy`）暴露给 MUPTI/内部使用。[src/driver/mu_entry.cpp:1424-1431]

## CHAIN-M02-MEMCPY：同步/异步拷贝

```text
muMemcpyHtoD_v2(dstDevice, srcHost, ByteCount)
  -> muapiMemcpyHtoD_v2                                   [src/driver/mu_memory.cpp:845-855]
     -> GetMemcpy3DFrom1D(copy3D, dst, src, ByteCount, memcpy_host_to_device)
     -> Musa::Context::GeneralMemcpy(TlsCtxTop(), nullptr, copy3D, true)
muMemcpyHtoDAsync_v2
  -> muapiMemcpyHtoDAsync_v2                              [src/driver/mu_memory.cpp:861-871]
     -> GeneralMemcpy(..., hStream, memBlocking)          // blocking 取 Platform settings
```

## CHAIN-M02-LAUNCH：内核启动

```text
muLaunchKernel(...)
  -> muapiLaunchKernel                                    [src/driver/mu_module.cpp:231-268]
     -> !f -> MUSA_ERROR_INVALID_VALUE
     -> 组装 MUSA_KERNEL_NODE_PARAMS
     -> Musa::Context::GeneralLaunchKernel(TlsCtxTop(), hStream, nodeParams,
            0, 0, 0, 0, launchBlocking)
```

`muapiLaunchKernelEx` 逐条校验 `MUlaunchConfig::attrs[]`：`MU_LAUNCH_ATTRIBUTE_CLUSTER_DIMENSION`/`MU_LAUNCH_ATTRIBUTE_PROGRAMMATIC_STREAM_SERIALIZATION` 接受，其余属性返回 `MUSA_ERROR_NOT_SUPPORTED` 或 `MUSA_ERROR_INVALID_VALUE`。[src/driver/mu_module.cpp:270-334]

## CHAIN-M02-MODULE：模块加载与函数查找

```text
muModuleLoadData(module, image)
  -> muapiModuleLoadData                                  [src/driver/mu_module.cpp:122-146]
     -> pContext->CreateModule(&pModule)
     -> pModule->LoadFatBinary(image)
     -> 成功才 *module=pModule；失败 pContext->DestroyModule(pModule) 回滚
muModuleLoad(fname)
  -> muapiModuleLoad                                      [src/driver/mu_module.cpp:87-120]
     -> 读文件失败 -> MUSA_ERROR_FILE_NOT_FOUND；成功则转 muapiModuleLoadData
```

## CHAIN-M02-MEMPOOL：内存池创建

```text
muMemPoolCreate(pool, poolProps)
  -> muapiMemPoolCreate                                   [src/driver/mu_mempool.cpp:21-125]
     -> allocType 必须 PINNED/MANAGED，否则 INVALID_VALUE
     -> location switch 填 Hal::MemoryPoolCreateInfo
     -> new Musa::MemoryPool(pDevice) -> Init()；失败 delete
```

## 缺口

1. `mu_wrappers_generated.cpp` 由 `generated_musa_meta.h` 等生成，生成脚本与 `mu*` 到 `muapi*` 的完整符号映射未做专门核对（[src/driver/mu_wrappers_generated.cpp:1-15]）。
2. `cmake/export_symbols.cmake` 的导出清单需要和最终动态库导出表做一次对应检查（[cmake/export_symbols.cmake:5-26]）。
3. `Musa::Context::GeneralMemcpy`/`GeneralLaunchKernel` 的 Core 内部分支（含 capture 与 batch）未在本模块逐行展开。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（5 处，例：[src/musa/core/platform.cpp:378-385]）
- `src/driver/mu_context.cpp`（4 处，例：[src/driver/mu_context.cpp:121-133]）
- `src/driver/mu_module.cpp`（4 处，例：[src/driver/mu_module.cpp:231-268]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
