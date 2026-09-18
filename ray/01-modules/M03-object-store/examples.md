# Object Store 与 Object Manager：示例与观测点

```python
# 伪代码，仅表达接口边界，未运行
handle = entry(config)
result = handle.run()
```

观察入口状态、正常链 `put/get → ObjectManager Pull/Push → Plasma/filesystem → chunk receive → available/free`、错误 `对象丢失、传输失败、ObjectStoreFull、owner/node failure。` 和清理 `CancelPull、FreeObjects、Stop 终止传输并释放 buffers。`。
