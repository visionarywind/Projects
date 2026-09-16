# LCP 69. Hello LeetCode!

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/rMeRt2/
- 题目 slug：`rMeRt2`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/rMeRt2/solutions/1876097/z-by-endlesscheng-6ver/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理+状压DP](https://leetcode.cn/problems/rMeRt2/solutions/1876097/z-by-endlesscheng-6ver/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`z-by-endlesscheng-6ver`
- topic id：`1876097`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1rT411P7NA) 已出炉，欢迎点赞三连，在评论区分享你对这场力扣杯的看法~

记录关键思路，详细的说明见视频讲解。

1. 用位运算表示字母选择情况，由于一个字母可以选多个，因此要对二进制「分区」，每个区域表示对应字母的个数。
2. 写一个记忆化搜索，$f(i,\textit{mask})$ 表示从第 $i$ 个单词开始选，已经选择的单词为 $\textit{mask}$ 时，后续消耗代币的最小值。枚举 $\textit{words}[i]$ 的所有合法选择方案转移到 $f(i+1,\textit{mask}')$。
3. 因此需要预处理每个 $\textit{words}[i]$ 的每种选择字母的方案所消耗的代币的最小值，由于字符串很短，直接写个暴搜即可。

```py [sol-Python3]
# (字母在二进制上的起始位置, 这个字母能选择的上限, 位掩码)
RULES = {
    'e': (0, 4, 7),
    'l': (3, 3, 3),
    'o': (5, 2, 3),
    'h': (7, 1, 1),
    't': (8, 1, 1),
    'c': (9, 1, 1),
    'd': (10, 1, 1),
}
FULL = 2012  # 0b11111011100，每个字母都选到了对应的上限

# 合并两种选择字母的方案
def merge(cur: int, add: int) -> int:
    for pos, limit, m in RULES.values():
        c1 = (cur >> pos) & m
        c2 = (add >> pos) & m
        if c1 + c2 > limit:
            return -1
        cur += c2 << pos
    return cur

class Solution:
    def Leetcode(self, words: List[str]) -> int:
        # 预处理每个单词的每种选择字母的方案所消耗的代币的最小值
        costs = []
        for word in words:
            cost = {}
            def dfs(s: str, mask: int, tot: int) -> None:
                if mask not in cost or tot < cost[mask]:
                    cost[mask] = tot
                for i, c in enumerate(s):  # 枚举选择字母的位置
                    if c not in RULES:
                        continue
                    pos, limit, m = RULES[c]
                    if (mask >> pos) & m < limit:  # 可以选字母 c
                        dfs(s[:i] + s[i + 1:], mask + (1 << pos), tot + i * (len(s) - 1 - i))
            dfs(word, 0, 0)
            costs.append(cost)

        @cache
        def dfs(i: int, mask: int) -> int:
            if i == len(words):
                return 0 if mask == FULL else inf  # inf 表示不合法，没有选完要求的字母
            res = inf
            for add, tot in costs[i].items():
                if tot >= res: continue  # 剪枝
                m = merge(mask, add)
                if m >= 0:
                    res = min(res, tot + dfs(i + 1, m))
            return res
        ans = dfs(0, 0)
        return ans if ans < inf else -1
```

```go [sol-Go]
const keys = "elohtcd"
const full = 2012 // 0b11111011100，每个字母都选到了对应的上限

// pos：字母在二进制上的起始位置
// limit：这个字母能选择的上限
// mask：位掩码
var rules = ['z' + 1]struct{ pos, limit, mask int }{
	'e': {0, 4, 7},
	'l': {3, 3, 3},
	'o': {5, 2, 3},
	'h': {7, 1, 1},
	't': {8, 1, 1},
	'c': {9, 1, 1},
	'd': {10, 1, 1},
}

// 合并两种选择字母的方案
func merge(cur, add int) int {
	for _, c := range keys {
		r := rules[c]
		c1 := cur >> r.pos & r.mask
		c2 := add >> r.pos & r.mask
		if c1+c2 > r.limit {
			return -1
		}
		cur += c2 << r.pos
	}
	return cur
}

func Leetcode(words []string) int {
	const inf = math.MaxInt32 / 2
	n := len(words)
	// 预处理每个单词的每种选择字母的方案所消耗的代币的最小值
	costs := make([][1 << 11]int, n)
	for i, word := range words {
		for j := range costs[i] {
			costs[i][j] = inf
		}
		var f func(string, int, int)
		f = func(s string, mask, tot int) {
			costs[i][mask] = min(costs[i][mask], tot)
			for j, c := range s { // 枚举选择字母的位置
				r := rules[c]
				if mask>>r.pos&r.mask < r.limit { // 可以选字母 c
					f(s[:j]+s[j+1:], mask+1<<r.pos, tot+j*(len(s)-1-j))
				}
			}
		}
		f(word, 0, 0)
	}

	dp := make([][1 << 11]int, n)
	for i := range dp {
		for j := range dp[i] {
			dp[i][j] = -1
		}
	}
	var f func(int, int) int
	f = func(i, mask int) int {
		if i == n {
			if mask == full {
				return 0
			}
			return inf // inf 表示不合法，没有选完要求的字母
		}
		ptr := &dp[i][mask]
		if *ptr != -1 {
			return *ptr
		}
		res := inf
		for add, tot := range costs[i] {
			if tot >= res { // 剪枝
				continue
			}
			m2 := merge(mask, add)
			if m2 >= 0 {
				res = min(res, f(i+1, m2)+tot)
			}
		}
		*ptr = res
		return res
	}
	ans := f(0, 0)
	if ans == inf {
		return -1
	}
	return ans
}
```

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
