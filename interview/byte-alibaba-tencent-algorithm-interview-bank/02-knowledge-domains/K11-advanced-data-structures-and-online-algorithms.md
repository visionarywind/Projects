# K11 高级数据结构与在线算法

## 核心问题

- **缓存淘汰设计**：LRU / LFU 要求 `get` 与 `put` 均摊 `O(1)`，核心是"哈希表 + 有序链"的组合。
- **单调结构**：单调栈求下一个更大/更小元素，单调队列维护滑动窗口最值。
- **跳表**：以多层索引实现平均 `O(log n)` 的有序查找与插入，是平衡树的随机化替代。
- **线段树 / 树状数组**：支持区间查询与点/区间更新的两类经典结构。
- **在线算法**：输入逐步到达、必须立即回答的场景，与允许先收集全部数据再处理的离线算法相对。

学习目标是能识别"需要频繁查询/更新"的结构需求、选择合适的数据结构、论证复杂度并写出边界正确的 C++17 实现。

## 训练顺序

1. 先用 LRU 打通"哈希表 + 双向链表"的 O(1) 设计范式，再做 LFU（频率桶 + 分层链表）。
2. 再练单调栈/单调队列，理解"弹出无用元素"的均摊分析。
3. 然后做树状数组（点更新 + 前缀和）与线段树（区间更新 + 区间查询 + 懒标记）。
4. 最后理解在线与离线的取舍，并了解跳表的随机化平衡思想。

## 核心模式

### 1. LRU 缓存（哈希表 + 双向链表）

```cpp
class LRUCache {
    struct Node { int key, value; Node *prev, *next; };
    int cap_;
    Node head_, tail_;                          // 哨兵节点，消除空指针分支
    unordered_map<int, Node*> pos_;
    int size_ = 0;

    void detach(Node* p) { p->prev->next = p->next; p->next->prev = p->prev; }
    void pushFront(Node* p) {
        p->next = head_.next; p->prev = &head_;
        head_.next->prev = p; head_.next = p;
    }
    void moveToFront(Node* p) { detach(p); pushFront(p); }

public:
    explicit LRUCache(int capacity) : cap_(capacity) {
        head_.next = &tail_;
        tail_.prev = &head_;
        pos_.reserve(capacity * 2);
    }
    int get(int key) {
        auto it = pos_.find(key);
        if (it == pos_.end()) return -1;
        moveToFront(it->second);                // get 会改变顺序
        return it->second->value;
    }
    void put(int key, int value) {
        if (cap_ <= 0) return;                  // 容量为 0，不缓存任何键
        auto it = pos_.find(key);
        if (it != pos_.end()) { it->second->value = value; moveToFront(it->second); return; }
        if ((int)size_ == cap_) {               // 容量满，淘汰尾部最旧
            Node* victim = tail_.prev;
            pos_.erase(victim->key);
            detach(victim);
            delete victim;
            --size_;
        }
        Node* node = new Node{key, value, nullptr, nullptr};
        pos_[key] = node;
        pushFront(node);
        ++size_;
    }
};
```

不变量：链表从 `head` 到 `tail` 恰为"最近使用 → 最久未使用"的顺序；哈希表键集合与链表节点集合一致。复杂度：`get`/`put` 均摊 `O(1)`，空间 `O(capacity)`。

> 关键：LRU 只查询顺序的**两端**（移到头、取最旧），因此可绕过"动态有序集合 Ω(log n)"下界；若需要任意名次查询，则必须用平衡树/跳表。

### 2. 单调栈（下一个更大元素）

```cpp
vector<int> nextGreater(const vector<int>& a) {
    int n = (int)a.size();
    vector<int> res(n, -1);
    vector<int> st;                             // 存下标，对应值单调递减
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && a[st.back()] < a[i]) {
            res[st.back()] = a[i];              // i 就是栈顶的下一个更大元素
            st.pop_back();
        }
        st.push_back(i);
    }
    return res;
}
```

不变量：栈内下标递增、对应值递减。复杂度：每个元素入栈出栈各一次，`O(n)` 时间、`O(n)` 空间。

### 3. 单调队列（滑动窗口最大值）

```cpp
vector<int> maxSlidingWindow(const vector<int>& a, int k) {
    deque<int> dq;                              // 存下标，对应值单调递减
    vector<int> res;
    for (int i = 0; i < (int)a.size(); ++i) {
        while (!dq.empty() && dq.front() <= i - k) dq.pop_front();  // 过期出队
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();  // 维持单调
        dq.push_back(i);
        if (i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}
```

不变量：`dq` 的队首是当前窗口最大值。复杂度：`O(n)` 时间、`O(k)` 空间。

### 4. 树状数组 Fenwick（点更新 + 前缀和）

```cpp
struct Fenwick {
    int n;
    vector<long long> tree;
    explicit Fenwick(int n) : n(n), tree(n + 1, 0) {}
    void add(int i, long long v) {              // i 从 1 开始
        for (; i <= n; i += i & (-i)) tree[i] += v;
    }
    long long sum(int i) const {                // 前缀和 [1..i]
        long long s = 0;
        for (; i > 0; i -= i & (-i)) s += tree[i];
        return s;
    }
    long long rangeSum(int l, int r) const { return sum(r) - sum(l - 1); }
};
```

不变量：`tree[i]` 覆盖区间 `(i - lowbit(i), i]` 的和。复杂度：`add` 与 `sum` 均为 `O(log n)`，空间 `O(n)`。可推广到区间加 + 单点查（差分技巧）与区间加 + 区间和（双树状数组）。

### 5. 线段树（区间更新 + 区间查询 + 懒标记）

```cpp
struct SegTree {
    int n;
    vector<long long> tree, lazy;
    SegTree(int n) : n(n), tree(4 * n, 0), lazy(4 * n, 0) {}
    void apply(int node, int l, int r, long long v) {
        tree[node] += v * (r - l + 1);
        lazy[node] += v;
    }
    void push(int node, int l, int r) {
        if (lazy[node] == 0) return;
        int mid = (l + r) / 2;
        apply(node * 2, l, mid, lazy[node]);
        apply(node * 2 + 1, mid + 1, r, lazy[node]);
        lazy[node] = 0;
    }
    void update(int node, int l, int r, int ql, int qr, long long v) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { apply(node, l, r, v); return; }
        push(node, l, r);
        int mid = (l + r) / 2;
        update(node * 2, l, mid, ql, qr, v);
        update(node * 2 + 1, mid + 1, r, ql, qr, v);
        tree[node] = tree[node * 2] + tree[node * 2 + 1];
    }
    long long query(int node, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return tree[node];
        push(node, l, r);
        int mid = (l + r) / 2;
        return query(node * 2, l, mid, ql, qr) + query(node * 2 + 1, mid + 1, r, ql, qr);
    }
};
```

不变量：每个节点的 `tree` 值等于其覆盖区间的聚合值（已计入自身 `lazy`）。复杂度：建树 `O(n)`，单次更新/查询 `O(log n)`，空间 `O(n)`（数组开 `4n`）。

### 6. 跳表（Skip List，思想 + 结构）

跳表用多层有序链表实现期望 `O(log n)` 的查找、插入、删除：

- 底层是完整有序链；每上一层以概率 `p`（常用 `1/2`）保留节点，形成稀疏索引。
- 查找从最高层开始向右走，遇到更大元素就下降一层，期望比较次数 `O(log n)`。
- 插入时随机决定新节点的层数，`O(log n)` 期望时间。
- 相比平衡树：实现简单、无需旋转，但依赖随机化，最坏 `O(n)`（概率极低）。

在 C++ 面试中通常要求说清"多层索引 + 随机层数"的思想与期望复杂度，鲜少要求完整手写。

## 常见陷阱

- **LRU 的 `get` 忘记刷新顺序**：`get` 是写操作，命中后必须移到头部，否则淘汰对象错误。
- **LRU 节点生命周期**：`delete` 与哈希表 `erase` 的先后顺序；指针悬挂或重复释放。
- **忘记哨兵**：头尾插入删除时的空指针分支易漏，哨兵节点可消除这类边界。
- **树状数组用 0 下标**：`lowbit(0)=0` 会死循环，必须 1-based。
- **线段树数组开小**：递归线段树数组需 `4n`，开 `2n` 会越界。
- **懒标记未下推**：`query`/`update` 递归子区间前必须 `push`，否则子节点读到过期值。
- **单调栈方向搞反**：求"下一个更大"用递减栈，求"下一个更小"用递增栈，比较符号写反结果全错。
- **单调队列过期判断**：`dq.front() <= i - k` 用 `<=` 保证窗口恰含 `k` 个元素；写成 `<` 会多留一个。
- **在线/离线混淆**：在必须立即回答的在线场景用了需要全局信息的离线算法（如排序后再处理），逻辑不成立。
- **随机化结构最坏情况**：跳表、哈希表都期望快但最坏可达 `O(n)`，涉及最坏保证的题目需改平衡树或补充论证。

## 复杂度速查

| 结构 / 操作 | 时间 | 空间 | 备注 |
|---|---|---|---|
| LRU `get`/`put` | 均摊 `O(1)` | `O(capacity)` | 哈希 + 双向链表 |
| LFU `get`/`put` | 均摊 `O(1)` | `O(capacity)` | 频率桶 + 分层链表 |
| 单调栈 | `O(n)` | `O(n)` | 下一个更大/更小 |
| 单调队列 | `O(n)` | `O(k)` | 滑动窗口最值 |
| 跳表 查找/插入/删除 | 期望 `O(log n)` | 期望 `O(n)` | 随机层数，最坏 `O(n)` |
| 树状数组 更新/查询 | `O(log n)` | `O(n)` | 点更新 + 前缀和 |
| 线段树 区间更新/查询 | `O(log n)` | `O(4n)` | 带懒标记 |
| 平衡 BST 查找/插入 | `O(log n)` | `O(n)` | 最坏保证 |

说明：均摊分析中，单调结构的总代价由"每个元素至多入出一次"保证。

## 题目索引

本域对应的真实题目文件：

- `../01-question-bank/expert/E001-lru-cache.md` —— LRU 的 O(1) 设计、手写链表与 `std::list` 两种实现、下界论证与并发/LFU/TTL 追问，是 K11 的核心综合题。

延伸方向（题面变体，非独立文件）：LFU 缓存（频率分层）、最小栈（单调结构）、滑动窗口最大值（单调队列）、区间和检索（树状数组）、设计跳表，可在覆盖矩阵 `../00-overview/coverage-matrix.md` 的 `K11` 条目下继续补题。
