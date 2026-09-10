# M06 风险与债务

- 架构宏和 PTX/WGMMA 指令与 GPU capability 强绑定。
- shared-memory padding/swizzle 错误可能同时造成错误结果和性能退化。
- stages、寄存器占用和动态 shared memory 存在资源权衡。
- wheel、makefile 和即时 benchmark 三套构建路径可能漂移。
- 历史 TFLOPS 不具备跨环境可比性。

证据：`[kernels/hgemm/setup.py:21-38]`、`[kernels/hgemm/makefile:10-18]`。
