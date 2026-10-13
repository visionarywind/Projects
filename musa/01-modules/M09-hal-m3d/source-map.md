# HAL/M3D 适配：源码地图

- 文档目的：hal m3d：源码文件/目录到文档章节的映射。
- 适用范围：M09 模块的《HAL/M3D 适配：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 分层与路径

| 层 | 路径 | 关注点 | 锚点 |
|---|---|---|---|
| HAL 接口头 | `src/hal/*.h` | 纯虚接口、结构体、枚举；本目录内没有实现文件 | [src/hal/halMemMgr.h:15-40] |
| HAL 工厂入口 | `src/hal/m3d/lib.cpp` | `Hal::CreatePlatform` / `DestroyPlatform`，唯一对外的创建/销毁函数 | [src/hal/m3d/lib.cpp:6-17] |
| 对象销毁基类 | `src/hal/m3d/base.cpp` | `IBase::Destroy` 实现为 `delete this` | [src/hal/m3d/base.cpp:5-7] |
| 适配实现 | `src/hal/m3d/*.cpp` | Platform / Device / Queue / CmdBuffer / Memory / MemMgr / MemoryPool / Kernel | [src/hal/m3d/memMgr.cpp:81-147] |
| 结果转换 | `src/hal/m3d/common.h` | `ResultCast`、`ExceptionToHalResult` 等 HAL 结果归一 | [src/hal/m3d/common.h:44-173] |
| 资源包装 | `src/hal/m3d/ownPtr.h` | `OwnPtr` 用 `Reserve`/`Release` 管理 IM3d 对象内存与 `Destroy` | [src/hal/m3d/ownPtr.h:34-47] |
| 私有子模块 | `src/hal/m3d/m3d` | `m3d`、`scpc`、`util` 三个静态库的来源；本知识库只标注边界 | [src/hal/m3d/CMakeLists.txt:36-40] |
| 构建 | `src/hal/m3d/CMakeLists.txt` | `halM3d` 目标、宏定义、包含目录 | [src/hal/m3d/CMakeLists.txt:56-59] |
| 上游消费方 | `src/musa/core` | Core 对象持有 HAL 句柄并调用接口族 | [src/musa/core/CMakeLists.txt:11] |

`src/hal/CMakeLists.txt` 只有一行 `add_subdirectory(m3d)`，`src/CMakeLists.txt` 按 `util`、`hal`、`musa`、`driver` 顺序递归 [src/CMakeLists.txt:7-10]。

## 关键文件清单

| 文件 | 要点 | 读它的原因 |
|---|---|---|
| `src/hal/halDevice.h` | `IDevice` 全部创建与查询方法 | [src/hal/halDevice.h:279-372] |
| `src/hal/halQueue.h` | `QueueFamily`、`QueueSubmitInfo`、`IQueue` | [src/hal/halQueue.h:270-336] |
| `src/hal/halMemory.h` | `MemoryAllocInfo`、`IMemory` 的 Map/Unmap 契约 | [src/hal/halMemory.h:282-407] |
| `src/hal/halMemoryPool.h` | `MemoryPoolInfo` 五字段与 `IMemoryPool` | [src/hal/halMemoryPool.h:62-103] |
| `src/hal/m3d/memMgr.h` | 池注册表 typedef、位移常量与 `MakeKey` 打包规则 | [src/hal/m3d/memMgr.h:36-104] |
| `src/hal/m3d/memoryPool.h` | `ResSegment`、free bucket、segment tracker | [src/hal/m3d/memoryPool.h:43-103] |
| `src/hal/m3d/device.h` | `Device` 的 override 列表、成员与 `s_MtgpuMaxGpuMmuBits` | [src/hal/m3d/device.h:22-125] |

## 证据锚点

- 构建目标与子模块边界：[src/hal/m3d/CMakeLists.txt:22-23]
- 平台创建入口 `Hal::CreatePlatform`：[src/hal/m3d/lib.cpp:6-13]
- 分配入口 `MemMgr::Allocate` 的规模校验：[src/hal/m3d/memMgr.cpp:81-96]
- 队列提交转发 `Queue::Submit`：[src/hal/m3d/queue.cpp:182-240]
- Core 到 HAL 的链接关系：[src/musa/core/CMakeLists.txt:11]

## 搜索建议

```bash
rg -n "GetMemMgr|IQueue|IMemoryPool" src/musa/core src/hal --glob "!src/hal/m3d/m3d/**"
rg -n "CreatePlatform|M3dToHalResult" src/hal
rg -n "queueCapability|QueueFamily" src/hal/m3d
```

上述命令是阅读建议，不代表已在本地或远端执行；`src/hal/m3d/m3d` 是私有子模块目录，检索时应显式排除。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/CMakeLists.txt`（3 处，例：[src/hal/m3d/CMakeLists.txt:36-40]）
- `src/hal/m3d/lib.cpp`（2 处，例：[src/hal/m3d/lib.cpp:6-17]）
- `src/hal/m3d/memMgr.cpp`（2 处，例：[src/hal/m3d/memMgr.cpp:81-147]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
