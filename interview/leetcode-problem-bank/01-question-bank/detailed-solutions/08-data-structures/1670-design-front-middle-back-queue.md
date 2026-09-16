# 1670. 设计前中后队列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/design-front-middle-back-queue/
- 题目 slug：`design-front-middle-back-queue`
- 来源专题：常用数据结构
- 来源分类路径：四、队列 / §4.2 设计
- 难度分：1610
- 外部题解来源：https://leetcode.cn/problems/design-front-middle-back-queue/solutions/2544784/tu-jie-liang-ge-shuang-duan-dui-lie-jian-43pr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】两个双端队列+简洁写法（Python/Java/C++/Go/Rust）](https://leetcode.cn/problems/design-front-middle-back-queue/solutions/2544784/tu-jie-liang-ge-shuang-duan-dui-lie-jian-43pr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-liang-ge-shuang-duan-dui-lie-jian-43pr`
- topic id：`2544784`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![LC1670-c.png](https://pic.leetcode.cn/1701084361-JejjZq-LC1670-c.png){:width=500}

```py [sol-Python3]
class FrontMiddleBackQueue:
    __slots__ = 'left', 'right'

    def __init__(self):
        self.left = deque()
        self.right = deque()

    # 调整长度，保证 0 <= len(right) - len(left) <= 1
    # 从而保证可以在正中间插入删除元素
    def balance(self):
        if len(self.left) > len(self.right):
            self.right.appendleft(self.left.pop())
        elif len(self.right) > len(self.left) + 1:
            self.left.append(self.right.popleft())

    def pushFront(self, val: int) -> None:
        self.left.appendleft(val)
        self.balance()

    def pushMiddle(self, val: int) -> None:
        if len(self.left) < len(self.right):
            self.left.append(val)
        else:
            self.right.appendleft(val)

    def pushBack(self, val: int) -> None:
        self.right.append(val)
        self.balance()

    def popFront(self) -> int:
        if not self.right:  # 整个队列为空
            return -1
        val = self.left.popleft() if self.left else self.right.popleft()
        self.balance()
        return val

    def popMiddle(self) -> int:
        if not self.right:  # 整个队列为空
            return -1
        if len(self.left) == len(self.right):
            return self.left.pop()
        return self.right.popleft()

    def popBack(self) -> int:
        if not self.right:  # 整个队列为空
            return -1
        val = self.right.pop()
        self.balance()
        return val
```

```java [sol-Java]
class FrontMiddleBackQueue {
    private final Deque<Integer> left = new ArrayDeque<>();
    private final Deque<Integer> right = new ArrayDeque<>();

    // 调整长度，保证 0 <= right.size() - left.size() <= 1
    // 从而保证可以在正中间插入删除元素
    private void balance() {
        if (left.size() > right.size()) {
            right.addFirst(left.pollLast());
        } else if (right.size() > left.size() + 1) {
            left.addLast(right.pollFirst());
        }
    }

    public void pushFront(int val) {
        left.addFirst(val);
        balance();
    }

    public void pushMiddle(int val) {
        if (left.size() < right.size()) {
            left.addLast(val);
        } else {
            right.addFirst(val);
        }
    }

    public void pushBack(int val) {
        right.addLast(val);
        balance();
    }

    public int popFront() {
        if (right.isEmpty()) { // 整个队列为空
            return -1;
        }
        int val = left.isEmpty() ? right.pollFirst() : left.pollFirst();
        balance();
        return val;
    }

    public int popMiddle() {
        if (right.isEmpty()) { // 整个队列为空
            return -1;
        }
        if (left.size() == right.size()) {
            return left.pollLast();
        }
        return right.pollFirst();
    }

    public int popBack() {
        if (right.isEmpty()) { // 整个队列为空
            return -1;
        }
        int val = right.pollLast();
        balance();
        return val;
    }
}
```

```cpp [sol-C++]
class FrontMiddleBackQueue {
private:
    deque<int> left;
    deque<int> right;

    // 调整长度，保证 0 <= right.size() - left.size() <= 1
    // 从而保证可以在正中间插入删除元素
    void balance() {
        if (left.size() > right.size()) {
            right.push_front(left.back());
            left.pop_back();
        } else if (right.size() > left.size() + 1) {
            left.push_back(right.front());
            right.pop_front();
        }
    }

public:
    void pushFront(int val) {
        left.push_front(val);
        balance();
    }

    void pushMiddle(int val) {
        if (left.size() < right.size()) {
            left.push_back(val);
        } else {
            right.push_front(val);
        }
    }

    void pushBack(int val) {
        right.push_back(val);
        balance();
    }

    int popFront() {
        if (right.empty()) { // 整个队列为空
            return -1;
        }
        int val;
        if (left.empty()) {
            val = right.front();
            right.pop_front();
        } else {
            val = left.front();
            left.pop_front();
        }
        balance();
        return val;
    }

    int popMiddle() {
        if (right.empty()) { // 整个队列为空
            return -1;
        }
        int val;
        if (left.size() == right.size()) {
            val = left.back();
            left.pop_back();
        } else {
            val = right.front();
            right.pop_front();
        }
        return val;
    }

    int popBack() {
        if (right.empty()) { // 整个队列为空
            return -1;
        }
        int val = right.back();
        right.pop_back();
        balance();
        return val;
    }
};
```

```go [sol-Go 链表]
// 第一种写法：链表
type FrontMiddleBackQueue struct {
    left  *list.List
    right *list.List
}

func Constructor() FrontMiddleBackQueue {
    return FrontMiddleBackQueue{
        left:  list.New(),
        right: list.New(),
    }
}

// 调整长度，保证 0 <= right.Len() - left.Len() <= 1
// 从而保证可以在正中间插入删除元素
func (q *FrontMiddleBackQueue) balance() {
    if q.left.Len() > q.right.Len() {
        q.right.PushFront(q.left.Remove(q.left.Back()))
    } else if q.right.Len() > q.left.Len()+1 {
        q.left.PushBack(q.right.Remove(q.right.Front()))
    }
}

func (q *FrontMiddleBackQueue) PushFront(val int) {
    q.left.PushFront(val)
    q.balance()
}

func (q *FrontMiddleBackQueue) PushMiddle(val int) {
    if q.left.Len() < q.right.Len() {
        q.left.PushBack(val)
    } else {
        q.right.PushFront(val)
    }
}

func (q *FrontMiddleBackQueue) PushBack(val int) {
    q.right.PushBack(val)
    q.balance()
}

func (q *FrontMiddleBackQueue) PopFront() (val int) {
    if q.right.Len() == 0 { // 整个队列为空
        return -1
    }
    if q.left.Len() > 0 {
        val = q.left.Remove(q.left.Front()).(int)
    } else {
        val = q.right.Remove(q.right.Front()).(int)
    }
    q.balance()
    return
}

func (q *FrontMiddleBackQueue) PopMiddle() int {
    if q.right.Len() == 0 { // 整个队列为空
        return -1
    }
    if q.left.Len() == q.right.Len() {
        return q.left.Remove(q.left.Back()).(int)
    }
    return q.right.Remove(q.right.Front()).(int)
}

func (q *FrontMiddleBackQueue) PopBack() int {
    if q.right.Len() == 0 { // 整个队列为空
        return -1
    }
    val := q.right.Remove(q.right.Back()).(int)
    q.balance()
    return val
}
```

```go [sol-Go 四个 slice]
// 第二种写法：四个 slice
type FrontMiddleBackQueue struct {
    left  *Deque
    right *Deque
}

func Constructor() FrontMiddleBackQueue {
    return FrontMiddleBackQueue{
        left:  &Deque{},
        right: &Deque{},
    }
}

// 调整长度，保证 0 <= right.Len() - left.Len() <= 1
// 从而保证可以在正中间插入删除元素
func (q *FrontMiddleBackQueue) balance() {
    if q.left.Len() > q.right.Len() {
        q.right.PushFront(q.left.PopBack())
    } else if q.right.Len() > q.left.Len()+1 {
        q.left.PushBack(q.right.PopFront())
    }
}

func (q *FrontMiddleBackQueue) PushFront(val int) {
    q.left.PushFront(val)
    q.balance()
}

func (q *FrontMiddleBackQueue) PushMiddle(val int) {
    if q.left.Len() < q.right.Len() {
        q.left.PushBack(val)
    } else {
        q.right.PushFront(val)
    }
}

func (q *FrontMiddleBackQueue) PushBack(val int) {
    q.right.PushBack(val)
    q.balance()
}

func (q *FrontMiddleBackQueue) PopFront() (val int) {
    if q.right.Len() == 0 { // 整个队列为空
        return -1
    }
    if q.left.Len() > 0 {
        val = q.left.PopFront()
    } else {
        val = q.right.PopFront()
    }
    q.balance()
    return
}

func (q *FrontMiddleBackQueue) PopMiddle() int {
    if q.right.Len() == 0 { // 整个队列为空
        return -1
    }
    if q.left.Len() == q.right.Len() {
        return q.left.PopBack()
    }
    return q.right.PopFront()
}

func (q *FrontMiddleBackQueue) PopBack() int {
    if q.right.Len() == 0 { // 整个队列为空
        return -1
    }
    val := q.right.PopBack()
    q.balance()
    return val
}

// 两个 slice 头对头，即可实现双端队列
// 但这并不是一个「工业级」的实现，因为 slice 没有「缩容」的概念
// 这意味着在大量的 pop 操作后，会产生大量无法被自动 GC 的空间
type Deque struct {
    left  []int
    right []int
}

func (q Deque) Empty() bool {
    return len(q.left) == 0 && len(q.right) == 0
}

func (q Deque) Len() int {
    return len(q.left) + len(q.right)
}

func (q *Deque) PushFront(v int) {
    q.left = append(q.left, v)
}

func (q *Deque) PushBack(v int) {
    q.right = append(q.right, v)
}

func (q *Deque) PopFront() (v int) {
    if len(q.left) > 0 {
        q.left, v = q.left[:len(q.left)-1], q.left[len(q.left)-1]
    } else {
        v, q.right = q.right[0], q.right[1:]
    }
    return
}

func (q *Deque) PopBack() (v int) {
    if len(q.right) > 0 {
        q.right, v = q.right[:len(q.right)-1], q.right[len(q.right)-1]
    } else {
        v, q.left = q.left[0], q.left[1:]
    }
    return
}

// 省略了一部分没用到的方法，完整实现请看
// https://github.com/EndlessCheng/codeforces-go/blob/master/copypasta/deque.go
```

```rust [sol-Rust]
use std::collections::VecDeque;

struct FrontMiddleBackQueue {
    left: VecDeque<i32>,
    right: VecDeque<i32>,
}

impl FrontMiddleBackQueue {
    fn new() -> Self {
        Self {
            left: VecDeque::new(),
            right: VecDeque::new(),
        }
    }

    // 调整长度，保证 0 <= right.len() - left.len() <= 1
    // 从而保证可以在正中间插入删除元素
    fn balance(&mut self) {
        if self.left.len() > self.right.len() {
            self.right.push_front(self.left.pop_back().unwrap());
        } else if self.right.len() > self.left.len() + 1 {
            self.left.push_back(self.right.pop_front().unwrap());
        }
    }

    fn push_front(&mut self, val: i32) {
        self.left.push_front(val);
        self.balance();
    }

    fn push_middle(&mut self, val: i32) {
        if self.left.len() < self.right.len() {
            self.left.push_back(val);
        } else {
            self.right.push_front(val);
        }
    }

    fn push_back(&mut self, val: i32) {
        self.right.push_back(val);
        self.balance();
    }

    fn pop_front(&mut self) -> i32 {
        if self.right.is_empty() { // 整个队列为空
            return -1;
        }
        let val = if self.left.is_empty() {
            self.right.pop_front().unwrap()
        } else {
            self.left.pop_front().unwrap()
        };
        self.balance();
        val
    }

    fn pop_middle(&mut self) -> i32 {
        if self.right.is_empty() { // 整个队列为空
            return -1;
        }
        if self.left.len() == self.right.len() {
            return self.left.pop_back().unwrap();
        }
        self.right.pop_front().unwrap()
    }

    fn pop_back(&mut self) -> i32 {
        if self.right.is_empty() { // 整个队列为空
            return -1;
        }
        let val = self.right.pop_back().unwrap();
        self.balance();
        val
    }
}
```

#### 复杂度分析

- 时间复杂度：所有操作均为 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(q)$，其中 $q$ 为操作次数。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、队列 / §4.2 设计`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、队列 / §4.2 设计`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
