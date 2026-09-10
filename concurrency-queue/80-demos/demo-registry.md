# Demo 注册表

- 文档目的：列出真实 Demo 及其证据边界。
- 最后更新：2026-09-10
- 前置阅读：[总入口](../README.md)
- 后续阅读：[D01](D01-unit-test-smoke/README.md)

| ID | Demo | 真实性 | 用途 | 执行状态 |
|---|---|---|---|---|
| D01 | unit test smoke | 仓库真实 executable | 端到端正确性和资源生命周期 | native smoke/`test_threaded` 已验证 |
| D02 | benchmark run | 仓库真实 executable/script | 性能测量和结果提取 | 未验证 |

D01 是默认学习路径；D02 只在需要性能问题时进入。两者都不能把“源码存在命令”写成“当前环境执行成功”。
