# M05 测试、开发与风险

- 文档目的：解释 01-modules/M05-stream-submit/testing.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M05-stream-submit/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 测试

- `unittest_stream_push.cpp` 直接验证 `beginPushCount`、`currentPush.stream/channel`、`channelOfLastWork` 和 `streamEndPush`（静态确认：[tests/cuda_test/stream/unittest_stream_push.cpp:30-196,198-259]）。
- `unittest_stream_sync.cpp`、`sync/` 下 marker/semaphore 测试覆盖等待和生命周期；测试文件存在但未执行。
- `basic_sanity:kernel` 检查 launch 后 `beginPushCount` 增长、context synchronize 后结果可见，并读取 per-thread stream 的 compute channel（静态确认：[tests/cuda_test/basic_sanity.cu:214-234]）。

## 修改配方

1. 新 stream flag：同步修改 API VerifyFlags、CUI flag conversion、stream policy、getter/setter 和测试。
2. 新异步资源：在 create 的正常路径、`Error`、detach、reclaim 四处定义所有权。
3. 修改 push：保留 channel/MPS submission lock 规则，确认跨 channel flush 和 tracking semaphore。
4. 修改 reclaim：不能将 public handle 失效等同于 CUI 对象立即 free；必须保留 marker 保护。

## 风险

- lock ordering：QMD portable registration 放锁外是明确设计，随意移入 context lock 可能死锁（证据：[src/api/apistream.c:94-99]）。
- detached stream 过早回收会使 GPU 仍访问的 QMD/semaphore 失效。
- backend Init 成功前后的错误回滚分支不同，新增资源必须挂到正确阶段。
- channel 最终设备提交依赖外部 backend，静态证据不支持“已到 GPU”。

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
