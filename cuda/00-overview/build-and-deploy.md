# 构建与部署

## 主构建

该快照使用 NVIDIA nvmake/nvmk，不是独立 CMake 项目。`cuda.nvmk` 组织 API、CUI、架构 HAL、工具和 kernel/syscall 源，并生成 Linux `libcuda.so.1.1` 等目标（静态确认：[cuda.nvmk:348-492,513-597,1695-1817]）。

## 测试构建

`tests/makefile.nvmk` 先构建 `libcuda_test`，再构建 `cuda_test`；`tests/cuda_test/cuda_test.nvmk` 为每个 case 建 binary，并生成 dispatcher 列表（静态确认：[tests/makefile.nvmk:3-18]；[tests/cuda_test/cuda_test.nvmk:60-153]）。Linux DVS 包装入口见 `[tests/dvs-build-unix.sh:1-43]`。

## 部署边界

需要外部 driver/common、RM/NVRM、compiler、工具链、目标 OS 和 GPU。当前没有运行 nvmake、nvcc、DVS 或安装部署，因此所有命令仅为源码记录，结果未验证。
