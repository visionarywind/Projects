# D01 修改练习

1. 为 `cuMemAlloc` 增加参数错误测试，保持输出指针先清零和登记失败回滚。
2. 为 stream destroy 增加 detached→marker completion 的断言，验证 public handle 与内部对象的两阶段生命周期。
3. 为 launch 增加非法 function/stream 跨 context 和缺失 parameter metadata case。
4. 为 `basic_sanity` 增加多 GPU/过滤和 CNP unsupported 的明确日志。
5. 修改架构 HAL 前，检查 `cuda.nvmk` 的 `NVCFG_GLOBAL_ARCH_*` 条件和对应 QMD/launch 文件。

每个练习需在完整外部树、工具链和 GPU 环境中执行；当前仅提供源码级设计。
