# 2141. 同时运行 N 台电脑的最长时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-running-time-of-n-computers/
- 题目 slug：`maximum-running-time-of-n-computers`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.2 求最大
- 难度分：2265
- 外部题解来源：https://leetcode.cn/problems/maximum-running-time-of-n-computers/solutions/1214440/liang-chong-jie-fa-er-fen-da-an-pai-xu-t-grd8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案 / 排序+贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-running-time-of-n-computers/solutions/1214440/liang-chong-jie-fa-er-fen-da-an-pai-xu-t-grd8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-jie-fa-er-fen-da-an-pai-xu-t-grd8`
- topic id：`1214440`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分答案

如果可以让 $n$ 台电脑同时运行 $x$ 分钟，那么必然可以同时运行 $x-1,x-2,\ldots$ 分钟（要求更宽松）；如果无法让 $n$ 台电脑同时运行 $x$ 分钟，那么必然无法同时运行 $x+1,x+2,\ldots$ 分钟（要求更苛刻）。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

假设可以让 $n$ 台电脑同时运行 $x$ 分钟，那么对于电量大于 $x$ 的电池，其只能被使用 $x$ 分钟，因此每个电池的使用时间至多为 $\min(\textit{batteries}[i], x)$。累加所有电池的使用时间，记作 $\textit{sum}$。那么要让 $n$ 台电脑同时运行 $x$ 分钟，**必要条件**是 $n\cdot x\le \textit{sum}$。

下面证明该条件也是**充分**的，即如果 $n\cdot x\le \textit{sum}$ 成立，那么一定存在一种安排电池的方式，可以让 $n$ 台电脑同时运行 $x$ 分钟。

构造方法如下：

对于电量 $\ge x$ 的电池，我们可以让其给一台电脑供电 $x$ 分钟。由于一个电池不能同时给多台电脑供电，因此该电池若给一台电脑供电 $x$ 分钟，那它就不能用于其他电脑了（我们只有 $x$ 分钟，无论电池有多少电，电池只能用 $x$ 分钟，剩余的电再多也没机会用）。我们可以将所有电量 $\ge x$ 的电池各给一台电脑供电。

对于其余电池，设其电量和为 $\textit{sum}'$，剩余 $n'$ 台电脑未被供电。我们可以随意选择剩下的电池，供给剩余的第一台电脑（用完一个电池就换下一个电池），多余的电池电量与剩下的电池一起供给剩余的第二台电脑，依此类推。注意由于这些电池的电量均小于 $x$，按照这种做法是不会出现同一个电池在同一时间供给多台电脑的（如果某个电池供给了两台电脑，可以将这个电池的供电时间**划分到第一台电脑的末尾和第二台电脑的开头**，因为电池电量小于 $x$，所以时间不会重叠）。注：想象我们把其余电池连成一条线，每隔 $x$ 切一刀，每段分给一台电脑。

由于 $\textit{sum}'=\textit{sum}-(n-n')\cdot x$，结合 $n\cdot x\le \textit{sum}$ 可以得到 $n'\cdot x\le \textit{sum}'$，按照上述供电方案（用完一个电池就换下一个电池），这 $n'$ 台电脑可以运行至少 $x$ 分钟。充分性得证。

### 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$0$。不运行任何电脑，一定满足要求。
- 开区间右端点初始值：平均值加一，即 $\left\lfloor\dfrac{\sum \textit{batteries}[i]}{n}\right\rfloor + 1$。一定无法满足要求。

```py [sol-Python3]
class Solution:
    def maxRunTime(self, n: int, batteries: List[int]) -> int:
        l, r = 0, sum(batteries) // n + 1
        while l + 1 < r:
            x = (l + r) // 2
            if n * x <= sum(min(b, x) for b in batteries):
                l = x
            else:
                r = x
        return l
```

```py [sol-Python3 库函数]
class Solution:
    def maxRunTime(self, n: int, batteries: List[int]) -> int:
        r = sum(batteries) // n
        # 二分找最小的不满足要求的 x+1，那么最大的满足要求的就是 x
        check = lambda x: n * (x + 1) > sum(min(b, x + 1) for b in batteries)
        return bisect_left(range(r), True, key=check)
```

```java [sol-Java]
class Solution {
    public long maxRunTime(int n, int[] batteries) {
        long tot = 0;
        for (int b : batteries) {
            tot += b;
        }

        long l = 0;
        long r = tot / n + 1;
        while (l + 1 < r) {
            long x = l + (r - l) / 2;
            long sum = 0;
            for (int b : batteries) {
                sum += Math.min(b, x);
            }
            if (n * x <= sum) {
                l = x;
            } else {
                r = x;
            }
        }
        return l;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxRunTime(int n, vector<int>& batteries) {
        long long tot = reduce(batteries.begin(), batteries.end(), 0LL);
        long long l = 0, r = tot / n + 1;
        while (l + 1 < r) {
            long long x = l + (r - l) / 2;
            long long sum = 0;
            for (long long b : batteries) {
                sum += min(b, x);
            }
            (n * x <= sum ? l : r) = x;
        }
        return l;
    }
};
```

```go [sol-Go]
func maxRunTime(n int, batteries []int) int64 {
	tot := 0
	for _, b := range batteries {
		tot += b
	}

	return int64(sort.Search(tot/n, func(x int) bool {
		x++
		sum := 0
		for _, b := range batteries {
			sum += min(b, x)
		}
		return n*x > sum
	}))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log (S/n))$，其中 $m$ 是 $\textit{batteries}$ 的长度，$S$ 是 $\textit{batteries}$ 的元素和。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：排序 + 贪心

受解法一的启发，我们可以得出如下贪心策略：

记电池电量和为 $\textit{sum}$，则理论上至多可以供电 $x=\Big\lfloor\dfrac{\textit{sum}}{n}\Big\rfloor$ 分钟。我们对电池电量从大到小排序，然后从电量最大的电池开始遍历：

- 若该电池电量超过 $x$，则将其供给一台电脑，问题缩减为 $n-1$ 台电脑的子问题。
- 若该电池电量不超过 $x$，则其余电池的电量均不超过 $x$，此时有

  $$
  n\cdot x=n\cdot\Big\lfloor\dfrac{\textit{sum}}{n}\Big\rfloor \le \textit{sum}
  $$

  根据解法一的结论，这些电池可以给 $n$ 台电脑供电 $x$ 分钟。

由于随着问题规模减小，$x$ 不会增加，因此若遍历到一个电量不超过 $x$ 的电池时，可以直接返回 $x$ 作为答案。

```py [sol-Python3]
class Solution:
    def maxRunTime(self, n: int, batteries: List[int]) -> int:
        batteries.sort(reverse=True)
        s = sum(batteries)
        for b in batteries:
            if b <= s // n:
                return s // n
            s -= b
            n -= 1
```

```java [sol-Java]
class Solution {
    public long maxRunTime(int n, int[] batteries) {
        Arrays.sort(batteries);

        long sum = 0;
        for (int b : batteries) {
            sum += b;
        }

        for (int i = batteries.length - 1; ; i--) {
            if (batteries[i] <= sum / n) {
                return sum / n;
            }
            sum -= batteries[i];
            n--;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxRunTime(int n, vector<int>& batteries) {
        ranges::sort(batteries, greater());
        long long sum = reduce(batteries.begin(), batteries.end(), 0LL);
        for (int i = 0; ; i++) {
            if (batteries[i] <= sum / n) {
                return sum / n;
            }
            sum -= batteries[i];
            n--;
        }
    }
};
```

```go [sol-Go]
func maxRunTime(n int, batteries []int) int64 {
	slices.Sort(batteries)
	sum := 0
	for _, b := range batteries {
		sum += b
	}
	for i := len(batteries) - 1; ; i-- {
		if batteries[i] <= sum/n {
			return int64(sum / n)
		}
		sum -= batteries[i]
		n--
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 是 $\textit{batteries}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 专题训练

1. 二分题单的「**§2.2 求最大**」。
2. 贪心题单的「**§1.1 从最小/最大开始贪心**」。

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

本题来自 `二、二分答案 / §2.2 求最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.2 求最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
