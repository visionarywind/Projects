# 155. 最小栈

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/min-stack/
- 题目 slug：`min-stack`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/min-stack/solutions/2974438/ben-zhi-shi-wei-hu-qian-zhui-zui-xiao-zh-x0g8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：保存前缀最小值 / 保存差值，不用辅助栈（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/min-stack/solutions/2974438/ben-zhi-shi-wei-hu-qian-zhui-zui-xiao-zh-x0g8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-zhi-shi-wei-hu-qian-zhui-zui-xiao-zh-x0g8`
- topic id：`2974438`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：额外保存前缀最小值

### 引入

给你一个数组 $\textit{nums}$，如何计算**每个**前缀的最小值？

定义 $\textit{preMin}[i]$ 表示 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 的最小值。

这可以从左到右计算：

- $\textit{preMin}[0] = \textit{nums}[0]$。
- $\textit{preMin}[1] = \min(\textit{nums}[0], \textit{nums}[1])$。
- $\textit{preMin}[2] = \min(\textit{nums}[0], \textit{nums}[1], \textit{nums}[2]) = \min(\textit{preMin}[1], \textit{nums}[2])$。
- $\textit{preMin}[3] = \min(\textit{nums}[0], \textit{nums}[1], \textit{nums}[2], \textit{nums}[3]) = \min(\textit{preMin}[2], \textit{nums}[3])$。
- ……

一般地，我们有

$$
\textit{preMin}[i] = \min(\textit{preMin}[i-1], \textit{nums}[i])
$$

### 回到本题

把 $\textit{nums}$ 视作栈，本题相当于在 $\textit{nums}$ 的末尾**动态地添加/删除元素**。

- 栈中除了保存添加的元素，还保存前缀最小值。（栈中保存的是 pair）
- 添加元素：设当前栈的大小是 $n$。添加元素 $\textit{val}$ 后，额外维护 $\textit{preMin}[n] = \min(\textit{preMin}[n-1], \textit{val})$，其中 $\textit{preMin}[n-1]$ 是添加 $\textit{val}$ 之前，栈顶保存的前缀最小值。
- 删除元素：弹出栈顶即可。

### 细节

一开始栈为空（$n=0$），添加 $\textit{val}$ 时，我们没有对应的 $\textit{preMin}[n-1]$。需要特判栈为空的情况吗？

不需要。初始化的时候，在栈底加一个 $\infty$ 哨兵，作为 $\textit{preMin}[-1]$。

> **注**：题目保证 $\texttt{pop},\texttt{top},\texttt{getMin}$ 都是在**非空栈**上操作的。

```py [sol-Python3]
class MinStack:
    def __init__(self):
        # 这里的 0 写成任意数都可以，反正用不到
        self.st = [(0, inf)]  # 栈底哨兵

    def push(self, val: int) -> None:
        self.st.append((val, min(self.st[-1][1], val)))

    def pop(self) -> None:
        self.st.pop()

    def top(self) -> int:
        return self.st[-1][0]

    def getMin(self) -> int:
        return self.st[-1][1]
```

```java [sol-Java]
class MinStack {
    // 注意不要使用 Stack 类，因为它继承自 Vector，是同步的，会导致一些性能问题
    private final Deque<int[]> st = new ArrayDeque<>();

    public MinStack() {
        // 添加栈底哨兵 Integer.MAX_VALUE
        // 这里的 0 写成任意数都可以，反正用不到
        st.push(new int[]{0, Integer.MAX_VALUE});
    }

    public void push(int val) {
        st.push(new int[]{val, Math.min(getMin(), val)});
    }

    public void pop() {
        st.pop();
    }

    public int top() {
        return st.peek()[0];
    }

    public int getMin() {
        return st.peek()[1];
    }
}
```

```cpp [sol-C++]
class MinStack {
    stack<pair<int, int>> st;

public:
    MinStack() {
        // 添加栈底哨兵 INT_MAX
        // 这里的 0 写成任意数都可以，反正用不到
        st.emplace(0, INT_MAX);
    }

    void push(int val) {
        st.emplace(val, min(getMin(), val)); 
    }

    void pop() {
        st.pop();
    }

    int top() {
        return st.top().first;
    }

    int getMin() {
        return st.top().second;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int st[30001][2] = {{0, INT_MAX}}; // 栈底哨兵（0 写成任意数都可以，反正用不到）

typedef struct {
    int top; // 当前栈顶下标
} MinStack;

MinStack* minStackCreate() {
    MinStack* obj = malloc(sizeof(MinStack));
    obj->top = 0;
    return obj;
}

void minStackPush(MinStack* obj, int val) {
    obj->top++;
    st[obj->top][0] = val;
    st[obj->top][1] = MIN(st[obj->top - 1][1], val);
}

void minStackPop(MinStack* obj) {
    obj->top--;
}

int minStackTop(MinStack* obj) {
    return st[obj->top][0];
}

int minStackGetMin(MinStack* obj) {
    return st[obj->top][1];
}

void minStackFree(MinStack* obj) {
    free(obj);
}
```

```go [sol-Go]
type pair struct{ val, preMin int }

type MinStack []pair

func Constructor() MinStack {
    // 这里的 0 写成任意数都可以，反正用不到
    return MinStack{{0, math.MaxInt}} // 栈底哨兵
}

func (st *MinStack) Push(val int) {
    *st = append(*st, pair{val, min(st.GetMin(), val)})
}

func (st *MinStack) Pop() {
    *st = (*st)[:len(*st)-1]
}

func (st MinStack) Top() int {
    return st[len(st)-1].val
}

func (st MinStack) GetMin() int {
    return st[len(st)-1].preMin
}
```

```js [sol-JavaScript]
class MinStack {
    constructor() {
        // 这里的 0 写成任意数都可以，反正用不到
        this.st = [[0, Infinity]]; // 栈底哨兵
    }

    push(val) {
        this.st.push([val, Math.min(this.getMin(), val)]);
    }

    pop() {
        this.st.pop();
    }

    top() {
        return this.st[this.st.length - 1][0];
    }

    getMin() {
        return this.st[this.st.length - 1][1];
    }
}
```

```rust [sol-Rust]
struct MinStack {
    st: Vec<(i32, i32)>,
}

impl MinStack {
    fn new() -> Self {
        // 添加栈底哨兵 i32::MAX
        // 这里的 0 写成任意数都可以，反正用不到
        Self { st: vec![(0, i32::MAX)] }
    }

    fn push(&mut self, val: i32) {
        self.st.push((val, self.get_min().min(val)));
    }

    fn pop(&mut self) {
        self.st.pop();
    }

    fn top(&self) -> i32 {
        self.st.last().unwrap().0
    }

    fn get_min(&self) -> i32 {
        self.st.last().unwrap().1
    }
}
```

#### 复杂度分析

- 时间复杂度：所有操作均为 $\mathcal{O}(1)$。**注**：严格地说，$\texttt{push}$ 是均摊 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(q)$。其中 $q$ 是 $\texttt{push}$ 调用的次数。最坏情况下，只有 $\texttt{push}$ 操作，需要 $\mathcal{O}(q)$ 的空间保存元素。

## 方法二：保存差值

**进阶问题**：如果不允许额外保存前缀最小值，栈中只能保存整数（不能保存数对），怎么做？

如果栈中保存的是 $\textit{val}$ 与前缀最小值的**差值**，那么只要我们能实时维护前缀最小值，就能通过差值还原 $\textit{val}$。

例如依次插入 $\textit{val} = 5,6,8,1,2$，计算过程如下表。

**如何阅读下表**：

1. 表格中的差值等于插入的 $\textit{val}$ 减去插入**之前**的最小值。
2. $\texttt{push}(\textit{val})$ 从上到下阅读，$\texttt{top}$ 和 $\texttt{pop}$ 从下到上阅读。

| $\texttt{push}(\textit{val})$  | 最小值  | 差值  | $\texttt{top}$  | $\texttt{pop}$  |
|:---:|:---:|:---:|---|---|
| $ $  | $\infty$  | $ $  |  $ $ | $ $ |
| $5$  | $5$  | $-\infty$  | 最小值  | 最小值增加 $\infty$  |
| $6$  | $5$  | $1$  | 最小值$+$差值  | 最小值不变  |
| $8$  | $5$  | $3$  | 最小值$+$差值  | 最小值不变  |
| $1$  | $1$  | $-4$  |  最小值 | 最小值增加 $4$  |
| $2$  | $1$  | $1$  | 最小值$+$差值  | 最小值不变  |

一般地：

- 初始化前缀最小值 $\textit{mn} = \infty$。
- $\texttt{push}(\textit{val})$：先把 $(\textit{val} - \textit{mn})$ 入栈，再更新 $\textit{mn}$ 为 $\min(\textit{mn},\textit{val})$。
- $\texttt{top}$：返回 $\textit{mn} + \max(栈顶,0)$。如果栈顶大于 $0$，说明 $\textit{val}$ 比 $\textit{mn}$ 多一个栈顶的值；否则 $\textit{val}$ 就是 $\textit{mn}$。
- $\texttt{pop}$：把 $\textit{mn}$ 减少 $\min(栈顶,0)$。如果栈顶小于 $0$，这会把 $\textit{mn}$ 增大；否则 $\textit{mn}$ 不变。
- $\texttt{getMin}$：返回 $\textit{mn}$ 即可。

代码实现时，为避免溢出，需要用 64 位整数。

> **注**：题目保证 $\texttt{pop},\texttt{top},\texttt{getMin}$ 都是在**非空栈**上操作的。

```py [sol-Python3]
class MinStack:
    def __init__(self):
        self.st = []
        self.mn = inf

    def push(self, val: int) -> None:
        # 栈中保存 val - 之前的最小值
        self.st.append(val - self.mn)
        self.mn = min(self.mn, val)

    def pop(self) -> None:
        # 如果栈顶是负数，增大 mn，否则不变
        self.mn -= min(self.st.pop(), 0)

    def top(self) -> int:
        # 如果栈顶是正数，说明实际的 val 比 mn 大，否则 val 等于 mn
        return self.mn + max(self.st[-1], 0)

    def getMin(self) -> int:
        return self.mn
```

```java [sol-Java]
class MinStack {
    // 注意不要使用 Stack 类，因为它继承自 Vector，是同步的，会导致一些性能问题
    private final Deque<Long> st = new ArrayDeque<>();
    private long mn = Long.MAX_VALUE / 2; // 避免 val - mn 溢出

    public void push(int val) {
        // 栈中保存 val - 之前的最小值
        st.push(val - mn);
        mn = Math.min(mn, val);
    }

    public void pop() {
        // 如果栈顶是负数，增大 mn，否则不变
        mn -= Math.min(st.pop(), 0);
    }

    public int top() {
        // 如果栈顶是正数，说明实际的 val 比 mn 大，否则 val 等于 mn
        return (int) (mn + Math.max(st.peek(), 0));
    }

    public int getMin() {
        return (int) mn;
    }
}
```

```cpp [sol-C++]
class MinStack {
    stack<long long> st;
    long long mn = LLONG_MAX / 2; // 避免 val - mn 溢出

public:
    void push(int val) {
        // 栈中保存 val - 之前的最小值
        st.push(val - mn);
        mn = min(mn, 1LL * val);
    }

    void pop() {
        // 如果栈顶是负数，增大 mn，否则不变
        mn -= min(st.top(), 0LL);
        st.pop();
    }

    int top() {
        // 如果栈顶是正数，说明实际的 val 比 mn 大，否则 val 等于 mn
        return mn + max(st.top(), 0LL);
    }

    int getMin() {
        return mn;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

long long st[30000];

typedef struct {
    int top; // 当前栈顶下标
    long long mn;
} MinStack;

MinStack* minStackCreate() {
    MinStack* obj = malloc(sizeof(MinStack));
    obj->top = -1;
    obj->mn = LLONG_MAX / 2; // 避免 val - mn 溢出
    return obj;
}

void minStackPush(MinStack* obj, int val) {
    // 栈中保存 val - 之前的最小值
    st[++obj->top] = val - obj->mn;
    obj->mn = MIN(obj->mn, val);
}

void minStackPop(MinStack* obj) {
    // 如果栈顶是负数，增大 mn，否则不变
    long long x = st[obj->top--];
    obj->mn -= MIN(x, 0);
}

int minStackTop(MinStack* obj) {
    // 如果栈顶是正数，说明实际的 val 比 mn 大，否则 val 等于 mn
    return obj->mn + MAX(st[obj->top], 0);
}

int minStackGetMin(MinStack* obj) {
    return obj->mn;
}

void minStackFree(MinStack* obj) {
    free(obj);
}
```

```go [sol-Go]
type MinStack struct {
    st  []int
    min int
}

func Constructor() MinStack {
    return MinStack{min: math.MaxInt / 2} // 避免 val-m.min 溢出
}

func (m *MinStack) Push(val int) {
    // 栈中保存 val - 之前的 min
    m.st = append(m.st, val-m.min)
    m.min = min(m.min, val)
}

func (m *MinStack) Pop() {
    // 如果栈顶是负数，增大 min，否则不变
    m.min -= min(m.st[len(m.st)-1], 0)
    m.st = m.st[:len(m.st)-1]
}

func (m MinStack) Top() int {
    // 如果栈顶是正数，说明实际的 val 比 min 大，否则 val 等于 min
    return m.min + max(m.st[len(m.st)-1], 0)
}

func (m MinStack) GetMin() int {
    return m.min
}
```

```js [sol-JavaScript]
class MinStack {
    constructor() {
        this.st = [];
        this.mn = Infinity;
    }

    push(val) {
        // 栈中保存 val - 之前的最小值
        this.st.push(val - this.mn);
        this.mn = Math.min(this.mn, val);
    }

    pop() {
        // 如果栈顶是负数，增大 mn，否则不变
        this.mn -= Math.min(this.st.pop(), 0);
    }

    top() {
        // 如果栈顶是正数，说明实际的 val 比 mn 大，否则 val 等于 mn
        return this.mn + Math.max(this.st[this.st.length - 1], 0);
    }

    getMin() {
        return this.mn;
    }
}
```

```rust [sol-Rust]
struct MinStack {
    st: Vec<i64>,
    mn: i64,
}

impl MinStack {
    fn new() -> Self {
        Self {
            st: vec![],
            mn: i64::MAX / 2, // 避免 val - mn 溢出
        }
    }

    fn push(&mut self, val: i32) {
        // 栈中保存 val - 之前的最小值
        self.st.push(val as i64 - self.mn);
        self.mn = self.mn.min(val as i64);
    }

    fn pop(&mut self) {
        // 如果栈顶是负数，增大 mn，否则不变
        self.mn -= self.st.pop().unwrap().min(0);
    }

    fn top(&self) -> i32 {
        // 如果栈顶是正数，说明实际的 val 比 mn 大，否则 val 等于 mn
        (self.mn + (*self.st.last().unwrap()).max(0)) as _
    }

    fn get_min(&self) -> i32 {
        self.mn as _
    }
}
```

#### 复杂度分析

- 时间复杂度：所有操作均为 $\mathcal{O}(1)$。**注**：严格地说，$\texttt{push}$ 是均摊 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(q)$。其中 $q$ 是 $\texttt{push}$ 调用的次数。最坏情况下，只有 $\texttt{push}$ 操作，需要 $\mathcal{O}(q)$ 的空间保存元素。

## 变形题

1. 改成队列（queue），$\texttt{getMin}$ 返回队列中的最小元素。
2. 改成双端队列（deque），$\texttt{getMin}$ 返回双端队列中的最小元素。

解答：[[Tutorial] Minimum Deque](https://codeforces.com/blog/entry/122003)。

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

本题来自 `三、栈 / §3.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
