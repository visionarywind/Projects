# M04 Driver 测试

- 证据状态：测试目录和构建开关已确认；结果未执行

## 分层测试

- 参数校验：空指针、长度、设备 ID、vector 数量和地址。
- 生命周期：HDC client/session 创建、关闭、busy destroy；queue open/init/uninit/release。
- ABI：esched ioctl 命令、结构体布局、32/64 位和版本兼容。
- 设备集成：HAL、SDK-driver、内核模块、设备节点、固件和 NPU。
- 故障注入：连接失败、DMA map/unmap 失败、设备 reset、超时和热插拔（若平台支持）。

## SVM cache 专项

- V2：mapped/unmapped 命中、exact/upper-bound、node split/merge、cache/nocache、threshold shrink 和底层释放失败回滚。
- V3：cache 条件 bypass、多个 range、area split/merge、expand 失败回滚、完整 idle range、`DRV_ERROR_BUSY` 延迟 recycle。
- 产品矩阵：至少分别构建/验证 `ascend910B`（V2）与 `ascend950`（V3），不能将一套产物的结果外推到另一套。

这些是待执行回归项；本环境未运行构建、Driver UT、NPU、固件或性能测试。


1. 活动 session 时必须拒绝 HDC client 销毁。
2. file release 后 `private_data` 必须清空且 context 不再使用。
3. 非法 vector 不应进入 DMA/HDC 路径。
4. ioctl 错误应保留命令、设备和底层错误上下文。

没有匹配硬件时只能运行编译和不依赖设备的 UT，不能宣称设备集成测试通过。
