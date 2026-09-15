# 全局错误模型

- 文档目的：解释 00-overview/global-error-model.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/global-error-model.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


错误通常在三层传播：

1. API wrapper 先返回参数、句柄、context 和 capture 错误；输出参数在适用入口先清零。
2. CUI 将资源创建/登记/同步失败原样或按 API 契约映射为 `CUresult`，并执行局部回滚。
3. 异步 channel/设备错误可写入 context sticky error，后续 `cuiInitCheckCtx` 再观察（静态确认：[src/cui/cuiinit.c:2910-3039]）。

典型边界：memobj 全局登记失败释放对象；context TLS push 失败销毁新 context；stream 创建后 QMD pool 登记失败 detach；launch push 失败仍结束 stream push、回滚 UVM running 状态并通知 tools（静态确认：[src/api/apimem.c:100-111]；[src/api/apictx.c:73-84]；[src/api/apistream.c:83-109]；[src/cui/cuilaunch.c:779-817]）。

具体 sticky error 产生点分散于 DMAL/channel，外部 RM 错误映射仍不完整。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
