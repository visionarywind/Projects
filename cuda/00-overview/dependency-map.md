# 依赖地图

```text
公开 CUDA/OpenCL ABI
 → src/api 或 ICD dispatch
 → CUI globals/TLS/context
 → device + memobj + stream/channel
 → HAL/DMAL
 → 外部 common/RM/NVRM/compiler/firmware
```

## 依赖分类

- **编译依赖**：nvmake/nvmk、common build rules、nvcc、Boost、testutils、外部 compiler/gpgpucomp。
- **源码依赖**：M01 wrapper 依赖 M02；M02 建立 M03/M04/M05/M06 所需全局对象；M06 依赖 M05 QMD/stream 和 M07 syscall。
- **运行依赖**：RM/NVRM、firmware、GPU、OS driver model；OpenCL 还依赖 vendor library 和 platform dispatch。
- **验证依赖**：DVS runner、`nvidia-smi`、persistence service、权限和多 GPU 条件。

上述方向由源码清单、入口调用和模块文档归纳；外部版本和具体链接图未知。
