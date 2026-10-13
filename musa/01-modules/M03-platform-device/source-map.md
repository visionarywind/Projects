# 平台与设备管理：源码地图

- 文档目的：platform device：源码文件/目录到文档章节的映射。
- 适用范围：M03 模块的《平台与设备管理：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 层 | 路径 | 关注点 |
|---|---|---|
| 配置/入口 | `src/musa/core/platform.cpp、device.cpp` | 对外契约、选项、入口函数 |
| Core | `src/musa/core` | 对象、状态、生命周期 |
| Command/Graph | `src/musa/core/command`、`node`、`graph` | 异步工作表示 |
| HAL | `src/hal/hal*.h`、`src/hal/m3d` | 资源映射和硬件调用 |
| 观测 | `src/driver/mupti`、`mugdb`、`muasan` | trace/accessor/debug |

## 证据锚点

[src/musa/core/platform.cpp:84-140] [src/musa/core/device.cpp:856-926]

## 关键文件首读区间

| 文件 | 先看哪一段 | 那一段在做什么 |
|---|---|---|
| `src/musa/core/platform.cpp` | [src/musa/core/platform.cpp:33-83] `GetVisibleDeviceIndices` | 设备排序规则与可见性筛选（含越界/重复 index 的 `break`） |
| `src/musa/core/device.cpp` | [src/musa/core/device.cpp:42-45] `GetAttribute` | 属性查询 switch 的入口：取 HAL properties 后按 attrib 分派 |
| `src/musa/core/platform.h` | [src/musa/core/platform.h:141-171] `m_Devices` | Platform 私有成员：HAL 指针、初始化状态、设备集合与 pool 数组 |
| `src/musa_shared_include/musa.h` | [src/musa_shared_include/musa.h:769-925] `MU_DEVICE_ATTRIBUTE_MAX` | 公共设备属性枚举 `MUdevice_attribute` 的全集与上界 |
| `src/musa_shared_include/musa.h` | [src/musa_shared_include/musa.h:6263-6263] `muDeviceGetAttribute` | 公共查询接口签名：`int *pi` + `MUdevice_attribute` + `MUdevice` |

## 搜索建议

```bash
grep -R "platform" src/driver src/musa/core src/hal
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（2 处，例：[src/musa/core/platform.cpp:84-140] `Init`）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:856-926] `LaterInit`）
- `src/musa/core/platform.h`（1 处，例：[src/musa/core/platform.h:141-171] `m_Devices`）
- `src/musa_shared_include/musa.h`（2 处，例：[src/musa_shared_include/musa.h:769-925] `MU_DEVICE_ATTRIBUTE_MAX`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
