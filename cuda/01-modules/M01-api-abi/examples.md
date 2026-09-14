# M01 代表性例子

### 分配

`cuMemAlloc_v2` 的可追踪例子是 output 置零→context 检查→`CUmemdesc`→`memobjAlloc`→global register→返回 device pointer。它同时展示 ABI wrapper、锁、所有权转移和错误回滚。

### launch

`cuLaunchKernel` 在普通 stream 和 capture stream 之间分支；同一个公开入口可以产生异步 push，也可以只创建 graph node。证据：[src/api/apilaunch.c:223-301]。
