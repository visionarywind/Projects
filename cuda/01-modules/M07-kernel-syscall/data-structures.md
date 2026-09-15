# M07 数据结构

- 文档目的：解释 01-modules/M07-kernel-syscall/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M07-kernel-syscall/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 数据 | 作用 | 生命周期 |
|---|---|---|
| syscall imports bitset | 表示函数/模块需要的 device-side syscall | module/function 建立时形成，launch setup 读取 |
| `CUsyscall`/`scFunc` | syscall 对应的 function 与 memobj | context syscall library 持有；launch tracking 保活 |
| CNP launch queue 状态 | 支撑 device child launch | 父/子 launch 异步期间有效，具体实现依赖 `cuicnplaunchqueue.c` 和外部 kernel |
| `ctx->syslib.prt` reason buffer | 每 warp/SM 的 trap reason，host/device mapped | syscall init 创建，context destroy 释放 |
| syscall callback lock | 串行化 module/kernel/trap/debugger/resize 回调 | syscall library 初始化到 destroy |
| `activeSyscalls` + `refcount` | 跟踪模块引用和当前激活 syscall | module load/unload 改变 |
| `CUIsyscallResources` | 聚合 register/shared-memory/sync-stack 需求 | launch setup 临时值 |
| 生成 kernel/asm blob | 驱动辅助代码和架构机器码 | 构建产物或加载镜像，不等同手写 C 源 |

可确认的内存关系是 syscall function 的 `memobj` 在 launch memory tracking 中登记；具体引用计数和销毁函数需结合 `cuisyscall.c` 及各 syscall 子目录继续核对（静态确认：[src/cui/cuilaunch.c:164-218]）。

syscall library 还持有 syscall/trampoline/entry-exit/CG modules、reason buffer、per-context constant data 和 callback lock；这些对象按 context 创建，destroy 逆序释放，并在 destroy callback 可能产生 GPU work 后执行同步（静态确认：[src/cui/cuisyscall.c:378-568,610-734]）。`refcount` 以 module imports 为粒度，`activeSyscalls` 只在引用从 0 变为 1 或回到 0 时切换（静态确认：[src/cui/cuisyscall.c:1229-1273]）。

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
