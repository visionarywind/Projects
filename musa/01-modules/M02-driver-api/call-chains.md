# Driver API 导出层：调用链

- 模块：M02 Driver API 导出层
- 证据状态：关键 API 入口、初始化辅助、工具/Profiler accessors 已静态确认；完整 wrapper 生成规则仍需专门分析。

## CHAIN-M02-INIT：显式初始化

```text
muInit(0) / 调用方
  -> muapiInit(flags)                                      [src/driver/mu_context.cpp:121-133]
     ├─ flags != 0 -> MUSA_ERROR_INVALID_VALUE
     └─ Musa::CreatePlatform(nullptr)
        -> Platform::Get().Init()                          [src/musa/core/lib.cpp:14-18]
```

注意：绝大多数非 init API 不会自动创建平台，而是调用 `InitPlatform()` 检查平台是否已存在；若 `Musa::GetPlatform` 失败则返回 `MUSA_ERROR_NOT_INITIALIZED`。[src/driver/internal.h:306-316]

## CHAIN-M02-DEVICE-QUERY：设备句柄和属性查询

```text
muDeviceGet(&device, ordinal)
  -> muapiDeviceGet                                        [src/driver/mu_device.cpp:14-37]
     -> InitPlatform()
     -> Platform::Get().GetIDeviceView(ordinal)
     -> pDevice->GetId()

muDeviceGetAttribute(&pi, attrib, dev)
  -> muapiDeviceGetAttribute                               [src/driver/mu_device.cpp:71-94]
     -> InitPlatform()
     -> Platform::Get().GetDeviceCount()
     -> Platform::Get().GetIDeviceView(dev)
     -> Device::GetAttribute(pi, attrib)                   [src/musa/core/device.cpp:40-104]
        -> switch(attrib) reads Hal::DeviceProperties fields
```

`GetIDeviceView` 只是按 seqID 返回 `m_Devices[seqID]`，不触发 `LaterInit`；需要区分它和 `Platform::GetDevice`。[src/musa/core/platform.cpp:358-364]

## CHAIN-M02-CONTEXT-TLS：当前 Context 设置

```text
muDevicePrimaryCtxRetain(&ctx, dev)
  -> muapiDevicePrimaryCtxRetain                           [src/driver/mu_context.cpp:472-488]
     -> Platform::Get().GetDevice(dev)
        -> Device::LaterInit() if needed                   [src/musa/core/platform.cpp:140-147]
     -> pDevice->GetPrimaryContext()
     -> pContext->Retain()

muCtxSetCurrent(ctx)
  -> muapiCtxSetCurrent                                    [src/driver/mu_context.cpp:280-295]
     -> TlsCtxPop/TlsCtxPush
```

## CHAIN-M02-EXPORT-ACCESSOR：内部访问表

`mu_entry.cpp` 中的 `internalAccessors` 把 `muapiInit`、`muapiDriverGetVersion`、`muapiDeviceGetCount`、`muapiCtxCreate`、`muapiCtxDestroy` 暴露给 MUPTI/内部使用。[src/driver/mu_entry.cpp:1240-1247]

## 缺口

1. `mu_wrappers_generated.cpp` 的生成来源和 public symbol 到 `muapi*` 的完整分发表尚未完成专门文档。
2. `cmake/export_symbols.cmake` 需要和最终动态库导出表做一次对应检查。
3. API wrapper 中的注入、trace、错误转换宏还未逐层展开。
