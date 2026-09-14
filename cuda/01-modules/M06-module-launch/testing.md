# M06 测试与验证

- `basic_sanity:kernel` 通过内部 module/function 查找 `gpuIncrement`，检查 CNP 能力、launchCount、push count 和结果（静态确认：[tests/cuda_test/basic_sanity.cu:133-163,169-234]）。
- capture 分支的 graph node 创建和失败失效逻辑由 API common 实现，但当前没有运行时 capture 结果（静态确认：[src/api/apilaunch.c:252-286]）。
- 参数 metadata 缺失、非法 grid、function/stream 跨 context 属于应覆盖的错误路径；源码检查存在，测试执行未验证。

不要把测试中的成功预期写成设备行为已证实；当前工作区缺少 nvmake、GPU 和外部依赖运行条件。
