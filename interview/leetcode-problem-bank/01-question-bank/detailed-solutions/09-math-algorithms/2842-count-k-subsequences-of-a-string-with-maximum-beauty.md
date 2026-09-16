# 2842. 统计一个字符串的 k 子序列美丽值最大的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-k-subsequences-of-a-string-with-maximum-beauty/
- 题目 slug：`count-k-subsequences-of-a-string-with-maximum-beauty`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.2 组合计数
- 难度分：2092
- 外部题解来源：https://leetcode.cn/problems/count-k-subsequences-of-a-string-with-maximum-beauty/solutions/2424876/zu-he-shu-xue-pythonjavacgo-by-endlessch-whff/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[组合数学（Python/Java/C++/Go）](https://leetcode.cn/problems/count-k-subsequences-of-a-string-with-maximum-beauty/solutions/2424876/zu-he-shu-xue-pythonjavacgo-by-endlessch-whff/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zu-he-shu-xue-pythonjavacgo-by-endlessch-whff`
- topic id：`2424876`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1um4y1M7Rv/) 第四题。

## 提示 1

思考：$k=1$ 要怎么做？

从出现次数最多的字符中选一个字母。

## 提示 2

思考：$k=2$ 要怎么做？

从出现次数最多的开始选，如果有多个出现次数最多的呢？

例如 $s=\texttt{aaaabbbbcccc},\ k=2$，那么需要从 $3$ 种字母中选 $k$ 种，每种都有 $4$ 个字符可以选（题目说相同字符组成的子序列也算不同的），所以方案数为

$$
4^k \cdot C_3^k
$$

## 提示 3

统计每个字符**出现次数的个数**，然后从大到小遍历次数 $c$ 及其个数 $\textit{num}$。

- 如果 $\textit{num}<k$，那么这 $c$ 种字符每种选一个，方案数为 $c^{\textit{num}}$，然后将 $k$ 减去 $\textit{num}$。
- 如果 $\textit{num}\ge k$，根据上面的讨论，方案数为 $c^k\cdot C_{\textit{num}}^k$。

所有方案数相乘即为答案。

如果 $k$ 太大（循环中没有出现 $\textit{num}\ge k$），那么不存在合法子序列，返回 $0$。

有关**模运算**的小知识见文末的讲解。

代码中用到了**快速幂**，请看 [50. Pow(x, n)](https://leetcode.cn/problems/powx-n/)。

```py [sol-Python3]
class Solution:
    def countKSubsequencesWithMaxBeauty(self, s: str, k: int) -> int:
        MOD = 10 ** 9 + 7
        ans = 1
        cnt = Counter(Counter(s).values())
        for c, num in sorted(cnt.items(), reverse=True):
            if num >= k:
                return ans * pow(c, k, MOD) * comb(num, k) % MOD
            ans *= pow(c, num, MOD)
            k -= num
        return 0  # k 太大，无法选 k 个不一样的字符
```

```java [sol-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;

    public int countKSubsequencesWithMaxBeauty(String s, int k) {
        var cnt = new int[26];
        for (char c : s.toCharArray())
            cnt[c - 'a']++;
        var cc = new TreeMap<Integer, Integer>();
        for (int c : cnt)
            if (c > 0)
                cc.merge(c, 1, Integer::sum);

        long ans = 1;
        for (var e : cc.descendingMap().entrySet()) {
            int c = e.getKey(), num = e.getValue();
            if (num >= k)
                return (int) (ans * pow(c, k) % MOD * comb(num, k) % MOD);
            ans = ans * pow(c, num) % MOD;
            k -= num;
        }
        return 0; // k 太大，无法选 k 个不一样的字符
    }

    private long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0)
                res = res * x % MOD;
            x = x * x % MOD;
        }
        return res;
    }

    // 适用于 n 和 k 都比较小的场景（本题至多 26）
    private long comb(long n, int k) {
        long res = n;
        for (int i = 2; i <= k; i++)
            res = res * --n / i; // n,n-1,n-2,... 中的前 i 个数至少有一个因子 i
        return res % MOD;
    }
}
```

```cpp [sol-C++]
class Solution {
    const long long MOD = 1e9 + 7;

    long long pow(long long x, int n) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) res = res * x % MOD;
            x = x * x % MOD;
        }
        return res;
    }

    // 适用于 n 和 k 都比较小的场景（本题至多 26）
    long long comb(long long n, int k) {
        auto res = n;
        for (int i = 2; i <= k; i++)
            res = res * --n / i; // n,n-1,n-2,... 中的前 i 个数至少有一个因子 i
        return res % MOD;
    }

public:
    int countKSubsequencesWithMaxBeauty(string s, int k) {
        int cnt[26]{};
        for (char c: s)
            cnt[c - 'a']++;
        map<int, int> cc;
        for (int c: cnt)
            if (c) cc[-c]++; // -c 方便从大到小遍历

        long long ans = 1;
        for (auto [c, num]: cc) {
            if (num >= k)
                return ans * pow(-c, k) % MOD * comb(num, k) % MOD;
            ans = ans * pow(-c, num) % MOD;
            k -= num;
        }
        return 0; // k 太大，无法选 k 个不一样的字符
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

func countKSubsequencesWithMaxBeauty(s string, k int) int {
	cnt := [26]int{}
	for _, b := range s {
		cnt[b-'a']++
	}
	cc := map[int]int{}
	for _, c := range cnt {
		if c > 0 {
			cc[c]++
		}
	}

	type KV struct{ cnt, num int }
	kv := make([]KV, 0, len(cc))
	for k, v := range cc {
		kv = append(kv, KV{k, v})
	}
	sort.Slice(kv, func(i, j int) bool { return kv[i].cnt > kv[j].cnt })

	ans := 1
	for _, p := range kv {
		if p.num >= k {
			return ans * pow(p.cnt, k) % mod * comb(p.num, k) % mod
		}
		ans = ans * pow(p.cnt, p.num) % mod
		k -= p.num
	}
	return 0 // k 太大，无法选 k 个不一样的字符
}

func pow(x, n int) int {
	res := 1
	for ; n > 0; n /= 2 {
		if n%2 > 0 {
			res = res * x % mod
		}
		x = x * x % mod
	}
	return res
}

// 适用于 n 和 k 都比较小的场景（本题至多 26）
func comb(n, k int) int {
	res := n
	for i := 2; i <= k; i++ {
		res = res * (n - i + 1) / i // n,n-1,n-2,... 中的前 i 个数至少有一个因子 i
	}
	return res % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。时间主要用在遍历字符串 $s$ 上了。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。其中 $|\Sigma|$ 为字符集合的大小，本题中字符均为小写字母，所以 $|\Sigma|=26$。

## 算法小课堂：模运算

如果让你计算 $1234\cdot 6789$ 的**个位数**，你会如何计算？

由于只有个位数会影响到乘积的个位数，那么 $4\cdot 9=36$ 的个位数 $6$ 就是答案。

对于 $1234+6789$ 的个位数，同理，$4+9=13$ 的个位数 $3$ 就是答案。

你能把这个结论抽象成数学等式吗？

一般地，涉及到取模的题目，通常会用到如下等式（上面计算的是 $m=10$）：

$$
(a+b)\bmod m = ((a\bmod m) + (b\bmod m)) \bmod m
$$

$$
(a\cdot b) \bmod m=((a\bmod m)\cdot  (b\bmod m)) \bmod m
$$

证明：根据**带余除法**，任意整数 $a$ 都可以表示为 $a=km+r$，这里 $r$ 相当于 $a\bmod m$。那么设 $a=k_1m+r_1,\ b=k_2m+r_2$。

第一个等式：

$$
\begin{aligned}
&\ (a+b) \bmod m\\
=&\ ((k_1+k_2) m+r_1+r_2)\bmod m\\
=&\ (r_1+r_2)\bmod m\\
=&\ ((a\bmod m) + (b\bmod m)) \bmod m
\end{aligned}
$$

第二个等式：

$$
\begin{aligned}
&\ (a\cdot b) \bmod m\\
=&\ (k_1k_2m^2+(k_1r_2+k_2r_1)m+r_1r_2)\bmod m\\
=&\ (r_1r_2)\bmod m\\
=&\ ((a\bmod m)\cdot  (b\bmod m)) \bmod m
\end{aligned}
$$

**根据这两个恒等式，可以随意地对代码中的加法和乘法的结果取模**。

## 本地原创解析

### 1. 题意重述

本题来自 `二、组合数学 / §2.2 组合计数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.2 组合计数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
