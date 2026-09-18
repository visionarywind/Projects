# Ray Train：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`python/ray/train/base_trainer.py:128-936`、`data_parallel_trainer.py:29-603`、TrainController/WorkerGroup|公共/模块入口|已确认|
|实现|trainer 将 config/scaling 转为 controller 和 worker group，各 worker 执行 loop 并报告结果。|真实状态改变|已确认|
|测试|python/ray/train/tests/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
