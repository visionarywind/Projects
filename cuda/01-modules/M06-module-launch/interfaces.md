# M06 接口契约

- 文档目的：解释 01-modules/M06-module-launch/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `cuiFuncValidateParams`：依赖 function metadata，输出合法 packed size/参数状态。
- `cuiLaunchSetup_common`：输入 context/function/stream/QMD/const banks，配置 syscall、HAL 和 ABI staging。
- `cuiLaunch`：要求 context lock；要求 begin push 与 end push 配对。
- `cuiGraphCreateKernelNode`：capture 期间取得已验证参数与依赖；更新失败负责 node 回滚。
- `cuiGraphInstantiate`：成功后 graph exec 持有 per-context locks、QMD、constant-bank、internal streams/markers 和可选 scheduler backing；任一步失败必须销毁全部已分配资源。
- `cuiGraphLaunch`：要求 executable graph；通过 marker/UVM DAG/stream tracking 延长资源生命周期，失败必须恢复临时 graph state。
- `cuiGraphExecDestroy`：允许先前 launch 尚未完成，但资源回收必须经 marker、stream detach 和 `memobjFree` 的等待边界。
- `cuGraphExecUpdate`：只允许不改变拓扑和资源形状的兼容参数更新；function/context/memory type 等变化会被拒绝。
- `cuiLaunchFinalizeParams`：仅在未 packed 且有 packed size 时复制参数。

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
