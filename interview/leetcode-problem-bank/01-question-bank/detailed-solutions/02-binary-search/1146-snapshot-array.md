# 1146. 快照数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/snapshot-array/
- 题目 slug：`snapshot-array`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.2 进阶
- 难度分：1771
- 外部题解来源：https://leetcode.cn/problems/snapshot-array/solutions/2756291/ji-lu-xiu-gai-li-shi-ha-xi-biao-er-fen-c-b1sh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[记录修改历史：哈希表+二分查找（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/snapshot-array/solutions/2756291/ji-lu-xiu-gai-li-shi-ha-xi-biao-er-fen-c-b1sh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-lu-xiu-gai-li-shi-ha-xi-biao-er-fen-c-b1sh`
- topic id：`2756291`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意解读

调用 $\texttt{snap}()$ 时，复制一份当前数组，作为「历史版本」。返回这是第几个历史版本（从 $0$ 开始）。

调用 $\texttt{get}(\textit{index}, \textit{snapId})$ 时，返回第 $\textit{snapId}$ 个历史版本的下标为 $\textit{index}$ 的元素值。

## 暴力？

每次调用 $\texttt{snap}()$，就复制一份数组，可以吗？

不行，最坏情况下，复制 $50000$ 次长为 $50000$ 的数组，会「超出内存限制」。

## 思路

假设每调用一次 $\texttt{set}$，就生成一个快照（复制一份数组）。仅仅是一个元素发生变化，就去复制整个数组，这太浪费了。

能否**不复制数组**呢？

换个视角，调用 $\texttt{set}(\textit{index}, \textit{val})$ 时，不去修改数组，而是往 $\textit{index}$ 的**历史修改记录**末尾添加一条数据：此时的快照编号和 $\textit{val}$。

举例说明：

- 在快照编号等于 $2$ 时，调用 $\texttt{set}(0, 6)$。
- 在快照编号等于 $3$ 时，调用 $\texttt{set}(0, 1)$。
- 在快照编号等于 $3$ 时，调用 $\texttt{set}(0, 7)$。
- 在快照编号等于 $5$ 时，调用 $\texttt{set}(0, 2)$。
- 这四次调用结束后，下标 $0$ 的历史修改记录 $\textit{history}[0] = [(2,6),(3,1),(3,7),(5,2)]$，每个数对中的第一个数为调用 $\texttt{set}$ 时的快照编号，第二个数为调用 $\texttt{set}$ 时传入的 $\textit{val}$。注意历史修改记录中的快照编号是**有序**的。

那么：

- 调用 $\texttt{get}(0, 4)$。由于历史修改记录中的快照编号是有序的，我们可以在 $\textit{history}[0]$ 中**二分查找**快照编号 $\le 4$ 的最后一条修改记录，即 $(3,7)$。修改记录中的 $\textit{val}=7$ 就是答案。
- 调用 $\texttt{get}(0, 1)$。在 $\textit{history}[0]$ 中，快照编号 $\le 1$ 的记录不存在，说明在快照编号 $\le 1$ 时，我们没有修改下标 $0$ 保存的元素，返回初始值 $0$。

关于二分查找的原理，请看视频讲解：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

对于 $\texttt{snap}()$，只需把当前快照编号加一（快照编号初始值为 $0$），返回加一前的快照编号。

```py [sol-Python3]
class SnapshotArray:
    def __init__(self, _: int):
        self.cur_snap_id = 0
        self.history = defaultdict(list)  # 每个 index 的历史修改记录

    def set(self, index: int, val: int) -> None:
        self.history[index].append((self.cur_snap_id, val))

    def snap(self) -> int:
        self.cur_snap_id += 1
        return self.cur_snap_id - 1

    def get(self, index: int, snap_id: int) -> int:
        # 找快照编号 <= snap_id 的最后一次修改记录
        # 等价于找快照编号 >= snap_id+1 的第一个修改记录，它的上一个就是答案
        j = bisect_left(self.history[index], (snap_id + 1,)) - 1
        return self.history[index][j][1] if j >= 0 else 0
```

```java [sol-Java]
class SnapshotArray {
    // 当前快照编号，初始值为 0
    private int curSnapId;

    // 每个 index 的历史修改记录
    private final Map<Integer, List<int[]>> history = new HashMap<>();

    public SnapshotArray(int length) {
    }

    public void set(int index, int val) {
        history.computeIfAbsent(index, k -> new ArrayList<>()).add(new int[]{curSnapId, val});
    }

    public int snap() {
        return curSnapId++;
    }

    public int get(int index, int snapId) {
        if (!history.containsKey(index)) {
            return 0;
        }
        List<int[]> h = history.get(index);
        int j = search(h, snapId);
        return j < 0 ? 0 : h.get(j)[1];
    }

    // 返回最大的下标 i，满足 h[i][0] <= x
    // 如果不存在则返回 -1
    private int search(List<int[]> h, int x) {
        // 开区间 (left, right)
        int left = -1;
        int right = h.size();
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // h[left][0] <= x
            // h[right][1] > x
            int mid = left + (right - left) / 2;
            if (h.get(mid)[0] <= x) {
                left = mid; // 区间缩小为 (mid, right)
            } else {
                right = mid; // 区间缩小为 (left, mid)
            }
        }
        // 根据循环不变量，此时 h[left][0] <= x 且 h[left+1][0] = h[right][0] > x
        // 所以 left 是最大的满足 h[left][0] <= x 的下标
        // 如果不存在，则 left 为其初始值 -1
        return left;
    }
}
```

```cpp [sol-C++]
class SnapshotArray {
    unordered_map<int, vector<pair<int, int>>> history; // 每个 index 的历史修改记录
    int cur_snap_id = 0;

public:
    SnapshotArray(int) {}

    void set(int index, int val) {
        history[index].emplace_back(cur_snap_id, val);
    }

    int snap() {
        return cur_snap_id++;
    }

    int get(int index, int snap_id) {
        auto& h = history[index];
        // 找快照编号 <= snap_id 的最后一次修改记录
        // 等价于找快照编号 >= snap_id+1 的第一个修改记录，它的上一个就是答案
        int j = ranges::lower_bound(h, pair(snap_id + 1, 0)) - h.begin() - 1;
        return j >= 0 ? h[j].second : 0;
    }
};
```

```go [sol-Go]
type pair struct{ snapId, val int }

type SnapshotArray struct {
    curSnapId int
    history   map[int][]pair // 每个 index 的历史修改记录
}

func Constructor(int) SnapshotArray {
    return SnapshotArray{history: map[int][]pair{}}
}

func (sa *SnapshotArray) Set(index, val int) {
    sa.history[index] = append(sa.history[index], pair{sa.curSnapId, val})
}

func (sa *SnapshotArray) Snap() int {
    sa.curSnapId++
    return sa.curSnapId - 1
}

func (sa *SnapshotArray) Get(index, snapId int) int {
    h := sa.history[index]
    // 找快照编号 <= snapId 的最后一次修改记录
    // 等价于找快照编号 >= snapId+1 的第一个修改记录，它的上一个就是答案
    j := sort.Search(len(h), func(j int) bool { return h[j].snapId >= snapId+1 }) - 1
    if j >= 0 {
        return h[j].val
    }
    return 0
}
```

```js [sol-JavaScript]
class SnapshotArray {
    constructor(length) {
        this.curSnapId = 0;
        this.history = new Map(); // 每个 index 的历史修改记录
    }

    set(index, val) {
        if (!this.history.has(index)) {
            this.history.set(index, []);
        }
        this.history.get(index).push([this.curSnapId, val]);
    }

    snap() {
        return this.curSnapId++;
    }

    get(index, snapId) {
        if (!this.history.has(index)) {
            return 0;
        }
        const h = this.history.get(index);
        const j = this.search(h, snapId);
        return j < 0 ? 0 : h[j][1];
    }

    // 返回最大的下标 i，满足 h[i][0] <= x
    // 如果不存在则返回 -1
    search(h, x) {
        // 开区间 (left, right)
        let left = -1;
        let right = h.length;
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // h[left][0] <= x
            // h[right][1] > x
            const mid = Math.floor((left + right) / 2);
            if (h[mid][0] <= x) {
                left = mid; // 区间缩小为 (mid, right)
            } else {
                right = mid; // 区间缩小为 (left, mid)
            }
        }
        // 根据循环不变量，此时 h[left][0] <= x 且 h[left+1][0] = h[right][0] > x
        // 所以 left 是最大的满足 h[left][0] <= x 的下标
        // 如果不存在，则 left 为其初始值 -1
        return left;
    }
}
```

```rust [sol-Rust]
use std::collections::HashMap;

struct SnapshotArray {
    cur_snap_id: i32,
    history: HashMap<i32, Vec<(i32, i32)>>, // 每个 index 的历史修改记录
}

impl SnapshotArray {
    fn new(_: i32) -> Self {
        Self {
            cur_snap_id: 0,
            history: HashMap::new(),
        }
    }

    fn set(&mut self, index: i32, val: i32) {
        self.history.entry(index).or_default().push((self.cur_snap_id, val));
    }

    fn snap(&mut self) -> i32 {
        self.cur_snap_id += 1;
        self.cur_snap_id - 1
    }

    fn get(&self, index: i32, snap_id: i32) -> i32 {
        if let Some(h) = self.history.get(&index) {
            let j = h.partition_point(|&(id, _)| id <= snap_id);
            if j > 0 {
                return h[j - 1].1;
            }
        }
        0
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化、$\texttt{set}$、$\texttt{snap}$ 均为 $\mathcal{O}(1)$，$\texttt{get}$ 为 $\mathcal{O}(\log q)$，其中 $q$ 为 $\texttt{set}$ 的调用次数。
- 空间复杂度：$\mathcal{O}(q)$。

## 相似题目

- [981. 基于时间的键值存储](https://leetcode.cn/problems/time-based-key-value-store/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、二分查找 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `一、二分查找 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
