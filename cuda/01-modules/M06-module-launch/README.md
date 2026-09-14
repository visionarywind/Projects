# M06 Module/Launch：模块、函数、参数与图

- **职责**：加载 CUDA module/fatbinary/ELF/JIT 产物，建立 `CUmod`/`CUfunc`，解析 kernel 参数元数据，并把 launch 请求转换成 `CUIlaunchData`、constant-bank 参数和 QMD。
- **证据状态**：API launch、参数、tracking、HAL setup、module ELF/JIT、shared image、graph instantiate/launch/unload 已静态确认；架构 QMD/ABI、compiler 和最终 push/设备执行未闭合。
- **核心文件**：`cuimod.c`、`cuifunc.c`、`cuiparam.c`、`cuilaunch.c`、`cuigraph.c`、`cuielf.c`、`cuielf32.c`、`cuielf64.c`、`cuijitlink.c`。

## 两种执行模式

```text
普通 stream:
API → CUIlaunchData → parameter pack → profiler/tools → setup → HAL/QMD → submit

capture stream:
API → validate kernel params → graph node + dependencies → later graph launch
```

普通和 capture 共享 API 的 function/stream/context 检查，但副作用不同：capture 创建 graph node，普通路径推进异步提交（静态确认：[src/api/apilaunch.c:235-301]）。

## 参数与函数状态

旧编译器缺少 `EIATTR_KPARAM_INFO` 时，带 `kernelParams` 的现代 launch 返回 `CUDA_ERROR_INVALID_IMAGE`；参数验证产生 packed/unpacked 状态，launch 后更新 function 的 block shape、parameter size 和 dynamic shared size（静态确认：[src/api/apilaunch.c:141-207]）。

## 相关模块

M04 提供 launch memory tracking 需要的对象；M05 提供 stream/channel/QMD/marker；M03 提供架构 HAL；M07 提供 device syscall 和生成 kernel。
