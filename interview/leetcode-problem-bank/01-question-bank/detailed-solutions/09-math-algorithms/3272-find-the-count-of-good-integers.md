# 3272. 统计好整数的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-count-of-good-integers/
- 题目 slug：`find-the-count-of-good-integers`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.2 组合计数
- 难度分：2382
- 外部题解来源：https://leetcode.cn/problems/find-the-count-of-good-integers/solutions/2899725/mei-ju-suo-you-hui-wen-shu-zu-he-shu-xue-3d35/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举所有回文数+组合数学（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-count-of-good-integers/solutions/2899725/mei-ju-suo-you-hui-wen-shu-zu-he-shu-xue-3d35/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-suo-you-hui-wen-shu-zu-he-shu-xue-3d35`
- topic id：`2899725`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

考虑枚举所有长为 $n$ 的回文数。

首先，知道了回文数的左半边，就知道了回文数的右半边，所以可以枚举回文数的左半边。

设 $m = \left\lfloor\dfrac{n-1}{2}\right\rfloor$，设 $\textit{base} = 10^m$。

在 $[\textit{base}, 10\cdot\textit{base})$ 范围内枚举所有长为 $n$ 的回文数的左半边。

如果回文数 $x$ 能被 $k$ 整除，那么接下来需要解决的问题有两个：

1. 计算 $x$ 有多少个不同的排列。
2. 不能重复统计。

为了保证不重复统计，可以像 [49. 字母异位词分组](https://leetcode.cn/problems/group-anagrams/solutions/2718519/ha-xi-biao-fen-zu-jian-ji-xie-fa-pythonj-1ukv/) 那样，把 $x$ 的十进制字符串 $s$ **排序**，如果之前遇到过同样的字符串 $t$，那么 $s$ 生成的所有排列，$t$ 也能生成。用哈希表记录排序后的字符串，如果 $s$ 排序后在哈希表中，那么就跳过。

下面是组合数学时间。

本质上计算的是「**有重复元素的排列个数**」。

统计 $s$ 中的每个数字的出现次数 $\textit{cnt}$。

先填最高位。由于不能有前导零，最高位可以填的数有 $n-\textit{cnt}_0$ 个。其余 $n-1$ 个数随便排，有 $(n-1)!$ 种方案。

当然，这里面有重复的，例如 $x=34543$，其中两个 $3$ 和两个 $4$ 的排列就是重复的，由于这两个 $3$ **无法区分**，两个 $4$ **无法区分**，方案数要除以 $2!2!$。

综上，排列个数为

$$
\dfrac{(n-\textit{cnt}_0)\cdot (n-1)!}{\prod\limits_{i=0}^{9}\textit{cnt}_i!}
$$

加入答案。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1ajHYeoEG5/) 第三题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countGoodIntegers(self, n: int, k: int) -> int:
        fac = [factorial(i) for i in range(n + 1)]
        ans = 0
        vis = set()
        base = 10 ** ((n - 1) // 2)
        for i in range(base, base * 10):  # 枚举回文数左半边
            s = str(i)
            s += s[::-1][n % 2:]
            if int(s) % k:  # 回文数不能被 k 整除
                continue

            sorted_s = ''.join(sorted(s))
            if sorted_s in vis:  # 不能重复统计
                continue
            vis.add(sorted_s)

            cnt = Counter(sorted_s)
            res = (n - cnt['0']) * fac[n - 1]
            for c in cnt.values():
                res //= fac[c]
            ans += res
        return ans
```

```java [sol-Java]
class Solution {
    public long countGoodIntegers(int n, int k) {
        int[] factorial = new int[n + 1];
        factorial[0] = 1;
        for (int i = 1; i <= n; i++) {
            factorial[i] = factorial[i - 1] * i;
        }

        long ans = 0;
        Set<String> vis = new HashSet<>();
        int base = (int) Math.pow(10, (n - 1) / 2);
        for (int i = base; i < base * 10; i++) { // 枚举回文数左半边
            String s = Integer.toString(i);
            s += new StringBuilder(s).reverse().substring(n % 2);
            if (Long.parseLong(s) % k > 0) { // 回文数不能被 k 整除
                continue;
            }

            char[] sortedS = s.toCharArray();
            Arrays.sort(sortedS);
            if (!vis.add(new String(sortedS))) { // 不能重复统计
                continue;
            }

            int[] cnt = new int[10];
            for (char c : sortedS) {
                cnt[c - '0']++;
            }
            int res = (n - cnt[0]) * factorial[n - 1];
            for (int c : cnt) {
                res /= factorial[c];
            }
            ans += res;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long countGoodIntegers(int n, int k) {
        vector<int> factorial(n + 1);
        factorial[0] = 1;
        for (int i = 1; i <= n; i++) {
            factorial[i] = factorial[i - 1] * i;
        }

        long long ans = 0;
        unordered_set<string> vis;
        int base = pow(10, (n - 1) / 2);
        for (int i = base; i < base * 10; i++) { // 枚举回文数左半边
            string s = to_string(i);
            s += string(s.rbegin() + (n % 2), s.rend());
            if (stoll(s) % k) { // 回文数不能被 k 整除
                continue;
            }

            ranges::sort(s);
            if (!vis.insert(s).second) { // 不能重复统计
                continue;
            }

            int cnt[10]{};
            for (char c : s) {
                cnt[c - '0']++;
            }
            int res = (n - cnt[0]) * factorial[n - 1];
            for (int c : cnt) {
                res /= factorial[c];
            }
            ans += res;
        }
        return ans;
    }
};
```

```go [sol-Go]
func countGoodIntegers(n, k int) (ans int64) {
	factorial := make([]int, n+1)
	factorial[0] = 1
	for i := 1; i <= n; i++ {
		factorial[i] = factorial[i-1] * i
	}

	vis := map[string]bool{}
	base := int(math.Pow10((n - 1) / 2))
	for i := base; i < base*10; i++ { // 枚举回文数左半边
		x := i
		t := i
		if n%2 > 0 {
			t /= 10
		}
		for ; t > 0; t /= 10 {
			x = x*10 + t%10
		}
		if x%k > 0 { // 回文数不能被 k 整除
			continue
		}

		bs := []byte(strconv.Itoa(x))
		slices.Sort(bs)
		s := string(bs)
		if vis[s] { // 不能重复统计
			continue
		}
		vis[s] = true

		cnt := [10]int{}
		for _, c := range bs {
			cnt[c-'0']++
		}
		res := (n - cnt[0]) * factorial[n-1]
		for _, c := range cnt {
			res /= factorial[c]
		}
		ans += int64(res)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(10^m\cdot n\log n)$，其中 $m = \left\lfloor\dfrac{n-1}{2}\right\rfloor$。
- 空间复杂度：粗略估计是 $\mathcal{O}(10^m\cdot n)$。紧一点的估计是 $\mathcal{O}\left(\binom {m+1+D} D \cdot n\right)$，其中 $D=9$。本质上计算的是一个长为 $m+1$ 的非递减数组的个数，其中数组元素范围是 $[0,D]$，这有 $\binom {m+1+D} D$ 个。但考虑到回文数还需要被 $k$ 整除，实际比这少。可以转化成网格图路径证明，详见 [题解方法二](https://leetcode.cn/problems/find-the-count-of-monotonic-pairs-ii/solutions/2876190/qian-zhui-he-you-hua-dppythonjavacgo-by-3biek/)。

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
