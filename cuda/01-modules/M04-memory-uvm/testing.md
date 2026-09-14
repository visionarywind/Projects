# M04 测试、开发与风险

## 测试证据

- `basic_sanity:init` 直接创建 v3020 context，使用 driver 内部 `memobjAlloc` 分配 2 MiB device memobj，随后逐个 `memobjFree` 并销毁 context（静态确认：[tests/cuda_test/basic_sanity.cu:49-100]）。
- `basic_sanity:kernel` 使用 portable + mapped host allocation，取得 device pointer，查找 memobj 并验证 host/device/UVA片段关系（静态确认：[tests/cuda_test/basic_sanity.cu:169-209]）。
- `unittest_memcpy.cpp` 覆盖 pageable host、device、同步 memcpy、跨 context 和三维 descriptor；P2P 测试在 GPU 少于 2 个时 waive（静态确认：[tests/cuda_test/memcpy/unittest_memcpy.cpp:43-85,87-170]）。测试均未执行。

## 修改配方

1. 新内存 API：在 API wrapper 构造完整 `CUmemdesc`，明确 `apiSource`，再进入 memobj；不要直接调用底层 RM。
2. 新 backing 类型：更新 `isValidAllocation`、compatibility 比较、HAL block size 和失败释放。
3. 修改映射：同时检查 UVA lookup、P2P register/unregister、host pointer lookup 和 free 的 base-pointer 语义。
4. 修改异步释放：确认 launch memory tracking 和 context synchronize 的顺序，不能在 GPU 仍可能访问时回收 backing。

## 风险

- descriptor 字段遗漏会错误复用不兼容 block（证据：[src/cui/memobj.c:126-163]）。
- global registration 失败而不回滚会留下局部对象（证据：[src/api/apimem.c:105-111]）。
- 释放 interior pointer 被拒绝是契约，不应为“方便”放宽（证据：[src/api/apimem.c:315-319]）。
- UVM page-fault/迁移路径跨越当前源码边界，任何性能或一致性结论都需运行时证据。
