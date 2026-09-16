# 3133. 数组最后一个元素的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-array-end/
- 题目 slug：`minimum-array-end`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质
- 难度分：1935
- 外部题解来源：https://leetcode.cn/problems/minimum-array-end/solutions/2759113/wei-yun-suan-jian-ji-xie-fa-pythonjavacg-nw8t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[位运算，两种简洁写法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-array-end/solutions/2759113/wei-yun-suan-jian-ji-xie-fa-pythonjavacg-nw8t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wei-yun-suan-jian-ji-xie-fa-pythonjavacg-nw8t`
- topic id：`2759113`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请先阅读：[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

从集合的视角看，$x$ 是每个 $\textit{nums}[i]$ 的**子集**。换句话说，$\textit{nums}[i]$ 一定是 $x$ 的**超集**。

例如 $x = 100100$，那么 $\textit{nums}[i]$ 一定在如下序列中：

$$
1\underline{00}1\underline{00},1\underline{00}1\underline{01},1\underline{00}1\underline{10},1\underline{00}1\underline{11},1\underline{01}1\underline{00},1\underline{01}1\underline{01},\cdots
$$

只看下划线上的数，是一个自然数序列

$$
0000,0001,0010,0011,0100,0101,\cdots
$$

为了让 $\textit{nums}[n-1]$ 尽量小，我们应当选择 $x$ 的超集中最小的 $n$ 个数。

所以把 $x$ 的二进制中的 $0$ 视作「空位」，把 $n-1$ 二进制的每个比特逐个填入空位，即为最小的 $\textit{nums}[n-1]$。

如果空位不足，往 $x$ 的前面添加前导零即可。

请看 [视频讲解](https://www.bilibili.com/video/BV1Pw4m1C79N/) 第三题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def minEnd(self, n: int, x: int) -> int:
        n -= 1  # 先把 n 减一，这样下面讨论的 n 就是原来的 n-1
        i = j = 0
        while n >> j:
            # x 的第 i 个比特值是 0，即「空位」
            if (x >> i & 1) == 0:
                # 空位填入 n 的第 j 个比特值
                x |= (n >> j & 1) << i
                j += 1
            i += 1
        return x
```

```java [sol-Java]
class Solution {
    public long minEnd(int n, int x) {
        n--; // 先把 n 减一，这样下面讨论的 n 就是原来的 n-1
        long ans = x;
        int i = 0, j = 0;
        while ((n >> j) > 0) {
            // x 的第 i 个比特值是 0，即「空位」
            if ((ans >> i & 1) == 0) {
                // 空位填入 n 的第 j 个比特值
                ans |= (long) (n >> j & 1) << i;
                j++;
            }
            i++;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minEnd(int n, int x) {
        n--; // 先把 n 减一，这样下面讨论的 n 就是原来的 n-1
        long long ans = x;
        int i = 0, j = 0;
        while (n >> j) {
            // x 的第 i 个比特值是 0，即「空位」
            if ((ans >> i & 1) == 0) {
                // 空位填入 n 的第 j 个比特值
                ans |= (long long) (n >> j & 1) << i;
                j++;
            }
            i++;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minEnd(n, x int) int64 {
	n-- // 先把 n 减一，这样下面讨论的 n 就是原来的 n-1
	i, j := 0, 0
	for n>>j > 0 {
		// x 的第 i 个比特值是 0，即「空位」
		if x>>i&1 == 0 {
			// 空位填入 n 的第 j 个比特值
			x |= n >> j & 1 << i
			j++
		}
		i++
	}
	return int64(x)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log x + \log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 优化

把 $x$ 取反，用 lowbit 枚举其中的 $1$，就是要填的空位。

```py [sol-Python3]
class Solution:
    def minEnd(self, n: int, x: int) -> int:
        n -= 1
        j = 0
        t = ~x
        while n >> j:
            lb = t & -t
            x |= (n >> j & 1) * lb
            j += 1
            t ^= lb
        return x
```

```java [sol-Java]
class Solution {
    public long minEnd(int n, int x) {
        n--;
        long ans = x;
        int j = 0;
        for (long t = ~x, lb; (n >> j) > 0; t ^= lb) {
            lb = t & -t;
            ans |= (long) (n >> j++ & 1) * lb;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minEnd(int n, int x) {
        n--;
        long long ans = x;
        int j = 0;
        for (long long t = ~x, lb; n >> j; t ^= lb) {
            lb = t & -t;
            ans |= (long long) (n >> j++ & 1) * lb;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minEnd(n, x int) int64 {
	n--
	j := 0
	for t, lb := ^x, 0; n>>j > 0; t ^= lb {
		lb = t & -t
		x |= n >> j & 1 * lb
		j++
	}
	return int64(x)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。循环次数只和入参 $n$ 有关。
- 空间复杂度：$\mathcal{O}(1)$。

## 更快的做法？

请看《Hacker's Delight》第 7.5 节。

## 思考题

额外输入一个 $\textit{forbidden}$ 数组，表示禁止出现在 $\textit{nums}$ 中的数。

在这种额外约束下，$\textit{nums}[n-1]$ 的最小值是多少？

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

本题来自 `三、与或（AND/OR）的性质`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、与或（AND/OR）的性质`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
