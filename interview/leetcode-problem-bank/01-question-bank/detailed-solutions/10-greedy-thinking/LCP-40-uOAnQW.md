# LCP 40. 心算挑战

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/uOAnQW/
- 题目 slug：`uOAnQW`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/uOAnQW/solutions/991699/pai-xu-tan-xin-by-endlesscheng-wgk7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序+贪心（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/uOAnQW/solutions/991699/pai-xu-tan-xin-by-endlesscheng-wgk7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-tan-xin-by-endlesscheng-wgk7`
- topic id：`991699`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

## 思路

为了选取尽量大的数，将 $\textit{cards}$ **从大到小**排序，并累加前 $\textit{cnt}$ 个数，记作 $s$。

分类讨论：

- 如果 $s$ 是偶数，这是我们能得到的最大得分，直接返回 $s$。
- 如果 $s$ 是奇数，那么可以：
   - 从前 $\textit{cnt}$ 个数中去掉一个**最小的奇数**，从后 $n-\textit{cnt}$ 个数中加进来一个**最大的偶数**，这样得分就变成偶数了。
   - 从前 $\textit{cnt}$ 个数中去掉一个**最小的偶数**，从后 $n-\textit{cnt}$ 个数中加进来一个**最大的奇数**，这样得分就变成偶数了。
   - 两种情况取最大值。

## 细节

讨论 $s$ 是奇数时的细节。

在前 $\textit{cnt}$ 个数中，$x=\textit{cards}[\textit{cnt}-1]$ 要么是最小的奇数，要么是最小的偶数，所以上面说的两种情况，一定有一种情况要替换 $x$。

在前 $\textit{cnt}$ 个数中的另一个被替换的数，就是奇偶性和 $x$ 不同的最小元素了。

## 答疑

**问**：为什么只替换一个数就行，替换多个数一定不优吗？

**答**：如果 $s$ 是奇数，要替换成偶数，去掉的数与加进来的数中，一定有奇偶性不同的，这样的数只需要一对就行，多余的数替换掉只会让 $s$ 变小。

**问**：能做到 $\mathcal{O}(n)$ 时间吗？

**答**：可以，把排序换成快速选择算法，详见 C++。

```py [sol-Python3]
class Solution:
    def maximumScore(self, cards: List[int], cnt: int) -> int:
        cards.sort(reverse=True)
        s = sum(cards[:cnt])  # 最大的 cnt 个数之和
        if s % 2 == 0:  # s 是偶数
            return s

        def replaced_sum(x: int) -> int:
            for v in cards[cnt:]:
                if v % 2 != x % 2:  # 找到一个最大的奇偶性和 x 不同的数
                    return s - x + v  # 用 v 替换 s
            return 0

        x = cards[cnt - 1]
        ans = replaced_sum(x)  # 替换 x
        for v in cards[cnt - 1::-1]:
            if v % 2 != x % 2:  # 找到一个最小的奇偶性和 x 不同的数
                ans = max(ans, replaced_sum(v))  # 替换
                break
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumScore(int[] cards, int cnt) {
        Arrays.sort(cards);
        int n = cards.length;
        int s = 0;
        for (int i = n - cnt; i < n; i++) {
            s += cards[i]; // 最大的 cnt 个数之和
        }
        if (s % 2 == 0) { // s 是偶数
            return s;
        }

        int x = cards[n - cnt];
        int ans = replacedSum(cards, cnt, s, x); // 替换 x
        for (int i = n - cnt + 1; i < n; i++) {
            if (cards[i] % 2 != x % 2) { // 找到一个最小的奇偶性和 x 不同的数
                ans = Math.max(ans, replacedSum(cards, cnt, s, cards[i])); // 替换
                break;
            }
        }
        return ans;
    }

    private int replacedSum(int[] cards, int cnt, int s, int x) {
        for (int i = cards.length - cnt - 1; i >= 0; i--) {
            if (cards[i] % 2 != x % 2) { // 找到一个最大的奇偶性和 x 不同的数
                return s - x + cards[i]; // 用 cards[i] 替换 s
            }
        }
        return 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumScore(vector<int>& cards, int cnt) {
        ranges::sort(cards, greater());
        int s = reduce(cards.begin(), cards.begin() + cnt); // 最大的 cnt 个数之和
        if (s % 2 == 0) { // s 是偶数
            return s;
        }

        auto replaced_sum = [&](int x) -> int {
            for (int i = cnt; i < cards.size(); i++) {
                if (cards[i] % 2 != x % 2) { // 找到一个最大的奇偶性和 x 不同的数
                    return s - x + cards[i]; // 用 cards[i] 替换 s
                }
            }
            return 0;
        };

        int x = cards[cnt - 1];
        int ans = replaced_sum(x); // 替换 x
        for (int i = cnt - 2; i >= 0; i--) { // 前 cnt-1 个数
            if (cards[i] % 2 != x % 2) { // 找到一个最小的奇偶性和 x 不同的数
                ans = max(ans, replaced_sum(cards[i])); // 替换
                break;
            }
        }
        return ans;
    }
};
```

```cpp [sol-C++ 快速选择]
class Solution {
public:
    int maximumScore(vector<int>& cards, int cnt) {
        ranges::nth_element(cards, cards.end() - cnt); // 快速选择
        int s = reduce(cards.end() - cnt, cards.end()); // 最大的 cnt 个数之和
        if (s % 2 == 0) { // s 是偶数
            return s;
        }

        int n = cards.size();
        // 加进来的最大偶数/奇数
        int mx[2] = {INT_MIN / 2, INT_MIN / 2}; // 除 2 防止最下面减法溢出
        for (int i = 0; i < n - cnt; i++) {
            int v = cards[i];
            mx[v % 2] = max(mx[v % 2], v);
        }

        // 要去掉的最小偶数/奇数
        int mn[2] = {INT_MAX / 2, INT_MAX / 2};
        for (int i = n - cnt; i < n; i++) {
            int v = cards[i];
            mn[v % 2] = min(mn[v % 2], v);
        }

        return max(s + max(mx[0] - mn[1], mx[1] - mn[0]), 0);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return *(int*)b - *(int*)a;
}

int replaced_sum(int* cards, int cardsSize, int cnt, int s, int x) {
    for (int i = cnt; i < cardsSize; i++) {
        if (cards[i] % 2 != x % 2) { // 找到一个最大的奇偶性和 x 不同的数
            return s - x + cards[i]; // 用 cards[i] 替换 s
        }
    }
    return 0;
}

int maximumScore(int* cards, int cardsSize, int cnt) {
    qsort(cards, cardsSize, sizeof(int), cmp);
    int s = 0;
    for (int i = 0; i < cnt; i++) {
        s += cards[i]; // 最大的 cnt 个数之和
    }
    if (s % 2 == 0) { // s 是偶数
        return s;
    }

    int x = cards[cnt - 1];
    int ans = replaced_sum(cards, cardsSize, cnt, s, x); // 替换 x
    for (int i = cnt - 2; i >= 0; i--) { // 前 cnt-1 个数
        if (cards[i] % 2 != x % 2) { // 找到一个最小的奇偶性和 x 不同的数
            ans = MAX(ans, replaced_sum(cards, cardsSize, cnt, s, cards[i])); // 替换
            break;
        }
    }
    return ans;
}
```

```go [sol-Go]
func maximumScore(cards []int, cnt int) int {
    slices.SortFunc(cards, func(a, b int) int { return b - a })
    s := 0
    for _, v := range cards[:cnt] {
        s += v
    }
    if s%2 == 0 { // s 是偶数
        return s
    }

    replacedSum := func(x int) int {
        for _, v := range cards[cnt:] {
            if v%2 != x%2 { // 找到一个最大的奇偶性和 x 不同的数
                return s - x + v // 用 v 替换 s
            }
        }
        return 0
    }

    x := cards[cnt-1]
    ans := replacedSum(x) // 替换 x
    for i := cnt - 2; i >= 0; i-- { // 前 cnt-1 个数
        if cards[i]%2 != x%2 { // 找到一个最小的奇偶性和 x 不同的数
            ans = max(ans, replacedSum(cards[i])) // 替换
            break
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var maximumScore = function(cards, cnt) {
    cards.sort((a, b) => b - a);
    let s = _.sum(cards.slice(0, cnt)); // 最大的 cnt 个数之和
    if (s % 2 === 0) { // s 是偶数
        return s;
    }

    function replacedSum(x) {
        for (let i = cnt; i < cards.length; i++) {
            if (cards[i] % 2 !== x % 2) { // 找到一个最大的奇偶性和 x 不同的数
                return s - x + cards[i]; // 用 cards[i] 替换 s
            }
        }
        return 0;
    }

    let x = cards[cnt - 1];
    let ans = replacedSum(x); // 替换 x
    for (let i = cnt - 2; i >= 0; i--) { // 前 cnt-1 个数
        if (cards[i] % 2 !== x % 2) { // 找到一个最小的奇偶性和 x 不同的数
            ans = Math.max(ans, replacedSum(cards[i])); // 替换
            break;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_score(mut cards: Vec<i32>, cnt: i32) -> i32 {
        cards.sort_unstable_by(|a, b| b.cmp(a));
        let cnt = cnt as usize;
        let s = cards[..cnt].iter().sum::<i32>(); // 最大的 cnt 个数之和
        if s % 2 == 0 { // s 是偶数
            return s;
        }

        let replaced_sum = |x: i32| -> i32 {
            for &v in cards[cnt..].iter() {
                if v % 2 != x % 2 { // 找到一个最大的奇偶性和 x 不同的数
                    return s - x + v; // 用 cards[i] 替换 s
                }
            }
            0
        };

        let x = cards[cnt - 1];
        let mut ans = replaced_sum(x); // 替换 x
        for &v in cards[..cnt - 1].iter().rev() { // 前 cnt-1 个数
            if v % 2 != x % 2 { // 找到一个最小的奇偶性和 x 不同的数
                ans = ans.max(replaced_sum(v)); // 替换
                break;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。瓶颈在排序上，用快速选择算法可以做到 $\mathcal{O}(n)$，详见 C++。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销和切片开销。

## 思考题

如果要求元素和必须是 $3$ 的倍数呢？

见 [1262. 可被三整除的最大和](https://leetcode.cn/problems/greatest-sum-divisible-by-three/)。

更多相似题目，见下面贪心题单中的「**从最小/最大开始贪心**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
