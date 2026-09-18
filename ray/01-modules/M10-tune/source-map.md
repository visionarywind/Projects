# Ray Tune：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/tune/tuner.py:43-425`、`tuner_internal.py:63-698`、TuneController/Trial|公共/模块入口|已确认|
|实现|Tuner 解析 trainable/config，controller 驱动 trial，search/scheduler 决策。|真实状态改变|已确认|
|测试|python/ray/tune/tests/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
