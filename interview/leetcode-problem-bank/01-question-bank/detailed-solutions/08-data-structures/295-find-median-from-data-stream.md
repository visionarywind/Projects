# 295. 数据流的中位数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-median-from-data-stream/
- 题目 slug：`find-median-from-data-stream`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-median-from-data-stream/solutions/3015873/ru-he-zi-ran-yin-ru-da-xiao-dui-jian-ji-4v22k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[如何自然引入大小堆？简洁写法！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-median-from-data-stream/solutions/3015873/ru-he-zi-ran-yin-ru-da-xiao-dui-jian-ji-4v22k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-zi-ran-yin-ru-da-xiao-dui-jian-ji-4v22k`
- topic id：`3015873`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

比如现在有 $6$ 个数：$1,5,6,2,3,4$，要计算中位数，可以把这 $6$ 个数从小到大排序，得到 $1,2,3,4,5,6$，中间 $3$ 和 $4$ 的平均值 $3.5$ 就是中位数。

回顾一下百科中关于中位数的定义：

> 中位数……可将数值集合划分为相等的两部分。

中位数把这 $6$ 个数均分成了左右两部分，小的那一组记作 $\textit{left} = [1,2,3]$，大的那一组记作 $\textit{right}=[4,5,6]$。我们要计算的中位数，就来自 $\textit{left}$ 中的**最大值**，以及 $\textit{right}$ 中的**最小值**。

随着 $\texttt{addNum}$ 不断地添加数字，我们需要：

- 保证 $\textit{left}$ 的大小和 $\textit{right}$ 的大小尽量相等。**规定**：在有奇数个数时，$\textit{left}$ 比 $\textit{right}$ 多 $1$ 个数。
- 保证 $\textit{left}$ 的所有元素都小于等于 $\textit{right}$ 的所有元素。

只要时时刻刻满足以上两个要求（满足中位数的定义），我们就可以用 $\textit{left}$ 中的最大值以及 $\textit{right}$ 中的最小值计算中位数。

分类讨论：

- 如果当前 $\textit{left}$ 的大小和 $\textit{right}$ 的大小相等：
    - 如果添加的数字 $\textit{num}$ 比较大，比如添加 $7$，那么把 $7$ 加到 $\textit{right}$ 中。现在 $\textit{left}$ 比 $\textit{right}$ 少 $1$ 个数，不符合前文的规定，所以必须把 $\textit{right}$ 的最小值从 $\textit{right}$ 中去掉，添加到 $\textit{left}$ 中。如此操作后，可以保证 $\textit{left}$ 的所有元素都小于等于 $\textit{right}$ 的所有元素。
    - 如果添加的数字 $\textit{num}$ 比较小，比如添加 $0$，那么把 $0$ 加到 $\textit{left}$ 中。
    - 这两种情况可以**合并**：无论 $\textit{num}$ 是大是小，都可以先把 $\textit{num}$ 加到 $\textit{right}$ 中，然后把 $\textit{right}$ 的最小值从 $\textit{right}$ 中去掉，并添加到 $\textit{left}$ 中。
- 如果当前 $\textit{left}$ 比 $\textit{right}$ 多 $1$ 个数：
    - 如果添加的数字 $\textit{num}$ 比较大，比如添加 $7$，那么把 $7$ 加到 $\textit{right}$ 中。
    - 如果添加的数字 $\textit{num}$ 比较小，比如添加 $0$，那么把 $0$ 加到 $\textit{left}$ 中。现在 $\textit{left}$ 比 $\textit{right}$ 多 $2$ 个数，不符合前文的规定，所以必须把 $\textit{left}$ 的最大值从 $\textit{left}$ 中去掉，添加到 $\textit{right}$ 中。如此操作后，可以保证 $\textit{left}$ 的所有元素都小于等于 $\textit{right}$ 的所有元素。
    - 这两种情况可以**合并**：无论 $\textit{num}$ 是大是小，都可以先把 $\textit{num}$ 加到 $\textit{left}$ 中，然后把 $\textit{left}$ 的最大值从 $\textit{left}$ 中去掉，并添加到 $\textit{right}$ 中。

最后，我们需要什么样的数据结构？这个数据结构要能高效地执行如下操作：

- 添加元素。
- 找到最大（小）值。
- 删除最大（小）值。

这个数据结构是**堆**。

$\textit{left}$ 是**最大堆**，$\textit{right}$ 是**最小堆**。

- 如果当前有奇数个元素，中位数是 $\textit{left}$ 的堆顶。
- 如果当前有偶数个元素，中位数是 $\textit{left}$ 的堆顶和 $\textit{right}$ 的堆顶的平均值。

```py [sol-Python3]
class MedianFinder:
    def __init__(self):
        self.left = []  # 最大堆
        self.right = []  # 最小堆

    def addNum(self, num: int) -> None:
        if len(self.left) == len(self.right):
            heappush_max(self.left, heappushpop(self.right, num))
        else:
            heappush(self.right, heappushpop_max(self.left, num))

    def findMedian(self) -> float:
        if len(self.left) > len(self.right):
            return self.left[0]
        return (self.left[0] + self.right[0]) / 2
```

```java [sol-Java]
class MedianFinder {
    private final PriorityQueue<Integer> left = new PriorityQueue<>((a, b) -> b - a); // 最大堆
    private final PriorityQueue<Integer> right = new PriorityQueue<>(); // 最小堆

    public void addNum(int num) {
        if (left.size() == right.size()) {
            right.offer(num);
            left.offer(right.poll());
        } else {
            left.offer(num);
            right.offer(left.poll());
        }
    }

    public double findMedian() {
        if (left.size() > right.size()) {
            return left.peek();
        }
        return (left.peek() + right.peek()) / 2.0;
    }
}
```

```cpp [sol-C++]
class MedianFinder {
    priority_queue<int> left; // 最大堆
    priority_queue<int, vector<int>, greater<>> right; // 最小堆

public:
    void addNum(int num) {
        if (left.size() == right.size()) {
            right.push(num);
            left.push(right.top());
            right.pop();
        } else {
            left.push(num);
            right.push(left.top());
            left.pop();
        }
    }

    double findMedian() {
        if (left.size() > right.size()) {
            return left.top();
        }
        return (left.top() + right.top()) / 2.0;
    }
};
```

```go [sol-Go]
type MedianFinder struct {
    left  hp // 入堆的元素取相反数，变成最大堆
    right hp // 最小堆
}

func Constructor() (_ MedianFinder) { return }

func (mf *MedianFinder) AddNum(num int) {
    if mf.left.Len() == mf.right.Len() {
        heap.Push(&mf.right, num)
        heap.Push(&mf.left, -heap.Pop(&mf.right).(int))
    } else {
        heap.Push(&mf.left, -num)
        heap.Push(&mf.right, -heap.Pop(&mf.left).(int))
    }
}

func (mf *MedianFinder) FindMedian() float64 {
    if mf.left.Len() > mf.right.Len() {
        return float64(-mf.left.IntSlice[0])
    }
    return float64(mf.right.IntSlice[0]-mf.left.IntSlice[0]) / 2
}

type hp struct{ sort.IntSlice }

func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

```go [sol-Go 写法二]
type MedianFinder struct {
    left  hp // 入堆的元素取相反数，变成最大堆
    right hp // 最小堆
}

func Constructor() (_ MedianFinder) { return }

func (mf *MedianFinder) AddNum(num int) {
    if mf.left.Len() == mf.right.Len() {
        heap.Push(&mf.left, -mf.right.pushPop(num))
    } else {
        heap.Push(&mf.right, -mf.left.pushPop(-num))
    }
}

func (mf *MedianFinder) FindMedian() float64 {
    if mf.left.Len() > mf.right.Len() {
        return float64(-mf.left.IntSlice[0])
    }
    return float64(mf.right.IntSlice[0]-mf.left.IntSlice[0]) / 2
}

type hp struct{ sort.IntSlice }

func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (hp) Pop() (_ any)  { return }

// pushPop 先将 v 入堆，然后弹出并返回堆顶
func (h *hp) pushPop(v int) int {
    if h.Len() > 0 && v > h.IntSlice[0] {
        v, h.IntSlice[0] = h.IntSlice[0], v
        heap.Fix(h, 0)
    }
    return v
}
```

```js [sol-JavaScript]
var MedianFinder = function() {
    this.left = new MaxPriorityQueue();
    this.right = new MinPriorityQueue();
};

MedianFinder.prototype.addNum = function(num) {
    if (this.left.size() === this.right.size()) {
        this.right.enqueue(num);
        this.left.enqueue(this.right.dequeue());
    } else {
        this.left.enqueue(num);
        this.right.enqueue(this.left.dequeue());
    }
};

MedianFinder.prototype.findMedian = function() {
    if (this.left.size() > this.right.size()) {
        return this.left.front();
    }
    return (this.left.front() + this.right.front()) / 2;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

struct MedianFinder {
    left: BinaryHeap<i32>, // 最大堆
    right: BinaryHeap<i32>, // 入堆的元素取相反数，变成最小堆
}

impl MedianFinder {
    fn new() -> Self {
        Self {
            left: BinaryHeap::new(),
            right: BinaryHeap::new(),
        }
    }

    fn add_num(&mut self, num: i32) {
        if self.left.len() == self.right.len() {
            self.right.push(-num);
            self.left.push(-self.right.pop().unwrap());
        } else {
            self.left.push(num);
            self.right.push(-self.left.pop().unwrap());
        }
    }

    fn find_median(&self) -> f64 {
        if self.left.len() > self.right.len() {
            *self.left.peek().unwrap() as _
        } else {
            (*self.left.peek().unwrap() - self.right.peek().unwrap()) as f64 / 2.0
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：初始化和 $\texttt{findMedian}$ 都是 $\mathcal{O}(1)$，$\texttt{addNum}$ 是 $\mathcal{O}(\log q)$，其中 $q$ 是 $\texttt{addNum}$ 的调用次数。每次操作堆需要 $\mathcal{O}(\log q)$ 的时间。
- 空间复杂度：$\mathcal{O}(q)$。

## 思考题

如果还有删除操作（删除数据流中的**任意**元素），要求仍然用堆实现，要怎么做？

见 [480. 滑动窗口中位数](https://leetcode.cn/problems/sliding-window-median/)。

## 专题训练

见下面数据结构题单的「**§5.7 对顶堆**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
