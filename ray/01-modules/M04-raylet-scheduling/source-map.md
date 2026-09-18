# Raylet 调度与资源：源码地图

|层次|证据|作用|状态|
|---|---|---|---|
|入口|`src/ray/raylet/node_manager.cc:199-373,575-609,1415-`、local/cluster lease manager|公共/模块入口|已确认|
|实现|NodeManager 管理 worker、资源、节点；lease managers 完成局部和集群选择。|真实状态改变|已确认|
|测试|src/ray/raylet/|回归边界|已定位、未运行|

阅读顺序：入口 → 实现函数体 → 异常/清理 → 测试。
