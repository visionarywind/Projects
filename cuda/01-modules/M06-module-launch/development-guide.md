# M06 开发指南

1. 修改 module metadata 时同步 ELF parser、function creation、parameter validation/packing 和 debugger registration。
2. 修改 launchData 字段时同步 graph instantiate、HAL launchCheck、ABI encode 和 QMD。
3. 修改 launch setup 时保留 syscall→HAL check→const bank/QMD→push 的顺序。
4. 对失败路径检查 UVM DAG rollback、streamEndPush、tools launch-end 和 function transient state。
5. JIT/ELF 依赖外部 compiler/toolchain，本工作区仅做静态分析。
