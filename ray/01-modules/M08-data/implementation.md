# Ray Data：实现

## 入口到副作用
read 构造 logical Read；Dataset 累积 plan；executor 物化 stages、blocks 和 backpressure。

## 正常
read_datasource → logical plan → StreamingExecutor → block tasks → Dataset

## 分支与异常
- map/batches、streaming、repartition、split、cache、retry。
- datasource/decode、pickle object column、task failure、OOM。

## 清理
executor shutdown 取消 stages 并释放 refs/cache。

## 数据与执行上下文
- Dataset、Block、Metadata、plan、ExecutionTask、block refs。
- driver、Ray tasks/actors、executor、object store。
