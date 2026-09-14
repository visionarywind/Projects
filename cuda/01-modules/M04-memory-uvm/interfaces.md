# M04 接口契约

- `memobjAlloc`：根据 descriptor 创建对象并选择/创建 backing；调用者需处理失败回滚。
- `memglobalsRegisterMemobj`：使 object 可通过 unified/device pointer 查找；失败时对象必须释放。
- `memobjGetByUnifiedPtr`/`memobjGetByDevicePtr`：查找不等于取得永久所有权。
- `memobjFreeNoSync`：只在调用者已完成同步/依赖处理后使用。

descriptor compatibility 比较 location、cache、mapping、owner、API source、page size、fixed address、compression、handle types 等字段。
