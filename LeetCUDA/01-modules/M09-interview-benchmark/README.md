# M09 Interview 整合与 benchmark

- 文档目的：说明 `notes-v2.cu` 学习/测试/benchmark 二进制的组织方式。
- 适用范围：`kernels/interview/`。
- 对应源码版本：`0983c65`。
- 证据状态：目录、构建脚本、主入口和资源检查已确认。
- 最后更新：2026-09-10
- 前置阅读：[M01](../M01-foundation/README.md)
- 后续阅读：[M06](../M06-hgemm-tensorcore/README.md)、[M07](../M07-flash-attention/README.md)

## 结论摘要

M09 把面试常见 CUDA 原语和高级 kernel 合并到一个编译单元：`notes-v2.cu` include `base/sgemv/sgemm/hgemm/flash_attn/ffpa_attn`，通过静态 globals 和 CLI 选择 test/bench；`build.sh` 为 sm_86/sm_89/sm_90a/sm_120a 选择 defines、库和输出名。它是复习/基准入口，不是普通 PyTorch extension。

## 文件层次

```text
common.cuh -> base.cuh -> sgemv.cuh/sgemm.cuh/hgemm.cuh/flash_attn.cuh
                                           -> notes-v2.cu (tests + CLI)
```

## 运行生命周期

脚本 compile（可 ccache）→ link cuBLAS/cuDNN/NVRTC/driver → 二进制解析 flags → correctness/smoke test 或 benchmark → `check()` 检查 CUDA → 输出误差/TFLOPS。显式 `cudaMalloc` 对象在测试函数释放；新增测试需同步错误路径。

## CLI/架构风险

`build.sh` 固定 nvcc 路径并按 SM 开关 CuTe/WGMMA/TMA；编译一份不代表其他 GPU 可运行。动态 shared memory 通过 `check_smem_feasible` 检查可行性。[kernels/interview/notes-v2.cu:82-90]

## 相关文档

- [design.md](design.md)
- [source-map.md](source-map.md)
- [interfaces.md](interfaces.md)
- [call-chains.md](call-chains.md)
- [testing.md](testing.md)
- [development-guide.md](development-guide.md)
- [risks-and-debt.md](risks-and-debt.md)

## 源码证据摘要

- `[kernels/interview/notes-v2.cu:22-53]`。
- `[kernels/interview/notes-v2.cu:82-97]`。
- `[kernels/interview/build.sh:45-81,134-185]`。

## 未解决问题

完整 CLI 参数表和每个 phase 的 test/bench 选择关系需继续从文件后半段整理。
