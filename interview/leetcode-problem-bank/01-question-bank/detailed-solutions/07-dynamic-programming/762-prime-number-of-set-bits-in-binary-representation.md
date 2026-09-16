# 762. 二进制表示中质数个计算置位

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/prime-number-of-set-bits-in-binary-representation/
- 题目 slug：`prime-number-of-set-bits-in-binary-representation`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/prime-number-of-set-bits-in-binary-representation/solutions/3905546/san-chong-fang-fa-bao-li-mei-ju-shu-wei-ej6m6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：暴力枚举 / 数位 DP / 组合数学（Python/Java/C++/Go）](https://leetcode.cn/problems/prime-number-of-set-bits-in-binary-representation/solutions/3905546/san-chong-fang-fa-bao-li-mei-ju-shu-wei-ej6m6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-fang-fa-bao-li-mei-ju-shu-wei-ej6m6`
- topic id：`3905546`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力枚举

枚举 $[\textit{left},\textit{right}]$ 中的整数 $x$，计算 $x$ 二进制中的 $1$ 的个数 $c$。如果 $c$ 是质数，那么答案增加一。

由于 $[1,10^6]$ 中的二进制数至多有 $19$ 个 $1$，所以只需 $19$ 以内的质数，即

$$
2, 3, 5, 7, 11, 13, 17, 19
$$

```py [sol-Python3]
primes = {2, 3, 5, 7, 11, 13, 17, 19}

class Solution:
    def countPrimeSetBits(self, left: int, right: int) -> int:
        ans = 0
        for x in range(left, right + 1):
            if x.bit_count() in primes:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    private static final Set<Integer> primes = Set.of(2, 3, 5, 7, 11, 13, 17, 19);

    public int countPrimeSetBits(int left, int right) {
        int ans = 0;
        for (int x = left; x <= right; x++) {
            if (primes.contains(Integer.bitCount(x))) {
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 注：也可以用哈希集合做，由于本题质数很少，用数组也可以
    static constexpr int primes[] = {2, 3, 5, 7, 11, 13, 17, 19};

public:
    int countPrimeSetBits(int left, int right) {
        int ans = 0;
        for (uint32_t x = left; x <= right; x++) {
            if (ranges::contains(primes, popcount(x))) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
// 注：也可以用哈希集合做，由于本题质数很少，用 slice 也可以
var primes = []int{2, 3, 5, 7, 11, 13, 17, 19}

func countPrimeSetBits(left, right int) (ans int) {
	for x := left; x <= right; x++ {
		if slices.Contains(primes, bits.OnesCount(uint(x))) {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\textit{right}-\textit{left})$。
- 空间复杂度：$\mathcal{O}(1)$。不计入质数集合的空间。

## 方法二：上下界数位 DP

[数位 DP v1.0 模板讲解](https://www.bilibili.com/video/BV1rS4y1s721/?t=19m36s)

[数位 DP v2.0 模板讲解](https://www.bilibili.com/video/BV1Fg4y1Q7wv/?t=31m28s)（上下界数位 DP）

对于本题，在递归边界（$i=n$）我们需要判断是否填了质数个 $1$，所以需要参数 $\textit{cnt}_1$ 表示填过的 $1$ 的个数。其余同 v2.0 模板。

```py [sol-Python3]
primes = {2, 3, 5, 7, 11, 13, 17, 19}

class Solution:
    def countPrimeSetBits(self, left: int, right: int) -> int:
        high_s = list(map(int, bin(right)[2:]))  # 避免在 dfs 中频繁调用 int()
        n = len(high_s)
        low_s = list(map(int, bin(left)[2:].zfill(n)))  # 添加前导零，长度和 high_s 对齐

        # 在 dfs 的过程中，统计二进制中的 1 的个数 cnt1
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, cnt1: int, limit_low: bool, limit_high: bool) -> int:
            if i == n:
                return 1 if cnt1 in primes else 0

            lo = low_s[i] if limit_low else 0
            hi = high_s[i] if limit_high else 1

            res = 0
            for d in range(lo, hi + 1):
                res += dfs(i + 1, cnt1 + d, limit_low and d == lo, limit_high and d == hi)
            return res

        return dfs(0, 0, True, True)
```

```java [sol-Java]
class Solution {
    private static final Set<Integer> primes = Set.of(2, 3, 5, 7, 11, 13, 17, 19);

    public int countPrimeSetBits(int left, int right) {
        int n = 32 - Integer.numberOfLeadingZeros(right);
        int[][] memo = new int[n][n + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1);
        }
        return dfs(n - 1, 0, true, true, left, right, memo);
    }

    // 在 dfs 的过程中，统计二进制中的 1 的个数 cnt1
    private int dfs(int i, int cnt1, boolean limitLow, boolean limitHigh, int left, int right, int[][] memo) {
        if (i < 0) {
            return primes.contains(cnt1) ? 1 : 0;
        }
        if (!limitLow && !limitHigh && memo[i][cnt1] != -1) {
            return memo[i][cnt1];
        }

        int lo = limitLow ? left >> i & 1 : 0;
        int hi = limitHigh ? right >> i & 1 : 1;

        int res = 0;
        for (int d = lo; d <= hi; d++) {
            res += dfs(i - 1, cnt1 + d, limitLow && d == lo, limitHigh && d == hi, left, right, memo);
        }

        if (!limitLow && !limitHigh) {
            memo[i][cnt1] = res;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 注：也可以用哈希集合做，由于本题质数很少，用数组也可以
    static constexpr int primes[] = {2, 3, 5, 7, 11, 13, 17, 19};

public:
    int countPrimeSetBits(int left, int right) {
        int n = bit_width((uint32_t) right);
        vector memo(n, vector<int>(n + 1, -1));

        // 在 dfs 的过程中，统计二进制中的 1 的个数 cnt1
        auto dfs = [&](this auto&& dfs, int i, int cnt1, bool limit_low, bool limit_high) -> int {
            if (i < 0) {
                return ranges::contains(primes, cnt1);
            }
            if (!limit_low && !limit_high && memo[i][cnt1] != -1) {
                return memo[i][cnt1];
            }

            int lo = limit_low ? left >> i & 1 : 0;
            int hi = limit_high ? right >> i & 1 : 1;

            int res = 0;
            for (int d = lo; d <= hi; d++) {
                res += dfs(i - 1, cnt1 + d, limit_low && d == lo, limit_high && d == hi);
            }

            if (!limit_low && !limit_high) {
                memo[i][cnt1] = res;
            }
            return res;
        };

        return dfs(n - 1, 0, true, true);
    }
};
```

```go [sol-Go]
// 注：也可以用哈希集合做，由于本题质数很少，用数组也可以
var primes = []int{2, 3, 5, 7, 11, 13, 17, 19}

func countPrimeSetBits(left int, right int) int {
	n := bits.Len(uint(right))
	memo := make([][]int, n)
	for i := range memo {
		memo[i] = make([]int, n+1)
		for j := range memo[i] {
			memo[i][j] = -1
		}
	}

	// 在 dfs 的过程中，统计二进制中的 1 的个数 cnt1
	var dfs func(int, int, bool, bool) int
	dfs = func(i, cnt1 int, limitLow, limitHigh bool) (res int) {
		if i < 0 {
			if slices.Contains(primes, cnt1) {
				return 1
			}
			return 0
		}
		if !limitLow && !limitHigh {
			p := &memo[i][cnt1]
			if *p >= 0 {
				return *p
			}
			defer func() { *p = res }()
		}

		lo := 0
		if limitLow {
			lo = left >> i & 1
		}
		hi := 1
		if limitHigh {
			hi = right >> i & 1
		}

		for d := lo; d <= hi; d++ {
			res += dfs(i-1, cnt1+d, limitLow && d == lo, limitHigh && d == hi)
		}
		return
	}

	return dfs(n-1, 0, true, true)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log^2 \textit{right})$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(\log^2 \textit{right})$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(\log^2 \textit{right})$。
- 空间复杂度：$\mathcal{O}(\log^2 \textit{right})$。保存多少状态，就需要多少空间。

## 方法三：组合数学

```py [sol-Python3]
primes = [2, 3, 5, 7, 11, 13, 17, 19]

class Solution:
    def calc(self, high: int) -> int:
        # 转换成计算 < high + 1 的合法正整数个数
        # 这样转换可以方便下面的代码把 high 也算进来
        high += 1
        res = ones = 0
        for i in range(high.bit_length() - 1, -1, -1):
            if high >> i & 1 == 0:
                continue
            # 如果这一位填 0，那么后面可以随便填
            # 问题变成在 i 个位置中填 k 个 1 的方案数，满足 ones + k 是质数
            for p in primes:
                k = p - ones  # 剩余需要填的 1 的个数
                if k > i:
                    break
                if k >= 0:
                    res += comb(i, k)
            # 这一位填 1，继续计算
            ones += 1
        return res

    def countPrimeSetBits(self, left: int, right: int) -> int:
        return self.calc(right) - self.calc(left - 1)
```

```py [sol-Python3 预处理]
MX = 20
comb = [[0] * MX for _ in range(MX)]
for i in range(MX):
    comb[i][0] = 1
    for j in range(1, i + 1):
        comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j]

primes = [2, 3, 5, 7, 11, 13, 17, 19]

class Solution:
    def calc(self, high: int) -> int:
        # 转换成计算 < high + 1 的合法正整数个数
        # 这样转换可以方便下面的代码把 high 也算进来
        high += 1
        res = ones = 0
        for i in range(high.bit_length() - 1, -1, -1):
            if high >> i & 1 == 0:
                continue
            # 如果这一位填 0，那么后面可以随便填
            # 问题变成在 i 个位置中填 k 个 1 的方案数，满足 ones + k 是质数
            for p in primes:
                k = p - ones  # 剩余需要填的 1 的个数
                if k > i:
                    break
                if k >= 0:
                    res += comb[i][k]
            # 这一位填 1，继续计算
            ones += 1
        return res

    def countPrimeSetBits(self, left: int, right: int) -> int:
        return self.calc(right) - self.calc(left - 1)
```

```java [sol-Java]
class Solution {
    private static final int MX = 20;
    private static final int[][] comb = new int[MX][MX];
    private static final int[] primes = {2, 3, 5, 7, 11, 13, 17, 19};
    private static boolean initialized = false;

    // 这样写比 static block 快
    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        // 预处理组合数
        for (int i = 0; i < MX; i++) {
            comb[i][0] = 1;
            for (int j = 1; j <= i; j++) {
                comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j];
            }
        }
    }

    public int countPrimeSetBits(int left, int right) {
        return calc(right) - calc(left - 1);
    }

    private int calc(int high) {
        // 转换成计算 < high + 1 的合法正整数个数
        // 这样转换可以方便下面的代码把 high 也算进来
        high++;
        int res = 0;
        int ones = 0;
        for (int i = 31 - Integer.numberOfLeadingZeros(high); i >= 0; i--) {
            if ((high >> i & 1) == 0) {
                continue;
            }
            // 如果这一位填 0，那么后面可以随便填
            // 问题变成在 pos 个位置中填 k 个 1 的方案数，满足 ones + k 是质数
            for (int p : primes) {
                int k = p - ones; // 剩余需要填的 1 的个数
                if (k > i) {
                    break;
                }
                if (k >= 0) {
                    res += comb[i][k];
                }
            }
            ones++; // 这一位填 1，继续计算
        }
        return res;
    }
}
```

```cpp [sol-C++]
constexpr int MX = 20;
int comb[MX][MX];

auto init = [] {
    // 预处理组合数
    for (int i = 0; i < MX; i++) {
        comb[i][0] = 1;
        for (int j = 1; j <= i; j++) {
            comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j];
        }
    }
    return 0;
}();

class Solution {
    static constexpr int primes[] = {2, 3, 5, 7, 11, 13, 17, 19};

    int calc(int high) {
        // 转换成计算 < high + 1 的合法正整数个数
        // 这样转换可以方便下面的代码把 high 也算进来
        high++;
        int res = 0, ones = 0;
        for (int i = bit_width((uint32_t) high) - 1; i >= 0; i--) {
            if ((high >> i & 1) == 0) {
                continue;
            }
            // 如果这一位填 0，那么后面可以随便填
            // 问题变成在 i 个位置中填 k 个 1 的方案数，满足 ones + k 是质数
            for (int p : primes) {
                int k = p - ones; // 剩余需要填的 1 的个数
                if (k > i) {
                    break;
                }
                if (k >= 0) {
                    res += comb[i][k];
                }
            }
            ones++; // 这一位填 1，继续计算
        }
        return res;
    }

public:
    int countPrimeSetBits(int left, int right) {
        return calc(right) - calc(left - 1);
    }
};
```

```go [sol-Go]
const mx = 20

var comb [mx][mx]int
var primes = []int{2, 3, 5, 7, 11, 13, 17, 19}

func init() {
	// 预处理组合数
	for i := range comb {
		comb[i][0] = 1
		for j := 1; j <= i; j++ {
			comb[i][j] = comb[i-1][j-1] + comb[i-1][j]
		}
	}
}

func calc(high int) (res int) {
	// 转换成计算 < high + 1 的合法正整数个数
	// 这样转换可以方便下面的代码把 high 也算进来
	high++
	ones := 0
	for i := bits.Len(uint(high)) - 1; i >= 0; i-- {
		if high>>i&1 == 0 {
			continue
		}
		// 如果这一位填 0，那么后面可以随便填
		// 问题变成在 i 个位置中填 k 个 1 的方案数，满足 ones + k 是质数
		for _, p := range primes {
			k := p - ones // 剩余需要填的 1 的个数
			if k > i {
				break
			}
			if k >= 0 {
				res += comb[i][k]
			}
		}
		// 这一位填 1，继续计算
		ones++
	}
	return res
}

func countPrimeSetBits(left, right int) int {
	return calc(right) - calc(left-1)
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}\left(\dfrac{\log^2 \textit{right}}{\log\log \textit{right}}\right)$。循环 $\mathcal{O}(\log \textit{right})$ 次，每次循环会遍历 $\mathcal{O}(\log \textit{right})$ 以内的质数，根据质数密度，这有 $\mathcal{O}\left(\dfrac{\log \textit{right}}{\log\log \textit{right}}\right)$ 个。预处理组合数后，计算组合数的时间为 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

1. 动态规划题单的「**十、数位 DP**」。
2. 数学题单的「**§2.2 组合计数**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `十、数位 DP / §10.1 统计合法元素的数目`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十、数位 DP / §10.1 统计合法元素的数目`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
