# M03 开发指南

1. 新架构：加入 HAL header、实现、`NVCFG` 条件和构建源清单，并确认 `ccArch` case。
2. 新 DMAL backend：实现 channel init/deinit/push/completion，核对 semaphores 和 error cleanup。
3. 修改 device ordering：同时更新 remap、environment filter、测试 ordinal 和文档语义。
4. 不把当前树中的候选架构当作实际启用架构；需外部 nvmake 配置确认。
