# M02 ACL 数据结构与资源

- 证据状态：句柄类别和核心资源已确认；内部布局未知

## 主要对象

| 对象 | 语义 | 生命周期风险 |
|---|---|---|
| Device ID | 进程当前设备选择 | Reset 或切换设备可能影响后续 API |
| Context | 设备执行上下文 | Stream/Event 通常依附 Context |
| Stream | 异步任务有序队列 | 销毁前须处理未完成任务 |
| Event | 跨 Stream 同步/完成标记 | 不能在异步使用期间释放 |
| DataBuffer/Tensor | 数据地址和形状描述 | 地址、容量、shape 必须匹配 |
| Model/ModelRI | 已加载模型及执行资源 | 卸载顺序影响 buffer 和 stream |
| Callback | 异步完成通知 | 回调捕获对象必须存活 |

`CMakeLists.txt` 将 allocator、device、event、stream、memory、context、kernel、model、data buffer 和 callback 作为主要实现源文件 `[acl/CMakeLists.txt:167-190]`。内部句柄结构、引用计数、线程归属和跨设备规则尚未从本次分析完整确认。

## 资源图

```text
Device → Context → Stream/Event
                  ├→ DataBuffer/Tensor
                  └→ Model/Kernel task
```

## 不变量

1. 对象句柄必须来自本层创建 API。
2. buffer 地址和长度在提交时可读写且满足模型约束。
3. 异步提交期间不销毁被任务或回调引用的对象。
4. 销毁顺序应从任务/模型到同步对象、Context、Device。
