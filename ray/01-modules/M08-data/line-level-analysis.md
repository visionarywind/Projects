# Ray Data：行级分析

## 证据锚点
- `python/ray/data/read_api.py:678-828`、`dataset.py:270-8709`、`streaming_executor.py:107-740`

## 结论
- 入口负责验证/归一化，副作用发生于：read 构造 logical Read；Dataset 累积 plan；executor 物化 stages、blocks 和 backpressure。
- 分支：map/batches、streaming、repartition、split、cache、retry。
- 错误：datasource/decode、pickle object column、task failure、OOM。

动态行为未运行，不写成已验证。
