# M05 性能与风险

主要风险是 tile 边界越界、shared load/compute barrier 不匹配、double-buffer parity 错误、寄存器压力导致 occupancy 下降，以及 layout/stride 与 launcher 不一致。优化时依次确认 coalescing、shared bank conflict、寄存器占用和 Tensor Core 利用率，不以单一历史 TFLOPS 判断改动。

证据：`[kernels/sgemm/sgemm.cu:168-482]`。
