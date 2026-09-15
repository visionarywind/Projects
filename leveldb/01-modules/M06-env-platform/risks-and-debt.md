# M06 风险与技术债务

- 文档目的：登记平台、并发和资源管理风险。
- 适用范围：M06。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：触发机制已确认，严重度需实验。
- 最后更新：2026-09-10
- 前置阅读：[M06 design](design.md)
- 后续阅读：[项目风险](../../99-roadmap/risk-register.md)
## 结论摘要

本页聚焦 01-modules/M06-env-platform/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

| 风险 | 证据 | 影响 | 验证 |
|---|---|---|---|
| Env 虚接口实现不完整 | `env.h:50-217`、`EnvWrapper` | 编译失败或运行时错误 | 平台全矩阵构建 |
| fd/mmap limiter 配对错误 | `env_posix.cc:68-128` | fd 泄漏或过量资源 | limit/fault injection |
| RandomAccessFile 生命周期错误 | `env.h:260-270` | 并发 UAF/错误读 | 并发读 + ASan |
| Schedule 任务未串行化 | `env.h:191-197` | DB 状态竞态 | TSan/shutdown 压测 |
| Cache handle 未 Release | `cache.h:44-80` | 缓存析构断言/泄漏 | cache lifecycle test |
| Cache deleter 过早执行 | `cache.cc:226-237` | UAF/double free | eviction + iterator |
| Arena 指针越过 owner | `arena.cc:13-17` | UAF | ASan |
| CondVar 锁状态不匹配 | `port_stdcxx.h:68-87` | 死锁/未定义行为 | stress test |
| 可选依赖宏不一致 | `port_stdcxx.h:24-33` | ABI/链接或格式不兼容 | 多配置构建 |
| legacy DeleteFile/DeleteDir 递归 | `env.cc:20-28` | 栈溢出/功能失效 | 自定义 Env 测试 |

## 技术债务

M06 依赖裸指针、手工 delete 和平台宏；公共 Env ABI 及持久化压缩格式使大规模替换成本高。注释中的兼容 API 表明旧接口仍需维护，但未来版本可能移除，新增代码应使用现代名称。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题

需要真实 CI 矩阵、文件系统故障注入和 Sanitizer 结果来确定上述风险的实际优先级。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
