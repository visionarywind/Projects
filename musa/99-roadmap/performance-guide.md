# 性能指南

优先测量而不是猜测：API CPU 时间、排队延迟、submit 延迟、GPU 执行时间、同步等待时间和内存分配/回收。避免在热路径增加无界日志；使用已有 correlation/submission/submit trace 机制进行采样。修改依赖扫描、平台串行锁或 copy manager 前，必须做多 stream/多设备对比。
