# 1423. 可获得的最大点数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-points-you-can-obtain-from-cards/
- 题目 slug：`maximum-points-you-can-obtain-from-cards`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.1 基础
- 难度分：1574
- 外部题解来源：https://leetcode.cn/problems/maximum-points-you-can-obtain-from-cards/solutions/2551432/liang-chong-fang-fa-ni-xiang-si-wei-zhen-e3gb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：逆向思维/正向思维（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-points-you-can-obtain-from-cards/solutions/2551432/liang-chong-fang-fa-ni-xiang-si-wei-zhen-e3gb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-ni-xiang-si-wei-zhen-e3gb`
- topic id：`2551432`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：逆向思维

拿走 $k$ 张，剩下 $n-k$ 张。这里 $n$ 是 $\textit{cardPoints}$ 的长度。

由于拿走的点数和 + 剩下的点数和 = 所有点数和 = 常数，所以为了最大化拿走的点数和，应当**最小化剩下的点数和**。

由于只能从开头或末尾拿牌，所以最后剩下的牌必然是**连续**的。

至此，问题变成：

- 计算长为 $n-k$ 的连续子数组和的最小值。

这可以用**定长滑动窗口**解决。

### 算法

1. 设 $m=n-k$，计算第一个长为 $m$ 的子数组元素和，即 $s = \textit{cardPoints}[0] + \textit{cardPoints}[1] + \cdots + \textit{cardPoints}[m-1]$。初始化 $\textit{minS}=s$。
2. 计算下一个子数组的元素和，即 $s' = \textit{cardPoints}[1] + \textit{cardPoints}[2] + \cdots + \textit{cardPoints}[m]$。由于 $s'-s = \textit{cardPoints}[m]-\textit{cardPoints}[0]$，所以只需要把 $s$ 增加 $\textit{cardPoints}[m]-\textit{cardPoints}[0]$，就可以 $\mathcal{O}(1)$ 算出下一个子数组的元素和。
3. 依照这个方法，从 $i=m$ 开始向后枚举，每次把 $s$ 增加 $\textit{cardPoints}[i]-\textit{cardPoints}[i-m]$，然后用 $s$ 更新 $\textit{minS}$ 的最小值。
4. 最后，用 $\textit{cardPoints}$ 的元素和，减去 $\textit{minS}$，就得到了答案。

**注**：若用 [模板](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/) 中的写法，需要特判 $m=0$（也就是 $n=k$）的情况，此时直接返回 $\textit{cardPoints}$ 的元素和。

```py [sol-Python3]
class Solution:
    def maxScore(self, cardPoints: List[int], k: int) -> int:
        n = len(cardPoints)
        m = n - k
        min_s = s = sum(cardPoints[:m])
        for i in range(m, n):
            s += cardPoints[i] - cardPoints[i - m]
            min_s = min(min_s, s)
        return sum(cardPoints) - min_s
```

```py [sol-Python3 写法二]
class Solution:
    def maxScore(self, cardPoints: List[int], k: int) -> int:
        m = len(cardPoints) - k
        min_s = s = sum(cardPoints[:m])
        for in_, out in zip(cardPoints[m:], cardPoints):
            s += in_ - out
            min_s = min(min_s, s)
        return sum(cardPoints) - min_s
```

```java [sol-Java]
class Solution {
    public int maxScore(int[] cardPoints, int k) {
        int n = cardPoints.length;
        int m = n - k;
        int s = 0;
        for (int i = 0; i < m; i++) {
            s += cardPoints[i];
        }

        int total = s;
        int minS = s;
        for (int i = m; i < n; i++) {
            total += cardPoints[i];
            s += cardPoints[i] - cardPoints[i - m];
            minS = Math.min(minS, s);
        }
        return total - minS;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxScore(vector<int>& cardPoints, int k) {
        int n = cardPoints.size();
        int m = n - k;
        int s = reduce(cardPoints.begin(), cardPoints.begin() + m);
        int min_s = s;
        for (int i = m; i < n; i++) {
            s += cardPoints[i] - cardPoints[i - m];
            min_s = min(min_s, s);
        }
        return reduce(cardPoints.begin(), cardPoints.end()) - min_s;
    }
};
```

```go [sol-Go]
func maxScore(cardPoints []int, k int) int {
    n := len(cardPoints)
    m := n - k
    s := 0
    for _, x := range cardPoints[:m] {
        s += x
    }
    total := s
    minS := s
    for i := m; i < n; i++ {
        total += cardPoints[i]
        s += cardPoints[i] - cardPoints[i-m]
        minS = min(minS, s)
    }
    return total - minS
}
```

```js [sol-JavaScript]
var maxScore = function (cardPoints, k) {
    const n = cardPoints.length;
    const m = n - k;
    let s = 0;
    for (let i = 0; i < m; i++) {
        s += cardPoints[i];
    }
    let total = s;
    let minS = s;
    for (let i = m; i < n; i++) {
        total += cardPoints[i];
        s += cardPoints[i] - cardPoints[i - m];
        minS = Math.min(minS, s);
    }
    return total - minS;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_score(card_points: Vec<i32>, k: i32) -> i32 {
        let n = card_points.len();
        let m = n - k as usize;
        let mut s = card_points.iter().take(m).sum::<i32>();
        let mut min_s = s;
        for i in m..n {
            s += card_points[i] - card_points[i - m];
            min_s = min_s.min(s);
        }
        card_points.iter().sum::<i32>() - min_s
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{cardPoints}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。Python 忽略切片开销。

## 方法二：正向思维

答案等于如下结果的最大值：

- 前 $k$ 个数的和。
- 前 $k-1$ 个数以及后 $1$ 个数的和。
- 前 $k-2$ 个数以及后 $2$ 个数的和。
- ……
- 前 $2$ 个数以及后 $k-2$ 个数的和。
- 前 $1$ 个数以及后 $k-1$ 个数的和。
- 后 $k$ 个数的和。

### 算法

1. 计算前 $k$ 个数的和，记作 $s$。初始化答案 $\textit{ans}=s$。
2. 从 $i=1$ 开始枚举到 $i=k$。
3. 每次枚举，把 $s$ 增加 $\textit{cardPoints}[n-i] - \textit{cardPoints}[k-i]$，然后更新 $\textit{ans}$ 的最大值。
4. 返回 $\textit{ans}$。

```py [sol-Python3]
class Solution:
    def maxScore(self, cardPoints: List[int], k: int) -> int:
        ans = s = sum(cardPoints[:k])
        for i in range(1, k + 1):
            s += cardPoints[-i] - cardPoints[k - i]
            ans = max(ans, s)
        return ans
```

```py [sol-Python3 zip]
class Solution:
    def maxScore(self, cardPoints: List[int], k: int) -> int:
        ans = s = sum(cardPoints[-k:])  # 为方便下面 zip，改为先计算后 k 个数的和
        for x, y in zip(cardPoints, cardPoints[-k:]):
            s += x - y
            ans = max(ans, s)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxScore(int[] cardPoints, int k) {
        int s = 0;
        for (int i = 0; i < k; i++) {
            s += cardPoints[i];
        }
        int ans = s;
        for (int i = 1; i <= k; i++) {
            s += cardPoints[cardPoints.length - i] - cardPoints[k - i];
            ans = Math.max(ans, s);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxScore(vector<int>& cardPoints, int k) {
        int s = reduce(cardPoints.begin(), cardPoints.begin() + k);
        int ans = s;
        for (int i = 1; i <= k; i++) {
            s += cardPoints[cardPoints.size() - i] - cardPoints[k - i];
            ans = max(ans, s);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxScore(cardPoints []int, k int) int {
    s := 0
    for _, x := range cardPoints[:k] {
        s += x
    }
    ans := s
    for i := 1; i <= k; i++ {
        s += cardPoints[len(cardPoints)-i] - cardPoints[k-i]
        ans = max(ans, s)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxScore = function (cardPoints, k) {
    let s = 0;
    for (let i = 0; i < k; i++) {
        s += cardPoints[i];
    }
    let ans = s;
    for (let i = 1; i <= k; i++) {
        s += cardPoints[cardPoints.length - i] - cardPoints[k - i];
        ans = Math.max(ans, s);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_score(card_points: Vec<i32>, k: i32) -> i32 {
        let k = k as usize;
        let mut s = card_points.iter().take(k).sum::<i32>();
        let mut ans = s;
        for i in 1..=k {
            s += card_points[card_points.len() - i] - card_points[k - i];
            ans = ans.max(s);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k)$。
- 空间复杂度：$\mathcal{O}(1)$。Python 忽略切片开销。

## 思考题

把题目改成：拿走的卡牌数量无限制，但是 $\textit{cardPoints}$ 中有负数。如何求出可以获得的最大点数和？

欢迎在评论区分享你的思路。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、定长滑动窗口 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
