# 2244. 完成所有任务需要的最少轮数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-rounds-to-complete-all-tasks/
- 题目 slug：`minimum-rounds-to-complete-all-tasks`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.1 基础
- 难度分：1372
- 外部题解来源：https://leetcode.cn/problems/minimum-rounds-to-complete-all-tasks/solutions/1427626/ha-xi-biao-tan-xin-by-endlesscheng-tgtf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-rounds-to-complete-all-tasks/solutions/1427626/ha-xi-biao-tan-xin-by-endlesscheng-tgtf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ha-xi-biao-tan-xin-by-endlesscheng-tgtf`
- topic id：`1427626`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

每轮完成的都是**相同难度级别**的任务，假设难度为 $1$ 的任务有 $c$ 个，问题变成：

- 每轮可以把 $c$ 减少 $2$，或者减少 $3$。把 $c$ 减少到 $0$ 最少要多少轮？

例如 $c=10$ 时，$3+3+2+2=10$，最少要 $4$ 轮。

贪心地想，尽量多地使用「减少 $3$」，可以让轮数尽量少。

分类讨论：

- 如果 $c=1$，无法完成，返回 $-1$。
- 如果 $c=3k\ (k\ge 1)$，只用「减少 $3$」就能完成，轮数为 $\dfrac{c}{3}$。
- 如果 $c=3k+1\ (k\ge 1)$，即 $c=3k'+4\ (k'\ge 0)$，我们可以先把 $c$ 减少到 $4$，然后使用两次「减少 $2$」，轮数为 $\dfrac{c-4}{3} + 2 =\dfrac{c+2}{3} = \left\lceil\dfrac{c}{3}\right\rceil$。
- 如果 $c=3k+2\ (k\ge 0)$，我们可以先把 $c$ 减少到 $2$，然后使用一次「减少 $2$」，轮数为 $\dfrac{c-2}{3} + 1 = \dfrac{c+1}{3} = \left\lceil\dfrac{c}{3}\right\rceil$。

综上所述，对于 $c\ (c\ge 2)$ 个相同难度级别的任务，最少需要操作

$$
\left\lceil\dfrac{c}{3}\right\rceil = \left\lfloor\dfrac{c+2}{3}\right\rfloor
$$

次。

用哈希表统计不同难度任务的个数，按照上式计算轮数，累加轮数即为答案。

```py [sol-Python3]
class Solution:
    def minimumRounds(self, tasks: List[int]) -> int:
        cnt = Counter(tasks)
        if 1 in cnt.values():
            return -1
        return sum((c + 2) // 3 for c in cnt.values())
```

```java [sol-Java]
class Solution {
    public int minimumRounds(int[] tasks) {
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int t : tasks) {
            cnt.merge(t, 1, Integer::sum);
        }
        int ans = 0;
        for (int c : cnt.values()) {
            if (c == 1) {
                return -1;
            }
            ans += (c + 2) / 3;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumRounds(vector<int>& tasks) {
        unordered_map<int, int> cnt;
        for (int t : tasks) {
            cnt[t]++;
        }
        int ans = 0;
        for (auto& [_, c] : cnt) {
            if (c == 1) {
                return -1;
            }
            ans += (c + 2) / 3;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumRounds(tasks []int) int {
    cnt := map[int]int{}
    for _, t := range tasks {
        cnt[t]++
    }
    ans := 0
    for _, c := range cnt {
        if c == 1 {
            return -1
        }
        ans += (c + 2) / 3
    }
    return ans
}
```

```js [sol-JavaScript]
var minimumRounds = function(tasks) {
    const cnt = new Map();
    for (const t of tasks) {
        cnt.set(t, (cnt.get(t) ?? 0) + 1);
    }
    let ans = 0;
    for (const c of cnt.values()) {
        if (c === 1) {
            return -1;
        }
        ans += Math.ceil(c / 3);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn minimum_rounds(tasks: Vec<i32>) -> i32 {
        let mut cnt = HashMap::new();
        for t in tasks {
            *cnt.entry(t).or_insert(0) += 1;
        }
        let mut ans = 0;
        for &c in cnt.values() {
            if c == 1 {
                return -1;
            }
            ans += (c + 2) / 3;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{tasks}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

额外输入两个正整数 $a$ 和 $b$，表示每一轮可以完成 $a$ 个或者 $b$ 个相同难度级别的任务。哪些情况应该输出 $-1$？完成所有任务需要的最少轮数是多少？

欢迎在评论区分享你的思路/代码。

**相关题目**：[P3951 [NOIP2017 提高组] 小凯的疑惑 / [蓝桥杯 2013 省] 买不到的数目](https://www.luogu.com.cn/problem/P3951)

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

本题来自 `四、数学贪心 / §4.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
