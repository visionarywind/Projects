# Ray Serve：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → serve.start → controller deploy → DeploymentState → router → replica → response → 结果/事件
```

## 分支
滚动更新、autoscaling、reconfigure、ingress、batching、health。

## 异常
deploy/start/health/route/user exception。

## 终止
shutdown/delete 停 controller/router/replicas。
