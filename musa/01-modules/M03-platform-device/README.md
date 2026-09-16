# 平台与设备管理（M03）

- 源码范围：`src/musa/core/platform.cpp、device.cpp`
- 对应版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证

## 一句话职责

平台与设备管理负责 `Init -> HAL platform -> visible devices -> Device/LaterInit`。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)。
3. 涉及异步、硬件或安装时，结合 [`risks-and-debt.md`](risks-and-debt.md)。

## 边界

- **本模块拥有**：本目录列出的对象、状态或构建规则。
- **本模块不拥有**：上游 API 的公共 ABI 契约和下游 GPU/内核驱动的最终实现。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

[src/musa/core/platform.cpp:84-140] [src/musa/core/device.cpp:856-926]

## 深度状态

正常路径已建立；分支、失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。
