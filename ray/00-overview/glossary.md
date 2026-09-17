# 术语表

- **Driver**：调用 Ray API、提交任务并消费结果的用户进程。
- **Worker**：执行 task 或 Actor 方法的运行时进程。
- **CoreWorker**：语言 worker 与 C++ distributed runtime 的桥接组件。
- **Task**：无持久对象状态的远程函数执行单元。
- **Actor**：由 Ray 管理生命周期、持有状态的远程对象。
- **Object/ObjectRef**：跨进程/节点访问的不可变值及其逻辑引用。
- **Raylet**：节点级任务、worker 和资源管理组件。
- **GCS**：集群级控制元数据、服务发现和发布订阅相关组件。
- **Object Manager/Store**：对象本地存储、传输及可用性管理边界。
- **Runtime env**：为 task/Actor/worker 准备依赖和运行环境的机制。
- **AI libraries**：Data、Train、Tune、RLlib、Serve 等上层库。
- **证据状态**：已确认、推断、未知；“未验证”专指命令/运行行为尚未执行。

## 相关文档
[项目总览](project-overview.md) · [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`。

## 未解决问题
个别新 API/实验性 channel 的术语需以相应模块文档补充。

## 下一步阅读建议
结合架构图阅读上述术语，而不是孤立记忆定义。
