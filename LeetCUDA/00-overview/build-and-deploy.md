# 构建、运行、测试与部署总览

- 文档目的：汇总仓库内真实出现的构建、运行、测试、质量检查和清理入口。
- 适用范围：CUDA/PyTorch GPU 环境；命令均需按模块选择。
- 对应源码版本：`0983c65`。
- 证据状态：命令来自仓库脚本/README；本环境未执行 GPU 命令。
- 最后更新：2026-09-10
- 前置阅读：[dependency-map.md](dependency-map.md)
- 后续阅读：[../99-roadmap/quick-start.md](../99-roadmap/quick-start.md)

## 结论摘要

全仓库没有统一 CMake/CI 构建入口。普通模块直接运行 Python 脚本并由 `torch.utils.cpp_extension.load` 编译；HGEMM/FlashAttention 有 `setup.py`/makefile；interview 使用 `build.sh` 选择 SM 架构。所有命令都依赖 NVIDIA GPU、CUDA Toolkit 和适配的 PyTorch，构建和测试必须标记具体模块与硬件。

## 环境要求

- Python：HGEMM `setup.py` 明确 `>=3.10`；其他脚本未统一声明。
- PyTorch：普通扩展和 tensor 输入必需；FlashAttention README 建议 PyTorch >= 2.0、CUDA >= 12.0。[kernels/flash-attn/README.md:106-112]
- CUDA：`nvcc`；interview build script 固定 `/usr/local/cuda/bin/nvcc`。
- GPU：SM 80/86/89/90a/120a 等能力依模块编译 flags；不要在不支持的 GPU 上照搬示例。
- 可选库：torchvision（NMS）、flash-attn（FA 对比）、CUTLASS/cudnn-frontend（CuTe/interview）、cuBLAS/cuDNN。

## 命令矩阵

| 目的 | 命令 | 验证状态 |
|---|---|---|
| 初始化子模块 | `git submodule update --init --recursive --force` | 未在本机执行；来自 README/CONTRIBUTE |
| 普通 elementwise | `cd kernels/elementwise && TORCH_CUDA_ARCH_LIST=Ada python3 elementwise.py` | 未验证；来自 README |
| NMS correctness/benchmark | `cd kernels/nms && TORCH_CUDA_ARCH_LIST=Ada python3 nms.py` | 未验证；来自 `nms/README.md` |
| FlashAttention | `cd kernels/flash-attn && python3 -m pip install flash-attn --no-build-isolation && TORCH_CUDA_ARCH_LIST=Ada python3 flash_attn_mma.py --D 64` | 未验证；安装会改变环境，执行前需明确授权 |
| HGEMM wheel | `cd kernels/hgemm && python3 setup.py bdist_wheel` | 未验证 |
| HGEMM Python | `cd kernels/hgemm && TORCH_CUDA_ARCH_LIST=Ada python3 hgemm.py --wmma` | 未验证 |
| HGEMM C++ | `cd kernels/hgemm && make && ./hgemm_mma_stage.bin` | 未验证 |
| Interview build | `cd kernels/interview && ./build.sh --arch sm_89` | 未验证 |
| Interview clean | `cd kernels/interview && ./build.sh --clean` | 未验证；会删除本目录构建产物 |
| 代码质量 | `pre-commit run --all-files` | 未验证 |

## Debug/Release 与清理

仓库没有统一 Debug/Release profile。可见的 debug 机制是：

- interview `--debug`/`--verbose` 影响输出，并由 build defines 控制专题代码；
- HGEMM makefile 提供 `*_debug` target，加 `-DHGEMM_*_DEBUG`；
- Python extension 通常固定 `-O3 --use_fast_math`；
- `build.sh` 使用 `-O3 -std=c++20`，可选 ccache，`--clean` 删除 `.o/.bin`。

不要把 `--use_fast_math` 下的误差结果与严格 IEEE 结果混为一谈。

## 运行与部署边界

项目输出是学习用 `.so`、wheel 或本地二进制，不包含服务部署清单、容器镜像、RPC/API server 或发布流水线。所谓“部署”在当前仓库内主要是安装 wheel 或准备运行机器；需要外部系统时标为不适用/未知。

## 常见失败排查

1. `nvcc not found`：检查固定路径和 `CUDA_HOME`。
2. header not found：确认子模块已 checkout，检查 include path。
3. no kernel image：目标 SM 不在编译 flags 中，重新选择 `TORCH_CUDA_ARCH_LIST`/`--arch`。
4. extension build cache/ABI 错误：清理对应 torch extension cache，确认 PyTorch/CUDA ABI。
5. benchmark 结果异常：先确认 warmup、`torch.cuda.synchronize()`、输入布局和误差阈值。
6. NMS 对拍失败：先检查排序稳定性、索引映射和坐标 IoU 语义（`+1` 与连续坐标不同）。

## 相关文档

- [runtime-model.md](runtime-model.md)
- [global-error-model.md](global-error-model.md)
- [../99-roadmap/quick-start.md](../99-roadmap/quick-start.md)

## 源码证据摘要

- `[README.md:53-105]`：Interview 命令。
- `[kernels/interview/build.sh:18-43,118-185]`：缓存、架构、清理、编译。
- `[kernels/hgemm/README.md:103-141]`：wheel/Python 测试。
- `[.pre-commit-config.yaml:1-41]`：质量检查。

## 未解决问题

- 没有 CI GPU job 验证各模块。
- 没有统一的容器/锁定依赖文件。
- 不能在无 GPU 环境确认编译和运行。

## 下一步阅读建议

使用 [quick-start.md](../99-roadmap/quick-start.md) 选择一个最小模块，不要一次构建全仓库。
