# Ray Data：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/data/read_api.py:678-828`、`dataset.py:270-8709`、`streaming_executor.py:107-740`|公共/模块入口|已确认|
|实现|read 构造 logical Read；Dataset 累积 plan；executor 物化 stages、blocks 和 backpressure。|真实状态改变|已确认|
|测试|python/ray/data/tests/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
