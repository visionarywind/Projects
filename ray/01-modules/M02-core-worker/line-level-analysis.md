# CoreWorker、任务提交与执行：行级分析

## 证据锚点
- `python/ray/_raylet.pyx:3938-4032`、`src/ray/core_worker/core_worker.cc:2056-2135`、`src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`

## 结论
- 入口负责验证/归一化，副作用发生于：Cython 转换 options；C++ 生成 TaskSpec，异步解析依赖、排队、申请 lease 和推送任务。
- 分支：依赖未就绪、idle worker、spillback、placement group、取消、generator backpressure。
- 错误：依赖/ runtime env / worker 启动失败、不可调度、Raylet 失联。

动态行为未运行，不写成已验证。
