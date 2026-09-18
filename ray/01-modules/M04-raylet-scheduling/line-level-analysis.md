# Raylet 调度与资源：行级分析

## 证据锚点
- `src/ray/raylet/node_manager.cc:199-373,575-609,1415-`、local/cluster lease manager

## 结论
- 入口负责验证/归一化，副作用发生于：NodeManager 管理 worker、资源、节点；lease managers 完成局部和集群选择。
- 分支：资源不足、placement group、labels、租约重用、spillback、节点变化。
- 错误：不可调度、worker/Raylet 断连、placement group 删除。

动态行为未运行，不写成已验证。
