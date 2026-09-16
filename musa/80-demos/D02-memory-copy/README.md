# D02：内存拷贝/批量异步候选

候选入口：`tests/memcpyBatchAsync.cu`、`tests/memcpy3DBatchAsync.cu`。预计覆盖 M02/M04/M05/M06/M09。当前仅确认文件候选，未读取或运行源码；不要把候选流程写成已验证结果。

推荐追踪：`muapiMemcpyAsync` -> `Context::GeneralMemcpy` -> `Stream::CmdCopyMemory` -> `AsyncMemcpyCommand` -> `QueueCommand` -> `Submit`。
