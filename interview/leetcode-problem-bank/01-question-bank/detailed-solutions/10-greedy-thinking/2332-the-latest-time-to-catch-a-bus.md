# 2332. 坐上公交的最晚时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/the-latest-time-to-catch-a-bus/
- 题目 slug：`the-latest-time-to-catch-a-bus`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1841
- 外部题解来源：https://leetcode.cn/problems/the-latest-time-to-catch-a-bus/solutions/1658352/pai-xu-by-endlesscheng-h9w9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[插队（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/the-latest-time-to-catch-a-bus/solutions/1658352/pai-xu-by-endlesscheng-h9w9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-by-endlesscheng-h9w9`
- topic id：`1658352`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

## 分类讨论

- 如果最后一班公交还有空位：
    - 如果最后一班公交发车时，没有乘客到达公交站，我们可以**在发车时到达公交站**。~~极限操作~~
    - 如果发车时恰好有乘客到达公交站，由于题目要求不能跟别的乘客同时到达，我们可以顺着这位乘客**往前找没人到达的时刻**，在这个时刻「插队」。
- 如果最后一班公交没有空位：
    - 找最后一个上车的乘客 A，然后**往前找没人到达的时刻**，在这个时刻「插队」，把 A 挤下去。（可怜的 A）

为什么可以插队？万一前面的乘客没有上车，我们不就也没法上车了吗？

这是不会的，因为先来先上车，**如果一个乘客上了车，那么他前面的乘客也肯定上了车**。

## 思路

至此，本题的算法分为两个过程：

1. 不考虑自己，模拟乘客上车的过程。
2. 根据上面的分类讨论，寻找合适的插队时机。

## 模拟乘客上车

1. 为方便模拟，把 $\textit{buses}$ 和 $\textit{passengers}$ 都从小到大排序。
2. 双指针遍历 $\textit{buses}$ 和 $\textit{passengers}$。
3. 对于 $\textit{buses}[i]$，初始化 $c = \textit{capacity}$。
4. 不断循环，如果 $c> 0$ 且 $\textit{passengers}[j]\le \textit{buses}[i]$，那么第 $j$ 位乘客可以上车，把 $c$ 减一，$j$ 加一。如果没法上车，只能等下一班车。
5. 双指针遍历结束后，$j-1$ 就是最后一个上车的乘客。这里减一是因为第 $j$ 位乘客上车后我们把 $j$ 加一了。

> 注：双指针遍历结束时，有可能所有乘客都上车了，最后几班公交没有任何乘客。这不会影响我们的结论，仍然选最后一班公交到达的时刻上车。

## 寻找插队时机

1. 为方便写代码，先把 $j$ 减一。
2. 如果双指针遍历结束时的 $c>0$，那么最后一班公交一定是有空位的，初始化答案 $\textit{ans}= \textit{buses}[n-1]$，否则初始化 $\textit{ans}=\textit{passengers}[j]$。
3. 如果 $\textit{ans} = \textit{passengers}[j]$，就往前找插队的时机，把 $\textit{ans}$ 和 $j$ 都减一，直到 $\textit{ans}\ne\textit{passengers}[j]$，我们在 $\textit{ans}$ 这一时刻插队。

[本题视频讲解](https://www.bilibili.com/video/BV1Le4y1R7xu)

```py [sol-Python3]
class Solution:
    def latestTimeCatchTheBus(self, buses: List[int], passengers: List[int], capacity: int) -> int:
        buses.sort()
        passengers.sort()

        # 模拟乘客上车
        j = 0
        for t in buses:
            c = capacity
            while c and j < len(passengers) and passengers[j] <= t:
                j += 1
                c -= 1

        # 寻找插队时机
        j -= 1
        ans = buses[-1] if c else passengers[j]
        while j >= 0 and ans == passengers[j]:  # 往前找没人到达的时刻
            ans -= 1
            j -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int latestTimeCatchTheBus(int[] buses, int[] passengers, int capacity) {
        Arrays.sort(buses);
        Arrays.sort(passengers);

        // 模拟乘客上车
        int j = 0;
        int c = 0;
        for (int t : buses) {
            for (c = capacity; c > 0 && j < passengers.length && passengers[j] <= t; c--) {
                j++;
            }
        }

        // 寻找插队时机
        j--;
        int ans = c > 0 ? buses[buses.length - 1] : passengers[j];
        while (j >= 0 && ans == passengers[j]) {
            ans--; // 往前找没人到达的时刻
            j--;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int latestTimeCatchTheBus(vector<int>& buses, vector<int>& passengers, int capacity) {
        ranges::sort(buses);
        ranges::sort(passengers);

        // 模拟乘客上车
        int j = 0, c;
        for (int t : buses) {
            for (c = capacity; c && j < passengers.size() && passengers[j] <= t; c--) {
                j++;
            }
        }

        // 寻找插队时机
        j--;
        int ans = c ? buses.back() : passengers[j];
        while (j >= 0 && ans == passengers[j]) {
            ans--; // 往前找没人到达的时刻
            j--;
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int latestTimeCatchTheBus(int* buses, int busesSize, int* passengers, int passengersSize, int capacity) {
    qsort(buses, busesSize, sizeof(int), cmp);
    qsort(passengers, passengersSize, sizeof(int), cmp);

    // 模拟乘客上车
    int j = 0, c;
    for (int i = 0; i < busesSize; i++) {
        for (c = capacity; c && j < passengersSize && passengers[j] <= buses[i]; c--) {
            j++;
        }
    }

    // 寻找插队时机
    j--;
    int ans = c ? buses[busesSize - 1] : passengers[j];
    while (j >= 0 && ans == passengers[j]) {
        ans--; // 往前找没人到达的时刻
        j--;
    }
    return ans;
}
```

```go [sol-Go]
func latestTimeCatchTheBus(buses, passengers []int, capacity int) (ans int) {
    slices.Sort(buses)
    slices.Sort(passengers)

    // 模拟乘客上车
    j, c := 0, 0
    for _, t := range buses {
        for c = capacity; c > 0 && j < len(passengers) && passengers[j] <= t; c-- {
            j++
        }
    }
    
    // 插队
    if c > 0 {
        ans = buses[len(buses)-1] // 最后一班公交还有空位，在发车时到达
    } else {
        ans = passengers[j-1] // 上一个上车的乘客
    }
    for j--; j >= 0 && ans == passengers[j]; j-- { // 往前找没人到达的时刻
        ans--
    }
    return
}
```

```js [sol-JavaScript]
var latestTimeCatchTheBus = function(buses, passengers, capacity) {
    buses.sort((a, b) => a - b);
    passengers.sort((a, b) => a - b);

    // 模拟乘客上车
    let j = 0, c;
    for (const t of buses) {
        for (c = capacity; c > 0 && j < passengers.length && passengers[j] <= t; c--) {
            j++;
        }
    }

    // 寻找插队时机
    j--;
    let ans = c > 0 ? buses[buses.length - 1] : passengers[j];
    while (j >= 0 && ans === passengers[j]) {
        ans--; // 往前找没人到达的时刻
        j--;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn latest_time_catch_the_bus(mut buses: Vec<i32>, mut passengers: Vec<i32>, capacity: i32) -> i32 {
        buses.sort_unstable();
        passengers.sort_unstable();

        // 模拟乘客上车
        let mut j = 0;
        let mut c = 0;
        for &t in &buses {
            c = capacity;
            while c > 0 && j < passengers.len() && passengers[j] <= t {
                j += 1;
                c -= 1;
            }
        }

        // 寻找插队时机
        j -= 1;
        let mut ans = if c > 0 { *buses.last().unwrap() } else { passengers[j] };
        while j < passengers.len() && ans == passengers[j] {
            ans -= 1; // 往前找没人到达的时刻
            j -= 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n+m\log m)$，其中 $n$ 是 $\textit{buses}$ 的长度，$m$ 是 $\textit{passengers}$ 的长度。瓶颈在排序上。请注意，虽然代码写了二重循环，但 $j$ 最多增加 $\mathcal{O}(m)$ 次，再算上遍历 $\textit{buses}$ 的时间，所以二重循环的总循环次数是 $\mathcal{O}(n+m)$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
