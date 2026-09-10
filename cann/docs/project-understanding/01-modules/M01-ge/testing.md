# M01 GE 测试

- 证据状态：测试边界已确认；本环境未执行

## 测试层次

- Graph/Node/Tensor/Anchor 元数据和图合法性 UT。
- Parser 到 AscendIR 的模型解析测试。
- Compiler Pass、Engine、内存/流规划和模型序列化测试。
- V1/V2 Executor 的 Load、Execute、UnLoad、重复调用和异常清理测试。
- 与 Runtime/ACL/Driver 和 NPU 的模型集成、动态 shape、异步 callback 测试。

## V2 回归矩阵

1. 未 Load 时 Execute，应返回状态错误。
2. 输入/输出数量不匹配、空指针和不允许的 IO 地址复用。
3. 外部 Stream 与内部 allocator 的组合。
4. init graph、main graph、de-init graph 任一路径失败后的清理。
5. ExecuteSync 超时、异步完成和重复 UnLoad。

构建入口见 [examples.md](examples.md)；无设备时不宣称模型端到端通过。
