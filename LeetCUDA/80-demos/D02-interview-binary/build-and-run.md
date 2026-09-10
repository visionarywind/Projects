# D02-S01 构建与运行

## 构建命令（未验证）

```bash
cd kernels/interview
./build.sh --arch sm_89
```

其他源码支持的选择包括 `sm_86`、`sm_90a`、`sm_120a` 和 `all`；`--clean` 清理输出。[kernels/interview/build.sh:45-126]。脚本使用 `/usr/local/cuda/bin/nvcc`、C++20、CUTLASS/cudnn-frontend include，并按架构设置宏/链接参数。[kernels/interview/build.sh:13-81,134-185]。

## 运行命令（未验证）

实际可用 flag 应先执行：

```bash
./notes_v2_sm89.bin --help
```

README/根 README 中的 benchmark 示例为：

```bash
./notes_v2_sm120a.bin --bench --mnk 4096,4096,4096 --bhnd 1,32,16384,128
```

该命令要求对应 binary 已构建、GPU 架构匹配、库和输入满足约束；当前未执行。

## 构建产物

- `notes-v2.cu` 先编译为 `notes-v2.o`；
- 再链接为架构后缀 binary；
- `--arch all` 依次构建支持的架构；
- 不同 binary 的 gencode/宏决定可用 WGMMA/TMA/CuTe 路径，不能交叉替代。

证据：[kernels/interview/build.sh:134-185]。

## 环境记录模板

运行前保存：

```text
GPU:
Driver:
CUDA toolkit:
nvcc:
CUTLASS submodule commit:
cudnn-frontend submodule commit:
arch binary:
```

## 当前阻断

本知识库没有安装依赖、更新子模块、调用 nvcc 或启动 binary；因此没有真实编译/运行结果。
