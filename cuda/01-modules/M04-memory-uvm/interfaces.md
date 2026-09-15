# M04 接口契约

- `memobjAlloc`：根据 descriptor 创建对象并选择/创建 backing；调用者需处理失败回滚。
- `memglobalsRegisterMemobj`：使 object 可通过 unified/device pointer 查找；失败时对象必须释放。
- `memobjGetByUnifiedPtr`/`memobjGetByDevicePtr`：查找不等于取得永久所有权。
- `memobjFreeNoSync`：只在调用者已完成同步/依赖处理后使用。
- `suballocatorSuballocate`：在兼容 descriptor tree 中 best-fit；未找到区域时返回 success 且 `memobj->memblock == NULL`，由调用者创建新 block。
- `suballocatorSuballocateFromNewMemblock`：绑定请求区并把剩余区插入 tree；整 block 用尽时转为 `noSuballoc`。
- `suballocatorFreeMemobj`：要求 memmgr lock 和可 suballocate block；把 memobj 存储复用为 free node 并合并相邻区域。
- `memblockAlloc`/`memblockFree`：suballocator 与 DMAL/UVA 的所有权边界；只有 block 创建/最终销毁才触发物理/平台资源操作。

descriptor compatibility 比较 location、cache、mapping、owner、API source、page size、fixed address、compression、handle types 等字段。
