# M07 风险与技术债

| 项目 | 状态 |
|---|---|
| 生成链不可见 | `cudaAsm.nvmk`/`cudaSyscalls.nvmk` 依赖外部工具和配置 |
| 架构 binary 差异 | Kepler–Ampere 各有 SASS/QMD/launch 文件 |
| CNP 异步依赖 | child launch 可能继续使用父 launch 资源 |
| 外部边界 | compiler/gpgpucomp、RM、firmware 未在树中完整出现 |
| trampoline/device ABI | 原函数 PC 经 extern symbol table 链入架构 trampoline | cubin、编译器和设备侧调用约定不在当前手写 C 源码中闭合 |
| import refcount | module bitset 驱动 0→1 初始化、0 时停用 | callback 失败与卸载回滚需要跨 module/context 检查 |
| 生成物选择 | `cudaSyscalls.nvmk` 按 `DEV_ARCHS` 生成并嵌入每架构 cubin | 未运行 nvcc/bin2c，不能把产物匹配标为已验证 |

证据：运行时初始化/回调/销毁见 `[src/cui/cuisyscall.c:124-205,378-734,1157-1347]`；生成源清单见 `[cudaSyscalls.nvmk:46-71,111-160]`；CNP 测试见 `[tests/cuda_test/basic_sanity.cu:136-145,214-225]`。模块深度已达到 host 侧静态主线，设备侧 queue/ABI 仍未知。
