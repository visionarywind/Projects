# M01 数据结构与行级分析

- 文档目的：解释 01-modules/M01-api-abi/data-structures.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 数据结构

- `CUdeviceptr_v1/v2`：公开 ABI 宽度差异；内部统一使用 `CUdeviceptr_v2`（静态确认：[src/cuda_types.h:29-34]）。
- `CUctx`、`CUIstream`、`CUfunc`：wrapper 通过 TLS/句柄转换获得，不能把公开 handle 直接当作任意对象指针使用。
- `CUmemdesc`：内存分配的属性载体；`type/owner/apiSource/location/mapDevice` 决定后续 memobj 行为（静态确认：[src/api/apimem.c:90-98]）。
- `CUIlaunchData`：block/grid/shared memory 与参数描述的 HAL 无关载体（静态确认：[src/api/apilaunch.c:135-179]）。

## 行级关键点

1. `[src/api/apimem.c:63-65]` 先清零输出指针，保证错误返回不暴露未初始化地址。
2. `[src/api/apimem.c:73-87]` 先做全局/context/capture 检查，再检查空指针和零大小。
3. `[src/api/apilaunch.c:239-247]` 强制 function 和 stream 来自同一 context，避免跨 context 资源提交。
4. `[src/api/apilaunch.c:252-286]` capture 分支在 graph 锁下创建节点；失败会 invalidate capture。
5. `[src/api/apistream.c:94-109]` context lock 释放后再做 QMD pool 登记，失败转入 stream detach。

## 并发边界

API wrapper 的 context lock 保护 context-owned 状态；stream pool 和 graph capture 有独立锁。具体锁顺序不能从函数名推断，应沿调用点验证（静态确认：[src/api/apistream.c:83-99]；更完整规则未知）。

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
