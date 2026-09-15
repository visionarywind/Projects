# 开发配方

- 新 API：同步修改 `inc/cuda.h`、`src/api`、generated API 输入、`.def` 和 `cuda.nvmk`。
- 新资源：先定义 owner/lock/异步完成条件，再补创建失败和 destroy 回滚。
- 改 launch：同时核对参数 metadata、memory tracking、QMD/HAL、tools callback 和 stream end push。
- 改显存池化：同步 `canSuballocate`、descriptor compatibility、HAL block size/alignment、radix best-fit/split/coalesce、memblock DMAL free 和碎片测试；不要把 `CUheap` VA 索引改成物理池语义。
- 改 Graph 资源：同步 capture origin/invalidation、per-context `CUIgraphCtxData`、QMD/constant-bank/internal stream/marker、scheduler backing、launch tracking 和 destroy reverse path；为 instantiate/launch 失败补注入测试。
- 改架构：检查 `NVCFG(GLOBAL_ARCH_*)`、`cuda.nvmk` 源清单与具体 HAL 实现。
- 改测试：先在 `cuda_test.nvmk` 定义 binary，再注册 case，覆盖成功、错误和清理。
