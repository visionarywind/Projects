# M06 开发指南

- 文档目的：指导修改 Env、port、Cache 和 Arena。
- 适用范围：M06。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：约束来自源码接口；性能和平台结论需实验。
- 最后更新：2026-09-10
- 前置阅读：[M06 line-level-analysis](line-level-analysis.md)
- 后续阅读：[跨模块影响](../../90-cross-module/change-impact-map.md)
## 结论摘要

本页聚焦 01-modules/M06-env-platform/development-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 修改 Env

1. 先确认是公共 ABI 变化还是单个平台实现变化。
2. 新增 Env 虚方法时同步 POSIX、Windows、MemEnv、EnvWrapper 和测试替身。
3. 明确线程访问契约、返回对象 ownership、错误 Status 和 Slice/scratch 生命周期。
4. 对写入路径测试 Append/Flush/Sync/Close/失败清理。
5. 不能把 POSIX 特性（fd、mmap、fcntl）泄漏到公共头文件。

## 修改 port

1. 检查 `port/port.h` 的平台选择和配置宏。
2. 可选库不可用时 wrapper 必须返回 false/不支持，而不是链接失败或产生未定义数据。[port/port_stdcxx.h:89-217](../../../source/leveldb/port/port_stdcxx.h#L89-L217)
3. 保持 Mutex/CondVar 的锁注解和 adopt_lock 配对。

## 修改 Cache/Arena

1. 画出每个 handle 的 Insert/Lookup/Release/Erase 路径。
2. deleter 必须只在 refs 为零时执行。
3. Arena 返回的指针不能跨 owner 生命周期。
4. 用故障注入、ASan/TSan 和高并发读验证。

## 跨模块检查

- M02：Schedule、锁、关闭和文件删除。
- M03：Arena 与 MemTable entry 生命周期。
- M05：Table/Block cache handle 和 RandomAccessFile。
- M07：平台矩阵、可选依赖和测试注册。

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
