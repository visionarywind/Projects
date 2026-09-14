# M08 数据结构

| 对象 | 作用 |
|---|---|
| callback enable table | 按 domain/cbid 快速判断是否 dispatch |
| debugger context/module/grid maps | 将 driver 对象映射到调试 target 状态 |
| memcheck error/table buffers | device 错误信息和全局 allocation 镜像 |
| profiler global object | profile mode、mutex、perfmon 状态 |

## 所有权与异步边界

- callback enable table 和 callback 参数由 tools/etbl 层维护，但其中的 context、stream、module、function 指针借用 M02/M05/M06 对象；工具不能把 callback 返回视为对象所有权转移。
- memcheck `errorEntry`、device allocation table 和 host mirror 是与 `CUctx`/device memory 绑定的临时资源；创建失败应沿调用者回滚，context/record 销毁后才可释放 device 侧镜像（静态确认：[src/devtools/memcheck/memcheck.c:120-256]）。
- debugger shared variables 的生命周期跨越 attach session，具体 client/RPC 持有者不在当前树中；`cudbgApiCheck` 只证明访问前置条件，不证明跨进程同步。
- profiler global object 的 mutex 和 mode 是进程级状态，perfmon 命令却嵌入具体 launch push；模式销毁与 GPU counter 完成之间存在未闭合的异步边界。

工具对象与 M02/M04/M06 共享生命周期；callback 不等价于 GPU completion。具体注册/注销和外部 client 协议仍未知（静态确认：[src/etbl/tools/tools_callbacks_internal.h:22-41]；[src/devtools/debugger/cudbgapi.c:401-557]）。
