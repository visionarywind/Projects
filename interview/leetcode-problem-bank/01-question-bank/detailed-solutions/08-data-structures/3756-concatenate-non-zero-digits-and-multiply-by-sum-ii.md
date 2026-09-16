# 3756. 连接非零数字并乘以其数字和 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/concatenate-non-zero-digits-and-multiply-by-sum-ii/
- 题目 slug：`concatenate-non-zero-digits-and-multiply-by-sum-ii`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：1968
- 外部题解来源：https://leetcode.cn/problems/concatenate-non-zero-digits-and-multiply-by-sum-ii/solutions/3839697/san-ge-qian-zhui-he-pythonjavacgo-by-end-6e1a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三个前缀和（Python/Java/C++/Go）](https://leetcode.cn/problems/concatenate-non-zero-digits-and-multiply-by-sum-ii/solutions/3839697/san-ge-qian-zhui-he-pythonjavacgo-by-end-6e1a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-ge-qian-zhui-he-pythonjavacgo-by-end-6e1a`
- topic id：`3839697`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

对于数字和，用**前缀和**计算即可。关于前缀和数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

本题的难点是计算子串中的非零字符组成的数字。

先解决没有 $0$ 的简单情况。

比如 $s=12345$，计算子串 $34$ 对应的数字。

我们可以先计算 $s$ 的每个前缀（包括空前缀）对应的数字，即 $\textit{preNum} = [0,1,12,123,1234,12345]$。

想一想，如何从这些数中得到 $34$？

我们可以计算 $1234 - 12\cdot 10^2 = 1234-1200 = 34$。其中 $2$ 是子串 $34$ 的长度。

一般地，子串 $[l,r]$ 对应的数字为

$$
\textit{preNum}[r+1] - \textit{preNum}[l] \cdot 10^{r-l+1}
$$

然后解决包含 $0$ 的情况。

这意味着我们还需要计算子串中的非零字符个数，代替上式中的 $r-l+1$。

把 $s$ 中的非零字符视作 $1$，计算其前缀和。

代码实现时，注意取模。为什么可以在中途取模？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1arUKBbEks/?t=52m5s)，欢迎点赞关注~

```py [sol-Python3]
MOD = 1_000_000_007
MAX_N = 100_001

# 预处理 10 的幂
pow10 = [1] * MAX_N
for i in range(1, MAX_N):
    pow10[i] = (pow10[i - 1] * 10) % MOD

class Solution:
    def sumAndMultiply(self, s: str, queries: List[List[int]]) -> List[int]:
        n = len(s)
        sum_d = [0] * (n + 1)         # s 的前缀和
        pre_num = [0] * (n + 1)       # s 的前缀对应的数字（模 MOD）
        sum_non_zero = [0] * (n + 1)  # s 的前缀中的非零数字个数
        for i, d in enumerate(map(int, s)):
            sum_d[i + 1] = sum_d[i] + d
            pre_num[i + 1] = (pre_num[i] * 10 + d) % MOD if d else pre_num[i]
            sum_non_zero[i + 1] = sum_non_zero[i] + (d > 0)

        ans = []
        for l, r in queries:
            r += 1  # 避免下面多次计算 r+1
            length = sum_non_zero[r] - sum_non_zero[l]
            x = pre_num[r] - pre_num[l] * pow10[length]
            ans.append(x * (sum_d[r] - sum_d[l]) % MOD)
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MAX_N = 100_001;
    private static final int[] pow10 = new int[MAX_N];
    private static boolean initialized = false;

    // 这样写比 static block 快
    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        // 预处理 10 的幂
        pow10[0] = 1;
        for (int i = 1; i < MAX_N; i++) {
            pow10[i] = (int) (pow10[i - 1] * 10L % MOD);
        }
    }

    public int[] sumAndMultiply(String s, int[][] queries) {
        int n = s.length();
        int[] sumD = new int[n + 1];       // s 的前缀和
        int[] preNum = new int[n + 1];     // s 的前缀对应的数字（模 MOD）
        int[] sumNonZero = new int[n + 1]; // s 的前缀中的非零数字个数
        for (int i = 0; i < n; i++) {
            int d = s.charAt(i) - '0';
            sumD[i + 1] = sumD[i] + d;
            preNum[i + 1] = d > 0 ? (int) ((preNum[i] * 10L + d) % MOD) : preNum[i];
            sumNonZero[i + 1] = sumNonZero[i] + (d > 0 ? 1 : 0);
        }

        int[] ans = new int[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int l = queries[i][0];
            int r = queries[i][1] + 1; // 注意这里已经把 r 加一了
            int length = sumNonZero[r] - sumNonZero[l];
            long x = preNum[r] - (long) preNum[l] * pow10[length] % MOD + MOD; // +MOD 保证结果非负
            ans[i] = (int) (x * (sumD[r] - sumD[l]) % MOD);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
static constexpr int MOD = 1'000'000'007;
static constexpr int MAX_N = 100'001;
int pow10[MAX_N];

int init = [] {
    // 预处理 10 的幂
    pow10[0] = 1;
    for (int i = 1; i < MAX_N; i++) {
        pow10[i] = pow10[i - 1] * 10LL % MOD;
    }
    return 0;
}();

class Solution {
public:
    vector<int> sumAndMultiply(const string& s, const vector<vector<int>>& queries) {
        int n = s.size();
        vector<int> sum_d(n + 1), pre_num(n + 1), sum_non_zero(n + 1);
        for (int i = 0; i < n; i++) {
            int d = s[i] - '0';
            sum_d[i + 1] = sum_d[i] + d; // s 的前缀和
            pre_num[i + 1] = d > 0 ? (pre_num[i] * 10LL + d) % MOD : pre_num[i]; // s 的前缀对应的数字（模 MOD）
            sum_non_zero[i + 1] = sum_non_zero[i] + (d > 0); // s 的前缀中的非零数字个数
        }

        vector<int> ans;
        ans.reserve(queries.size()); // 预分配空间
        for (auto& q : queries) {
            int l = q[0], r = q[1] + 1; // 注意这里已经把 r 加一了
            int length = sum_non_zero[r] - sum_non_zero[l];
            long long x = pre_num[r] - 1LL * pre_num[l] * pow10[length] % MOD + MOD; // +MOD 保证结果非负
            ans.push_back(x * (sum_d[r] - sum_d[l]) % MOD);
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const maxN = 100_001

var pow10 = [maxN]int{1}

func init() {
	// 预处理 10 的幂
	for i := 1; i < maxN; i++ {
		pow10[i] = pow10[i-1] * 10 % mod
	}
}

func sumAndMultiply(s string, queries [][]int) []int {
	n := len(s)
	sumD := make([]int, n+1)       // s 的前缀和
	preNum := make([]int, n+1)     // s 的前缀对应的数字（模 mod）
	sumNonZero := make([]int, n+1) // s 的前缀中的非零数字个数
	for i, ch := range s {
		d := int(ch - '0')
		sumD[i+1] = sumD[i] + d
		preNum[i+1] = preNum[i]
		sumNonZero[i+1] = sumNonZero[i]
		if d > 0 {
			preNum[i+1] = (preNum[i]*10 + d) % mod
			sumNonZero[i+1]++
		}
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		l, r := q[0], q[1]+1
		length := sumNonZero[r] - sumNonZero[l]
		x := preNum[r] - preNum[l]*pow10[length]%mod + mod // +mod 保证结果非负
		ans[i] = x * (sumD[r] - sumD[l]) % mod
	}
	return ans
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n+q)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 相似题目

[2156. 查找给定哈希值的子串](https://leetcode.cn/problems/find-substring-with-given-hash-value/)

## 专题训练

见下面数据结构题单的「**一、前缀和**」。

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

本题来自 `一、前缀和 / §1.5 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.5 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
