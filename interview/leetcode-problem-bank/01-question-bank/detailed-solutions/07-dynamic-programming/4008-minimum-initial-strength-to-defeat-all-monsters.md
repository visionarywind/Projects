# 4008. 击败所有怪物的最小初始强度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-initial-strength-to-defeat-all-monsters/
- 题目 slug：`minimum-initial-strength-to-defeat-all-monsters`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.1 一维 DP
- 难度分：1776
- 外部题解来源：https://leetcode.cn/problems/minimum-initial-strength-to-defeat-all-monsters/solutions/4005388/liang-chong-fang-fa-er-fen-da-an-dao-tui-oake/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案 / 逆向思维，附思考题（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-initial-strength-to-defeat-all-monsters/solutions/4005388/liang-chong-fang-fa-er-fen-da-an-dao-tui-oake/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-fen-da-an-dao-tui-oake`
- topic id：`4005388`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

先用**差分数组**求出 $\textit{bonus}$ 数组。[差分数组原理讲解](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)。

## 方法一：二分答案

### 转化

如果初始强度为 $m$ 时可以击败所有怪物，那么更大的初始强度也可以击败所有怪物。

如果初始强度为 $m$ 时无法击败所有怪物，那么更小的初始强度也无法击败所有怪物。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题转化成一个判定性问题：

- 给定初始强度 $m$，能否击败所有怪物？

根据题目描述，从左到右遍历 $\textit{monsters}$，模拟即可。

如果可以击败所有怪物，说明答案 $\le m$，否则答案 $> m$。

### 细节

下面代码采用开区间二分。使用闭区间或者半闭半开区间也是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$-1$。初始强度不能是负数。
- 开区间右端点初始值：$\sum_{i}\textit{monsters}[i]$。即使不考虑加成，也能击败所有怪物。

> 对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

[本题视频讲解](https://www.bilibili.com/video/BV1Y73R6zEwG/?t=9m14s)，包含两种方法，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minInitialStrength(self, monsters: list[int], boosts: list[list[int]]) -> int:
        n = len(monsters)
        bonus = [0] * (n + 1)
        for l, r, v in boosts:
            bonus[l] += v
            bonus[r + 1] -= v

        # 差分数组的前缀和即原数组
        for i in range(1, n):
            bonus[i] += bonus[i - 1]

        def check(strength: int) -> bool:
            for x, b in zip(monsters, bonus):
                if strength + b < x:
                    return False
                strength = max(strength - x, 0)
            return True

        return bisect_left(range(sum(monsters)), True, key=check)
```

```java [sol-Java]
class Solution {
    public long minInitialStrength(int[] monsters, int[][] boosts) {
        int n = monsters.length;
        long[] bonus = new long[n + 1];
        for (int[] b : boosts) {
            bonus[b[0]] += b[2];
            bonus[b[1] + 1] -= b[2];
        }

        // 差分数组的前缀和即原数组
        for (int i = 1; i < n; i++) {
            bonus[i] += bonus[i - 1];
        }

        long sum = 0;
        for (int x : monsters) {
            sum += x;
        }

        long left = -1;
        long right = sum;
        while (left + 1 < right) {
            long mid = left + (right - left) / 2;
            long strength = mid;
            boolean ok = true;
            for (int i = 0; i < n; i++) {
                if (strength + bonus[i] < monsters[i]) {
                    ok = false;
                    break;
                }
                strength = Math.max(strength - monsters[i], 0);
            }
            if (ok) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minInitialStrength(vector<int>& monsters, vector<vector<int>>& boosts) {
        int n = monsters.size();
        vector<long long> bonus(n + 1);
        for (auto& b : boosts) {
            bonus[b[0]] += b[2];
            bonus[b[1] + 1] -= b[2];
        }

        // 差分数组的前缀和即原数组
        for (int i = 1; i < n; i++) {
            bonus[i] += bonus[i - 1];
        }

        long long left = -1;
        long long right = reduce(monsters.begin(), monsters.end(), 0LL);
        while (left + 1 < right) {
            long long mid = left + (right - left) / 2;
            long long strength = mid;
            bool ok = true;
            for (int i = 0; i < n; i++) {
                if (strength + bonus[i] < monsters[i]) {
                    ok = false;
                    break;
                }
                strength = max(strength - monsters[i], 0LL);
            }
            (ok ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func minInitialStrength(monsters []int, boosts [][]int) int64 {
	n := len(monsters)
	bonus := make([]int, n+1)
	for _, b := range boosts {
		bonus[b[0]] += b[2]
		bonus[b[1]+1] -= b[2]
	}

	// 差分数组的前缀和即原数组
	for i := 1; i < n; i++ {
		bonus[i] += bonus[i-1]
	}

	sum := 0
	for _, x := range monsters {
		sum += x
	}

	ans := sort.Search(sum, func(strength int) bool {
		for i, x := range monsters {
			if strength+bonus[i] < x {
				return false
			}
			strength = max(strength-x, 0)
		}
		return true
	})
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n\log S)$，其中 $n$ 是 $\textit{monsters}$ 的长度，$m$ 是 $\textit{boosts}$ 的长度，$S = \sum_{i}\textit{monsters}[i]$。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：逆向思维

正向思考的困难是，由于不知道后面的怪物信息，无法确定初始强度取多少合适。

那么倒着想呢？击败最后一个怪物，需要多少强度？在此基础上，击败倒数第二个怪物，需要多少强度？

定义 $f_i$ 为只考虑击败 $[i,n-1]$ 中的怪物，所需的最小强度。

除了满足 $f_i\ge 0$ 以外，还必须满足两个条件：

1. 要能击败当前怪物，即 $f_i + \textit{bonus}[i]\ge \textit{monsters}[i]$，移项得 $f_i \ge \textit{monsters}[i] - \textit{bonus}[i]$。
2. 要能击败后续怪物，即 $\max(f_i - \textit{monsters}[i], 0)\ge f_{i+1}$。

分类讨论：

- 如果 $f_{i+1} > 0$，那么条件 2 相当于 $f_i - \textit{monsters}[i]\ge f_{i+1}$，即 $f_i\ge f_{i+1} + \textit{monsters}[i]$。此时条件 1 一定成立。所以有 $f_i = f_{i+1} + \textit{monsters}[i]$。
- 如果 $f_{i+1} = 0$，那么条件 2 一定成立，只需满足条件 1 以及 $f_i\ge 0$，所以有 $f_i = \max(\textit{monsters}[i] - \textit{bonus}[i],0)$。

综上，可以得到如下递推式

$$
f_i =
\begin{cases}
f_{i+1} + \textit{monsters}[i], & f_{i+1} > 0     \\
\max(\textit{monsters}[i] - \textit{bonus}[i],0), & f_{i+1} = 0     \\
\end{cases}
$$

初始值 $f_n = 0$。没有怪物，最小强度为 $0$。

答案为 $f_0$。

代码实现时，$f$ 可以简化成一个变量。

### 写法一

```py [sol-Python3]
class Solution:
    def minInitialStrength(self, monsters: list[int], boosts: list[list[int]]) -> int:
        n = len(monsters)
        bonus = [0] * (n + 1)
        for l, r, v in boosts:
            bonus[l] += v
            bonus[r + 1] -= v

        # 差分数组的前缀和即原数组
        for i in range(1, n):
            bonus[i] += bonus[i - 1]

        f = 0
        for i in range(n - 1, -1, -1):
            if f > 0:
                f += monsters[i]
            else:
                f = max(monsters[i] - bonus[i], 0)
        return f
```

```java [sol-Java]
class Solution {
    public long minInitialStrength(int[] monsters, int[][] boosts) {
        int n = monsters.length;
        long[] bonus = new long[n + 1];
        for (int[] b : boosts) {
            bonus[b[0]] += b[2];
            bonus[b[1] + 1] -= b[2];
        }

        // 差分数组的前缀和即原数组
        for (int i = 1; i < n; i++) {
            bonus[i] += bonus[i - 1];
        }

        long f = 0;
        for (int i = n - 1; i >= 0; i--) {
            if (f > 0) {
                f += monsters[i];
            } else {
                f = Math.max(monsters[i] - bonus[i], 0);
            }
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minInitialStrength(vector<int>& monsters, vector<vector<int>>& boosts) {
        int n = monsters.size();
        vector<long long> bonus(n + 1);
        for (auto& b : boosts) {
            bonus[b[0]] += b[2];
            bonus[b[1] + 1] -= b[2];
        }

        // 差分数组的前缀和即原数组
        for (int i = 1; i < n; i++) {
            bonus[i] += bonus[i - 1];
        }

        long long f = 0;
        for (int i = n - 1; i >= 0; i--) {
            if (f > 0) {
                f += monsters[i];
            } else {
                f = max(monsters[i] - bonus[i], 0LL);
            }
        }
        return f;
    }
};
```

```go [sol-Go]
func minInitialStrength(monsters []int, boosts [][]int) int64 {
	n := len(monsters)
	bonus := make([]int, n+1)
	for _, b := range boosts {
		bonus[b[0]] += b[2]
		bonus[b[1]+1] -= b[2]
	}

	// 差分数组的前缀和即原数组
	for i := 1; i < n; i++ {
		bonus[i] += bonus[i-1]
	}

	f := 0
	for i := n - 1; i >= 0; i-- {
		if f > 0 {
			f += monsters[i]
		} else {
			f = max(monsters[i]-bonus[i], 0)
		}
	}
	return int64(f)
}
```

### 写法二

根据递推式，如果 $f_i > 0$，那么后续的 $f_j\ (j<i)$ 均大于 $0$。

所以可以先倒序找到第一个 $\textit{monsters}[i] > \textit{bonus}[i]$ 的 $i$，累加 $\textit{monsters}[i] - \textit{bonus}[i]$ 以及 $[0,i-1]$ 中的 $\textit{monsters}[j]$ 之和，即为答案。

```py [sol-Python3]
class Solution:
    def minInitialStrength(self, monsters: list[int], boosts: list[list[int]]) -> int:
        n = len(monsters)
        bonus = [0] * (n + 1)
        for l, r, v in boosts:
            bonus[l] += v
            bonus[r + 1] -= v

        # 差分数组的前缀和即原数组
        for i in range(1, n):
            bonus[i] += bonus[i - 1]

        for i in range(n - 1, -1, -1):
            if monsters[i] > bonus[i]:
                return sum(monsters[:i + 1]) - bonus[i]
        return 0
```

```java [sol-Java]
class Solution {
    public long minInitialStrength(int[] monsters, int[][] boosts) {
        int n = monsters.length;
        long[] bonus = new long[n + 1];
        for (int[] b : boosts) {
            bonus[b[0]] += b[2];
            bonus[b[1] + 1] -= b[2];
        }

        // 差分数组的前缀和即原数组
        for (int i = 1; i < n; i++) {
            bonus[i] += bonus[i - 1];
        }

        for (int i = n - 1; i >= 0; i--) {
            if (monsters[i] > bonus[i]) {
                long ans = -bonus[i];
                for (int j = i; j >= 0; j--) {
                    ans += monsters[j];
                }
                return ans;
            }
        }
        return 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minInitialStrength(vector<int>& monsters, vector<vector<int>>& boosts) {
        int n = monsters.size();
        vector<long long> bonus(n + 1);
        for (auto& b : boosts) {
            bonus[b[0]] += b[2];
            bonus[b[1] + 1] -= b[2];
        }

        // 差分数组的前缀和即原数组
        for (int i = 1; i < n; i++) {
            bonus[i] += bonus[i - 1];
        }

        for (int i = n - 1; i >= 0; i--) {
            if (monsters[i] > bonus[i]) {
                return reduce(monsters.begin(), monsters.begin() + i + 1, 0LL) - bonus[i];
            }
        }
        return 0;
    }
};
```

```go [sol-Go]
func minInitialStrength(monsters []int, boosts [][]int) int64 {
	n := len(monsters)
	bonus := make([]int, n+1)
	for _, b := range boosts {
		bonus[b[0]] += b[2]
		bonus[b[1]+1] -= b[2]
	}

	// 差分数组的前缀和即原数组
	for i := 1; i < n; i++ {
		bonus[i] += bonus[i-1]
	}

	for i := n - 1; i >= 0; i-- {
		if monsters[i] > bonus[i] {
			ans := -bonus[i]
			for _, x := range monsters[:i+1] {
				ans += x
			}
			return int64(ans)
		}
	}
	return 0
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{monsters}$ 的长度，$m$ 是 $\textit{boosts}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

根据最后一个写法，思考如下问题：

1. 额外输入一个数组 $\textit{queries}$，其中 $\textit{queries}[i] = [\ell_i,r_i]$。对于每个询问，计算击败 $\textit{monsters}$ 的连续子数组 $[\ell_i,r_i]$ 中的所有怪物，所需的最小初始强度。你能做到单个询问 $\mathcal{O}(1)$ 时间复杂度吗？
2. 如果还有修改单个 $\textit{monsters}[i]$ 的操作呢？
3. 如果还有修改 $\textit{monsters}$ 的子数组的操作（区间加/减）呢？

欢迎在评论区分享你的思路/代码。

## 专题训练

1. 二分题单的「**§2.1 求最小**」。
2. 数据结构题单的「**§2.1 一维差分**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.1 一维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.1 一维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
