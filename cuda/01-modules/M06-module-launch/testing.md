# M06 测试与验证

- `basic_sanity:kernel` 通过内部 module/function 查找 `gpuIncrement`，检查 CNP 能力、launchCount、push count 和结果（静态确认：[tests/cuda_test/basic_sanity.cu:133-163,169-234]）。
- capture 分支的 graph node 创建和失败失效逻辑由 API common 实现，但当前没有运行时 capture 结果（静态确认：[src/api/apilaunch.c:252-286]）。
- 参数 metadata 缺失、非法 grid、function/stream 跨 context 属于应覆盖的错误路径；源码检查存在，测试执行未验证。

## 资源生命周期覆盖

当前 `tests/cuda_test/tests.nvmk` 未注册专用 CUDA Graph/capture 测试。`channel`/`marker` 测试中的 queued-dependency graph 是提交层内部图，不等同于 `cuGraph*` API。应新增以下可注入/可运行场景：

1. capture begin → kernel/memcpy/memset → end 的成功路径，以及 wrong-thread、unjoined 和 invalidated capture 的销毁路径；
2. 多 kernel/conditional graph instantiate 的 QMD、constant-bank、internal stream、marker 和 scheduler backing 分配/释放；
3. graph launch 重复执行、跨 context launch stream、节点中途失败和 destroy-after-launch；
4. QMD semaphore pool、`memobjAlloc` 或 `cuiStreamCreate` 失败时的 instantiate rollback；
5. `cuGraphExecUpdate` 的 topology/function/memcpy/memset 参数拒绝和成功更新。

以上均为测试建议或静态覆盖，当前没有 GPU/nvmake 执行结果。

不要把测试中的成功预期写成设备行为已证实；当前工作区缺少 nvmake、GPU 和外部依赖运行条件。
