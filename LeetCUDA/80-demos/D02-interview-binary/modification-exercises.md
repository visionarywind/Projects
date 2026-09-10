# D02-S06 修改练习

1. 为 HGEMM warmup 同步补 `check()`，比较错误传播一致性。
2. 在 `check_smem_feasible` 旁记录每个 stage/swizzle 配置的 skip 原因和 shared-memory bytes。
3. 增加一个小矩阵 `M/N/K` 的 correctness-only 路径，分离 benchmark 与验证。
4. 为不同 `--arch` 输出编译宏和实际 binary 名称，避免运行错架构。
5. 给 host/device/event/stream/TMA map 资源引入统一清理 helper 或 RAII，并对照现有显式释放。
6. 对 FA 非整 tile、非对齐 sequence、D=64/128 运行 sanitizer，再更新 M07 风险。

所有练习未执行；修改应同时更新源码证据、testing、risk-register 和本 Demo 映射。
