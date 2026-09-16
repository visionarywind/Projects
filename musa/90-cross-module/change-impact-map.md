# 变更影响地图

| 改动 | 必查文件/模块 | 回归重点 |
|---|---|---|
| 新增 Driver API | M02、export/accessor、头文件 | ABI、导出、v1/v2、错误码 |
| 改 Memory 类型 | M05、M04、M06、HAL | tracker、peer、异步释放 |
| 改 Stream 提交 | M06、M09、MUPTI/GDB | 顺序、依赖、线程退出 |
| 改 Kernel 参数 | M07、DispatchCommand、module | 参数布局、shared memory、metadata |
| 改 Graph node | M08、M06、M05/M07 | capture、拓扑、update、资源保活 |
| 改 CMake/安装 | M01、CI、README | 子模块、artifact、系统路径 |

先执行静态影响分析，再在授权硬件上执行最小专项测试。
