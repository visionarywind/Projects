# Ray Data：接口契约

## 输入
调用方须提供可验证、可序列化且符合生命周期的输入；入口证据：`python/ray/data/read_api.py:678-828`、`dataset.py:270-8709`、`streaming_executor.py:107-740`。

## 输出与副作用
- read_datasource → logical plan → StreamingExecutor → block tasks → Dataset
- 核心数据：Dataset、Block、Metadata、plan、ExecutionTask、block refs。

## 失败
datasource/decode、pickle object column、task failure、OOM。

跨模块修改需同步 binding/proto/日志/metrics/测试。
