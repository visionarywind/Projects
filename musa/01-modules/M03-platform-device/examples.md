# 平台与设备管理：例子

- 文档目的：platform device：用法示例与边界/错误输入说明。
- 适用范围：M03 模块的《平台与设备管理：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态最小例子：按序取设备

```text
1. muapiInit(flags)                      // Driver 初始化，内部触发 Platform::Get().Init()
2. muDeviceGetCount(&count)              // -> Platform::GetDeviceCount()，读 m_Devices.size()
3. muDeviceGet(ordinal, &dev)            // -> Platform::GetDevice()，首次触发 Device::LaterInit()
4. muDeviceGetAttribute(dev, attrib, &v) // -> Device::GetAttribute()，读 HAL 属性缓存
5. 退出前同步并释放资源；进程退出触发 ~Platform 清理
```

## 静态最小例子：按 PCI 查找设备

```text
1. muDeviceGetByPCIBusId("0000:31:00.0", &dev)
   -> Platform::GetDeviceByPCIBusId()：按字符串长度解析 domain:bus:device.function，再遍历设备比对 PCI 属性
   [src/musa/core/platform.cpp:152-195]
```

## 静态最小例子：创建上下文

```text
1. muCtxCreate(&ctx, flags, dev)
   -> Device::CreateContext()：统计 context 数选调度模式，new Context + Init，成功后登记并 Retain
   [src/musa/core/device.cpp:635-675]
```

## 对应源码方法

| 用户视角 | Core 实现 | 位置 |
|---|---|---|
| 初始化 | `Platform::Get().Init()` | [src/musa/core/platform.cpp:85-139] |
| 设备枚举/计数 | `Platform::GetDeviceCount` / `GetDevices` | [src/musa/core/platform.h:35] [src/musa/core/platform.h:60] |
| 按序取设备 | `Platform::GetDevice` | [src/musa/core/platform.cpp:141-150] |
| 属性查询 | `Device::GetAttribute` | [src/musa/core/device.cpp:42-448] |

这是源码阅读模型，不是已运行的示例。可用 D01 `muInfo` 验证查询类路径，用 D02/D04 验证异步/Graph 路径。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（3 处，例：[src/musa/core/platform.cpp:152-195]）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:635-675]）
- `src/musa/core/platform.h`（2 处，例：[src/musa/core/platform.h:35]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
