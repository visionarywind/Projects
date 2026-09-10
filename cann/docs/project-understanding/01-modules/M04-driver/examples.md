# M04 Driver 示例与构建

- 证据状态：入口来自仓库文件；本环境未执行

## 构建入口

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

Driver 顶层 CMake 在 `ENABLE_TEST` 时进入 `test/`，正式构建进入 `src/` 并支持打包 `[driver/CMakeLists.txt:10-57]`。具体编译选项和依赖应以仓库文档及目标平台为准。

## 调试 HDC client busy

1. 记录 client magic、服务类型和目标设备。
2. 枚举/记录仍活动的 session。
3. 先关闭 session，再重试 destroy。
4. 若仍失败，检查 PCIe/HDC/设备日志和 Driver 错误码。

## 调试 queue 打开失败

检查设备节点权限、PCI 设备 ID 表、进程 TGID context、HDC kernel connection 和内核日志；不要把无设备环境的 open 失败归因于代码缺陷。
