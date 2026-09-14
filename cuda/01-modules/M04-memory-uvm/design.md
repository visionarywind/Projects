# M04 设计

M04 采用 descriptor→object→backing block→HAL/DMAL resource 的分层，使统一地址、共享 backing、内部 suballocation 和外部导入能复用同一内存管理基础。`CUmemdesc` 记录语义，`CUmemobj` 记录 API-visible 切片，`CUmemblock` 表示一次实际 backing。

## 池化策略

源码中的通用池化是 `suballocator.c` 管理的 block 内空闲区，而不是独立的 `CUmemoryPool` 对象：可兼容的 descriptor 共用一个 radix tree；tree 以空闲区大小为 key，best-fit 取出并按 alignment 切分，free 时相邻区合并。HAL 仅决定建议 block size/alignment；真正的显存申请仍由 DMAL 负责。详见 [GPU 显存池化](gpu-memory-pooling.md)。

释放必须验证 API source 和 base pointer，并在异步使用完成后再释放；UVA registry 是 pointer lookup 的全局索引。最后一个 memobj 释放后，空闲区 tree 节点和整个 memblock 的 backing 按顺序销毁。
