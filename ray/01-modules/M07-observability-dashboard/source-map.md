# Dashboard、观测、调试与认证：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/dashboard/dashboard.py:33-107`、`head.py:49-551`、StateAPI/Reporter|公共/模块入口|已确认|
|实现|head 启动模块，StateAPI 聚合状态，Reporter 采集指标；auth 和脱敏是边界约束。|真实状态改变|已确认|
|测试|python/ray/dashboard/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
