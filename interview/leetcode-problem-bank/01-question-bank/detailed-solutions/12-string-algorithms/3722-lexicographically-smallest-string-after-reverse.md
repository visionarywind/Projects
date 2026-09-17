# 3722. 反转后字典序最小的字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lexicographically-smallest-string-after-reverse/
- 题目 slug：`lexicographically-smallest-string-after-reverse`
- 来源专题：字符串
- 来源分类路径：四、字符串哈希
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/lexicographically-smallest-string-after-reverse/solutions/3815537/mei-ju-hou-zhui-shu-zu-pythonjavacgo-by-9pwcs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举 k，附 O(nlogn) 后缀数组做法（Python/Java/C++/Go）](https://leetcode.cn/problems/lexicographically-smallest-string-after-reverse/solutions/3815537/mei-ju-hou-zhui-shu-zu-pythonjavacgo-by-9pwcs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-hou-zhui-shu-zu-pythonjavacgo-by-9pwcs`
- topic id：`3815537`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

枚举 $k=1,2,\ldots,n$，反转长为 $k$ 的前缀/后缀，用得到的字符串更新答案的最小值。

[本题视频讲解](https://www.bilibili.com/video/BV1zxxNzcERu/)，顺带讲了非暴力做法，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def lexSmallest(self, s: str) -> str:
        ans = s  # k = 1 时，操作不改变 s
        for k in range(2, len(s) + 1):
            ans = min(ans, s[:k][::-1] + s[k:], s[:-k] + s[-k:][::-1])
        return ans
```

```java [sol-Java]
class Solution {
    public String lexSmallest(String s) {
        int n = s.length();
        String ans = s; // k = 1 时，操作不改变 s
        for (int k = 2; k <= n; k++) {
            StringBuilder t = new StringBuilder(s.substring(0, k)).reverse();
            ans = min(ans, t + s.substring(k));

            t = new StringBuilder(s.substring(n - k)).reverse();
            ans = min(ans, s.substring(0, n - k) + t);
        }
        return ans;
    }

    private String min(String a, String b) {
        return a.compareTo(b) <= 0 ? a : b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string lexSmallest(string s) {
        int n = s.size();
        string ans = s; // k = 1 时，操作不改变 s
        for (int k = 2; k <= n; k++) {
            string t = s.substr(0, k);
            ranges::reverse(t);
            ans = min(ans, t + s.substr(k));

            t = s.substr(n - k);
            ranges::reverse(t);
            ans = min(ans, s.substr(0, n - k) + t);
        }
        return ans;
    }
};
```

```go [sol-Go]
func lexSmallest(s string) string {
	n := len(s)
	ans := s // k = 1 时，操作不改变 s
	for k := 2; k <= n; k++ {
		t := []byte(s[:k])
		slices.Reverse(t)
		ans = min(ans, string(t)+s[k:])

		t = []byte(s[n-k:])
		slices.Reverse(t)
		ans = min(ans, s[:n-k]+string(t))
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 附：后缀数组做法

构造字符串 $t = \textit{rev}(s) + \texttt{#} + s$，上面的做法相当于比较 $t$ 中两个子串的字典序大小。

这可以用**后缀数组**（或者字符串哈希+二分）快速比较。

对于后缀数组，在经过 $\mathcal{O}(n\log n)$ 的预处理后，可以 $\mathcal{O}(1)$ 比较任意两个子串的字典序大小。

```go
type sparseTable[T any] struct {
	st [][]T
	op func(T, T) T
}

func newSparseTable[T any](nums []T, op func(T, T) T) sparseTable[T] {
	n := len(nums)
	w := bits.Len(uint(n))
	st := make([][]T, w)
	for i := range st {
		st[i] = make([]T, n)
	}
	copy(st[0], nums)
	for i := 1; i < w; i++ {
		for j := range n - 1<<i + 1 {
			st[i][j] = op(st[i-1][j], st[i-1][j+1<<(i-1)])
		}
	}
	return sparseTable[T]{st, op}
}

// [l, r) 下标从 0 开始
func (s sparseTable[T]) query(l, r int) T {
	k := bits.Len(uint(r-l)) - 1
	return s.op(s.st[k][l], s.st[k][r-1<<k])
}

func lexSmallest(s string) string {
	n := len(s)
	t := []byte(s)
	slices.Reverse(t)
	t = append(t, '#')
	t = append(t, s...)

	// === 后缀数组模板开始 ===

	// 后缀数组 sa（后缀序）
	// sa[i] 表示后缀字典序中的第 i 个字符串（的首字母）在 s 中的位置
	// 特别地，后缀 s[sa[0]:] 字典序最小，后缀 s[sa[n-1]:] 字典序最大
	sa := (*struct {
		_  []byte
		sa []int32
	})(unsafe.Pointer(suffixarray.New(t))).sa

	// 计算后缀名次数组
	// 后缀 s[i:] 位于后缀字典序中的第 rank[i] 个
	// 特别地，rank[0] 即 s 在后缀字典序中的排名，rank[n-1] 即 s[n-1:] 在字典序中的排名
	// 相当于 sa 的反函数，即 rank[sa[i]] = i
	rank := make([]int, len(sa))
	for i, p := range sa {
		rank[p] = i
	}

	// 计算高度数组（也叫 LCP 数组）
	// height[0] = 0（哨兵）
	// height[i] = LCP(s[sa[i]:], s[sa[i-1]:])   (i > 0)
	height := make([]int, len(sa))
	h := 0
	// 计算 s 与 s[sa[rank[0]-1]:] 的 LCP（记作 LCP0）
	// 计算 s[1:] 与 s[sa[rank[1]-1]:] 的 LCP（记作 LCP1）
	// 计算 s[2:] 与 s[sa[rank[2]-1]:] 的 LCP
	// ...
	// 计算 s[n-1:] 与 s[sa[rank[n-1]-1]:] 的 LCP
	// 从 LCP0 到 LCP1，我们只去掉了 s[0] 和 s[sa[rank[0]-1]] 这两个字符
	// 所以 LCP1 >= LCP0 - 1
	// 这样就能加快 LCP 的计算了（类似滑动窗口）
	// 注：实际只计算了 n-1 对 LCP，因为我们跳过了 rank[i] = 0 的情况
	for i, rk := range rank {
		if h > 0 {
			h--
		}
		if rk > 0 {
			for j := int(sa[rk-1]); i+h < len(t) && j+h < len(t) && t[i+h] == t[j+h]; h++ {
			}
		}
		height[rk] = h
	}

	st := newSparseTable(height, func(a int, b int) int { return min(a, b) })
	// 返回 LCP(s[i:], s[j:])，即两个后缀的最长公共前缀
	lcp := func(i, j int) int {
		if i == j {
			return len(sa) - i
		}
		// 将 s[i:] 和 s[j:] 通过 rank 数组映射为 height 的下标
		ri, rj := rank[i], rank[j]
		if ri > rj {
			ri, rj = rj, ri
		}
		// ri+1 是因为 height 的定义是 sa[i] 和 sa[i-1]
		// rj+1 是因为 query 是左闭右开
		return st.query(ri+1, rj+1)
	}

	// 比较两个子串，返回 strings.Compare(s[l1:r1], s[l2:r2])
	compareSubstring := func(l1, r1, l2, r2 int) int {
		len1, len2 := r1-l1, r2-l2
		l := lcp(l1, l2)
		if l >= min(len1, len2) {
			// 一个是子串另一个子串的前缀，或者完全相等
			return len1 - len2
		}
		// 此时两个子串一定不相等
		return rank[l1] - rank[l2] // 也可以写 int(s[l1+l]) - int(s[l2+l])
	}

	// === 后缀数组模板结束 ===

	// 反转前缀
	ansK := 1
	for k := 2; k <= n; k++ {
		c := compareSubstring(n-k, n-k+ansK, n-ansK, n)
		if c < 0 || c == 0 && compareSubstring(n-k+ansK, n, n+1+ansK, n+1+k) < 0 {
			ansK = k
		}
	}
	pre := []byte(s[:ansK])
	slices.Reverse(pre)
	ans := string(pre) + s[ansK:]

	// 反转真后缀
	// 剪枝：如果 s[0] > ans[0]，那么反转真后缀一定不优
	if s[0] == ans[0] {
		ansK = 1
		for k := 2; k < n; k++ {
			c := compareSubstring(0, k-ansK, n*2+1-k, n*2+1-ansK)
			if c < 0 || c == 0 && compareSubstring(k-ansK, k, 0, ansK) < 0 {
				ansK = k
			}
		}
		suf := []byte(s[n-ansK:])
		slices.Reverse(suf)
		ans = min(ans, s[:n-ansK]+string(suf))
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n\log n)$。

## 专题训练

见下面字符串题单的「**八、后缀数组/后缀自动机**」和「**四、字符串哈希**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `四、字符串哈希`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、字符串哈希`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
