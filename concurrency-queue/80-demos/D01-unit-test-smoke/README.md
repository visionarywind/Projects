# D01 Unit Test Smoke

- 文档目的：把 unit test 作为真实端到端 Demo，从构建入口追到测试逻辑和资源检查。
- 证据状态：源码调用路径已确认；native 构建、全量 smoke 和 `test_threaded` 单项已验证。
- 最后更新：2026-09-10
- 前置阅读：[Demo 注册表](../demo-registry.md)
- 后续阅读：[执行轨迹](execution-trace.md)

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
