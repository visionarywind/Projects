# Context 与资源生命周期：开发指南

- 文档目的：context lifecycle：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M04 模块的《Context 与资源生命周期：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码提交并更新证据行号（`rg -n`/`sed -n` 核对 `context.cpp`、`device.cpp`）。
2. 判断改动属于 ABI（M02）、Core 生命周期、HAL 适配（M09）还是观测层（M10）。
3. 画出调用链和资源所有权：新增资源必须同时补 `Add*` 登记、`Find*` 查找、`Remove*` 摘除和 `ReleaseClientResources` 清理四件套。

## 修改中

- 保持现有 `CriticalBase` 集合 + `ReadLockedAccessor`/`WriteLockedAccessor` 风格；读路径用 `ReadLockedAccessor`，改集合用 `WriteLockedAccessor`。[src/musa/core/internal_types.h:213-264]
- 资源注册/销毁必须成对：`CreateStream` 与 `DestroyStream`、`CreateEvent` 与 `DestroyEvent`、`CreateGraph` 与 `DestroyGraph` 都遵循“写锁登记/摘除 + delete”。[src/musa/core/context.cpp:1198-1228] [src/musa/core/context.cpp:1321-1347] [src/musa/core/context.cpp:1618-1659]
- 默认流与 barrier 流的生命周期由 `Context` 独占：不要在 `ReleaseClientResources` 之外手工删除，否则析构会二次释放。[src/musa/core/context.cpp:293-354]
- HAL 错误通过既有 `HalToMuResult` 转换边界返回，不要在本模块引入新的错误码映射。[src/musa/core/internal_funcs.h:22-40]
- 异步命令的持有者与完成条件必须明确；`ResolveDependencyAndQueueCommand` 用 `LastCommand` 记录依赖，改动同步语义前先确认默认流/barrier 流依赖规则。[src/musa/core/context.cpp:2032-2062]

## 修改后

1. 先做静态 diff/格式检查。
2. 跑本目录校验脚本，确认引用行号与符号全部命中：
   `cd /var/folders/xy/9w9pq8216b19ttfmlr7md1j00000gn/T/opencode && python3 verify_kb.py && python3 verify_content.py`
3. 再在授权目标机做最小构建和专项测试；安装脚本只在明确授权时执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（5 处，例：[src/musa/core/context.cpp:1198-1228]）
- `src/musa/core/internal_types.h`（1 处，例：[src/musa/core/internal_types.h:213-264]）
- `src/musa/core/internal_funcs.h`（1 处，例：[src/musa/core/internal_funcs.h:22-40]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
