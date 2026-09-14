# M05 source map

| 文件 | 证据焦点 |
|---|---|
| `src/api/apistream.c` | stream flags、句柄和 common wrapper |
| `src/cui/cuistream.c` | pool、QMD、capture、detach/reclaim |
| `src/cui/channel.c`、`channel_manager.c` | DMAL channel 和 manager 资源 |
| `src/cui/stream_manager.c` | context stream 集合和并发策略 |
| `src/cui/marker.c`、`semaphore.c` | 异步完成信号 |
| `src/cui/pushbuffer.c`、`gpfifo.c` | 命令写入/提交（设备消费待深读） |
| `src/cui/qmd.c`、`task.c` | QMD/task 生命周期 |

核心已读证据：[src/api/apistream.c:43-109]、[src/cui/cuistream.c:1741-1877,1897-1954,1956-1986,2004-2088]、[src/cui/channel.c:43-173]。
