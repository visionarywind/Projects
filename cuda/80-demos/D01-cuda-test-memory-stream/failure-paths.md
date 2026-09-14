# D01 失败路径

| 失败点 | 源码行为 |
|---|---|
| 非零 init flags | wrapper 返回 `CUDA_ERROR_INVALID_VALUE` |
| 无效 device/context | API 参数或 handle 检查返回错误 |
| memobj 登记失败 | 释放局部 memobj 并返回原错误 |
| CNP 不支持 | 测试按 capability 调整 expected，不证明 launch 成功 |
| syscall/HAL/push 失败 | 结束 push，必要时回滚 UVM running，发送 launch end |
| GPU 未完成时 stream destroy | public handle 失效，内部 stream 留在 detached，marker 完成后回收 |

证据：[src/api/apiinit.c:19-47]、[src/api/apimem.c:100-111]、[src/cui/cuilaunch.c:779-817]、[src/cui/cuistream.c:1926-1954,2004-2088]。运行未验证。
