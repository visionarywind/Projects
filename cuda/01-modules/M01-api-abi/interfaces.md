# M01 接口契约

| 接口 | 前置 | 副作用 | 错误边界 |
|---|---|---|---|
| `cuapiInit` | flags 为 0 | 初始化全局 CUI | init 错误原样返回 |
| `cuapiMemAlloc_common` | output 非空、size>0、current context | 创建/注册 `CUmemobj` | registration 失败回滚 |
| `cuapiMemFree_common` | base pointer、允许 API source | 同步 context 后释放 | sync 失败保留对象 |
| `cuapiLaunchKernelCommon` | 合法 function/stream/context | graph node 或异步 launch | capture/launch 错误 |

公开句柄不是内部对象所有权证明；所有权由 context、global registry、stream pool 或 graph 记录决定。
