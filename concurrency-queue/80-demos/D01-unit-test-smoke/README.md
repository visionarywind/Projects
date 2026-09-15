# D01 Unit Test Smoke

- 文档目的：把 unit test 作为真实端到端 Demo，从构建入口追到测试逻辑和资源检查。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：源码调用路径已确认；native 构建、全量 smoke 和 `test_threaded` 单项已验证。
- 最后更新：2026-09-10
- 前置阅读：[Demo 注册表](../demo-registry.md)
- 后续阅读：[执行轨迹](execution-trace.md)
## 结论摘要

本页聚焦 80-demos/D01-unit-test-smoke/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 为什么选择它

`tests/unittests/unittests.cpp` 同时包含核心队列、blocking queue、C API、对象异常和线程测试，比一个只 enqueue/dequeue 的 hello 示例能覆盖更多真实边界。它由 `build/makefile` 编译，并由 main 解析测试选择和迭代次数。

## 构建与运行

```sh
cd build && make bin/unittests
./build/bin/unittests --disable-prompt --iterations 1
./build/bin/unittests --disable-prompt --run c_api_try_dequeue --iterations 1
```

以上命令中，全量 smoke 和 `test_threaded` 单项已在当前工作区执行并通过；`c_api_try_dequeue` 单项命令尚未执行。

## 参与文件

- `build/makefile`：编译 C API `.cpp`、common helpers 和 `unittests.cpp`。
- `tests/unittests/unittests.cpp`：测试注册、测试类、tracking allocator、主程序。
- `concurrentqueue.h`：核心状态改变。
- `blockingconcurrentqueue.h`/`lightweightsemaphore.h`：blocking 测试落点。
- `c_api/*`：C ABI 测试落点。

## 审计卡片

| 维度 | 观察 |
|---|---|
| 输入 | CLI options、测试名、iterations |
| 控制 | registerTest → selected/all tests |
| 状态 | queue/block/token/semaphore 和 tracking allocator |
| 输出 | assertion、stdout/stderr、进程退出码 |
| 清理 | test destructor、`postTest` allocation check |
| 未验证 | CMake、fuzz/benchmark 运行和 C API 单项命令 |
| 已验证 | native unit 构建、全量 smoke、`test_threaded` 单项 |

## 子页

- [execution-trace](execution-trace.md)
- [audit-table](audit-table.md)

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D01-unit-test-smoke/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| concurrency-queue/80-demos/D01-unit-test-smoke/README.md | 已定位 | 静态入口已定位，运行未验证 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 主 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
