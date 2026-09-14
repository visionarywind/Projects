# M08 设计

M08 采用旁路插桩而非复制 runtime：callback 通过 enable table 快速判断，debugger 通过状态检查和 attach 协议访问运行时，profiler 通过全局 mode/perfmon 影响 launch，memcheck 通过 patch 和额外 memory table 检测访问。

工具能力可能改变主路径（blocking、skip launch、code patch），因此必须记录其控制权而非只记录日志功能。
