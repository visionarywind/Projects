# 总览：全局错误模型

- 证据状态：错误分层模式已确认；完整错误码表和所有转换规则待补

## 错误流

```text
设备/驱动错误
  -> Runtime rtError_t / 扩展错误码
  -> ACL aclError 或 GE graphStatus/Status
  -> 日志、回调或调用方返回值
```

Runtime C API 使用统一宏验证句柄和返回错误 `[runtime/src/runtime/api/api_c.cc:118-153]`。ACL 设备包装在 Runtime 失败时使用 `ACL_GET_ERRCODE_RTS` 进行映射 `[acl/runtime/device.cpp:47-59]`。GE 异步回调在失败时记录错误并把 GERT Tensor 转为 GE Tensor `[ge/api/session/client/ge_api.cc:87-101]`。

## 错误类别

| 类别 | 示例 | 调用方动作 |
|---|---|---|
| 参数错误 | 空指针、数量不匹配、非法设备 ID | 修正参数，不重试同一请求 |
| 状态错误 | 未初始化、Executor 非 Loaded、活动 session 仍存在 | 按生命周期补齐或释放 |
| 资源错误 | 内存、Stream、Queue、HDC 资源不足 | 记录上下文，按策略清理/重试 |
| 设备/驱动错误 | 设备不可用、IOCTL/HDC 失败 | 检查驱动、固件、设备日志 |
| 编译/模型错误 | 图不合法、shape/算子不支持 | 回到模型和编译配置排查 |
| 异步错误 | callback 返回失败、任务执行失败 | 等待完成通知并保留原始错误上下文 |

## 诊断原则

- 优先保存最底层错误码、设备 ID、Context/Stream、模型/Graph 标识和调用阶段。
- 不以 `ACL_SUCCESS` 或 `GRAPH_SUCCESS` 推断设备任务已完成；异步路径必须同步或等待事件。
- 日志和 DFX 能力属于旁路诊断设施，不改变资源所有权。

## 未解决问题

需要从公共头文件和错误管理实现生成 Runtime→ACL、Runtime→GE、Driver→HAL 的完整映射表；当前文档不假设未核实的数值错误码。
