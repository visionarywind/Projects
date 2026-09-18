# Ray Data：执行流

## 正常流
```text
入口 → 配置/参数归一化 → 内部状态创建 → read_datasource → logical plan → StreamingExecutor → block tasks → Dataset → 结果/事件
```

## 分支
map/batches、streaming、repartition、split、cache、retry。

## 异常
datasource/decode、pickle object column、task failure、OOM。

## 终止
executor shutdown 取消 stages 并释放 refs/cache。
