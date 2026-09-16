# Driver API 导出层：接口契约

| 接口方向 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| 上游 -> 本模块 | API 参数、句柄或构建选项 | 状态更新、对象、库/工具 | 版本/对象有效 | `MUresult` 或配置错误 |
| 本模块 -> Core | 规范化参数 | Core 对象方法调用 | current context/Device 可用 | 向上传播 |
| 本模块 -> HAL | `Hal::*CreateInfo`、queue/memory 参数 | 底层资源/提交 | HAL device 有效 | `HalToMuResult` |
| 观测 -> 本模块 | accessor/query 请求 | ID、时间戳、metadata | 对象仍存活 | 访问失败/空值 |

公共 ABI 兼容性是 M02 的硬约束；其他模块应避免直接改变句柄布局和导出表顺序。此处的“应”是开发约束，不是源码事实。
