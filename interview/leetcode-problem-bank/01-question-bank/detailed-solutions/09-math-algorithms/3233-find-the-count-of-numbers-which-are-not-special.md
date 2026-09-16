# 3233. 统计不是特殊数字的数字数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-count-of-numbers-which-are-not-special/
- 题目 slug：`find-the-count-of-numbers-which-are-not-special`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.2 预处理质数（筛质数）
- 难度分：1509
- 外部题解来源：https://leetcode.cn/problems/find-the-count-of-numbers-which-are-not-special/solutions/2860276/yu-chu-li-zhi-shu-o1hui-da-pythonjavacgo-7xaq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理质数+O(1)回答（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-count-of-numbers-which-are-not-special/solutions/2860276/yu-chu-li-zhi-shu-o1hui-da-pythonjavacgo-7xaq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-zhi-shu-o1hui-da-pythonjavacgo-7xaq`
- topic id：`2860276`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

正难则反，统计区间 $[l,r]$ 内有多少个特殊数字。

这等价于区间 $[0,r]$ 内的特殊数字个数，减去区间 $[0,l-1]$ 内的特殊数字个数。

根据题意，只有质数的平方 $p^2$ 才是特殊数字，因为 $p^2$ 恰好有两个真因数 $1$ 和 $p$。而其他的数，$1$ 没有真因数，质数只有 $1$ 个真因数，不是 $1$ 不是质数也不是质数平方的数有至少三个真因数。

所以区间 $[0,i]$ 内的特殊数字个数等于：

- 区间 $[0,\lfloor\sqrt{i}\rfloor]$ 中的质数个数。

预处理 $\lfloor\sqrt{10^9}\rfloor = 31622$ 内的质数，然后用前缀和计算 $[0,i]$ 中的质数个数 $\pi(i)$，那么区间 $[l,r]$ 内的特殊数字个数就是

$$
\pi(\lfloor\sqrt{r}\rfloor) - \pi(\lfloor\sqrt{l-1}\rfloor)
$$

答案为区间内的整数个数，减去区间内的特殊数字个数，即

$$
r-l+1 - \left(\pi(\lfloor\sqrt{r}\rfloor) - \pi(\lfloor\sqrt{l-1}\rfloor)\right)
$$

如何筛质数？请看 [视频讲解](https://www.bilibili.com/video/BV1Mi421a7cZ/) 第二题，欢迎点赞关注~

```py [sol-Python3]
MX = 31622
pi = [0] * (MX + 1)
for i in range(2, MX + 1):
    if pi[i] == 0:  # i 是质数
        pi[i] = pi[i - 1] + 1
        for j in range(i * i, MX + 1, i):
            pi[j] = -1  # 标记 i 的倍数为合数
    else:
        pi[i] = pi[i - 1]

class Solution:
    def nonSpecialCount(self, l: int, r: int) -> int:
        return r - l + 1 - (pi[isqrt(r)] - pi[isqrt(l - 1)])
```

```java [sol-Java]
class Solution {
    private static final int MX = 31622;
    private static final int[] PI = new int[MX + 1];

    static {
        for (int i = 2; i <= MX; i++) {
            if (PI[i] == 0) { // i 是质数
                PI[i] = PI[i - 1] + 1;
                for (int j = i * i; j <= MX; j += i) { // 注：如果 MX 比较大，小心 i*i 溢出
                    PI[j] = -1; // 标记 i 的倍数为合数
                }
            } else {
                PI[i] = PI[i - 1];
            }
        }
    }

    public int nonSpecialCount(int l, int r) {
        return r - l + 1 - (PI[(int) Math.sqrt(r)] - PI[(int) Math.sqrt(l - 1)]);
    }
}
```

```cpp [sol-C++]
const int MX = 31622;
int pi[MX + 1];

auto init = [] {
    for (int i = 2; i <= MX; i++) {
        if (pi[i] == 0) { // i 是质数
            pi[i] = pi[i - 1] + 1;
            for (int j = i * i; j <= MX; j += i) { // 注：如果 MX 比较大，小心 i*i 溢出
                pi[j] = -1; // 标记 i 的倍数为合数
            }
        } else {
            pi[i] = pi[i - 1];
        }
    }
    return 0;
}();

class Solution {
public:
    int nonSpecialCount(int l, int r) {
        return r - l + 1 - (pi[(int) sqrt(r)] - pi[(int) sqrt(l - 1)]);
    }
};
```

```go [sol-Go]
const mx = 31622
var pi [mx + 1]int

func init() {
    for i := 2; i <= mx; i++ {
        if pi[i] == 0 { // i 是质数
            pi[i] = pi[i-1] + 1
            for j := i * i; j <= mx; j += i {
                pi[j] = -1 // 标记 i 的倍数为合数
            }
        } else {
            pi[i] = pi[i-1]
        }
    }
}

func nonSpecialCount(l, r int) int {
    cntR := pi[int(math.Sqrt(float64(r)))]
    cntL := pi[int(math.Sqrt(float64(l-1)))]
    return r - l + 1 - (cntR - cntL)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。不计入预处理的时间。
- 空间复杂度：$\mathcal{O}(1)$。不计入预处理的空间。

更多数学题目，见下面的数学题单。

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

本题来自 `一、数论 / §1.2 预处理质数（筛质数）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.2 预处理质数（筛质数）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
