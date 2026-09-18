# Ray Data：示例与观测点

```python
# 伪代码，仅表达接口边界，未运行
handle = entry(config)
result = handle.run()
```

观察入口状态、正常链 `read_datasource → logical plan → StreamingExecutor → block tasks → Dataset`、错误 `datasource/decode、pickle object column、task failure、OOM。` 和清理 `executor shutdown 取消 stages 并释放 refs/cache。`。
