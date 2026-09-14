# 技术债

- 源码目录缺少 Git 元数据，无法绑定具体 commit/branch。
- 外部 common/RM/compiler/firmware 和 nvmake 配置不完整，多个动态分派只能到边界。
- M06 的 ELF/JIT/graph 深度、M07 syscall/asm 生成链、M04 UVM fault/migration、M05 GPFIFO/marker 设备消费尚未闭合。
- OpenCL enqueue/interop 对象销毁、tools 注册/attach 协议、MODS/CI/package 仍需源码核对。
- 所有构建、DVS、GPU、OpenCL、debugger、profiler 结果未验证。

下一步以 `00-overview/analysis-state.md` 的模块表和 `99-roadmap/next-steps.md` 为准。
