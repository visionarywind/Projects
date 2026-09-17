# D01 修改练习

- 版本：HEAD `cfe4725d23`；练习未执行。

## 练习 1：参数与多个任务

让 `hello_world(name)` 返回带名字的字符串，提交 3 个调用，使用一次 `ray.get(refs)`。验证列表顺序保持与 ref 输入顺序一致。[契约：`worker.py:2896-2904`]

## 练习 2：异常传播

函数在指定参数下抛 `RuntimeError`，捕获 `ray.get` 的异常并检查 cause 类型。不要把任务函数异常误判为提交失败。

## 练习 3：资源与选项

使用 `.options(num_cpus=...)`，在 `_remote` 断点观察 `resources` 和 scheduling strategy；不在资源不足环境请求不可满足资源，避免无限等待。

## 练习 4：显式生命周期

在 main 中显式 `ray.init()` / `ray.shutdown()`，记录初始化返回、进程和关闭日志；对比隐式 auto-init 路径。

## 验收

每个练习应包含代码 diff、命令、实际输出、日志/断点证据、失败清理和恢复步骤。未执行前保持“未验证”。

## 相关文档
[调试](debug-walkthrough.md) · [测试配方](../../99-roadmap/testing-recipes.md)

## 源码证据摘要
`remote_function.py:201-353`；`worker.py:2881-3029`。

## 未解决问题
练习对应的最小 pytest target 待检索现有测试后确定。

## 下一步阅读建议
先做练习 1，再做故障和生命周期练习。
