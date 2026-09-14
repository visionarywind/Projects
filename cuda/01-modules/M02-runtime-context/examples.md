# M02 代表性例子

### sticky error

channel 异步错误不是必然在产生点返回；它被保存到 context，下一次 `cuiInitCheckCtx` 才可能观察到。这解释了 API 返回位置与 GPU 错误发生位置的时间差。

### 创建失败

`cuiCtxCreate` 成功前不会把部分初始化对象交给调用者；失败路径将 `ret` 清零，避免悬挂句柄。
