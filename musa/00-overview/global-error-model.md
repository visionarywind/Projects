# 全局错误模型

- 文档目的：错误码的定义、转换与跨层传播模型。
- 适用范围：00-overview《全局错误模型》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`project-overview.md`](project-overview.md)、[`../README.md`](../README.md)
- 后续阅读：[`architecture.md`](architecture.md)、[`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)


## 分层

1. Driver API 检查空指针、ordinal、当前 context、句柄类型并返回 `MUresult`。[src/driver/mu_device.cpp:14-45] [src/driver/mu_memory.cpp:271-320]
2. Core 对象方法返回 `MUresult`，失败时通常不把对象加入 Context/Platform tracker。[src/musa/core/context.cpp:1037-1089]
3. HAL result 通过 `HalToMuResult` 转换；该转换在 memory、queue、semaphore 初始化处普遍出现。[src/musa/core/memory.cpp:510-513] [src/musa/core/stream.cpp:950-1005]
4. 异步错误通过 stream/queue 查询和最后错误传播，不能仅依据 enqueue 的返回值判断执行完成。[src/musa/core/stream.cpp:1113-1146]

## 典型失败路径

```text
非法参数/无当前 context -> API 立即返回
HAL Create/Allocate 失败 -> Core 初始化失败 -> 不登记或回滚
enqueue 成功、GPU 执行失败 -> Query/Synchronize 时暴露错误
销毁前仍有命令 -> Synchronize/Destroy 可能失败或等待
```

## 排障原则

记录 API 返回值、当前 device/context、stream、correlation/submission id；对异步问题同时检查 `Query`、`Synchronize` 和 engine robust info。不要把未在目标环境执行的命令输出写成事实。

## 相关文档

- [`project-overview.md`](project-overview.md)
- [`architecture.md`](architecture.md)
- [`../README.md`](../README.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（2 处，例：[src/musa/core/stream.cpp:950-1005]）
- `src/driver/mu_device.cpp`（1 处，例：[src/driver/mu_device.cpp:14-45]）
- `src/driver/mu_memory.cpp`（1 处，例：[src/driver/mu_memory.cpp:271-320]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`architecture.md`](architecture.md)；阅读 [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)。
