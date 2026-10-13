# 工具与调试接口：开发指南

- 文档目的：tools debug：常见修改场景的落地入口、注意点与验证方式。
- 适用范围：M10 模块的《工具与调试接口：开发指南》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：部分推断（引用较少，证据密度不足）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 修改前

1. 锁定源码提交并更新证据行号；本快照为 5.3.0 develop，无 Git 元数据。
2. 判断改动属于哪一层：`muInfo` 工具输出、`Client`/导出表布局（M13 头文件）、驱动侧 hook 存储、tracepoint 内联、还是 GDB 客户端 `musadebugger`。
3. 若改动导出表，先读 [`interfaces.md`](interfaces.md) 的两种分发模式与 `interfaces.md` 的 `AccessorHint` 契约，确认所有权语义。
4. 画出调用链和资源所有权再改代码；重点核对静态表指针不会被消费方释放。

## 修改中

- **工具侧**：保持 `checkMuErrors` 风格与现有输出格式 [src/tools/muInfo.cpp:9-20]；新增查询字段前确认对应 `muDeviceGetAttribute` 枚举已在 `GetDeviceProps` 中逐项采集 [src/tools/muInfo.cpp:119-274]。
- **hook 侧**：新增 tracepoint 时同时满足三处——`export_table.h` 的 hook 结构、驱动侧静态存储字段、core 侧内联入口；每个入口都要先查 `ready` 再查 `AccessorHint` [src/driver/mupti/tracepoints.h:13-21]。
- **GDB 侧**：FIFO 协议改动需驱动侧 `tracepoints.cpp` 与客户端 `interface.cpp` 同步；`MSG_ID_*` 枚举定义在公共头文件，勿单侧改动。
- 不改变同步语义：`muInfo` 与 tracepoint 回调都是调用线程内同步，新增异步路径需单独论证。
- 不在没有硬件证据时修改 trap 行为（`Monitor`/`raise(SIGINT)` 路径）。

## 修改后

1. 静态检查：核对新符号出现在导出表/accessor 表；核对失败路径不暴露半初始化对象。
2. 本地格式：`git diff --check`（只验证本文档仓库格式，不验证远端项目）。
3. 目标机验证：构建 `muInfo` 与调试器库需在授权目标机执行；GDB/MUPTI/MUASAN 集成测试需要对应前端工具与 GPU。
4. 更新本目录文档的证据行号，并把行为结论与推断区分标注。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/tools/muInfo.cpp`（2 处，例：[src/tools/muInfo.cpp:9-20]）
- `src/driver/mupti/tracepoints.h`（1 处，例：[src/driver/mupti/tracepoints.h:13-21]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
