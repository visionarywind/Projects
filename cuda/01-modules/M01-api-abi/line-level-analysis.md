# M01 行级分析

- `[src/api/apiinit.c:19-47]`：先初始化全局 mutex，再拒绝非零 flags，最后进入 `cuiInit`；非法参数不会触发完整初始化。
- `[src/api/apimem.c:52-61]`：先把输出指针置零，使错误返回不会暴露未初始化地址。
- `[src/api/apimem.c:89-117]`：在 context lock 下分配 memobj，解锁后做 global registration；registration 是对象可见性的提交点。
- `[src/api/apimem.c:231-339]`：free 先按 unified/device pointer 查找，再校验 source/base pointer，成功同步后才 `memobjFreeNoSync`。
- `[src/api/apilaunch.c:252-286]`：capture 失败销毁新节点或使 capture 失效。
