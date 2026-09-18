# Object Store 与 Object Manager：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`src/ray/object_manager/object_manager.cc:66-718`、`python/ray/_private/worker.py:3032-3081`|公共/模块入口|已确认|
|实现|ObjectManager 启动 RPC，按 Plasma 或 filesystem source 发送对象块，接收端完成后通知等待者。|真实状态改变|已确认|
|测试|src/ray/object_manager/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
