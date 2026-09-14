# M02 数据结构与状态

| 对象 | 关键状态 | 持有/并发 |
|---|---|---|
| `CUglobals` | initialized、devices、managers、initMutex | 进程级；初始化锁保护 |
| `CUItlsThreadData` | current context、线程 API 状态 | 每线程 TLS |
| `CUctx` | ACTIVE/DESTROYED、API version、sticky error、memmgr/stream/module | context 持有；API 操作按 context 锁同步 |
| primary context/TSG | primary ownership、attach references | context create/destroy 策略管理 |

初始化失败按反向顺序释放 manager；TLS push 失败会销毁刚创建的 context。sticky error 由异步路径产生、在初始化检查处观察；具体全部产生点未闭合（静态确认：[src/cui/cuiinit.c:2910-3039,3060-3208]；[src/api/apictx.c:67-84]）。
