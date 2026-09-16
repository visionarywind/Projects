# 2827. 范围中美丽整数的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-beautiful-integers-in-the-range/
- 题目 slug：`number-of-beautiful-integers-in-the-range`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：2324
- 外部题解来源：https://leetcode.cn/problems/number-of-beautiful-integers-in-the-range/solutions/2396206/shu-wei-dppythonjavacgo-by-endlesscheng-4gvw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数位 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-beautiful-integers-in-the-range/solutions/2396206/shu-wei-dppythonjavacgo-by-endlesscheng-4gvw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-wei-dppythonjavacgo-by-endlesscheng-4gvw`
- topic id：`2396206`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下午两点[【b站@灵茶山艾府】](https://space.bilibili.com/206214)直播讲题，欢迎关注！

---

本文接着 [数位 DP 通用模板](https://leetcode.cn/problems/numbers-with-repeated-digits/solution/by-endlesscheng-c5vg/) 继续讲，因为核心逻辑是一样的。

转换成计算「不超过 $\textit{high}$ 的美丽整数的数目」减去「不超过 $\textit{low}-1$ 的美丽整数的数目」。

对于不超过 $\textit{high}$ 美丽整数的数目，先将 $\textit{high}$ 转换成字符串 $s$，设 $s$ 的长度为 $n$。

本题我们定义 $f(i,\textit{val}, \textit{diff}, \textit{isLimit},\textit{isNum})$ 表示构造第 $i$ 位及其之后数位的合法方案数，其中：

- 已经构造的数位，模 $k$ 等于 $\textit{val}$。根据取模的原则（请看本文末尾），当我们在第 $i$ 位填入数字 $d$ 后，将 $\textit{val}$ 更新为 $(\textit{val}\cdot 10 + d)\bmod k$。在递归终点，判断是否满足 $\textit{val}=0$。
- $\textit{diff}$ 表示奇数数位的数目与偶数数位的数目的差。在递归终点，判断是否满足 $\textit{diff}=0$。注意我们无需对奇数数位的数目和偶数数位的数目分别各用一个参数表示，那样效率更低。假设填入的数字是 $d$，那么将 $\textit{diff}$ 加上 $(d\bmod 2) \cdot 2 - 1$，对应着奇数加一，偶数减一。

代码实现时，如果你用的不是 Python，需要注意 $\textit{diff}$ 会出现负数，可以将递归入口处的 $\textit{diff}$ 设置为 $n$，在递归终点时判断是否满足 $\textit{diff}=n$。

```py [sol-Python3]
class Solution:
    def numberOfBeautifulIntegers(self, low: int, high: int, k: int) -> int:
        def calc(high: int) -> int:
            s = str(high)
            @cache  # 记忆化搜索
            def dfs(i: int, val: int, diff: int, is_limit: bool, is_num: bool) -> int:
                if i == len(s):
                    return int(is_num and val == 0 and diff == 0)  # 找到了一个合法数字
                res = 0
                if not is_num:  # 可以跳过当前数位
                    res = dfs(i + 1, val, diff, False, False)
                d0 = 0 if is_num else 1  # 如果前面没有填数字，必须从 1 开始（因为不能有前导零）
                up = int(s[i]) if is_limit else 9  # 如果前面填的数字都和 high 的一样，那么这一位至多填 s[i]（否则就超过 high 啦）
                for d in range(d0, up + 1):  # 枚举要填入的数字 d
                    res += dfs(i + 1, (val * 10 + d) % k, diff + d % 2 * 2 - 1, is_limit and d == up, True)
                return res
            return dfs(0, 0, 0, True, False)
        return calc(high) - calc(low - 1)
```

```java [sol-Java]
class Solution {
    public int numberOfBeautifulIntegers(int low, int high, int k) {
        return calc(high, k) - calc(low - 1, k);
    }

    private int calc(int high, int k) {
        var s = Integer.toString(high).toCharArray();
        int n = s.length;
        var memo = new int[n][k][n * 2 + 1];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < k; j++)
                Arrays.fill(memo[i][j], -1); // -1 表示没有计算过
        return dfs(0, 0, n, true, false, k, s, memo);
    }

    private int dfs(int i, int val, int diff, boolean isLimit, boolean isNum, int k, char[] s, int[][][] memo) {
        if (i == s.length)
            return isNum && val == 0 && diff == s.length ? 1 : 0; // 找到了一个合法数字
        if (!isLimit && isNum && memo[i][val][diff] != -1)
            return memo[i][val][diff];
        int res = 0;
        if (!isNum) // 可以跳过当前数位
            res = dfs(i + 1, val, diff, false, false, k, s, memo);
        int up = isLimit ? s[i] - '0' : 9; // 如果前面填的数字都和 high 的一样，那么这一位至多填数字 s[i]（否则就超过 high 啦）
        for (int d = isNum ? 0 : 1; d <= up; d++) // 枚举要填入的数字 d
            res += dfs(i + 1, (val * 10 + d) % k, diff + d % 2 * 2 - 1, isLimit && d == up, true, k, s, memo);
        if (!isLimit && isNum)
            memo[i][val][diff] = res; // 记忆化搜索
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    int calc(int high, int k) {
        auto s = to_string(high);
        int n = s.length(), memo[n][k + 1][n * 2 + 1];
        memset(memo, -1, sizeof(memo)); // -1 表示没有计算过
        function<int(int, int, int, bool, bool)> dfs;
        dfs = [&](int i, int val, int diff, bool is_limit, bool is_num) -> int {
            if (i == n)
                return is_num && val == 0 && diff == n; // 找到了一个合法数字
            if (!is_limit && is_num && memo[i][val][diff] != -1)
                return memo[i][val][diff];
            int res = 0;
            if (!is_num) // 可以跳过当前数位
                res = dfs(i + 1, val, diff, false, false);
            int up = is_limit ? s[i] - '0' : 9; // 如果前面填的数字都和 high 的一样，那么这一位至多填数字 s[i]（否则就超过 high 啦）
            for (int d = 1 - is_num; d <= up; d++) // 枚举要填入的数字 d
                res += dfs(i + 1, (val * 10 + d) % k, diff + d % 2 * 2 - 1, is_limit && d == up, true);
            if (!is_limit && is_num)
                memo[i][val][diff] = res; // 记忆化
            return res;
        };
        return dfs(0, 0, n, true, false);
    }

public:
    int numberOfBeautifulIntegers(int low, int high, int k) {
        return calc(high, k) - calc(low - 1, k);
    }
};
```

```go [sol-Go]
func numberOfBeautifulIntegers(low, high, k int) int {
	calc := func(high int) int {
		s := strconv.Itoa(high)
		n := len(s)
		memo := make([][][]int, n)
		for i := range memo {
			memo[i] = make([][]int, k+1)
			for j := range memo[i] {
				memo[i][j] = make([]int, n*2+1)
				for k := range memo[i][j] {
					memo[i][j][k] = -1 // -1 表示没有计算过
				}
			}
		}
		var dfs func(int, int, int, bool, bool) int
		dfs = func(i, val, diff int, isLimit, isNum bool) (res int) {
			if i == n {
				if isNum && val == 0 && diff == n {
					return 1 // 找到了一个合法数字
				}
				return 0
			}
			if !isLimit && isNum {
				p := &memo[i][val][diff]
				if *p >= 0 {
					return *p
				}
				defer func() { *p = res }() // 记忆化搜索
			}
			if !isNum { // 可以跳过当前数位
				res += dfs(i+1, val, diff, false, false)
			}
			up := 9
			if isLimit {
				up = int(s[i] - '0') // 如果前面填的数字都和 high 的一样，那么这一位至多填数字 s[i]（否则就超过 high 啦）
			}
			d := 0
			if !isNum {
				d = 1 // 如果前面没有填数字，必须从 1 开始（因为不能有前导零）
			}
			for ; d <= up; d++ { // 枚举要填入的数字 d
				res += dfs(i+1, (val*10+d)%k, diff+d%2*2-1, isLimit && d == up, true)
			}
			return
		}
		return dfs(0, 0, n, true, false)
	}
	return calc(high) - calc(low-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2kD)$，其中 $n=\mathcal{O}(\log\textit{high})$，也就是 $\textit{high}$ 的十进制表示的长度；$D=10$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2k)$，单个状态的计算时间为 $\mathcal{O}(D)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2kD)$。
- 空间复杂度：$\mathcal{O}(n^2k)$。即状态个数。

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
