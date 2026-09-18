# Object Store 与 Object Manager：行级分析

## 证据锚点
- `src/ray/object_manager/object_manager.cc:66-718`、`python/ray/_private/worker.py:3032-3081`

## 结论
- 入口负责验证/归一化，副作用发生于：ObjectManager 启动 RPC，按 Plasma 或 filesystem source 发送对象块，接收端完成后通知等待者。
- 分支：本地 Plasma、spill、pull 合并、取消、节点移除。
- 错误：对象丢失、传输失败、ObjectStoreFull、owner/node failure。

动态行为未运行，不写成已验证。
