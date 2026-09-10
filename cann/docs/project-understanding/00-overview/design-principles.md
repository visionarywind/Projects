# 总览：设计原则

- 对应源码版本：GE `47020afc8`；ACL `d26308d3`；Runtime `dae460b78`；Driver `6e2914c`
- 证据状态：分层和接口边界已确认；部分设计动机为推断

## 核心原则

1. **分层隔离**：应用面向 ACL/GE，ACL 和 GE 通过 Runtime API 使用设备能力，Runtime 再通过 HAL/Driver 接触设备。该边界体现在各仓 README 与 CMake 依赖中 `[acl/README.md:20-41]`、`[runtime/src/CMakeLists.txt:13-32]`、`[driver/src/CMakeLists.txt:9-30]`。
2. **图语义与执行资源分离**：GE 用 AscendIR 表示 Graph、Node、Tensor 和 Anchor；编译阶段产生执行模型，Executor 才管理 Stream、Event、内存和设备执行 `[ge/docs/zh/design/architecture.md:54-93]`。
3. **门面统一错误和对象校验**：Runtime C API 通过 `Api::Instance()` 进入内部实现，并在 API 层验证句柄、转换错误 `[runtime/src/runtime/api/api_c.cc:118-153]`；ACL 对 Runtime 返回值进行 ACL 错误码映射 `[acl/runtime/device.cpp:47-59]`。
4. **显式生命周期**：GE Session、V2 Executor、Runtime 设备对象和 Driver HDC client 都拒绝不安全的顺序调用；例如 V2 只有 Loaded 状态可执行，HDC client 存在活动 session 时不能销毁 `[ge/runtime/v2/core/model_v2_executor.cc:201-305]`、`[driver/src/ascend_hal/hdc/common/hdc_client.c:114-179]`。
5. **能力按构建目标拆分**：GE 可拆为 `ge-compiler`、`ge-executor` 和 `dflow-executor` `[ge/CMakeLists.txt:30-47]`；Driver 区分 HAL 和 SDK-driver `[driver/src/CMakeLists.txt:9-30]`。

## 需要保持的约束

- 头文件、共享库、模型格式、设备固件和工具链必须来自兼容的 CANN 版本；Runtime README 明确要求源码版本配套 `[runtime/README.md:16-19]`。
- 异步 API 的调用者必须保持输入、输出、回调和相关 stream/event 有效，直到完成通知。
- 不能把无硬件环境下的编译成功等同于设备运行成功。

## 证据边界

源码已确认“做什么”和主要状态检查；未从代码单独证明每个优化 Pass 顺序的设计动机、所有线程安全保证或所有闭源设备实现。因此此类结论标为“推断”或“未知”。
