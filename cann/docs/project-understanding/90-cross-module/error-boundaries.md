# 跨模块：错误边界

- 证据状态：主要转换点已确认；完整数值映射未知，异步设备错误未在硬件上验证

## 分层转换

```text
Driver drvError_t / errno / ioctl result
  -> HAL return
  -> Runtime rtError_t / extended error
  -> ACL aclError 或 GE Status/graphStatus
  -> synchronous return / async callback / log
```

Runtime 统一错误门面 `[runtime/src/runtime/api/api_c.cc:118-153]`；ACL 设备包装映射 Runtime 错误 `[acl/runtime/device.cpp:47-59]`；GE 异步回调记录失败并转换 Tensor `[ge/api/session/client/ge_api.cc:87-101]`。

## 内存路径的错误边界

| 路径 | 提交前/本地失败 | 跨层失败窗口 | 清理与残留风险 |
|---|---|---|---|
| KernelMemoryPool | 0 字节、超过 2 MiB 或无适配池时绕过/新增池；链表分配失败返回上层 | `DevMemAlloc` 申请 2 MiB backing 失败 | 池对象和 Driver backing 必须成对释放；碎片行为需压力验证 `[runtime/src/runtime/core/src/pool/memory_pool.cc:15-106]` |
| 普通 `rtMalloc` | Context、policy、对齐和 module/device 配置先校验 | `halMemAlloc`、huge/normal fallback 或 run-mode 分支失败 | 保留 device、size、policy、module 和原始 Driver 错误 `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]` `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114]` |
| SOMA async malloc | Segment 本地分配成功后配置 HAL/AICPU | `StreamMemPoolAsyncConfig` 或 `SomaMemMng` launch 失败 | 当前回收可能将 BUSY 段转为 CACHED；设备侧配置与本地 metadata 是否完全回滚未确认 `[runtime/src/runtime/api/impl/api_impl_soma.cc:64-109]` |
| SOMA async free | 区间/分配查询和 stream 归属先校验 | Driver VMM/free ioctl 或 AICPU 提交失败 | Driver V3 free ioctl 失败明确不回滚；本地与设备侧状态可能暂时不对称 `[driver/src/ascend_hal/svm/v3/api/master/svm_soma.c:862-901]` |
| SOMA trim/destroy | `CanDelete`、handle 和 pool state 先检查 | Driver trim/destroy 失败 | `MemPoolTrimImplicit` 当前直接成功返回，不能把调用点当成实际回收；设备侧 busy destroy 语义未验证 `[runtime/src/runtime/feature/soma/soma.cc:223-233,328-333]` |

## Driver ordinary cache 失败边界

普通 cache 失败不是单一错误：V2 可能在 mapped cache、idle size 或新 heap 之间回退；V3 的 cache area 不足会先扩展 normal backing。V2 shrink 底层释放失败时源码有 node/tree/统计回滚；V3 底层返回 `DRV_ERROR_BUSY` 时进入 recycle segment。底层内核/固件最终物理资源状态仍未验证。[../01-modules/M04-driver/driver-memory-pool-analysis.md](../01-modules/M04-driver/driver-memory-pool-analysis.md)


- 参数/句柄：当前层立即拒绝，并保留 API 和参数上下文。
- 状态/生命周期：调用方修正顺序；避免对 busy/未初始化错误盲目重试。
- 资源/设备：保留最底层错误、设备和队列信息，检查 Driver/固件。
- 模型/编译：回到 Graph、shape、算子和编译配置。
- 异步：以 callback/event/synchronize 作为完成边界，不能只看提交返回值。

## 诊断字段

建议跨层统一记录：时间戳、线程、设备、Context、Stream、Graph/Model、pool、VA、size、API 阶段、原始错误码、映射后错误码和资源句柄。
