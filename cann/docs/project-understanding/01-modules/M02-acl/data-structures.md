# M02 ACL 数据结构与资源

- 文档目的：解释 01-modules/M02-acl/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：句柄类别和核心资源已确认；内部布局未知
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-acl/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
