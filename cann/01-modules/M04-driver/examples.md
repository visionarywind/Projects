# M04 Driver 示例与构建

- 文档目的：解释 01-modules/M04-driver/examples.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cann HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：入口来自仓库文件；本环境未执行
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 构建入口

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

Driver 顶层 CMake 在 `ENABLE_TEST` 时进入 `test/`，正式构建进入 `src/` 并支持打包 `[driver/CMakeLists.txt:10-57]`。具体编译选项和依赖应以仓库文档及目标平台为准。

## 普通 rtMalloc 的池化示例

```text
第一次 rtMalloc(3 MiB)
  -> Runtime/halMemAlloc
  -> V3 cache 条件满足
  -> cache miss，normal malloc 得到 4 MiB range（示意）
  -> ga_add_range
  -> 分配 3 MiB，留下 1 MiB free area

rtFree
  -> free area 回到 size/address tree
  -> backing 暂时保留

第二次 rtMalloc(3 MiB)
  -> size_area_tree exact/upper-bound 命中
  -> 切分 free area
  -> 不再次申请 backing
```

V2 的等价过程是从 mapped cache tree 或 idle size tree 取 node；如果是 unmapped node，还要建立 mapping。以上地址和 range 大小仅为说明数据结构的示意，不是运行时统计。


1. 记录 client magic、服务类型和目标设备。
2. 枚举/记录仍活动的 session。
3. 先关闭 session，再重试 destroy。
4. 若仍失败，检查 PCIe/HDC/设备日志和 Driver 错误码。

## 调试 queue 打开失败

检查设备节点权限、PCI 设备 ID 表、进程 TGID context、HDC kernel connection 和内核日志；不要把无设备环境的 open 失败归因于代码缺陷。

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
