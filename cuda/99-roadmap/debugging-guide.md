# 调试指南

1. 先确认 API wrapper 的参数、输出清零和 context/TLS 检查。
2. 再检查 context sticky error、stream/channel 状态和 marker。
3. 对 launch 按 callback 阶段定位：syscall setup、function setup、pushbuffer、HAL control。
4. 使用源码已有 debugger/memcheck/profiler 边界理解额外同步和资源；这些工具未在本工作区运行。

不要将无 GPU 或无外部 RM 的静态观察写成运行时诊断结果。
