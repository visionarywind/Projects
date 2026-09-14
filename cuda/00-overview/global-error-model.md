# 全局错误模型

错误通常在三层传播：

1. API wrapper 先返回参数、句柄、context 和 capture 错误；输出参数在适用入口先清零。
2. CUI 将资源创建/登记/同步失败原样或按 API 契约映射为 `CUresult`，并执行局部回滚。
3. 异步 channel/设备错误可写入 context sticky error，后续 `cuiInitCheckCtx` 再观察（静态确认：[src/cui/cuiinit.c:2910-3039]）。

典型边界：memobj 全局登记失败释放对象；context TLS push 失败销毁新 context；stream 创建后 QMD pool 登记失败 detach；launch push 失败仍结束 stream push、回滚 UVM running 状态并通知 tools（静态确认：[src/api/apimem.c:100-111]；[src/api/apictx.c:73-84]；[src/api/apistream.c:83-109]；[src/cui/cuilaunch.c:779-817]）。

具体 sticky error 产生点分散于 DMAL/channel，外部 RM 错误映射仍不完整。
