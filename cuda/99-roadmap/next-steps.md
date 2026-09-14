# 后续路线

1. 深读 M08：callbacks、debugger attach、memcheck/profiler/DRS 注册与销毁链。
2. 深读 M09：OpenCL enqueue 到 CUI、对象生命周期、GL/D3D/external interop。
3. 深读 M10：MODS/package/CI 全量注册、DVS 结果模型和可复现实验边界。
4. 回读跨模块页面，补齐 M04 UVM、M05 submit、M06 module/graph、M07 syscall 新增证据的索引和依赖关系。
5. 在完整外部树和硬件环境中再执行构建与 DVS；把日志作为新的已验证证据。

当前阶段的结论仍以静态确认、推断、未知和未验证为主；DMAL/RM/firmware、compiler/gpgpucomp、nvcc 生成物和 GPU 行为没有被本轮运行验证。
