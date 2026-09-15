# 运行时模型

- 文档目的：解释 00-overview/runtime-model.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/runtime-model.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 进程角色

普通 HTTP 模式通常包含：

```text
HTTP 主进程
  ├─ TokenizerManager（接收请求、tokenize、派发结果）
  ├─ Detokenizer process（可选独立进程）
  └─ Scheduler process × PP/TP worker 组合
         └─ ModelRunner + model + device resources
```

**已确认**：`run_scheduler_process` 在 scheduler 子进程中先 `publish(server_args, role="scheduler")`，再构造 `Scheduler`；`run_detokenizer_process` 对 detokenizer 采用相同的 publish-before-construction 规律。

## 2. 对象生命周期

### 启动

1. CLI 形成 `ServerArgs`；
2. `resolve_once` 固化派生配置；
3. HTTP/IPC endpoint 建立；
4. scheduler 进程绑定 GPU 和 rank；
5. distributed WORLD 和模型并行组建立；
6. ModelConfig、ModelRunner 和模型权重就绪；
7. KV pool、attention backend、CUDA Graph 和采样 buffer 准备；
8. startup pipe 回传 ready 信息。

### 稳态

Scheduler 循环接收请求、决定 batch、调用 worker、处理 token 和输出。Tokenizer/detokenizer 通过 `rid` 把批量结果映射回请求。

### 退出/异常

父进程死亡检测、scheduler exception 传播、watchdog、metrics shutdown、host resource release 和进程树清理共同构成退出路径。具体信号和环境变量属于 M15/M04 的实现细节。

## 3. 三种时间尺度

- 请求尺度：单个请求从 waiting 到 finished/aborted。
- 调度尺度：每次 prefill/decode 迭代重建 batch。
- 启动尺度：只做一次的模型/通信/内存准备。

混淆这三个尺度是阅读 SGLang 最常见的错误。例如 KV cache allocation 是启动资源准备，KV block admission 是请求级调度，两者都叫“KV cache”但生命周期不同。

## 4. CPU 与设备异步

Scheduler 在 CPU 上准备 metadata，ModelRunner 在 forward stream 上执行设备工作；overlap、CUDA event 和 delayed sampling 可能使“函数返回”早于“所有设备结果可用于下一个状态更新”。具体同步点必须沿调用链寻找，不能以 Python 调用顺序臆测。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
