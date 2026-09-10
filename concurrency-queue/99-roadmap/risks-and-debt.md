# 风险与技术债务

- 文档目的：汇总源码和交付边界中需要持续关注的风险，不虚构已确认 bug。
- 证据状态：下表区分契约风险、验证缺口和未知行为。
- 最后更新：2026-09-10
- 前置阅读：[调试指南](debugging-guide.md)
- 后续阅读：[路线图](roadmap.md)

| 项目 | 类型 | 证据 | 当前判断 |
|---|---|---|---|
| queue 析构并发访问 | 契约风险 | README、destructor | 调用方必须先停止/等待 |
| token 并发共享 | 契约风险 | token 实现/README | 不应共享 |
| memory-order 修改缺少形式证明 | 验证缺口 | 核心原子协议 | 需模型/压力测试 |
| C ABI 非法输入/ownership | API 未决 | c_api cast/void* | 需定义和测试 |
| 平台 semaphore 行为 | 平台未知 | 条件编译分支 | 需目标机验证 |
| CMake 与 Make 入口分裂 | 维护复杂度 | CMake/Make | 修改需双向检查 |
| benchmark 可比性 | 测量风险 | 多 adapter | 需记录场景和环境 |
| 文档行号漂移 | 维护债务 | evidence index | 源码变更后重校验 |

这些项目不是自动确认的缺陷；它们是需要证据、契约或验证补齐的边界。
