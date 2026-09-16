# 2818. 操作使得分最大

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/apply-operations-to-maximize-score/
- 题目 slug：`apply-operations-to-maximize-score`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.3 质因数分解
- 难度分：2397
- 外部题解来源：https://leetcode.cn/problems/apply-operations-to-maximize-score/solutions/2385936/gong-xian-fa-dan-diao-zhan-pythonjavacgo-23c4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贡献法+单调栈（Python/Java/C++/Go）](https://leetcode.cn/problems/apply-operations-to-maximize-score/solutions/2385936/gong-xian-fa-dan-diao-zhan-pythonjavacgo-23c4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gong-xian-fa-dan-diao-zhan-pythonjavacgo-23c4`
- topic id：`2385936`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1wh4y1Q7XW/) 第四题。

本文接着 [贡献法+单调栈](https://leetcode.cn/problems/sum-of-subarray-minimums/solution/gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5/) 继续讲，请先阅读这篇题解，因为核心逻辑是一样的。

贪心地说，先考虑 $\textit{nums}$ 中最大的元素 $x$，我们需要知道：有多少个子数组可以取 $x$ 作为质数分数最高的元素。

我们可以先把 $[1,10^5]$ 中的每个数的质数分数（不同质因子的数目）预处理出来，记作数组 $\textit{omega}$。

然后用单调栈求出每个 $i$ 左侧质数分数【大于等于】$\textit{omega}[\textit{nums}[i]]$ 的最近的数的下标 $\textit{left}[i]$（不存在则为 $-1$），以及右侧质数分数【大于】$\textit{omega}[\textit{nums}[i]]$ 的最近的数的下标 $\textit{right}[i]$（不存在则为 $n$）。

请注意，我们不能在 $i$ 左侧包含质数分数和 $\textit{omega}[\textit{nums}[i]]$ 一样的数，否则不满足题目「如果多个元素质数分数相同且最高，选择下标最小的一个」的要求。所以对于左侧用【大于等于】。

那么子数组的左端点可以在 $(\textit{left}[i],i]$ 内，子数组的右端点可以在 $[i,\textit{right}[i])$ 内。

根据 [乘法原理](https://baike.baidu.com/item/%E4%B9%98%E6%B3%95%E5%8E%9F%E7%90%86/7538447)，一共有

$$
\textit{tot} = (i-\textit{left}[i])\cdot (\textit{right}[i]-i)
$$

个子数组以 $\textit{nums}[i]$ 作为这个子数组的质数分数。

设 $k'=\min(k, \textit{tot})$，则 $\textit{nums}[i]$ 对答案的贡献为

$$
\textit{nums}[i] ^ {k'}
$$

上式可以用快速幂计算，具体请看 [50. Pow(x, n)](https://leetcode.cn/problems/powx-n/)。

根据上式，按照 $\textit{nums}$ 从大到小的顺序计算贡献，一边计算一边更新剩余操作次数 $k$。

```py [sol-Python3]
MOD = 10 ** 9 + 7
MX = 10 ** 5 + 1
omega = [0] * MX
for i in range(2, MX):  # 预处理 omega
    if omega[i] == 0:  # i 是质数
        for j in range(i, MX, i):
            omega[j] += 1  # i 是 j 的一个质因子

class Solution:
    def maximumScore(self, nums: List[int], k: int) -> int:
        n = len(nums)
        left = [-1] * n  # 质数分数 >= omega[nums[i]] 的左侧最近元素下标
        right = [n] * n  # 质数分数 >  omega[nums[i]] 的右侧最近元素下标
        st = []
        for i, v in enumerate(nums):
            while st and omega[nums[st[-1]]] < omega[v]:
                right[st.pop()] = i
            if st: left[i] = st[-1]
            st.append(i)

        ans = 1
        for i, v, l, r in sorted(zip(range(n), nums, left, right), key=lambda z: -z[1]):
            tot = (i - l) * (r - i)
            if tot >= k:
                ans = ans * pow(v, k, MOD) % MOD
                break
            ans = ans * pow(v, tot, MOD) % MOD
            k -= tot  # 更新剩余操作次数
        return ans
```

```java [sol-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;
    private static final int MX = (int) 1e5 + 1;
    private static final int[] omega = new int[MX];

    static {
        for (int i = 2; i < MX; i++)
            if (omega[i] == 0) // i 是质数
                for (int j = i; j < MX; j += i)
                    omega[j]++; // i 是 j 的一个质因子
    }

    public int maximumScore(List<Integer> nums, int k) {
        var a = nums.toArray(Integer[]::new);
        int n = a.length;
        var left = new int[n]; // 质数分数 >= omega[nums[i]] 的左侧最近元素下标
        var right = new int[n];// 质数分数 >  omega[nums[i]] 的右侧最近元素下标
        Arrays.fill(right, n);
        var st = new ArrayDeque<Integer>();
        st.push(-1); // 方便赋值 left
        for (int i = 0; i < n; i++) {
            while (st.size() > 1 && omega[a[st.peek()]] < omega[a[i]])
                right[st.pop()] = i;
            left[i] = st.peek();
            st.push(i);
        }

        var ids = new Integer[n];
        for (int i = 0; i < n; i++) ids[i] = i;
        Arrays.sort(ids, (i, j) -> a[j] - a[i]);

        long ans = 1;
        for (int i : ids) {
            long tot = (long) (i - left[i]) * (right[i] - i);
            if (tot >= k) {
                ans = ans * pow(a[i], k) % MOD;
                break;
            }
            ans = ans * pow(a[i], (int) tot) % MOD;
            k -= tot; // 更新剩余操作次数
        }
        return (int) ans;
    }

    private long pow(long x, int n) {
        var res = 1L;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) res = res * x % MOD;
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
const int MX = 1e5 + 1;
int omega[MX];
int init = []() {
    for (int i = 2; i < MX; i++)
        if (omega[i] == 0) // i 是质数
            for (int j = i; j < MX; j += i)
                omega[j]++; // i 是 j 的一个质因子
    return 0;
}();

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

public:
    int maximumScore(vector<int> &nums, int k) {
        int n = nums.size();
        vector<int> left(n, -1); // 质数分数 >= omega[nums[i]] 的左侧最近元素下标
        vector<int> right(n, n); // 质数分数 >  omega[nums[i]] 的右侧最近元素下标
        stack<int> st;
        for (int i = 0; i < n; i++) {
            while (!st.empty() && omega[nums[st.top()]] < omega[nums[i]]) {
                right[st.top()] = i;
                st.pop();
            }
            if (!st.empty()) left[i] = st.top();
            st.push(i);
        }

        vector<int> id(n);
        iota(id.begin(), id.end(), 0);
        sort(id.begin(), id.end(), [&](const int i, const int j) {
            return nums[i] > nums[j];
        });

        long long ans = 1;
        for (int i: id) {
            long long tot = (long long) (i - left[i]) * (right[i] - i);
            if (tot >= k) {
                ans = ans * pow(nums[i], k) % MOD;
                break;
            }
            ans = ans * pow(nums[i], tot) % MOD;
            k -= tot; // 更新剩余操作次数
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod int = 1e9 + 7

// 预处理 omega
const mx int = 1e5
var omega [mx + 1]int8
func init() {
	for i := 2; i <= mx; i++ {
		if omega[i] == 0 { // i 是质数
			for j := i; j <= mx; j += i {
				omega[j]++ // i 是 j 的一个质因子
			}
		}
	}
}

func maximumScore(nums []int, k int) int {
	n := len(nums)
	left := make([]int, n)  // 质数分数 >= omega[nums[i]] 的左侧最近元素下标
	right := make([]int, n) // 质数分数 >  omega[nums[i]] 的右侧最近元素下标
	for i := range right {
		right[i] = n
	}
	st := []int{-1}
	for i, v := range nums {
		for len(st) > 1 && omega[nums[st[len(st)-1]]] < omega[v] {
			right[st[len(st)-1]] = i
			st = st[:len(st)-1]
		}
		left[i] = st[len(st)-1]
		st = append(st, i)
	}

	id := make([]int, n)
	for i := range id {
		id[i] = i
	}
	sort.Slice(id, func(i, j int) bool { return nums[id[i]] > nums[id[j]] })

	ans := 1
	for _, i := range id {
		tot := (i - left[i]) * (right[i] - i)
		if tot >= k {
			ans = ans * pow(nums[i], k) % mod
			break
		}
		ans = ans * pow(nums[i], tot) % mod
		k -= tot // 更新剩余操作次数
	}
	return ans
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

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。这里忽略预处理 $\textit{omega}$ 的时间和空间。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `一、数论 / §1.3 质因数分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.3 质因数分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
