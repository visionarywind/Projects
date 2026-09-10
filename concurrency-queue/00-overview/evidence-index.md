# 证据索引

- 文档目的：集中列出最常引用的源码证据，便于行号校验和增量维护。
- 最后更新：2026-09-10

| 主题 | 证据 |
|---|---|
| queue 构造/析构 | `concurrentqueue.h:833-919` |
| token 生命周期 | `concurrentqueue.h:671-733` |
| 公开 enqueue/dequeue | `concurrentqueue.h:1010-1361` |
| enqueue dispatch | `concurrentqueue.h:1395-1418` |
| free list | `concurrentqueue.h:1466-1579` |
| Block 空状态 | `concurrentqueue.h:1588-1714` |
| producer 基础索引 | `concurrentqueue.h:1727-1791` |
| explicit enqueue/dequeue | `concurrentqueue.h:1877-2081` |
| implicit enqueue/dequeue | `concurrentqueue.h:2515-2648` |
| block requisition | `concurrentqueue.h:3068-3143` |
| producer list | `concurrentqueue.h:3256-3306` |
| implicit hash | `concurrentqueue.h:3323-3509` |
| queue fields | `concurrentqueue.h:3678-3708` |
| blocking construction/enqueue | `blockingconcurrentqueue.h:23-129` |
| blocking waits | `blockingconcurrentqueue.h:146-548` |
| semaphore spinning/wait | `lightweightsemaphore.h:280-360` |
| semaphore signal | `lightweightsemaphore.h:421-430` |
| C ABI declarations | `c_api/concurrentqueue.h` |
| C ABI implementation | `c_api/concurrentqueue.cpp`, `c_api/blockingconcurrentqueue.cpp` |
| CMake interface target | `CMakeLists.txt:1-16` |
| Make targets | `build/makefile:28-50` |
| CI native/RISC-V | `.github/workflows/ci.yml` |
| README contract | `README.md:47-180` |

## 行号维护规则

源码变更后，先更新本表和受影响文档的证据范围，再做链接和证据一致性检查。行号是当前 HEAD 参考，不应在未重新读取源码时声称仍精确。
