# Ray Serve：实现

## 入口到副作用
controller 管理 deployment state，router 选择 replica，replica 执行用户 callable。

## 正常
serve.start → controller deploy → DeploymentState → router → replica → response

## 分支与异常
- 滚动更新、autoscaling、reconfigure、ingress、batching、health。
- deploy/start/health/route/user exception。

## 清理
shutdown/delete 停 controller/router/replicas。

## 数据与执行上下文
- Application、DeploymentTargetState、ReplicaID、route metadata。
- driver API、controller actor、proxy/router、replica actors。
