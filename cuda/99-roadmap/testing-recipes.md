# 测试配方

1. 静态核对 binary 在 case 前定义，确认 `.cu` device-link 和内部 fatbinary 宏。
2. 为每个改动覆盖成功、参数错误、context/资源错误和清理。
3. 运行环境具备后先 `--listGpus`，再使用明确的 case/模式执行。
4. 阅读 stdout/stderr 和 DVS summary，不把 waived 当 pass；零结果按 runner 失败。

## 资源管理专项

- **Suballocator**：准备同一 descriptor 的小于 generic block size 的两次分配，核对 memblock serial 是否复用；释放中间对象后检查 free region split/coalesce 和 `memblockGetInfo().frag`；再覆盖 `noSuballoc`、固定地址、sharing、descriptor 不兼容和 DMAL failure rollback。
- **Graph**：准备 capture success、invalidated/unjoined、multi-context instantiate、重复 launch、destroy-after-launch 和 exec update topology/function/memory-type failure。核对 QMD、constant-bank、internal stream、completion marker、scheduler backing 和 UVM running 的创建/回收成对性。
- 当前上述场景均未执行；`tests/cuda_test/tests.nvmk` 也没有专用 CUDA Graph/capture case，不能用 channel queued-dependency graph 测试代替。

本知识库未执行任何 nvmake、DVS、GPU 或系统权限命令。
