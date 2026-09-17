# M04 Driver：普通内存池化实现——V2/V3 源码分析

- 文档目的：解释 01-modules/M04-driver/driver-memory-pool-analysis.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：Driver `977336913`；Runtime `50be4c922`
- 证据状态：用户态源码静态分析已确认；未执行构建、硬件运行和性能测试
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M04-driver/driver-memory-pool-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 分析范围：普通 `rtMalloc/rtFree` 下沉到 Driver 后的 V2/V3 cache；不包含 Runtime KernelMemoryPool 和 SOMA 的完整实现

## 1. 先建立正确的心智模型

普通 `rtMalloc` 经过 Runtime 的普通设备内存路径后，可能进入 Driver cache，但不是所有请求都必然进入 cache：

```text
rtMalloc
  -> Runtime NpuDriver::DevMemAlloc
  -> halMemAlloc
  -> Driver SVM malloc manager
  -> 判断 size / align / flag / memtype / 设备能力
       -> cache allocator
       -> 或 normal allocator
```

Driver cache 不是“保存几个已经释放的指针”。它管理的是一组可复用的 backing range，以及 range 内的空闲区间：

```text
backing range
  -> 地址范围元数据
  -> 空闲 area/node
  -> 按地址索引
  -> 按大小索引
  -> 切分
  -> 相邻合并
  -> cache 统计
  -> shrink 和延迟回收
```

应与另外两类 Runtime 机制区分：

| 机制 | 所在层 | 主要对象 | 分配策略 |
|---|---|---|---|
| KernelMemoryPool | Runtime | 固定 2 MiB backing 中的内部块 | `MemoryList` 链表 First-Fit |
| SOMA | Runtime + Driver/HAL | `rtMemPool_t` 对应的 Segment | Runtime FREE Segment 按大小 Best-Fit |
| Driver ordinary cache | Driver SVM | backing range 中的 area/node | V2/V3 均按大小 exact/upper-bound 复用 |

因此，普通 `rtMalloc` 不走 SOMA，也不走 KernelMemoryPool；但在 Driver 层可能复用已保留的普通内存 backing。

## 2. 用一个例子理解“池化”

假设程序依次执行：

```c
void *p1;
void *p2;
rtMalloc(&p1, 3 * 1024 * 1024);
rtFree(p1);
rtMalloc(&p2, 3 * 1024 * 1024);
```

没有 Driver cache 时：

```text
第一次 rtMalloc
  -> 向底层申请/映射 backing
  -> 返回 p1

rtFree
  -> 解除映射并归还底层 backing

第二次 rtMalloc
  -> 再次申请/映射 backing
  -> 返回 p2
```

有 Driver cache 时：

```text
第一次 rtMalloc
  -> cache 没有可用 area
  -> normal allocator 申请一个 backing range
  -> 把 range 加入 cache allocator
  -> 从 range 中切出请求区域
  -> 返回 p1

rtFree
  -> 请求区域回到 cache 的空闲索引
  -> 与相邻空闲区域合并
  -> backing range 暂时保留

第二次 rtMalloc
  -> 在 cache 的大小索引中找到可用 area
  -> 切出并返回
  -> 不需要再次申请同一份 backing
```

注意：V3 的 `expand_granularity` 是 2 MiB 对齐粒度，不等于每次必定申请固定大小的大池。扩展大小由请求大小按扩展粒度和分配粒度对齐得到；16 MiB/32 MiB 主要是是否允许请求进入 cache 的阈值。

## 3. 从 Runtime 到 Driver 的入口

Runtime 普通内存路径根据内存策略、Huge Page、P2P、对齐、运行模式和设备能力选择参数，随后进入 `NpuDriver::DevMemAlloc`，再通过 `halMemAlloc` 下沉到 Driver：

```text
NpuDriver::DevMemAlloc
  -> DevMemAllocOnline / DevMemAllocOffline
  -> halMemAlloc
```

证据：

- `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114]`
- `[runtime/src/runtime/driver/npu_driver_mem.cc:1218-1326]`

V3 HAL 侧的包装关系为：

```text
halMemAlloc
  -> halMemAllocInner
  -> svm_mem_malloc
  -> svm_module_mem_malloc
  -> malloc manager
```

以及释放：

```text
halMemFree
  -> halMemFreeInner
  -> svm_mem_free
  -> svm_module_mem_free
  -> svm_free
```

证据：

- `[driver/src/ascend_hal/svm/v3/api/master/svm_alloc.c:211-288]`
- `[driver/src/ascend_hal/svm/v3/api/master/svm_alloc.c:291-356]`

## 4. V3：把它看成“多个可伸缩的 backing range + 一个区间分配器”

V3 最容易看懂的方式不是从所有函数名开始，而是先看它要解决的问题：

```text
Driver 先获得若干个底层 backing range
  -> 不要求这些 range 连续
  -> 将它们统一交给 gen_allocator
  -> 用户请求从所有 range 的空闲 area 中取一块
  -> 释放时归还 area
  -> 整个 range 空闲时才允许移除和 shrink
```

它不是一个固定地址、固定容量的单一大池，而是可以动态增加和删除多个不连续 range 的池。

### 4.1 V3 的三种核心对象

源码中的三个结构可以这样理解：

```text
cache_allocator
  代表“某个设备 + 某种内存属性”的一个 cache 实例

 ga_inst
  代表该 cache 实例的通用区间分配器

 ga_range
  代表一段从 normal allocator 获得的 backing 地址范围

 ga_area
  代表 ga_range 内当前空闲的一小段区间
```

结构定义证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_allocator.h:35-43]`
- `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:19-56]`

`cache_allocator` 保存：

```c
struct cache_allocator {
    u32 devid;
    u32 flag;
    void *ga_inst;
    pthread_rwlock_t rwlock;
    struct cache_stats stats;
    struct cache_strategy strategy;
};
```

其中：

- `devid`：设备编号；
- `flag`：普通页、Huge Page、P2P、UVA 等属性；
- `ga_inst`：真正管理空闲地址区间的对象；
- `stats`：总量、空闲量、当前分配量和峰值；
- `strategy`：cache 请求阈值、shrink 阈值和粒度。

### 4.2 V3 为什么要有 `range` 和 `area` 两层

假设一次扩展得到 16 MiB 的 backing：

```text
一个 ga_range：
[0x10000000, 16 MiB]

初始一个 ga_area：
[0x10000000, 16 MiB] FREE
```

先分配 4 MiB：

```text
ga_range 仍然是完整的 16 MiB backing

area 变为：
[0x10000000, 4 MiB]   BUSY/已返回
[0x10400000, 12 MiB]  FREE
```

再分配 6 MiB：

```text
[0x10000000, 4 MiB]   BUSY
[0x10400000, 6 MiB]   BUSY
[0x10a00000, 6 MiB]   FREE
```

`ga_range` 描述底层 backing 的所有权边界；`ga_area` 描述该 backing 内部还能否复用的空闲片段。只有最后所有 area 都空闲时，整个 range 才能被 shrink。

### 4.3 V3 的三棵索引树

`ga_inst` 维护两类核心索引，`ga_range` 自己还维护一棵地址树：

```text
addr_range_tree
  -> 查找某个地址属于哪个 ga_range

每个 ga_range.addr_area_tree
  -> 按地址管理该 range 内的空闲 ga_area
  -> 判断地址范围是否有效
  -> 查找左右相邻 area

size_area_tree
  -> 全局按空闲 area 大小查找
  -> 快速满足一个变量大小请求
```

一个 `ga_area` 同时挂在：

```text
range 的地址树
全局的大小树
```

这就是 V3 能够同时做到“按大小分配”和“按地址合并”的原因：

- 分配时主要查 `size_area_tree`；
- free 时先用地址关系找到相邻区域；
- 切分、合并时同步维护大小树和地址树。

### 4.4 V3 到底是不是 Best-Fit

按大小获取 area 的逻辑是：

```c
m_rb_node = multi_rbtree_get(size, &inst->size_area_tree);
if (m_rb_node == NULL) {
    m_rb_node = multi_rbtree_get_upper_bound(
        size, &inst->size_area_tree);
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:163-177]`

它的行为是：

```text
先找恰好等于请求大小的 area
如果没有，再找大于请求大小的最小 size key
```

所以从分配行为看属于按大小的 Best-Fit。更严格地说，当前已确认的是 exact/upper-bound；若同一 size key 中存在多个 area，二级选择顺序还需要结合 `multi_rbtree` 的完整实现确认，不能仅凭这一段代码断言一定按低地址、FIFO 或 LRU。

## 5. V3 一次分配的完整过程

### 5.1 cache 是否接管请求

malloc manager 先判断：

```c
go_cache = go_malloc_cache(
    devid, numa_id, flag, align, aligned_size);

if (go_cache) {
    ret = malloc_cache(
        devid, flag, align, aligned_size, start);
} else {
    ret = malloc_normal(
        devid, numa_id, flag, align, aligned_size, start);
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/malloc_mng/malloc_mng.c:500-516]`

cache 支持条件至少包括：

```c
ca != NULL
size <= ca->strategy.alloc_thres
align == ca->strategy.alloc_gran
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:278-284]`

所以“请求小”还不够，还必须使用正确的 cache 类型和对齐粒度。

### 5.2 cache hit

cache 分配调用：

```c
ret = svm_ga_alloc(ca->ga_inst, 0, va, size);
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:133-153]`

`svm_ga_alloc` 获取写锁后，在非固定地址模式中调用按大小分配：

```c
if (ga_flag_is_fixed_addr(flag)) {
    ret = ga_alloc_by_fixed_addr(inst, *addr, size);
} else if (ga_flag_is_in_fixed_addr_range(flag)) {
    ret = DRV_ERROR_NOT_SUPPORT;
} else {
    ret = ga_alloc_by_size(inst, size, addr);
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:600-621]`

如果找到一个 8 MiB 空闲 area，而请求为 3 MiB：

```text
原 area：
[0x10000000, 8 MiB]

分配 3 MiB 后：
[0x10000000, 3 MiB]  -> 返回给调用者
[0x10300000, 5 MiB]  -> 重新作为 FREE area
```

切分实现 `ga_try_slice_area` 的顺序是：

```text
ga_erase_area(old_area)
  -> 如果左边有剩余，创建 area_l
  -> 如果右边有剩余，创建 area_r
  -> ga_free_area(old_area)
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:219-253]`

这里的 `ga_area` 是空闲区元数据；用户得到的地址本身不是通过复制得到的，而是从已经归属于 cache 的地址范围中划出的一段。

### 5.3 cache miss

如果 `svm_ga_alloc` 找不到足够大的空闲 area，返回 `DRV_ERROR_OUT_OF_MEMORY`。这时不是立刻失败，而是尝试扩展 cache：

```c
ret = _cache_malloc(ca, va, size);
while (ret == DRV_ERROR_OUT_OF_MEMORY) {
    expand_size = svm_align_up(
        svm_align_up(size, ca->strategy.expand_granularity),
        ca->strategy.alloc_gran);

    ret = cache_expand_once(ca, expand_size);
    if (ret != DRV_ERROR_NONE) {
        return ret;
    }

    ret = _cache_malloc(ca, va, size);
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:133-153]`
- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:234-251]`

完整过程是：

```text
1. 在已有 range 中查找空闲 area
2. 找不到，计算 expand_size
3. 用 normal allocator 获取新 backing
4. 将新 backing 加入 gen_allocator
5. 再次在 size_area_tree 中分配
```

扩展 backing：

```c
static int cache_expand(struct cache_allocator *ca, u64 size, u64 *start)
{
    u32 normal_flag = cache_flag_to_normal_flag(ca->flag);
    u64 align = ca->strategy.alloc_gran;
    return cache_malloc_raw(
        ca->devid,
        align,
        start,
        size,
        normal_flag);
}
```

`cache_malloc_raw` 调用：

```c
ret = svm_normal_malloc(
    devid,
    normal_flag,
    align,
    &va,
    size);
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:156-180]`

加入新 range：

```c
ret = cache_expand(ca, size, &start);
if (ret != DRV_ERROR_NONE) {
    return ret;
}

ret = svm_ga_add_range(
    ca->ga_inst,
    start,
    size);
if (ret != DRV_ERROR_NONE) {
    (void)cache_shrink(ca, start, size);
    return ret;
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:192-209]`

这里的含义是：

```text
新 backing 申请成功
  -> 还没有正式成为 cache 的一部分
  -> 必须成功加入 ga_inst
  -> 加入失败则释放刚申请的 backing
```

### 5.4 normal allocator 具体做什么

V3 normal allocator 不负责 cache 内部的切分；它负责从更低层获得一段普通内存：

```c
if ((flag & SVM_NORMAL_MALLOC_FLAG_POPULATE_ONLY) == 0) {
    ret = normal_va_alloc(
        devid, flag, align, &tmp_va, size);
    if (ret != DRV_ERROR_NONE) {
        return ret;
    }
}

if ((flag & SVM_NORMAL_MALLOC_FLAG_VA_ONLY) == 0) {
    ret = normal_mem_populate(
        devid, flag, tmp_va, size);
    if (ret != DRV_ERROR_NONE) {
        normal_va_free(
            devid, flag, align, tmp_va, size);
        return ret;
    }
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/normal_malloc/normal_malloc.c:127-149]`

因此 V3 的职责边界是：

```text
normal_malloc
  -> 获取新的 VA 和 backing

cache_malloc/gen_allocator
  -> 管理已经获得的 range
  -> 在 range 内部切分和复用
```

## 6. V3 释放和 shrink

### 6.1 free 只先归还给 cache

malloc manager 会记录分配是否来自 cache；释放时依据这个记录选择：

```text
is_from_cache == true
  -> free_cache

is_from_cache == false
  -> free_normal
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/malloc_mng/malloc_mng.c:520-527]`

cache free 首先执行：

```c
ret = svm_ga_free(ca->ga_inst, va, size);
if (ret == DRV_ERROR_NONE) {
    cache_free_stats_update(ca, size);
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:254-275]`

`svm_ga_free` 创建 free area 并尝试合并：

```c
area = ga_create_area(inst, range, addr, size);
ret = ga_try_merge_area(area);
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:623-668]`

### 6.2 相邻 area 合并

假设 free 前：

```text
[FREE 2 MiB][BUSY 4 MiB][FREE 6 MiB]
```

释放中间区域后：

```text
[FREE 2 MiB][FREE 4 MiB][FREE 6 MiB]
```

`ga_try_merge_area` 通过地址树找左右邻居：

```c
area_l = ga_range_get_area(
    range,
    area->start - 1,
    1);

area_r = ga_range_get_area(
    range,
    area->start + area->size,
    1);
```

然后删除左右旧 area，形成一个更大的 area，再重新插入索引：

```text
[FREE 12 MiB]
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:255-295]`

### 6.3 何时释放 backing

free 不代表 backing 立即释放。释放后会：

```text
更新 cache strategy
  -> 判断是否超过 shrink 条件
  -> 选择一个完全空闲的 range
  -> 从 gen_allocator 回收 range
  -> 调用 normal free
```

代码路径：

```c
cache_strategy_update(ca);

if (cache_try_shrink(ca)) {
    while (cache_should_shrink(ca)) {
        ret = cache_shrink_once(ca, &shrink_size);
        if ((ret != DRV_ERROR_NONE) &&
            (ret != DRV_ERROR_BUSY)) {
            break;
        }
    }
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:254-275]`

一次 shrink：

```c
ret = svm_ga_recycle_one_idle_range(
    ca->ga_inst,
    &start,
    &size);
```

这里的 `idle range` 是整个 range 内部都没有活动 area 的 backing range，而不是任意一块空闲 area。

然后：

```c
ret = cache_shrink(ca, start, size);
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:212-232]`

所以：

```text
range 部分空闲
  -> 继续留在 cache

range 全部空闲
  -> 才可能 shrink
```

### 6.4 `DRV_ERROR_BUSY` 的含义

如果底层释放返回 `DRV_ERROR_BUSY`，说明当前 backing 仍有底层异步使用或释放条件尚未满足。V3 不把它当成普通成功，也不简单丢弃该 range，而是：

```c
if (ret == DRV_ERROR_BUSY) {
    cache_recycle_add_seg(
        start,
        size,
        align,
        ca->devid,
        ca->flag);
}
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:212-232]`

这形成延迟回收路径：

```text
逻辑上决定 shrink
  -> 底层当前 BUSY
  -> 进入 recycle segment
  -> 等待后续条件满足再处理
```

当前用户态源码能够确认存在该机制，但不能仅凭已读代码断言底层异步事件何时完成。

## 7. V3 初始化、cache 类型和销毁

V3 不是所有 flag 共用一个 cache。初始化代码按 host/device 准备多个 cache flag：

```c
static u32 g_host_cache_flag[] = {
    0,
    SVM_CACHE_MALLOC_FLAG_MASTER_UVA
};

static u32 g_dev_cache_flag[] = {
    0,
    SVM_CACHE_MALLOC_FLAG_PA_P2P,
    SVM_CACHE_MALLOC_FLAG_PA_HPAGE,
    SVM_CACHE_MALLOC_FLAG_PA_HPAGE |
        SVM_CACHE_MALLOC_FLAG_PA_P2P
};
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_init.c:24-45]`

初始化过程：

```text
设备初始化完成
  -> cache_dev_init
  -> cache_create
  -> cache_get_support_flag
  -> 为每种支持的 flag 创建 cache_allocator
```

创建失败时销毁已经创建的前序实例：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_init.c:47-68]`

设备退出时：

```text
cache_dev_uninit
  -> cache_destroy
       -> 先 svm_cache_shrink
       -> 再 cache_allocator_destroy
```

证据：

- `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_init.c:71-107]`

这解释了为什么 `flag` 很重要：不同 backing 属性不能随意混放在同一 cache 中。

## 8. V2：把它看成“heap + 多棵索引树”

V2 的总体思想与 V3 相同，但数据结构组织方式不同：

```text
一个或多个 devmm heap
  -> heap 内维护多个 node
  -> 同一个 node 同时挂到多棵树
  -> 按 VA 找、按 size 找、按 mapped 状态找
```

V2 核心节点：

```c
struct devmm_node_data {
    uint64_t va;
    uint64_t size;
    uint64_t total;
    uint32_t flag;
    DVmem_advise advise;
};

struct devmm_rbtree_node {
    struct multi_rb_node va_node;
    struct multi_rb_node size_node;
    struct multi_rb_node cache_node;
    struct devmm_node_data data;
};
```

heap 中的索引包括：

```text
alloced_tree
  -> 已分配节点

idle_va_tree
  -> 按地址管理空闲区间

idle_size_tree
  -> 按大小管理未映射空闲区间

idle_mapped_cache_tree[RW/RO]
  -> 按大小管理已映射的空闲 cache 节点
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_rbtree/devmm_rbtree.h:36-64]`

## 9. V2 一次分配的完整过程

V2 HAL 入口：

```text
halMemAlloc
  -> halMemAllocInner
  -> devmm_alloc_proc
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_svm.c:4430-4471]`

heap list 分配优先级：

```c
if (advise_is_nocache(advise) == false) {
    ptr = devmm_alloc_from_trees(
        heap_list,
        bytesize,
        advise,
        DEVMM_IDLE_MAPPED_TREE,
        va);
    if (ptr_is_valid(ptr)) {
        return ptr;
    }
}

return devmm_alloc_from_trees(
    heap_list,
    bytesize,
    advise,
    DEVMM_IDLE_SIZE_TREE,
    va);
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_interface.c:1578-1591]`

也就是：

```text
非 nocache 请求
  -> 先复用已经 mapped 的 cache node
  -> 再尝试未映射的 idle size node
  -> 最后必要时新建 heap
```

按大小查找采用：

```c
rb_node = multi_rbtree_get(size, tree);
if (rb_node == NULL) {
    rb_node = multi_rbtree_get_upper_bound(size, tree);
}
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_rbtree/devmm_rbtree.c:104-138]`

因此 V2 也表现为按大小的 Best-Fit：先精确匹配，再找大于请求的最小 size key。

现有 heap 都找不到时，新建 heap：

```text
devmm_alloc_com_heap
  -> 获取空闲 heap 槽位
  -> 计算 heap start/size/page size/cache threshold
  -> 初始化 heap 和 rbtree
  -> 从新 heap 的 idle size tree 分配
  -> 加入 heap list
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_interface.c:1464-1517]`
- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_interface.c:1594-1619]`

## 10. V2 的 node 切分、释放和 shrink

mapped cache node 大于请求时，V2 会切分 node：

```text
原 mapped idle node：
[FREE 8 MiB]

请求 3 MiB 后：
[BUSY 3 MiB][MAPPED CACHE 5 MiB]
```

关键代码会：

```text
erase old mapped cache node
  -> 更新已分配部分大小
  -> 为剩余部分创建新 cache node
  -> 将剩余 node 插回 mapped cache tree
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:688-819]`

释放时，V2 根据原始总大小、当前大小、cache threshold、nocache 标记和 UVM 状态选择 cache 或 nocache：

```text
满足 nocache/超阈值条件
  -> devmm_free_nocache_mem_process

否则
  -> devmm_free_cache_mem_process
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1341-1391]`

cache free 会：

```text
合并相邻 mapped node
  -> 插回 idle_mapped_cache_tree
  -> 增加 cur_cache_mem
  -> 必要时尝试 shrink
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1151-1185]`
- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1244-1278]`

只有 cache 超过动态条件才 shrink：

```c
if ((heap->cur_cache_mem[memtype] <
     heap->cache_mem_thres[memtype] * 2ul) &&
    (heap->cur_cache_mem[memtype] <
     heap->cache_mem_thres[memtype] + 0x8000000ul)) {
    return false;
}
return true;
```

证据：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:556-564]`

shrink 需要找到完整空闲的 mapped node，并检查：

```text
node->data.total == node->data.size
```

即原始 backing node 没有被切成仍在使用的部分。然后通过：

```c
devmm_free_phymem_to_os(...)
```

归还底层物理内存：

- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:635-685]`
- `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1303-1328]`

## 11. V2 和 V3 的本质相同点与不同点

### 11.1 相同点

```text
请求到达 Driver
  -> 优先查找 cache
  -> 按大小寻找可用空间
  -> 命中则切分复用
  -> 未命中则向 normal/heap 获取 backing
  -> free 后保留在 cache
  -> 相邻空闲区域合并
  -> 完整 backing 空闲且达到条件才 shrink
```

这是一种“延迟归还底层、优先重复利用”的策略。

### 11.2 不同点

| 项目 | V2 | V3 |
|---|---|---|
| 管理单位 | heap + `devmm_rbtree_node` | `ga_range` + `ga_area` |
| 空间组织 | 每个 heap 内多棵树 | 多个不连续 range 的统一 allocator |
| 已映射 cache | 单独 RW/RO mapped tree | 不同 cache allocator 实例隔离 |
| 大小查找 | `idle_size_tree` / mapped tree | `size_area_tree` |
| 地址关系 | `idle_va_tree` 和 VA node | range 地址树和 area 地址树 |
| 扩展 | 新 heap 或 heap 内映射 | normal malloc 后 `ga_add_range` |
| shrink 单位 | 完整 mapped node/backing | 完全 idle 的 range |
| 忙释放 | 底层 heap/Driver 语义 | 明确进入 recycle segment |

可以用一句话概括：

> V2 是“以 heap 为中心、节点同时挂多棵树”；V3 是“以 cache allocator 为中心、把多个不连续 backing range 交给通用区间分配器”。

## 12. 最终端到端图

### V3 cache hit

```text
rtMalloc
  -> Runtime NpuDriver::DevMemAlloc
  -> halMemAlloc
  -> svm_mem_malloc
  -> svm_module_mem_malloc
  -> _svm_malloc
  -> go_malloc_cache == true
  -> malloc_cache
  -> cache_malloc
  -> svm_ga_alloc
  -> size_area_tree exact/upper-bound
  -> ga_try_slice_area
  -> 返回 cache 中的 VA
```

### V3 cache miss

```text
cache_malloc
  -> svm_ga_alloc 返回 OUT_OF_MEMORY
  -> 计算 expand_size
  -> cache_expand_once
  -> svm_normal_malloc
       -> normal_va_alloc
       -> normal_mem_populate
            -> MPL/底层 Driver
  -> svm_ga_add_range
  -> 重试 svm_ga_alloc
  -> 返回新 range 中切出的 VA
```

### V3 free

```text
rtFree
  -> halMemFree
  -> svm_mem_free
  -> svm_free
  -> _svm_free
  -> free_cache
  -> svm_ga_free
  -> ga_try_merge_area
  -> 更新 cache 统计
  -> 必要时 svm_ga_recycle_one_idle_range
  -> svm_normal_free
  或因 DRV_ERROR_BUSY 进入延迟 recycle
```

### V2

```text
rtMalloc
  -> halMemAlloc
  -> devmm_alloc_proc
  -> heap list
  -> idle_mapped_cache_tree
  -> idle_size_tree
  -> 新 heap（必要时）
  -> 切分 node
  -> 返回地址

rtFree
  -> devmm_free_managed
  -> cache/nocache 判断
  -> cache free：合并并插回 mapped tree
  -> 超阈值且完整空闲：devmm_free_phymem_to_os
```

## 13. 结论边界

### 已确认

- Driver V2/V3 都有普通内存 cache；
- V3 cache 是多个可动态增加的 backing range；
- V3 `gen_allocator` 负责 range/area 的地址和大小管理；
- V3 area 支持按大小查找、切分、相邻合并；
- V3 cache 不足时通过 normal allocator 扩展；
- V3 free 后可能保留 backing，并在满足条件时 shrink；
- V2 使用 heap 和多棵按 VA/size/mapped 状态组织的树；
- V2 小请求优先复用已映射 cache；
- V2 也采用 exact/upper-bound 的按大小查找；
- V2/V3 都不是每次 free 都立即归还底层。

### 只能谨慎表述

- V2/V3 的用户态空闲区选择行为可以称为按大小 Best-Fit；
- 同一 size key 内的二级选择顺序尚不能仅凭当前已列代码断言；
- 某个具体 `rtMalloc` 是否命中 cache 取决于运行时 flag、size、align、设备能力和当前 cache 状态。

### 未验证/未知

- 闭源内核、固件和硬件内部的物理页分配算法；
- 所有 SoC 产品的最终 cache flag 组合；
- `DRV_ERROR_BUSY` 对应底层异步事件的具体完成时机；
- cache 相对于直接分配的真实性能收益；
### 13.3 仍需谨慎解释的实现细节

以下结论是基于当前版本用户态源码的静态分析，不能扩大解释为内核或硬件的实现结论：

- `multi_rbtree_get` 在同一个 size key 下如何选取多个节点，当前专题尚未取得足够证据；
- `heap->ops->heap_alloc`、`devmm_free_phymem_to_os` 以下的 ioctl、内核和固件路径未在本专题中完整展开；
- V3 recycle segment 的后续处理、底层异步事件完成时机和最终释放时序仍未完全确认；
- 具体 SoC、page type、P2P/UVA/Huge Page flag 是否创建并使用某一 cache，必须结合产品配置和运行时环境确认；
- 尚未进行构建、硬件运行、压力测试、ASAN/TSAN 或性能基准，因此不能从源码直接推出 cache 命中率或性能收益。

## 14. 补充：V2/V3 行级走读的关键细节

前面的章节给出了主流程；本节把几个容易被忽略的源码动作串起来，便于按代码复现一次申请过程。

### 14.1 V3：请求是否进入 cache 的完整判定

V3 malloc manager 先对用户请求做大小对齐，然后调用 `go_malloc_cache`：

```c
ret = get_aligned_size(devid, flag, align, *size, &aligned_size);

go_cache = go_malloc_cache(
    devid, numa_id, flag, align, aligned_size);

if (go_cache) {
    ret = malloc_cache(devid, flag, align, aligned_size, start);
} else {
    ret = malloc_normal(devid, numa_id, flag, align, aligned_size, start);
}
```

证据：[driver/src/ascend_hal/svm/v3/assign/malloc_mng/malloc_mng.c:492-517]

`go_malloc_cache` 并不是只判断请求大小：

```c
u32 cache_flag = svm_flag_to_cache_flag(flag);
return ((numa_id == SVM_MALLOC_NUMA_NO_NODE) &&
        svm_flag_is_support_cache(flag) &&
        svm_cache_is_support(
            devid, cache_flag, align, (u32)size));
```

证据：[driver/src/ascend_hal/svm/v3/assign/malloc_mng/malloc_mng.c:394-399]

而 `svm_cache_is_support` 还要求：

```c
ca != NULL &&
size <= ca->strategy.alloc_thres &&
align == ca->strategy.alloc_gran
```

证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:278-284]

所以一次请求进入 V3 ordinary cache 至少要同时满足：

1. NUMA 参数是无指定节点；
2. flag 属于支持 cache 的类型；
3. 对应设备和 cache flag 已有 `cache_allocator`；
4. 对齐值等于该 cache 的分配粒度；
5. 对齐后的大小不超过该 cache 的请求阈值。

### 14.2 V3：cache miss 为什么会扩展而不是立即失败

`_cache_malloc` 先调用 `svm_ga_alloc`。在普通非固定地址模式下，`svm_ga_alloc` 获取 `ga_inst` 写锁，并转到 `ga_alloc_by_size`；后者从 `size_area_tree` 进行 exact/upper-bound 查找。证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:133-153]、[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:600-621]

如果当前所有 range 都没有满足请求的 free area，`DRV_ERROR_OUT_OF_MEMORY` 在此处首先表示“cache 地址空间暂时没有合适的 area”。cache malloc 随后按如下公式计算扩展大小：

```c
expand_size = svm_align_up(
    svm_align_up(size, ca->strategy.expand_granularity),
    ca->strategy.alloc_gran);
```

再执行：

```text
cache_expand_once
  -> cache_expand
  -> cache_malloc_raw
  -> svm_normal_malloc
  -> svm_ga_add_range
  -> 重新调用 _cache_malloc
```

证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:192-210]、[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:234-252]

因此 `expand_granularity = 2 MiB` 的准确含义是扩展请求的对齐粒度，不是“每次固定申请一个 2 MiB 池”。实际扩展大小还受本次请求大小和 `alloc_gran` 影响。

`cache_expand` 将 cache flag 转换成 normal flag，并调用 `svm_normal_malloc`：

```c
u32 normal_flag = cache_flag_to_normal_flag(ca->flag);
u64 align = ca->strategy.alloc_gran;
return cache_malloc_raw(ca->devid, align, start, size, normal_flag);
```

证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:156-180]

新 backing 只有在 `svm_ga_add_range` 成功后才正式归入 cache；加入失败会调用 `cache_shrink` 释放刚申请的 backing。这是扩展路径的回滚边界。证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:192-210]

### 14.3 V3：range、area 与统计量的生命周期

`ga_inst` 创建时初始化全局地址范围树和全局空闲 area 大小树：

```c
inst = svm_ua_calloc(1, sizeof(struct ga_inst));
inst->attr = *attr;
pthread_rwlock_init(&inst->rwlock, NULL);
rbtree_init(&inst->addr_range_tree);
rbtree_init(&inst->size_area_tree);
```

证据：[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:438-464]

加入一个 range 后，range 的初始空闲 area 会同时进入：

- 该 range 的地址树；
- `ga_inst` 的全局大小树。

分配时从大小树取 area，切分时从旧索引删除并重新插入剩余 area；释放时通过 range 地址树找到左右邻居并合并。这样同一块地址既能按大小分配，又能按地址合并。

V3 的 cache 统计分别反映不同层次：

```text
cache_expand_stats_update：total += size，idle += size
cache_malloc_stats_update：idle -= size，cur_alloced += size，并更新 peak
cache_free_stats_update：idle += size，cur_alloced -= size
cache_shrink_stats_update：total -= size，idle -= size
```

证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:69-118]

这也解释了为什么 free 后通常只是 `idle` 增加，而不是 `total` 立即减少：backing 仍归 cache 持有。

只有当：

```c
range->idle_area_size == range->size
```

时，range 才被认定为完全空闲。证据：[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:388-405]

随后 `svm_ga_recycle_one_idle_range` 获取 `ga_inst` 写锁，找到一个完全 idle 的 range，先从 allocator 元数据中销毁它，再由 `cache_shrink` 调用 `svm_normal_free`。证据：[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:408-422]、[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:514-530]、[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:212-232]

若底层返回 `DRV_ERROR_BUSY`，源码将该 range 加入 recycle segment；当前已读用户态代码能够确认延迟回收机制存在，但不能确认硬件事件何时完成。证据：[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:212-232]

### 14.4 V2：一次请求从 heap 到 node 的走读

V2 `devmm_alloc_mem` 先检查 heap 和请求大小，再按 `heap->chunk_size` 对齐：

```c
if ((heap == NULL) || (bytesize > heap->heap_size) || (bytesize == 0)) {
    return DRV_ERROR_INVALID_VALUE;
}
alloc_size = align_up(bytesize, heap->chunk_size);
```

随后同时持有 heap 读锁和 tree mutex，在同一临界区内选择 node 并修改树：

```c
pthread_rwlock_rdlock(&heap->heap_rw_lock);
pthread_mutex_lock(&heap->tree_lock);
node = devmm_alloc_mem_get_node(...);
```

证据：[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1095-1137]

`devmm_alloc_mem_get_node` 的选择顺序是：

```text
请求不超过 need_cache_thres
  -> idle_mapped_cache_tree
  -> 失败后 idle_va/idle_size tree
否则
  -> 直接 idle_va/idle_size tree
```

mapped node 和 idle size node 的大小查找都遵循 exact/upper-bound：

```text
相等 size key 优先
没有相等 key 时取最小的更大 size key
```

证据：[driver/src/ascend_hal/svm/v2/devmm/devmm_rbtree/devmm_rbtree.c:104-138]

如果 node 已映射，进入 `devmm_alloc_from_mapped_node`，只需更新树和元数据；如果 node 未映射，进入 `devmm_alloc_from_unmapped_node`，需要调用映射操作。

### 14.5 V2：mapped node 命中和 unmapped node 命中的差异

#### mapped cache 命中

假设 idle mapped node 为 8 MiB，请求为 3 MiB：

```text
idle_mapped_cache_tree：
  [VA, 8 MiB, MAPPED]

申请后：
  alloced_tree：             [VA, 3 MiB, BUSY]
  idle_mapped_cache_tree：   [VA+3 MiB, 5 MiB, MAPPED]
```

代码先从 mapped tree 删除旧 node；如果旧 node 大于请求，则更新剩余 node 并创建本次分配 node；相等时直接复用原 node。证据：[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:688-821]

#### unmapped idle node 命中

未映射路径先计算实际映射范围：

```c
map_size = devmm_alloc_get_map_size(
    heap, node, alloc_size, memtype);
remain_size = map_size - alloc_size;
```

之后从 `idle_size_tree` 和 `idle_va_tree` 删除原 node：

- 原 node 大于 `map_size`：把未使用尾部重新作为 idle unmapped node；
- 原 node 等于 `map_size`：可直接把原 node 作为 map node；
- 创建新元数据失败：返回 `DRV_ERROR_OUT_OF_MEMORY`；
- `devmm_map_node` 失败：释放新建元数据并返回错误。

映射成功后设置 `DEVMM_NODE_MAPPED_FLG`，必要时按用户请求大小和映射剩余部分分段，再更新 cache/module 统计并插入 `alloced_tree`。证据：[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:917-978]

这里的关键区别是：

```text
mapped cache 命中：已有 backing/mapping，主要做元数据切分
unmapped node 命中：已有 VA 空间，但仍要建立 mapping
```

V2 用户态 cache 因而同时承担“虚拟地址空闲区管理”和“已映射 backing 复用”两种职责。

### 14.6 V2：heap list 的扩展和并发保护

当已有 heap 无法满足请求时，V2 不会在无保护状态下直接创建 heap。heap list 路径先用 `list_lock` 读锁遍历已有 heap并记录 version；只有确认虚拟地址不足时，才切换到写锁，重新检查 version，必要时创建并加入新 heap。

整体形态是：

```text
读锁：并发尝试已有 heap
  -> 成功：直接返回
  -> 地址不足：升级写锁
写锁：检查 version
  -> 其他线程已扩展：重新尝试
  -> 仍不足：创建 heap、初始化 rbtree、加入 heap list
```

证据：[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_interface.c:1594-1655]

新 heap 的属性包括 heap 大小、页大小、kernel page size、cache threshold、映射大小以及是否受限等。证据：[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_interface.c:1464-1517]

### 14.7 产品构建决定 V2/V3

源码中的选择是编译期产品选择：

```cmake
if (${PRODUCT} STREQUAL ascend910B)
    add_subdirectory(v2)
elseif(${PRODUCT} STREQUAL ascend950)
    add_subdirectory(v3)
endif()
```

证据：[driver/src/ascend_hal/svm/CMakeLists.txt:11-15]

`driver/build.sh` 将产品参数映射为：

```text
--soc=ascend910b    -> PRODUCT=ascend910B -> V2
--soc=ascend910_93  -> PRODUCT=ascend910B -> V2
--soc=ascend950     -> PRODUCT=ascend950  -> V3
```

证据：[driver/build.sh:46-65]

因此 V2/V3 不是同一份已编译库中的普通运行时开关。当前工作树没有已生成的 `CMakeCache.txt`、`build.ninja` 或硬件运行信息，不能据此断言“当前环境实际使能 V2”或“实际使能 V3”。

## 15. 最终总结：如何阅读一次 Driver 池化申请

### V2

看到一次普通分配时，依次问：

1. Runtime 传下来的 flag/advise 是否允许 cache；
2. 当前 heap list 是否有可用 heap；
3. 小请求是否先命中 mapped cache tree；
4. 未命中时是否找到 idle unmapped node；
5. node 是 mapped 还是 unmapped；
6. 是否需要按请求切分 node；
7. 映射或底层释放失败时，元数据是否回滚；
8. free 后是否达到 cache shrink 条件。

### V3

看到一次普通分配时，依次问：

1. `go_malloc_cache` 是否因 NUMA、flag、size 或 align 条件返回 false；
2. 对应 `cache_allocator` 的 `ga_inst` 是否已有 range；
3. `size_area_tree` 是否找到 exact/upper-bound area；
4. 命中后是否通过 `ga_try_slice_area` 切分；
5. 未命中时扩展大小如何按 2 MiB 粒度和 alloc granularity 对齐；
6. normal malloc 得到的 backing 是否成功加入 `ga_inst`；
7. free 后 area 是否与邻接 area 合并；
8. 是否存在完全 idle range 且满足 shrink threshold；
9. 底层 free 返回 BUSY 时是否进入延迟 recycle。

一句话总结：

> V2 把 cache 组织成多个 heap 中的多棵状态索引树；V3 把每类 cache 组织成一个可伸缩的区间分配器，并将多个不连续 backing range 统一纳入管理。两者都优先复用用户态保留的空间，只有在没有合适空闲空间或满足 shrink 条件时才触碰更底层的分配/释放路径。

## 16. 本轮验证状态

已完成：

- 目标版本源码静态阅读；
- V2/V3 构建选择规则核对；
- V2/V3 cache 分配、切分、合并、释放和 shrink 代码路径核对；
- 文档中的源码证据路径和行号整理。

未完成：

- Driver/Runtime 构建；
- 单元测试、压力测试和 Sanitizer；
- 真实设备上的 cache hit/miss 统计；
- 硬件、闭源内核和固件内部行为确认；
- 性能基准与收益比较。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
