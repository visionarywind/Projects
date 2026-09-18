# 启动、集群、Autoscaler、runtime env 与 Jobs：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/_private/worker.py:1439-1505`、`python/ray/scripts/scripts.py:800-1318,1976-2095`、autoscaler/runtime_env/job_manager|公共/模块入口|已确认|
|实现|init 区分已有集群和本地启动；scheduler 计算供需，agent 设置缓存环境，JobManager 管理作业。|真实状态改变|已确认|
|测试|python/ray/autoscaler/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
