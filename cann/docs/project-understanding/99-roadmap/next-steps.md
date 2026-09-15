# 后续分析路线

- 当前版本基线：GE `8ee1b040a`、ACL `50be4c922`、Runtime `50be4c922`、Driver `977336913`

## P0：补齐跨仓关键路径

1. 追踪 Runtime `Api::SetDevice`、Context、Stream、Memory 和 Task 的实现。
2. 追踪 Runtime 到 HAL 的实际导出符号、动态库和错误码。
3. 追踪 Driver HAL 到 SDK-driver、ioctl、内核模块和设备节点。

## P1：补齐模块深度

- GE Compiler Pass、EnginePartitioner、Memory Planner、V1/V2 差异。
- ACL Model、Single-op、Tensor/DataTransfer 和 callback。
- Runtime scheduler、DFX、内存和多设备并发。
- Driver HDC、Queue、esched、SVM、TRS、DMS、RoCE。

## P2：验证和工具化

- 完成链接断链检查和证据索引自动生成。
- 在无硬件环境跑可运行的静态检查/UT，并保存结果。
- 在匹配 NPU 上执行样例、集成、故障注入和性能测试。
- 用 GDB、trace、profiling 和日志确认异步生命周期与错误传播。

完成每项后更新 `analysis-state.md` 的覆盖率、断点和证据状态。
