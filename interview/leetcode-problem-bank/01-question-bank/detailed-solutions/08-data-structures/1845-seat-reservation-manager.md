# 1845. 座位预约管理系统

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/seat-reservation-manager/
- 题目 slug：`seat-reservation-manager`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1429
- 外部题解来源：https://leetcode.cn/problems/seat-reservation-manager/solutions/2838121/liang-chong-fang-fa-wei-hu-ke-yu-yue-de-tmub8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：维护可预约的座位/维护取消预约的座位（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/seat-reservation-manager/solutions/2838121/liang-chong-fang-fa-wei-hu-ke-yu-yue-de-tmub8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-wei-hu-ke-yu-yue-de-tmub8`
- topic id：`2838121`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：维护可预约的座位

我们需要一个数据结构维护可预约的座位：

- $\texttt{reserve}$：查找并删除最小值。
- $\texttt{unreserve}$：添加元素。

**最小堆**完美符合上述要求：

- 初始化：把 $1,2,3,\cdots,n$ 全部入堆。
- $\texttt{reserve}$：弹出并返回堆顶。
- $\texttt{unreserve}$：把 $\textit{seatNumber}$ 入堆。

```py [sol-Python3]
class SeatManager:
    def __init__(self, n: int):
        self.available = list(range(1, n + 1))  # 有序数组无需堆化

    def reserve(self) -> int:
        return heappop(self.available)

    def unreserve(self, seatNumber: int) -> None:
        heappush(self.available, seatNumber)
```

```java [sol-Java]
class SeatManager {
    private final PriorityQueue<Integer> available = new PriorityQueue<>();

    public SeatManager(int n) {
        for (int i = 1; i <= n; i++) {
            available.add(i);
        }
    }

    public int reserve() {
        return available.poll();
    }

    public void unreserve(int seatNumber) {
        available.add(seatNumber);
    }
}
```

```cpp [sol-C++]
class SeatManager {
    priority_queue<int, vector<int>, greater<>> available;

public:
    SeatManager(int n) {
        for (int i = 1; i <= n; i++) {
            available.push(i);
        }
    }

    int reserve() {
        int seatNumber = available.top();
        available.pop();
        return seatNumber;
    }

    void unreserve(int seatNumber) {
        available.push(seatNumber);
    }
};
```

```go [sol-Go]
type SeatManager struct {
    sort.IntSlice // 继承 Len, Less, Swap
}

func Constructor(n int) SeatManager {
    m := SeatManager{make([]int, n)}
    for i := range m.IntSlice {
        m.IntSlice[i] = i + 1
    }
    // 有序数组无需堆化
    return m
}

func (m *SeatManager) Reserve() int {
    return heap.Pop(m).(int)
}

func (m *SeatManager) Unreserve(seatNumber int) {
    heap.Push(m, seatNumber)
}

func (m *SeatManager) Push(v any) { m.IntSlice = append(m.IntSlice, v.(int)) }
func (m *SeatManager) Pop() any   { a := m.IntSlice; v := a[len(a)-1]; m.IntSlice = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var SeatManager = function(n) {
    this.available = new MinPriorityQueue();
    for (let i = 1; i <= n; i++) {
        this.available.enqueue(i);
    }
};

SeatManager.prototype.reserve = function() {
    return this.available.dequeue();
};

SeatManager.prototype.unreserve = function(seatNumber) {
    this.available.enqueue(seatNumber);
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

struct SeatManager {
    available: BinaryHeap<i32>,
}

impl SeatManager {
    fn new(n: i32) -> Self {
        let mut available = BinaryHeap::new();
        for i in 1..=n {
            available.push(-i); // 取相反数，变成最小堆
        }
        Self { available }
    }

    fn reserve(&mut self) -> i32 {
        -self.available.pop().unwrap()
    }

    fn unreserve(&mut self, seat_number: i32) {
        self.available.push(-seat_number);
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化为 $\mathcal{O}(n)$ 或 $\mathcal{O}(n\log n)$，取决于实现。$\texttt{reserve}$ 和 $\texttt{unreserve}$ 均为 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：维护取消预约的座位

方法一的时空复杂度均和 $n$ 有关，如果把数据范围改成 $n\le 10^9$，就会超时/爆内存。这种情况下要怎么做？

### 思路

想象有一个空房间，一开始没有椅子。

如果有人进入了房间（$\texttt{reserve}$），我们可以**添加一把新的椅子**给人坐（如果没有空出来的椅子）。

如果有人离开了椅子（$\texttt{unreserve}$），后面来的人**不需要新的椅子，直接坐空出来的椅子**就行。直到所有椅子都被坐满，此时必须要添加一把新的椅子给人坐。

### 实现

用一个变量 $\textit{seats}$ 表示目前房间内有多少把椅子，初始值为 $0$。用一个最小堆 $\textit{available}$ 维护空出来的椅子编号，初始为空。

- 对于 $\texttt{unreserve}(\textit{seatNumber})$，把 $\textit{seatNumber}$ 入堆。
- 对于 $\texttt{reserve}$，如果堆不为空（有空出来的椅子），就弹出并返回堆顶；否则把 $\textit{seats}$ 加一（添加一把新的椅子），然后返回 $\textit{seats}$。

```py [sol-Python3]
class SeatManager:
    def __init__(self, _: int):
        self.seats = 0  # 一开始没有椅子
        self.available = []

    def reserve(self) -> int:
        if self.available:  # 有空出来的椅子
            return heappop(self.available)  # 坐编号最小的
        self.seats += 1  # 添加一把新的椅子
        return self.seats

    def unreserve(self, seatNumber: int) -> None:
        heappush(self.available, seatNumber)  # 有人离开了椅子
```

```java [sol-Java]
class SeatManager {
    private final PriorityQueue<Integer> available = new PriorityQueue<>();
    private int seats;

    public SeatManager(int n) {
    }

    public int reserve() {
        if (!available.isEmpty()) { // 有空出来的椅子
            return available.poll(); // 坐编号最小的
        }
        return ++seats; // 添加一把新的椅子
    }

    public void unreserve(int seatNumber) {
        available.add(seatNumber); // 有人离开了椅子
    }
}
```

```cpp [sol-C++]
class SeatManager {
    int seats = 0;
    priority_queue<int, vector<int>, greater<>> available;

public:
    SeatManager(int) {}

    int reserve() {
        if (!available.empty()) { // 有空出来的椅子
            int seatNumber = available.top(); // 坐编号最小的
            available.pop();
            return seatNumber;
        }
        return ++seats; // 添加一把新的椅子
    }

    void unreserve(int seatNumber) {
        available.push(seatNumber); // 有人离开了椅子
    }
};
```

```go [sol-Go]
type SeatManager struct {
    sort.IntSlice // 继承 Len, Less, Swap
    seats int
}

func Constructor(int) SeatManager {
    return SeatManager{}
}

func (m *SeatManager) Reserve() int {
    if len(m.IntSlice) > 0 { // 有空出来的椅子
        return heap.Pop(m).(int) // 坐编号最小的
    }
    m.seats += 1 // 添加一把新的椅子
    return m.seats
}

func (m *SeatManager) Unreserve(seatNumber int) {
    heap.Push(m, seatNumber) // 有人离开了椅子
}

func (m *SeatManager) Push(v any) { m.IntSlice = append(m.IntSlice, v.(int)) }
func (m *SeatManager) Pop() any   { a := m.IntSlice; v := a[len(a)-1]; m.IntSlice = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var SeatManager = function(_) {
    this.seats = 0; // 一开始没有椅子
    this.available = new MinPriorityQueue();
};

SeatManager.prototype.reserve = function() {
    if (!this.available.isEmpty()) { // 有空出来的椅子
        return this.available.dequeue(); // 坐编号最小的
    }
    return ++this.seats; // 添加一把新的椅子
};

SeatManager.prototype.unreserve = function(seatNumber) {
    this.available.enqueue(seatNumber); // 有人离开了椅子
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

struct SeatManager {
    seats: i32,
    available: BinaryHeap<i32>,
}

impl SeatManager {
    fn new(_: i32) -> Self {
        Self { seats: 0, available: BinaryHeap::new() }
    }

    fn reserve(&mut self) -> i32 {
        if let Some(seat) = self.available.pop() { // 有空出来的椅子
            -seat // 坐编号最小的
        } else {
            self.seats += 1; // 添加一把新的椅子
            self.seats
        }
    }

    fn unreserve(&mut self, seat_number: i32) {
        self.available.push(-seat_number); // 有人离开了椅子
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化为 $\mathcal{O}(1)$，$\texttt{reserve}$ 和 $\texttt{unreserve}$ 均为 $\mathcal{O}(\log q)$，其中 $q$ 是 $\texttt{unreserve}$ 的调用次数。
- 空间复杂度：$\mathcal{O}(q)$。

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

本题来自 `五、堆（优先队列） / §5.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
