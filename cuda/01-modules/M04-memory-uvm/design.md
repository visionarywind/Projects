# M04 设计

M04 采用 descriptor→object→backing block→HAL resource 的分层，使统一地址、共享 backing、子分配和外部导入能复用同一内存管理基础。`CUmemdesc` 记录语义，`CUmemobj` 记录 API-visible 对象，`CUmemblock` 表示实际 backing。

释放必须验证 API source 和 base pointer，并在异步使用完成后再释放；UVA registry 是 pointer lookup 的全局索引。
