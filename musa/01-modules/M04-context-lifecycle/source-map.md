# Context 与资源生命周期：源码地图

- 文档目的：context lifecycle：源码文件/目录到文档章节的映射。
- 适用范围：M04 模块的《Context 与资源生命周期：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


| 层 | 路径 | 关注点 |
|---|---|---|
| 配置/入口 | `src/musa/core/context.cpp、device.cpp` | 对外契约、选项、入口函数 |
| Core | `src/musa/core` | 对象、状态、生命周期 |
| Command/Graph | `src/musa/core/command`、`node`、`graph` | 异步工作表示 |
| HAL | `src/hal/hal*.h`、`src/hal/m3d` | 资源映射和硬件调用 |
| 观测 | `src/driver/mupti`、`mugdb`、`muasan` | trace/accessor/debug |

## 证据锚点

[src/musa/core/context.cpp:41-475] [src/musa/core/context.cpp:1875-2040]

## 分层证据锚点

每个“层 → 关注点”都落到可复核的行级区间；行号来自本地快照 `source/musa`，随源码演进会漂移。

| 层 | 关注点 | 行级锚点 |
|---|---|---|
| Driver 入口 | 参数/句柄校验与 TLS context 栈 push/pop | [src/driver/mu_context.cpp:156-182] `muapiCtxCreate_v2`、[src/driver/mu_context.cpp:188-218] `muapiCtxDestroy_v2` |
| Core Context | 初始化顺序与销毁时的资源释放顺序 | [src/musa/core/context.cpp:1906-1965] `Context::Init`、[src/musa/core/context.cpp:1967-2005] `Context::Dispose` |
| Core 资源登记 | stream 容器的写锁增删与析构期强制清理 | [src/musa/core/context.cpp:93-126] `AddStream`、[src/musa/core/context.cpp:293-377] `ReleaseClientResources` |
| Device | context 集合登记/摘除与 primary context | [src/musa/core/device.cpp:635-675] `Device::CreateContext`、[src/musa/core/device.cpp:677-688] `Device::DestroyContext` |
| Stream | 析构期默认流/barrier 流同步与工作线程停止 | [src/musa/core/stream.cpp:850-877] `Stream::~Stream` |
| HAL 下游 | 队列提交边界（构建/接口已确认，硬件副作用未知） | [src/hal/m3d/queue.cpp:182-372] `Queue::Submit` |

## 搜索建议

```bash
grep -R "context" src/driver src/musa/core src/hal
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（6 处，例：[src/musa/core/context.cpp:1906-1965] `Context::Init`）
- `src/driver/mu_context.cpp`（2 处，例：[src/driver/mu_context.cpp:156-182] `muapiCtxCreate_v2`）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:635-675] `Device::CreateContext`）
- `src/musa/core/stream.cpp`（1 处，例：[src/musa/core/stream.cpp:850-877] `Stream::~Stream`）
- `src/hal/m3d/queue.cpp`（1 处，例：[src/hal/m3d/queue.cpp:182-372] `Queue::Submit`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
