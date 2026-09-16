# 3143. 正方形中的最多点数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-points-inside-the-square/
- 题目 slug：`maximum-points-inside-the-square`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.3 二分间接值
- 难度分：1697
- 外部题解来源：https://leetcode.cn/problems/maximum-points-inside-the-square/solutions/2775310/er-fen-bian-chang-tong-shi-ji-lu-da-an-z-92w5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从 O(nlog) 到 O(n)（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-points-inside-the-square/solutions/2775310/er-fen-bian-chang-tong-shi-ji-lu-da-an-z-92w5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-bian-chang-tong-shi-ji-lu-da-an-z-92w5`
- topic id：`2775310`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分间接值

由于正方形边长越大，越不合法，有单调性，所以可以**二分边长的一半**。

在二分中统计遇到的字符，如果没有遇到重复的字符，说明正方形合法，用字符个数更新答案的最大值。

关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

代码用到了一些位运算技巧，原理请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

⚠**注意**：虽然本题 $\text{check}(0)$ 一定是 $\texttt{true}$，但由于代码的做法是在 $\text{check}$ 中更新答案，为了保证在正方形边长为 $0$ 的情况下也能更新答案，二分范围要包含 $0$。

#### 答疑

**问**：为什么可以直接更新 $\textit{ans}$，为什么不需要写 `ans = max(ans, ...)`？

**答**：更新 $\textit{ans}$ 时必然会伴随着二分区间左边界 $\textit{left}$ 的更新，那么下一次更新 $\textit{ans}$ 的时候，正方形的边长一定更长，包含的点数不会变少，所以可以直接更新，无需写 `max`。

```py [sol-Python3]
class Solution:
    def maxPointsInsideSquare(self, points: List[List[int]], s: str) -> int:
        ans = 0
        def check(size: int) -> bool:
            vis = set()
            for (x, y), c in zip(points, s):
                if abs(x) <= size and abs(y) <= size:  # 点在正方形中
                    if c in vis:
                        return True
                    vis.add(c)
            nonlocal ans
            ans = len(vis)
            return False

        # 注意 range 并不会创建 list，它是 O(1) 的
        bisect_left(range(1_000_000_001), True, key=check)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int maxPointsInsideSquare(int[][] points, String S) {
        char[] s = S.toCharArray();
        int left = -1;
        int right = 1_000_000_001;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (check(mid, points, s)) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return ans;
    }

    boolean check(int size, int[][] points, char[] s) {
        int vis = 0;
        for (int i = 0; i < points.length; i++) {
            // 判断点是否在正方形中
            if (Math.abs(points[i][0]) <= size && Math.abs(points[i][1]) <= size) {
                int c = s[i] - 'a';
                if ((vis >> c & 1) > 0) { // c 在集合中
                    return false;
                }
                vis |= 1 << c; // 把 c 加入集合
            }
        }
        ans = Integer.bitCount(vis);
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxPointsInsideSquare(vector<vector<int>>& points, string s) {
        int ans = 0;

        auto check = [&](int size) -> bool {
            int vis = 0;
            for (int i = 0; i < points.size(); i++) {
                // 判断点是否在正方形中
                if (abs(points[i][0]) <= size && abs(points[i][1]) <= size) {
                    char c = s[i] - 'a';
                    if (vis >> c & 1) { // c 在集合中
                        return false;
                    }
                    vis |= 1 << c; // 把 c 加入集合
                }
            }
            ans = popcount(1u * vis);
            return true;
        };

        int left = -1, right = 1'000'000'001;
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            (check(mid) ? left : right) = mid;
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxPointsInsideSquare(points [][]int, s string) (ans int) {
    sort.Search(1_000_000_001, func(size int) bool {
        vis := 0
        for i, p := range points {
            if abs(p[0]) <= size && abs(p[1]) <= size { // 点在正方形中
                c := s[i] - 'a'
                if vis>>c&1 > 0 { // c 在集合中
                    return true
                }
                vis |= 1 << c // 把 c 加入集合
            }
        }
        ans = bits.OnesCount(uint(vis))
        return false
    })
    return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为 $s$ 的长度，$U=\max(|x_i|,|y_i|)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$ 或 $\mathcal{O}(1)$。其中 $|\Sigma|$ 为字符集合的大小，本题字符均为小写字母，所以 $|\Sigma|=26$。

## 方法二：维护次小距离的最小值

定义点 $(x,y)$ 到 $(0,0)$ 的**切比雪夫距离**为

$$
\max(|x|,|y|)
$$

定义 $\textit{minD}[c]$ 为标签为 $c$ 的所有点到 $(0,0)$ 的**最小**切比雪夫距离。

定义 $\textit{minD}_2[c]$ 为标签为 $c$ 的所有点到 $(0,0)$ 的**次小**切比雪夫距离。

![lc3143.png](https://pic.leetcode.cn/1722695389-Arudlg-lc3143.png)

换句话说，可以在正方形中的点需要满足

$$
\textit{minD}[c] < \textit{min}_2
$$

代码实现时，无需维护 $\textit{minD}_2[c]$，而是直接维护 $\textit{min}_2$。

```py [sol-Python3]
class Solution:
    def maxPointsInsideSquare(self, points: List[List[int]], s: str) -> int:
        min_d = defaultdict(lambda: inf)
        min2 = inf
        for (x, y), c in zip(points, s):
            d = max(abs(x), abs(y))
            if d < min_d[c]:
                # d 是目前最小的，那么 min_d[c] 是次小的
                min2 = min(min2, min_d[c])
                min_d[c] = d
            else:
                # d 可能是次小的
                min2 = min(min2, d)
        return sum(d < min2 for d in min_d.values())
```

```java [sol-Java]
class Solution {
    public int maxPointsInsideSquare(int[][] points, String s) {
        int[] minD = new int[26];
        Arrays.fill(minD, Integer.MAX_VALUE);
        int min2 = Integer.MAX_VALUE;
        for (int i = 0; i < points.length; i++) {
            int d = Math.max(Math.abs(points[i][0]), Math.abs(points[i][1]));
            int c = s.charAt(i) - 'a';
            if (d < minD[c]) {
                // d 是目前最小的，那么 minD[c] 是次小的
                min2 = Math.min(min2, minD[c]);
                minD[c] = d;
            } else {
                // d 可能是次小的
                min2 = Math.min(min2, d);
            }
        }
        int ans = 0;
        for (int d : minD) {
            if (d < min2) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxPointsInsideSquare(vector<vector<int>>& points, string s) {
        int min_d[26], min2 = INT_MAX;
        ranges::fill(min_d, INT_MAX);
        for (int i = 0; i < points.size(); i++) {
            int d = max(abs(points[i][0]), abs(points[i][1]));
            char c = s[i] - 'a';
            if (d < min_d[c]) {
                // d 是目前最小的，那么 min_d[c] 是次小的
                min2 = min(min2, min_d[c]);
                min_d[c] = d;
            } else {
                // d 可能是次小的
                min2 = min(min2, d);
            }
        }
        int ans = 0;
        for (int d : min_d) {
            ans += d < min2;
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxPointsInsideSquare(points [][]int, s string) (ans int) {
    minD := [26]int{}
    for i := range minD {
        minD[i] = math.MaxInt
    }
    min2 := math.MaxInt
    for i, p := range points {
        d := max(abs(p[0]), abs(p[1]))
        c := s[i] - 'a'
        if d < minD[c] {
            // d 是目前最小的，那么 minD[c] 是次小的
            min2 = min(min2, minD[c])
            minD[c] = d
        } else {
            // d 可能是次小的
            min2 = min(min2, d)
        }
    }
    for _, d := range minD {
        if d < min2 {
            ans++
        }
    }
    return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|)$，其中 $n$ 为 $s$ 的长度，$|\Sigma|$ 为字符集合的大小，本题字符均为小写字母，所以 $|\Sigma|=26$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.3 二分间接值`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.3 二分间接值`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
