# Ray Serve：调用链

## 主链
```text
serve.start → controller deploy → DeploymentState → router → replica → response
```

## 实现落点
controller 管理 deployment state，router 选择 replica，replica 执行用户 callable。

## 失败链
```text
依赖/输入失败 → deploy/start/health/route/user exception。 → 重试、失败传播或清理
```
