# 性能指南

- 先区分同步边界：`cuMemFree` 显式同步，普通 launch 通常异步；debugger/profiler 可能额外阻塞。
- 观察 stream 并发策略、QMD 轮换、marker 和 channel push，避免以 API 返回时间代表 GPU 完成。
- memory suballocation 由 descriptor 兼容性、固定地址、sharing 和 generic block size 决定；改动需关注碎片与 backing 复用。
- profiler/perfmon 和工具 callback 会改变并发、WFI 和 push 时序，性能数据必须注明工具配置。

本页不提供实测数字；当前没有 GPU/profile 结果。
