# M07 设计

M07 把 device-side service 视为 kernel launch 的附属 ABI，而不是普通 host API。syscall imports 由 function/module metadata 指示，launch setup 将相关 syscall function memory 纳入 tracking；架构汇编和 cubin 是目标架构生成结果。

CNP、printf、malloc、assert 等服务可能同时要求 device memory、queue、constant data 和异步完成协议。
