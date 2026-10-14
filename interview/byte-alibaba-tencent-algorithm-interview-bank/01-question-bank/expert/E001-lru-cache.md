# E001. LRU 缓存

- 难度：专家级
- 知识域：K11 高级数据结构与在线算法
- 标签：哈希表、双向链表、缓存淘汰、O(1) 设计、并发
- 公司标签：通用、字节、阿里、腾讯 `[推断]`
- 面试频率：高频 `[推断]`
- 前置题：无（本题为设计类专题，自包含）
- 关联题：E002（数据流 TopK 高频元素）、E003（全排列）
- 事实状态：`[已确认]` LRU 的 O(1) 设计与复杂度下界论证；`[推断]` 公司训练标签；`[待验证]` 并发版本与 STL 版本未在目标 OJ 逐例运行

## 1. 题目与约束

设计一个满足 LRU（Least Recently Used，最近最少使用）淘汰策略的缓存，支持：

- `int get(int key)`：键存在时返回其值，并把该键标记为最近使用；否则返回 `-1`；
- `void put(int key, int value)`：键存在时更新值并标记为最近使用；否则插入；若插入后容量超过 `capacity`，淘汰最久未使用的键。

约束（专家级设定）：`1 <= capacity <= 10^6`；操作总次数 `q <= 2·10^6`；`key`、`value` 为 32 位整数；**要求 get 与 put 均摊 O(1)**。

扩展约束：多线程下要求线程安全；内存受限设备上要求避免逐操作动态分配；支持 TTL 过期；淘汰策略可替换为 LFU。

关键点：`get` 会改变“最近使用”的顺序，因此它并不是一个只读操作，不能只用一个哈希表实现。

## 2. 面试思路

先暴露朴素解：用“值 + 时间戳”的哈希表，淘汰时线性扫描找最小时间戳，单次淘汰 O(n)，总代价 O(n·q)，不可接受。

优化切入点：把“查询某个键”和“维护最久未使用键”拆成两个子问题。

- 查询：哈希表，平均 O(1)；
- 维护全局新旧次序：需要一个支持“把任意元素移到最新端”“删除最旧端”的结构。双向链表在持有节点指针时可以 O(1) 完成这两件事。

设计决策清单（面试时主动说出）：

1. 用**双向**而非单向链表：删除任意节点需要前驱指针。
2. 用**哨兵头尾节点**：消除在头/尾插入删除时的空指针分支。
3. 手写节点 vs `std::list`：见第 4、5 节的定量对比。
4. `get` 命中后必须把节点移到头部，否则淘汰顺序错误。

## 3. 算法与正确性

数据结构：哈希表 `pos`：`key -> Node*`；双向链表按“最近使用”排序，`head` 侧最新，`tail` 侧最旧（两端均用哨兵节点）。

- `get(key)`：`pos.find(key)`；命中则 `moveToFront(node)` 并返回 `value`，否则返回 `-1`。
- `put(key,value)`：命中则更新 `value` 并 `moveToFront`；未命中且已满则取 `tail->prev`（最旧节点），从哈希表与链表中删除；再插入新节点到头部。

**结构不变量**：任意时刻，链表中的节点集合与哈希表的键集合相同；链表从 `head` 到 `tail` 恰好是这些键按“最近使用时间”从新到旧的顺序；哈希表的每个键指向链表中的唯一节点。

正确性：

- 若 `get/put` 命中，先做 `moveToFront`，恢复“命中键最新”的不变量。
- 若需淘汰，`tail->prev` 依不变量正是最久未使用的键，删除它符合 LRU 定义，不违反任何约束。
- 插入后链表与哈希表同步更新，不变量保持不变。

## 4. C++17 实现

手写双向链表 + 复用被淘汰节点，容量稳定后零逐操作分配：

```cpp
#include <bits/stdc++.h>
using namespace std;

class LRUCache {
    struct Node {
        int key = 0, value = 0;
        Node *prev = nullptr, *next = nullptr;
    };

    int cap_;
    size_t size_ = 0;
    Node head_, tail_;                       // 哨兵
    unordered_map<int, Node*> pos_;

    void detach(Node* p) {                   // 从链表摘除
        p->prev->next = p->next;
        p->next->prev = p->prev;
    }
    void pushFront(Node* p) {                // 插入头部
        p->next = head_.next;
        p->prev = &head_;
        head_.next->prev = p;
        head_.next = p;
    }
    void moveToFront(Node* p) { detach(p); pushFront(p); }

public:
    explicit LRUCache(int capacity) : cap_(capacity) {
        head_.next = &tail_;
        tail_.prev = &head_;
        pos_.reserve(capacity * 2);
        pos_.max_load_factor(0.7f);
    }

    int get(int key) {
        auto it = pos_.find(key);
        if (it == pos_.end()) return -1;
        moveToFront(it->second);
        return it->second->value;
    }

    void put(int key, int value) {
        if (cap_ <= 0) return;
        auto it = pos_.find(key);
        if (it != pos_.end()) {
            it->second->value = value;       // 先复制值，再移动位置
            moveToFront(it->second);
            return;
        }
        Node* victim = nullptr;
        if ((int)size_ == cap_) {
            victim = tail_.prev;             // 最久未使用
            pos_.erase(victim->key);
            detach(victim);
        }
        Node* node;
        if (victim) {                        // 复用被淘汰节点
            victim->key = key;
            victim->value = value;
            node = victim;
        } else {
            node = new Node{key, value, nullptr, nullptr};
            ++size_;
        }
        pos_[key] = node;
        pushFront(node);
    }
};
```

`std::list` 等价版本：

```cpp
class LRUCacheList {
public:
    explicit LRUCacheList(int capacity) : cap_(capacity) {}

    int get(int key) {
        auto it = pos_.find(key);
        if (it == pos_.end()) return -1;
        lst_.splice(lst_.begin(), lst_, it->second);   // 移到头部 O(1)
        return it->second->second;
    }

    void put(int key, int value) {
        if (cap_ <= 0) return;
        auto it = pos_.find(key);
        if (it != pos_.end()) {
            it->second->second = value;
            lst_.splice(lst_.begin(), lst_, it->second);
            return;
        }
        if ((int)lst_.size() == cap_) {
            pos_.erase(lst_.back().first);
            lst_.pop_back();
        }
        lst_.emplace_front(key, value);
        pos_[key] = lst_.begin();
    }

private:
    int cap_;
    list<pair<int,int>> lst_;                                    // front = 最新
    unordered_map<int, list<pair<int,int>>::iterator> pos_;
};
```

`std::list::splice` 是 O(1) 且不失效迭代器，因此 `splice(lst_.begin(), lst_, it->second)` 就是“移到头部”。代价是每次插入/淘汰触发一次分配/释放。

## 5. 复杂度

- 时间：`get`、`put` 均摊 O(1)。哈希表查找平均 O(1)，链表的移动/插入/删除 O(1)。最坏情况下哈希全部冲突退化为 O(n)，工程上用 `reserve` 与合适哈希控制。
- 空间：O(capacity)。每个节点约 2 个指针加 `key/value`；`unordered_map` 的桶另有指针开销。

**下界论证**：为什么能突破“有序结构 O(log n)”的印象？LRU 只查询全局次序的**两端**（移到头、取最旧），并不需要任意名次查询或区间查询。链表在持有节点指针时，端点操作是常数时间的；哈希表负责提供这个指针。因此 O(1) 并不违反比较模型下“动态有序集合”的 Ω(log n) 下界——那些下界针对的是“支持任意次序查询”的操作集合，而这里把这一昂贵需求从问题里剔除了。

方案定量对比：

| 方案 | get/put | 每键额外内存量级 | 优点 | 缺点 |
|---|---|---|---|---|
| 手写链表 + 复用节点 | O(1) | 2 个指针 | 分配次数≈0，局部性较好 | 需手写，易错 |
| `std::list` | O(1) 均摊 | 2 指针 + 分配头 | 代码短、迭代器稳定 | 频繁 malloc/free |
| 哈希表 + 时间戳 | 淘汰 O(n) | 时间戳 4 字节 | 实现最简 | 淘汰退化，不适合大 q |
| 按时间排序的平衡树 | O(log n) | 指针 + 颜色位 | 支持按时间范围查询 | 慢一个 log |

表中的“指针/字节”是按 64 位平台常见大小的量级描述 `[已确认]`，具体数值与实现和分配器相关。

## 6. 例子推演

`capacity=2`，操作序列 `put(1,1); put(2,2); get(1); put(3,3); get(2);`：

| 操作 | 命中 | 动作 | 链表（新→旧） | 返回 |
|---|---|---|---|---|
| put(1,1) | 否 | 插入 1 | 1 | - |
| put(2,2) | 否 | 插入 2 | 2, 1 | - |
| get(1) | 是 | 1 移到头部 | 1, 2 | 1 |
| put(3,3) | 否 | 已满，淘汰尾 2，插入 3 | 3, 1 | - |
| get(2) | 否 | - | 3, 1 | -1 |

结果符合 LRU：`get(2)` 返回 `-1`，因为 2 是最近最少使用的。

## 7. 测试与调试

- `capacity=1`：反复 `put` 不同键，验证只剩最后一个；`get` 旧键返回 `-1`。
- 覆盖已存在键：`put(k,v1)` 后 `put(k,v2)`，容量不增长，值为 `v2`，且顺序刷新。
- `get` 不应淘汰：命中已存在的最旧键应把它提升，而非删除。
- 随机差分测试：用一个“哈希表 + 时间戳 + 遍历取最小”的暴力实现作 oracle，随机生成操作序列，比对每个 `get` 的返回值。
- 内存测试：容量固定后统计 `new/delete` 次数，验证复用方案在被淘汰节点重新利用后分配次数不再增长。
- 调试切入点：每次操作后打印链表顺序与 `pos` 大小，检查二者键集合一致（不变量）。

## 8. 面试追问

1. **如何实现 LFU（最少使用频率淘汰）？** 需要“频率 + 同频率内 LRU”，可用 `key->节点` 加“频率桶 → 双向链表”，并维护当前最小频率指针，做到 O(1)；容量满时淘汰最小频率桶的尾节点。难点是频率更新时跨桶迁移。
2. **如何加入 TTL？** 每个节点记录过期时间；需要快速找到最早过期者，可再加一个按过期时间排序的结构（最小堆或时间轮）。要区分“访问不刷新过期时间”与“写入刷新过期时间”两种语义。
3. **多线程下如何加锁？** 粗粒度单锁实现简单，但 `get` 会写链表，无法简单用读锁，读操作也被串行化。方案有：分片（shard）按 key 哈希拆成若干独立 LRU，各持一把锁，以牺牲全局精确 LRU 换并行度；或改用近似算法（CLOCK/second-chance）避免 `get` 写链表，从而读可用共享锁。必须说明一致性弱化带来的残余风险。

## 9. 易错点与反例

- **反例**：`put` 命中已存在键时忘记 `moveToFront`。`cap=2` 时 `put(1,1); put(2,2); put(1,9); put(3,3)` 若不刷新顺序，会错误淘汰 1 而不是 2。
- **易错**：删除节点未用哨兵，删除头/尾节点时空指针崩溃。
- **易错**：先插入再判断容量。若插入后才淘汰，`capacity=1` 时会把自己的新节点淘汰掉。
- **易错**：`get` 返回节点内的值指针，淘汰后节点被复用导致读到脏数据；应先复制 `value` 再移动位置。
- **易错**：`std::list` 版本保存了迭代器，却对 `lst_` 做整体赋值，导致迭代器全部失效。

## 10. 变体与迁移

- 结构迁移：LRU 的“哈希表 + 有序链”骨架可迁移到 O(1) 的 LFU，以及“O(1) 插入/删除/随机访问”类结构设计题。
- 策略迁移：CLOCK / second-chance 用环形缓冲区加引用位近似 LRU，避免每次访问移动节点，适合高并发只读场景。
- 约束迁移：分片 LRU、W-TinyLFU 在容量与命中率之间权衡；一旦引入 TTL 或 LFU，O(1) 结论需重新论证。
- 在线迁移：与 E002 的“数据流”思路相通——LRU 本身就是在没有未来信息时对“将来是否还会用”的在线预测。

**证据与来源：** `[已确认]` 哈希表加双向链表实现 O(1) LRU 的经典方案与复杂度（公开经典算法资料）、`std::list::splice` 在 C++17 下为常数复杂度且不失效迭代器；`[已确认]` 比较模型下动态有序集合下界的适用范围，以及 LRU 只依赖端点操作因而可绕过该下界的论证；`[推断]` 公司训练标签来自公开面经归纳；手写版、STL 版、并发版均未在目标 OJ 与多线程压测下逐例运行，标记 `[待验证]`。
