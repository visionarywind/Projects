# M03 代表性例子

### 可见 ordinal

`CUDA_VISIBLE_DEVICES`/predicate 与 PCI 排序共同作用时，用户 ordinal 不应直接解释为物理 bus 顺序；`dispatch_main --listGpus` 也明确区分过滤后的 ordinal 和 bus order。

### channel 失败

DMAL Init 成功但 tracking semaphore 分配失败时，channel 不能插入 manager，必须 Deinit 并释放已创建对象。
