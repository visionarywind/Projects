# M05 测试、开发与风险

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
