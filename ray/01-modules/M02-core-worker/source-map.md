# CoreWorker、任务提交与执行：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/_raylet.pyx:3938-4032`、`src/ray/core_worker/core_worker.cc:2056-2135`、`src/ray/core_worker/task_submission/normal_task_submitter.cc:33-504`|公共/模块入口|已确认|
|实现|Cython 转换 options；C++ 生成 TaskSpec，异步解析依赖、排队、申请 lease 和推送任务。|真实状态改变|已确认|
|测试|src/ray/core_worker/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
