# Ray Data：调用链

## 主链
```text
read_datasource → logical plan → StreamingExecutor → block tasks → Dataset
```

## 实现落点
read 构造 logical Read；Dataset 累积 plan；executor 物化 stages、blocks 和 backpressure。

## 失败链
```text
依赖/输入失败 → datasource/decode、pickle object column、task failure、OOM。 → 重试、失败传播或清理
```
