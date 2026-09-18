# Ray Serve：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/serve/api.py:82-166,902-1010`、`controller.py:1139-1196`、router/replica/deployment_state|公共/模块入口|已确认|
|实现|controller 管理 deployment state，router 选择 replica，replica 执行用户 callable。|真实状态改变|已确认|
|测试|python/ray/serve/tests/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
