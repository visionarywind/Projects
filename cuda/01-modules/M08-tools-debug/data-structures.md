# M08 数据结构

| 对象 | 作用 |
|---|---|
| callback enable table | 按 domain/cbid 快速判断是否 dispatch |
| debugger context/module/grid maps | 将 driver 对象映射到调试 target 状态 |
| memcheck error/table buffers | device 错误信息和全局 allocation 镜像 |
| profiler global object | profile mode、mutex、perfmon 状态 |

工具对象与 M02/M04/M06 共享生命周期；callback 不等价于 GPU completion。具体注册/注销和外部 client 协议仍未知（静态确认：[src/etbl/tools/tools_callbacks_internal.h:22-41]；[src/devtools/debugger/cudbgapi.c:401-557]）。
