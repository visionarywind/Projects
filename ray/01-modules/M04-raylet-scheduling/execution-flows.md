# Raylet 调度与资源：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → 依赖满足 → RequestWorkerLease → 本地匹配或 spillback → worker 注册 → PushNormalTask → 结果/事件
```

## 分支
资源不足、placement group、labels、租约重用、spillback、节点变化。

## 异常
不可调度、worker/Raylet 断连、placement group 删除。

## 终止
DestroyWorker、disconnect/failure、node removal 释放资源。
