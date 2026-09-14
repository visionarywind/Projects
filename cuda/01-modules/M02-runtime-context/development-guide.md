# M02 测试与开发指南

- 修改初始化：同步新增资源的创建顺序、Error 回滚和 deinitialize。
- 修改 context 字段：检查所有 API version/state/sticky error 检查点。
- 修改 TLS：检查 current context、per-thread stream、线程退出和递归 API。
- 修改锁：记录 globals/initMutex、context lock、manager lock 的锁序。
- 使用 `basic_sanity:init` 验证 globals/TLS/context；测试和 GPU 运行均未执行。
