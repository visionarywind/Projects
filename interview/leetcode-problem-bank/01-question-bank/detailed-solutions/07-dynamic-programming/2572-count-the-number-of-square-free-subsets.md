# 2572. 无平方子集计数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-the-number-of-square-free-subsets/
- 题目 slug：`count-the-number-of-square-free-subsets`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：2420
- 外部题解来源：https://leetcode.cn/problems/count-the-number-of-square-free-subsets/solutions/2121032/liang-chong-xie-fa-01bei-bao-zi-ji-zhuan-3ooi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种写法：01背包/子集状压DP（Python/Java/C++/Go）](https://leetcode.cn/problems/count-the-number-of-square-free-subsets/solutions/2121032/liang-chong-xie-fa-01bei-bao-zi-ji-zhuan-3ooi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-xie-fa-01bei-bao-zi-ji-zhuan-3ooi`
- topic id：`2121032`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

# 方法一：转换成 0-1 背包方案数

把无平方因子数的数字记作 SF（square-free number）。

对于每个 $[2,30]$ 内的 SF，通过预处理得到每个 SF 的质因子集合，用二进制表示。二进制从低到高第 $i$ 个比特为 $1$ 表示第 $i$ 个质数在集合中，为 $0$ 表示第 $i$ 个质数不在集合中。

那么把每个是 SF 的 $\textit{nums}[i]$ 转换成对应的质因子集合，题目就变成「遍历所有由 $30$ 以内的质数组成的集合 $j$（这有 $2^{10}$ 个），对每个 $j$，计算选一些**不相交**的质因子集合，它们的**并**集**恰好**为 $j$ 的方案数」。

这是 0-1 背包求方案数的模型，具体可以看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

附：[本题视频讲解](https://www.bilibili.com/video/BV1jM411J7y7/)。

```py [sol1-Python3]
PRIMES = 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
SF_TO_MASK = [0] * 31  # SF_TO_MASK[i] 为 i 的质因子集合（用二进制表示）
for i in range(2, 31):
    for j, p in enumerate(PRIMES):
        if i % p == 0:
            if i % (p * p) == 0:  # 有平方因子
                SF_TO_MASK[i] = -1
                break
            SF_TO_MASK[i] |= 1 << j  # 把 j 加到集合中

class Solution:
    def squareFreeSubsets(self, nums: List[int]) -> int:
        MOD = 10 ** 9 + 7
        M = 1 << len(PRIMES)
        f = [0] * M  # f[j] 表示恰好组成质数集合 j 的方案数
        f[0] = 1  # 质数集合是空集的方案数为 1
        for x in nums:
            mask = SF_TO_MASK[x]
            if mask >= 0:  # x 是 SF
                for j in range(M - 1, mask - 1, -1):
                    if (j | mask) == j:  # mask 是 j 的子集
                        f[j] = (f[j] + f[j ^ mask]) % MOD  # 不选 mask + 选 mask
        return (sum(f) - 1) % MOD  # -1 去掉空集（nums 的空子集）
```

```java [sol1-Java]
class Solution {
    private static final int[] PRIMES = new int[]{2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    private static final int MOD = (int) 1e9 + 7, MX = 30, N_PRIMES = PRIMES.length, M = 1 << N_PRIMES;
    private static final int[] SF_TO_MASK = new int[MX + 1]; // SF_TO_MASK[i] 为 i 的质因子集合（用二进制表示）

    static {
        for (int i = 2; i <= MX; ++i)
            for (int j = 0; j < N_PRIMES; ++j) {
                int p = PRIMES[j];
                if (i % p == 0) {
                    if (i % (p * p) == 0) { // 有平方因子
                        SF_TO_MASK[i] = -1;
                        break;
                    }
                    SF_TO_MASK[i] |= 1 << j; // 把 j 加到集合中
                }
            }
    }

    public int squareFreeSubsets(int[] nums) {
        var f = new int[M]; // f[j] 表示恰好组成质数集合 j 的方案数
        f[0] = 1; // 质数集合是空集的方案数为 1
        for (int x : nums) {
            int mask = SF_TO_MASK[x];
            if (mask >= 0) // x 是 SF
                for (int j = M - 1; j >= mask; --j)
                    if ((j | mask) == j)  // mask 是 j 的子集
                        f[j] = (f[j] + f[j ^ mask]) % MOD; // 不选 mask + 选 mask
        }
        var ans = 0L;
        for (int v : f) ans += v;
        return (int) ((ans - 1) % MOD); // -1 去掉空集（nums 的空子集）
    }
}
```

```cpp [sol1-C++]
class Solution {
    static constexpr int PRIMES[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    static constexpr int MOD = 1e9 + 7, MX = 30, N_PRIMES = 10, M = 1 << N_PRIMES;
public:
    int squareFreeSubsets(vector<int> &nums) {
        int sf2mask[MX + 1]{}; // sf2mask[i] 为 i 的质因子集合（用二进制表示）
        for (int i = 2; i <= MX; ++i)
            for (int j = 0; j < N_PRIMES; ++j) {
                int p = PRIMES[j];
                if (i % p == 0) {
                    if (i % (p * p) == 0) { // 有平方因子
                        sf2mask[i] = -1;
                        break;
                    }
                    sf2mask[i] |= 1 << j; // 把 j 加到集合中
                }
            }

        int f[M]{1}; // f[j] 表示恰好组成质数集合 j 的方案数，其中质数集合是空集的方案数为 1
        for (int x : nums)
            if (int mask = sf2mask[x]; mask >= 0) // x 是 SF
                for (int j = M - 1; j >= mask; --j)
                    if ((j | mask) == j)  // mask 是 j 的子集
                        f[j] = (f[j] + f[j ^ mask]) % MOD; // 不选 mask + 选 mask
        return (accumulate(f, f + M, 0L) - 1) % MOD; // -1 去掉空集（nums 的空子集）
    }
};
```

```go [sol1-Go]
var primes = [...]int{2, 3, 5, 7, 11, 13, 17, 19, 23, 29}
var sf2mask = [31]int{} // sf2mask[i] 为 i 的质因子集合（用二进制表示）

func init() {
	for i := 2; i <= 30; i++ {
		for j, p := range primes {
			if i%p == 0 {
				if i%(p*p) == 0 { // 有平方因子
					sf2mask[i] = -1
					break
				}
				sf2mask[i] |= 1 << j // 把 j 加到集合中
			}
		}
	}
}

func squareFreeSubsets(nums []int) int {
	const mod int = 1e9 + 7
	const m = 1 << len(primes)
	f := [m]int{1} // f[j] 表示恰好组成质数集合 j 的方案数，其中质数集合是空集的方案数为 1
	for _, x := range nums {
		if mask := sf2mask[x]; mask >= 0 { // x 是 SF
			for j := m - 1; j >= mask; j-- {
				if j|mask == j { // mask 是 j 的子集
					f[j] = (f[j] + f[j^mask]) % mod // 不选 mask + 选 mask
				}
			}
		}
	}
	ans := 0
	for _, v := range f {
		ans += v
	}
	return (ans - 1) % mod // -1 去掉空集（nums 的空子集）
}
```

### 复杂度分析

- 时间复杂度：$O(n2^m)$，其中 $n$ 为 $\textit{nums}$ 的长度，$m=\pi(\max(\textit{nums}))$，$\pi(N)$ 表示 $N$ 以内的质数个数，对于本题的数据范围，$m\le 10$。
- 空间复杂度：$O(2^m)$。

# 方法二：子集状压 DP

如果把 $\textit{nums}$ 的长度增加到 $10^5$ 的话，方法一就太慢了。

毕竟 $\textit{nums}$ 的值域很小，把**相同数字一并处理**是更优的。

怎么转移呢？选择的数字乘积不能有平方因子，对应的质数集合也就不能有交集。那么枚举 $\textit{mask}$ 的补集 $\textit{other}$ 的子集 $j$，这样可以保证 $\textit{mask}$ 和 $j$ 是没有交集的。站在并集 $j\cup \textit{mask}$ 的角度，按照「选或不选」的思想，如果选了 $\textit{mask}$，那么就要从 $f[j]$ 转移过来了。

具体地，设 $x$ 是 $\textit{mask}$ 对应的 SF，$\textit{cnt}[x]$ 为 $x$ 在 $\textit{nums}$ 中的出现次数，那么需要从 $\textit{cnt}[x]$ 个 $\textit{mask}$ 中选**一个**，与 $j$ 并成 $j\cup \textit{mask}$，对应的转移为

$$
f[j\cup \textit{mask}] += f[j] \cdot \textit{cnt}[x]
$$

附：[视频讲解](https://www.bilibili.com/video/BV1jM411J7y7/)

```py [sol2-Python3]
PRIMES = 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
SF_TO_MASK = [0] * 31  # SF_TO_MASK[i] 为 i 的质因子集合（用二进制表示）
for i in range(2, 31):
    for j, p in enumerate(PRIMES):
        if i % p == 0:
            if i % (p * p) == 0:  # 有平方因子
                SF_TO_MASK[i] = -1
                break
            SF_TO_MASK[i] |= 1 << j  # 把 j 加到集合中

class Solution:
    def squareFreeSubsets(self, nums: List[int]) -> int:
        MOD = 10 ** 9 + 7
        cnt = Counter(nums)
        M = 1 << len(PRIMES)
        f = [0] * M  # f[j] 表示恰好组成质数集合 j 的方案数
        f[0] = pow(2, cnt[1], MOD)  # 用 1 组成空质数集合的方案数
        for x, c in cnt.items():
            mask = SF_TO_MASK[x]
            if mask > 0:  # x 是 SF
                j = other = (M - 1) ^ mask  # mask 的补集
                while True:  # 枚举 other 的子集 j
                    f[j | mask] = (f[j | mask] + f[j] * c) % MOD  # 不选 mask + 选 mask
                    j = (j - 1) & other
                    if j == other: break
        return (sum(f) - 1) % MOD  # -1 表示去掉空集（nums 的空子集）
```

```java [sol2-Java]
class Solution {
    private static final int[] PRIMES = new int[]{2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    private static final int MOD = (int) 1e9 + 7, MX = 30, N_PRIMES = PRIMES.length, M = 1 << N_PRIMES;
    private static final int[] SF_TO_MASK = new int[MX + 1]; // SF_TO_MASK[i] 为 i 的质因子集合（用二进制表示）

    static {
        for (int i = 2; i <= MX; ++i)
            for (int j = 0; j < N_PRIMES; ++j) {
                int p = PRIMES[j];
                if (i % p == 0) {
                    if (i % (p * p) == 0) { // 有平方因子
                        SF_TO_MASK[i] = -1;
                        break;
                    }
                    SF_TO_MASK[i] |= 1 << j; // 把 j 加到集合中
                }
            }
    }

    public int squareFreeSubsets(int[] nums) {
        var cnt = new int[MX + 1];
        int pow2 = 1;
        for (int x : nums)
            if (x == 1) pow2 = pow2 * 2 % MOD;
            else ++cnt[x];

        var f = new long[M]; // f[j] 表示恰好组成质数集合 j 的方案数
        f[0] = pow2; // 用 1 组成空质数集合的方案数
        for (int x = 2; x <= MX; ++x) {
            int mask = SF_TO_MASK[x], c = cnt[x];
            if (mask > 0 && c > 0) {
                int other = (M - 1) ^ mask, j = other; // mask 的补集 other
                do { // 枚举 other 的子集 j
                    f[j | mask] = (f[j | mask] + f[j] * cnt[x]) % MOD; // 不选 mask + 选 mask
                    j = (j - 1) & other;
                } while (j != other);
            }
        }
        var ans = -1L; // 去掉空集（nums 的空子集）
        for (var v : f) ans += v;
        return (int) (ans % MOD);
    }
}
```

```cpp [sol2-C++]
class Solution {
    static constexpr int PRIMES[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    static constexpr int MOD = 1e9 + 7, MX = 30, N_PRIMES = 10, M = 1 << N_PRIMES;
public:
    int squareFreeSubsets(vector<int> &nums) {
        int sf2mask[MX + 1]{}; // sf2mask[i] 为 i 的质因子集合（用二进制表示）
        for (int i = 2; i <= MX; ++i)
            for (int j = 0; j < N_PRIMES; ++j) {
                int p = PRIMES[j];
                if (i % p == 0) {
                    if (i % (p * p) == 0) { // 有平方因子
                        sf2mask[i] = 0;
                        break;
                    }
                    sf2mask[i] |= 1 << j; // 把 j 加到集合中
                }
            }

        int cnt[MX + 1]{}, pow2 = 1;
        for (int x : nums)
            if (x == 1) pow2 = pow2 * 2 % MOD;
            else ++cnt[x];

        long f[M]{pow2}; // f[j] 表示恰好组成质数集合 j 的方案数，其中用 1 组成空质数集合的方案数为 pow2
        for (int x = 2; x <= MX; ++x) {
            int mask = sf2mask[x], c = cnt[x];
            if (mask && c) {
                int other = (M - 1) ^ mask, j = other; // mask 的补集 other
                do { // 枚举 other 的子集 j
                    f[j | mask] = (f[j | mask] + f[j] * cnt[x]) % MOD; // 不选 mask + 选 mask
                    j = (j - 1) & other;
                } while (j != other);
            }
        }
        return accumulate(f, f + M, -1L) % MOD; // -1 表示去掉空集（nums 的空子集）
    }
};
```

```go [sol2-Go]
var primes = [...]int{2, 3, 5, 7, 11, 13, 17, 19, 23, 29}
var sf2mask = [31]int{} // sf2mask[i] 为 i 的质因子集合（用二进制表示）

func init() {
	for i := 2; i <= 30; i++ {
		for j, p := range primes {
			if i%p == 0 {
				if i%(p*p) == 0 { // 有平方因子
					sf2mask[i] = -1
					break
				}
				sf2mask[i] |= 1 << j // 把 j 加到集合中
			}
		}
	}
}

func squareFreeSubsets(a []int) int {
	const mod int = 1e9 + 7
	cnt, pow2 := [31]int{}, 1
	for _, x := range a {
		if x == 1 {
			pow2 = pow2 * 2 % mod
		} else {
			cnt[x]++
		}
	}

	const m = 1 << len(primes)
	f := [m]int{pow2} // f[j] 表示恰好组成质数集合 j 的方案数，其中用 1 组成空质数集合的方案数为 pow2
	for sf, mask := range sf2mask {
		if mask > 0 && cnt[sf] > 0 {
			other := (m - 1) ^ mask // mask 的补集
			for j := other; ; { // 枚举 other 的子集 j
				f[j|mask] = (f[j|mask] + f[j]*cnt[sf]) % mod // 不选 mask + 选 mask
				j = (j - 1) & other
				if j == other {
					break
				}
			}
		}
	}
	ans := -1 // 去掉空集（nums 的空子集）
	for _, v := range f {
		ans += v
	}
	return ans % mod
}
```

### 复杂度分析

- 时间复杂度：$O(n+q2^m)$，其中 $n$ 为 $\textit{nums}$ 的长度，$q$ 为 $\max(\textit{nums})$ 以内的 SF 个数，$m=\pi(\max(\textit{nums}))$，$\pi(N)$ 表示 $N$ 以内的质数个数，对于本题的数据范围，$q\le 18, m\le 10$。
- 空间复杂度：$O(2^m)$。

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
