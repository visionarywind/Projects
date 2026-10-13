# Context 与资源生命周期：测试策略

- 文档目的：context lifecycle：测试入口、缺口与建议用例（含未验证标注）。
- 适用范围：M04 模块的《Context 与资源生命周期：测试策略》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态检查

- 核对资源创建和销毁是否成对：`CreateStream`/`DestroyStream`、`CreateEvent`/`DestroyEvent`、`CreateMemory`/`DestroyMemory`、`CreateModule`/`DestroyModule`、`CreateGraph`/`DestroyGraph`、`CreateGraphExec`/`DestroyGraphExec`。[src/musa/core/context.cpp:1198-1228] [src/musa/core/context.cpp:1321-1347] [src/musa/core/context.cpp:1080-1088] [src/musa/core/context.cpp:1288-1313] [src/musa/core/context.cpp:1618-1659] [src/musa/core/context.cpp:1690-1738]
- 核对失败路径是否阻止半初始化对象进入登记表：`CreateStream` 在 `Init` 失败时 `delete stream` 后不 `AddStream`。[src/musa/core/context.cpp:1203-1215]
- 核对上下文级清理覆盖所有集合：`ReleaseClientResources` 遍历 texture/surface/graphics resource/mipmapped array/event/stream/memory/external。[src/musa/core/context.cpp:293-377]
- 核对 primary 与用户 context 的销毁顺序（用户 context 先于 primary、copy manager 在 primary 之前释放）。[src/musa/core/device.cpp:718-748]
- 运行 `git diff --check` 只验证本文档仓库的格式，不验证远端项目。

## 运行验证（未执行）

```bash
cmake -S . -B build -DMUSA_BUILD_UT=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

目标环境需要私有子模块、工具链、驱动和 GPU；本知识库没有声称上述命令成功。建议专项用例：

1. `muCtxCreate_v2` -> 创建 stream/event/memory/graph -> `muCtxDestroy_v2` 全路径；
2. primary context `Retain`/`Release`/`Reset` 与 green context 同步；
3. 默认流/barrier 流依赖：并发多流后 `Context::Synchronize` 顺序完成；
4. context 析构时漏销毁资源的 WARNING 输出与无泄漏（配合 MemoryTracker）。

模块专项用例见 `80-demos` 和远端 `tests` 文件。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（8 处，例：[src/musa/core/context.cpp:1198-1228]）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:718-748]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
