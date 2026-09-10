# M04 接口

M04 没有统一接口：各目录可能采用 Python `load()`、独立 `.cu` 或不同 tensor rank。新增或修改接口时必须从 Python 参数、binding 检查和 kernel 索引三处共同确认 dtype/device/shape/stride；不能按目录名推断支持 arbitrary stride。

证据边界：`[README.md:338-387]` 只证明主题和目录，不足以证明单个算子契约。
