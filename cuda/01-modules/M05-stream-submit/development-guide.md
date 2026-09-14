# M05 开发指南

1. 新异步资源必须加入 stream/context destroy、tracking 和 marker 完成路径。
2. 修改 push 时保持 begin/end 配对，即使 HAL finalize/control 失败也要结束 push。
3. 记录 stream pool、context、channel、MPS submission 和 semaphore 的锁序。
4. 架构特化操作通过 HAL/DMAL 接口加入，不能在通用 stream 层硬编码 QMD 布局。
5. 用 stream/channel/sync 测试做静态覆盖；本工作区未运行 GPU 测试。
