# M02 风险与测试

## 风险

1. **初始化部分成功**：新增初始化资源若未加入 `Error` 反向回滚，会泄漏或让后续状态假初始化；证据：[src/cui/cuiinit.c:3201-3208]。
2. **context 版本错配**：legacy API 在 v3020 context 上会返回 `CUDA_ERROR_INVALID_CONTEXT`，反之亦然；证据：[src/cui/cuiinit.c:3010-3025]。
3. **销毁竞态**：context destroy 假定调用者已处理 TLS；不遵守该前置条件可能留下 current context 悬挂，证据：[src/cui/cuictx.c:367-370]。
4. **sticky error 延迟观察**：错误可能在后续 API 才出现，测试若只检查发起调用会漏报，证据：[src/cui/cuiinit.c:3031-3037]。

## 测试证据

`basic_sanity.cu:init` 先验证未初始化状态，再 `cuInit(0)`，检查 globals、PID 和设备数；随后直接创建 context、分配/释放 memobj 并销毁 context（静态确认：[tests/cuda_test/basic_sanity.cu:49-100]）。测试未运行。

## 深度状态

初始化顺序、检查策略和 context 生命周期已完成静态深读；TLS 内部实现、context deinitialize 全量对象清单、各平台退出路径和 sticky error 产生端仍为下一批。
