# M06 源码地图、测试与开发

- 文档目的：解释 01-modules/M06-module-launch/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
