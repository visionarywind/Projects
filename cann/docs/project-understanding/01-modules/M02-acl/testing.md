# M02 ACL 测试

- 证据状态：测试入口已确认；测试结果未验证

## 测试层次

1. **Wrapper UT**：空指针、非法设备 ID、错误码映射和统计路径。
2. **Resource UT**：Context/Stream/Event/Memory 的创建、重复销毁和异常顺序。
3. **Model/Op UT**：模型加载、参数校验、异步 callback 和输出 buffer。
4. **Integration**：与 Runtime、Driver、固件和真实 NPU 联调。

## 回归重点

- Runtime 错误到 ACL 错误的每个分支。
- 异步 callback 在成功、失败、超时和对象提前销毁场景下的行为。
- 多设备/多 Context 和设备 reset 后的句柄失效。
- 利用率查询的保留参数和不支持字段。

执行入口见 [examples.md](examples.md)；没有硬件时只可运行不依赖设备的单元测试，不能宣称端到端通过。
