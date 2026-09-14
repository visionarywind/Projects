# M04 Driver 风险与技术债务

- 证据状态：风险评估；不是已验证缺陷

## 高风险

- 用户态 HAL、SDK-driver、内核模块、固件和 Runtime 版本/ABI 不匹配。
- ioctl、DMA、HDC 和 queue 参数涉及用户指针、长度和异步生命周期；校验或释放顺序错误可能造成崩溃或数据损坏。
- 活动 HDC session/queue 仍存在时释放 client/context，可能破坏底层通信；现有代码已对 HDC client 返回 busy `[driver/src/ascend_hal/hdc/common/hdc_client.c:114-179]`。

## 中风险

- SoC 条件编译（如 Ascend910B）使不同平台的组件集合和行为不同 `[driver/src/ascend_hal/CMakeLists.txt:9-29]`。
- 设备节点、PCI 设备 ID、内核版本和权限要求未形成统一兼容矩阵。
- Driver 错误向 Runtime/ACL 转换时可能丢失原始上下文。

## SVM ordinary cache 风险

- V2/V3 是产品编译分支；误以为当前环境使用另一代实现会导致错误定位和回归遗漏 `[driver/src/ascend_hal/svm/CMakeLists.txt:11-15]`。
- V2 的 mapped/unmapped 状态、V3 的 range/area 状态和底层 backing 必须一致；静态源码不能确认闭源内核中的物理页生命周期。
- cache threshold、fragmentation、shrink 和 `DRV_ERROR_BUSY` 处理需要长期压力与故障注入验证；当前没有命中率或性能数据。


- 追踪每个 HAL API 到 ioctl、内核模块和设备命令。
- 生成 ioctl ABI、结构体版本和符号导出清单。
- 增加故障注入、DMA 边界、并发销毁、热插拔和长期压力测试。
