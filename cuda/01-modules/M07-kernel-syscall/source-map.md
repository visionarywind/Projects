# M07 源码地图、数据与风险

## 文件地图

- `src/syscalls/assert`：device assert 相关资源。
- `src/syscalls/cnp`：child/nested launch 支持。
- `src/syscalls/graphs`：graph 相关 device support。
- `src/syscalls/malloc`、`minimalloc`、`hma-malloc`：device allocation 路径候选。
- `src/syscalls/printf`、`profile`、`debugger`：device-side observability。
- `src/kernels/x86_64`、`i686`：内建 kernel/架构变体。
- `src/asm`：架构汇编、模板和生成输入/输出。
- `src/cui/cuisyscall.c`：CUI 到 syscall 资源管理接口。

## 关键数据

- syscall resource bundle：launch 期间的 device-side syscall 地址、常量或 buffer（具体字段待核对）。
- CNP launch queue：child launch 请求的排队状态（类型声明见：[src/cuda_types.h:168-181]）。
- generated cubin/assembly：构建阶段产物，不应当当作当前源码的唯一证据。

## 风险

1. CNP、printf、device malloc 可能引入额外 launch/memory 依赖，必须纳入 tracking。
2. 架构 SASS/QMD 差异使同一 syscall 不能假定跨 GPU binary-compatible。
3. 外部 compiler/gpgpucomp 缺失时无法验证 device ABI、寄存器/constant-bank 布局和生成结果。

**深度状态**：目录和 launch 连接已完成；syscall 逐函数 ABI、kernel 生成命令、asm 到最终 cubin、CNP queue 消费未完成。
