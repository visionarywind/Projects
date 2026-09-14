# M06 源码地图、测试与开发

## 文件地图

| 文件 | 角色 |
|---|---|
| `src/api/apilaunch.c` | legacy/modern launch wrapper、capture 分支 |
| `src/cui/cuilaunch.c` | launchData、tracking、setup、参数打包 |
| `src/cui/cuifunc.c` | function 验证、参数 metadata、状态 |
| `src/cui/cuiparam.c` | 参数 size/packing 辅助 |
| `src/cui/cuimod.c` | module 生命周期和 function 注册 |
| `src/cui/cuielf*.c` | ELF/fatbinary 解析 |
| `src/cui/cuigraph.c`、`src/cui/cuigraph.h`、`src/cui/graph.c` | graph/capture 节点、实例资源和 teardown |
| `src/api/apigraph.c`、`src/api/apistream.c` | graph API、capture begin/end、exec update |
| `src/cui/cuistream.c` | capture state、origin/transitive streams、stream detach |
| `src/cui/cuijitlink.c` | PTX JIT（受 feature macro 控制） |

## 测试证据

`basic_sanity:kernel` 从 context module/function 链表中按 `extern "C"` 名称寻找 `gpuIncrement`，检查 `cnpLaunchable`、launchCount、push count 和最终结果（静态确认：[tests/cuda_test/basic_sanity.cu:133-163,169-234]）。测试未执行。

## 修改配方

1. 参数 ABI 修改：同步 function metadata 解析、validate、pack、legacy compatibility copy、HAL ABI encode。
2. 新 graph node：同步 capture validation、dependency、invalidate、destroy-on-update-failure。
3. 新 module 属性：检查 module/function ownership、context destroy 清理和 launch tracking。
4. 变更 launch setup：保留 syscall、tools/profiler、HAL check 和 QMD 绑定顺序。

## 未知

Graph 资源主线已补齐：per-context QMD/constant-bank/marker/internal stream、scheduler graph backing、launch tracking 和 destroy reverse path。仍未完整读取 ELF section/attribute 解析、JIT compiler 外部调用、各架构 QMD 字段和最终 push 方法；专用 graph 运行时测试尚未在 `tests.nvmk` 注册。
