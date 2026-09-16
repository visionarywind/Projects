# 2930. 重新排列后包含指定子字符串的字符串数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-strings-which-can-be-rearranged-to-contain-substring/
- 题目 slug：`number-of-strings-which-can-be-rearranged-to-contain-substring`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.3 容斥原理
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-strings-which-can-be-rearranged-to-contain-substring/solutions/2522964/olog-n-rong-chi-yuan-li-fu-ji-yi-hua-sou-okjf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(log n) 容斥原理（附记忆化搜索）](https://leetcode.cn/problems/number-of-strings-which-can-be-rearranged-to-contain-substring/solutions/2522964/olog-n-rong-chi-yuan-li-fu-ji-yi-hua-sou-okjf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`olog-n-rong-chi-yuan-li-fu-ji-yi-hua-sou-okjf`
- topic id：`2522964`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Ww411T7JP/) 第三题，欢迎点赞关注~

正难则反。总共有 $26^n$ 个字符串，减去不含 `leet` 的字符串个数，就得到了答案。

不含 `leet` 的字符串，需要至少满足如下三个条件中的一个：

1. 不含 `l`。
2. 不含 `t`。
3. 不含 `e` 或者恰好包含一个 `e`。

分类讨论。

#### 至少满足一个条件

1. 不含 `l`：每个位置可以填 $25$ 种字母，方案数为 $25^n$。
2. 不含 `t`：同上，方案数为 $25^n$。
3. 不含 `e` 或者恰好包含一个 `e`：不含 `e` 同上，方案数为 $25^n$；恰好包含一个 `e`，先从 $n$ 个位置中选一个填 `e`，然后剩下 $n-1$ 个位置不能包含 `e`，方案数为 $n\cdot 25^{n-1}$。加起来就是 $25^n + n\cdot 25^{n-1}$。

直接加起来，就是 $(3\cdot 25+n)\cdot 25^{n-1}$，但这样就重复统计了「至少满足两个条件」的情况，要减去。

#### 至少满足两个条件

1. 不含 `l` 和 `t`：每个位置可以填 $24$ 种字母，方案数为 $24^n$。
2. 不含 `l` 且 `e` 的个数不足两个：同「满足一个条件」中 3 的分析，额外不能填 `l`，方案数为 $24^n + n\cdot 24^{n-1}$。
3. 不含 `t` 且 `e` 的个数不足两个：同上，方案数为 $24^n + n\cdot 24^{n-1}$。

直接加起来，就是 $(3\cdot 24+2n)\cdot 24^{n-1}$，但这样就重复统计了「满足三个条件」的情况，要减去。

#### 满足三个条件

同「满足一个条件」中 3 的分析，额外不能填 `l` 和 `t`，方案数为 $23^n + n\cdot 23^{n-1}$。

#### 总结

不含 `leet` 的字符串的个数为「至少满足一个条件」减去「至少满足两个条件」加上「满足三个条件」，这就是**容斥原理**。

最后用 $26^n$ 减去不含 `leet` 的字符串的个数，得到答案：

$$
26^n - (3\cdot 25+n)\cdot 25^{n-1} + (3\cdot 24+2n)\cdot 24^{n-1} - (23+n)\cdot 23^{n-1}
$$

其中 $x^n$ 可以用快速幂计算，具体请看 [50. Pow(x, n)](https://leetcode.cn/problems/powx-n/)。

关于取模的知识点，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
class Solution:
    def stringCount(self, n: int) -> int:
        MOD = 10 ** 9 + 7
        return (pow(26, n, MOD)
              - pow(25, n - 1, MOD) * (75 + n)
              + pow(24, n - 1, MOD) * (72 + n * 2)
              - pow(23, n - 1, MOD) * (23 + n)) % MOD
```

```java [sol-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;

    public int stringCount(int n) {
        return (int) (((pow(26, n)
                      - pow(25, n - 1) * (75 + n)
                      + pow(24, n - 1) * (72 + n * 2)
                      - pow(23, n - 1) * (23 + n)) % MOD + MOD) % MOD); // 保证结果非负
    }

    private long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    const long long MOD = 1e9 + 7;

    long long pow(long long x, int n) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }

public:
    int stringCount(int n) {
        return ((pow(26, n)
               - pow(25, n - 1) * (75 + n)
               + pow(24, n - 1) * (72 + n * 2)
               - pow(23, n - 1) * (23 + n)) % MOD + MOD) % MOD; // 保证结果非负
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

func stringCount(n int) (ans int) {
	return ((pow(26, n)-
		     pow(25, n-1)*(75+n)+
		     pow(24, n-1)*(72+n*2)-
		     pow(23, n-1)*(23+n))%mod + mod) % mod // 保证结果非负
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
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

#### 附：记忆化搜索

看成是「至少装满型」分组背包：有 $n$ 组物品，每组都可以从 `a` 到 `z` 中选一个，求**至少**有 $1$ 个 `l`、$1$ 个 `t` 和 $2$ 个 `e` 的方案数。

```py [sol-Python3]
@cache
def dfs(i: int, L: int, t: int, e: int) -> int:
    if i == 0:
        return 1 if L == t == e == 0 else 0
    res = dfs(i - 1, 0, t, e)  # 选 l
    res += dfs(i - 1, L, 0, e)  # 选 t
    res += dfs(i - 1, L, t, max(e - 1, 0))  # 选 e
    res += dfs(i - 1, L, t, e) * 23  # 其它字母
    return res % (10 ** 9 + 7)

class Solution:
    def stringCount(self, n: int) -> int:
        return dfs(n, 1, 1, 2)
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、组合数学 / §2.3 容斥原理`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.3 容斥原理`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
