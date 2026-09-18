# Ray Data：设计与边界

- 源码锚点：`python/ray/data/read_api.py:678-828`、`dataset.py:270-8709`、`streaming_executor.py:107-740`
- 证据：静态源码已确认；真实运行未验证。

## 目标与边界
入口 ``python/ray/data/read_api.py:678-828`、`dataset.py:270-8709`、`streaming_executor.py:107-740`` 通过内部状态达到真实副作用，不把 API wrapper 当作实现终点。

## 设计不变量
1. 控制元数据、数据内容和资源所有权分开。
2. 异步结果的完成、失败、取消均可观察。
3. cleanup 覆盖正常、取消和依赖进程失联。

## 上下文
driver、Ray tasks/actors、executor、object store。
