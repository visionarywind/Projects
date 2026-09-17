# D01 数据、状态与资源轨迹

- 版本：HEAD `cfe4725d23`；静态分析，未运行。

## 状态变化

| 步骤 | 数据/状态 | 所有权/上下文 |
|---|---|---|
| 定义 | Python function + RemoteFunction options | Driver Python 对象 |
| 首次 submit | descriptor + pickled function + task options | Driver export → runtime |
| submit 返回 | ObjectRef | Driver 持有逻辑引用，实际对象由 object data plane 管理 |
| Worker 执行 | 字符串 `Hello, world!` | Worker 执行用户代码 |
| get | value materialized/deserialized | Object Manager/CoreWorker → Driver |
| print/exit | stdout；runtime cleanup | Driver/运行时边界，未验证 |

## 资源

静态代码没有在 Demo 中显式指定 CPU/GPU/内存；默认选项由 Ray runtime 解释。实际 worker 数、进程和 object store 行为不能仅由脚本确定。

## 相关文档
[执行轨迹](execution-trace.md) · [全局数据流](../../00-overview/global-data-flow.md)

## 源码证据摘要
`hello_world.py:3-9`；`remote_function.py:382-415,527-574`；`worker.py:2996-3029`。

## 未解决问题
引用计数、对象 pinning、spill 和进程退出清理待动态验证。

## 下一步阅读建议
结合 M03 的对象生命周期页面补充。
