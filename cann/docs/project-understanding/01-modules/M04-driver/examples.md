# M04 Driver 示例与构建

- 证据状态：入口来自仓库文件；本环境未执行

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
