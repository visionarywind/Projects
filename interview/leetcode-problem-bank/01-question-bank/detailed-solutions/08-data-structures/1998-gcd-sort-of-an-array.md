# 1998. 数组的最大公因数排序

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/gcd-sort-of-an-array/
- 题目 slug：`gcd-sort-of-an-array`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.3 中介并查集
- 难度分：2429
- 外部题解来源：https://leetcode.cn/problems/gcd-sort-of-an-array/solutions/979224/bing-cha-ji-fen-zu-pai-xu-by-endlesschen-kfav/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[并查集+分组排序](https://leetcode.cn/problems/gcd-sort-of-an-array/solutions/979224/bing-cha-ji-fen-zu-pai-xu-by-endlesschen-kfav/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bing-cha-ji-fen-zu-pai-xu-by-endlesschen-kfav`
- topic id：`979224`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
const mx int = 1e5
var pf [mx + 1][]int

func init() { // 预处理每个数的质因子
	for i := 2; i <= mx; i++ {
		if pf[i] == nil {
			for j := i; j <= mx; j += i {
				pf[j] = append(pf[j], i)
			}
		}
	}
}

func gcdSort(a []int) bool {
	n := len(a)
	u := newUnionFind(n + mx)
	for i, v := range a {
		for _, p := range pf[v] {
			u.merge(i, p+n) // 用并查集将下标 i 和质数 p 合并，为了区分下标集合和质数集合，将 p 加上 n
		}
	}

	// 整理处于同一集合的元素及其下标
	groups := make([]struct{ vs, is []int }, mx)
	for i, v := range a {
		p := u.find(i) - n
		groups[p].vs = append(groups[p].vs, v)
		groups[p].is = append(groups[p].is, i)
	}

	// 每一组内的元素可以直接或间接交换，因此对每组元素直接排序，然后按照在原数组中的顺序填回去
	for _, g := range groups {
		sort.Ints(g.vs)
		for j, v := range g.vs {
			a[g.is[j]] = v
		}
	}
	return sort.IntsAreSorted(a) // 判断数组是否有序
}

type uf struct {
	fa []int
}

func newUnionFind(n int) uf {
	fa := make([]int, n)
	for i := range fa {
		fa[i] = i
	}
	return uf{fa}
}

func (u uf) find(x int) int {
	if u.fa[x] != x {
		u.fa[x] = u.find(u.fa[x])
	}
	return u.fa[x]
}

func (u uf) merge(from, to int) {
	u.fa[u.find(from)] = u.find(to)
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.3 中介并查集`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.3 中介并查集`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
