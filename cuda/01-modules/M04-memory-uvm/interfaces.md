# M04 接口契约

- 文档目的：解释 01-modules/M04-memory-uvm/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-memory-uvm/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `memobjAlloc`：根据 descriptor 创建对象并选择/创建 backing；调用者需处理失败回滚。
- `memglobalsRegisterMemobj`：使 object 可通过 unified/device pointer 查找；失败时对象必须释放。
- `memobjGetByUnifiedPtr`/`memobjGetByDevicePtr`：查找不等于取得永久所有权。
- `memobjFreeNoSync`：只在调用者已完成同步/依赖处理后使用。
- `suballocatorSuballocate`：在兼容 descriptor tree 中 best-fit；未找到区域时返回 success 且 `memobj->memblock == NULL`，由调用者创建新 block。
- `suballocatorSuballocateFromNewMemblock`：绑定请求区并把剩余区插入 tree；整 block 用尽时转为 `noSuballoc`。
- `suballocatorFreeMemobj`：要求 memmgr lock 和可 suballocate block；把 memobj 存储复用为 free node 并合并相邻区域。
- `memblockAlloc`/`memblockFree`：suballocator 与 DMAL/UVA 的所有权边界；只有 block 创建/最终销毁才触发物理/平台资源操作。

descriptor compatibility 比较 location、cache、mapping、owner、API source、page size、fixed address、compression、handle types 等字段。

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
