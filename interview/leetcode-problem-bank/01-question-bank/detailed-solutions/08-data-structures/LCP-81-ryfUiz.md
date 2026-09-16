# LCP 81. 与非的谜题

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/ryfUiz/
- 题目 slug：`ryfUiz`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.3 线段树（无区间更新）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/ryfUiz/solutions/2260509/chai-wei-xian-duan-shu-by-endlesscheng-d18s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[拆位+线段树](https://leetcode.cn/problems/ryfUiz/solutions/2260509/chai-wei-xian-duan-shu-by-endlesscheng-d18s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-wei-xian-duan-shu-by-endlesscheng-d18s`
- topic id：`2260509`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

1. 拆位。
2. 想象成 $0$ 或者 $1$ 从左到右「穿过」数组 $\textit{arr}$，用线段树维护穿过某段区间后，这个比特如何变化。 
3. 线段树叶子：$0$ 和 $1$ 穿过（NAND）$0$ 都变成 $1$；$0$ 和 $1$ 穿过 $1$ 分别变成 $1$ 和 $0$。
4. 线段树合并区间：先变成穿过左儿子的结果，然后把这个结果穿过右儿子，得到穿过这段区间的结果。
5. $\textit{type}=0$ 就是线段树的单点修改。
6. $\textit{type}=1$ 可以拆位，对于每个比特位上的数字，分类讨论：
   1. 如果穿过整个数组，数字没变，那么 $n$ 次后仍然没变。
   2. 否则，如果 $x=1$，那么变成穿过一次数组后的结果。
   3. 否则，如果穿过两次和穿过一次的结果一样，那么 $n$ 次后的结果就是穿过一次的结果。
   4. 否则，穿过两次回到原数字，那么分 $x$ 的奇偶性讨论，奇变偶不变。

```go [sol1-Go]
var k int

type seg []struct {
	l, r int
	to   [2]int
}

func (t seg) set(o, val int) {
	t[o].to[1] = t[o].to[0] ^ val
}

func (t seg) maintain(o int) {
	a, b, c := t[o<<1].to, t[o<<1|1].to, [2]int{}
	for i := 0; i < k; i++ {
		c[0] |= b[a[0]>>i&1] >> i & 1 << i
		c[1] |= b[a[1]>>i&1] >> i & 1 << i
	}
	t[o].to = c
}

func (t seg) build(a []int, k, o, l, r int) {
	t[o].l, t[o].r = l, r
	t[o].to[0] = 1<<k - 1
	if l == r {
		t.set(o, a[l-1])
		return
	}
	m := (l + r) >> 1
	t.build(a, k, o<<1, l, m)
	t.build(a, k, o<<1|1, m+1, r)
	t.maintain(o)
}

func (t seg) update(o, i, val int) {
	if t[o].l == t[o].r {
		t.set(o, val)
		return
	}
	m := (t[o].l + t[o].r) >> 1
	if i <= m {
		t.update(o<<1, i, val)
	} else {
		t.update(o<<1|1, i, val)
	}
	t.maintain(o)
}

func getNandResult(K int, arr []int, operations [][]int) (ans int) {
	k = K
	t := make(seg, len(arr)*4)
	t.build(arr, k, 1, 1, len(arr))
	for _, op := range operations {
		if op[0] == 0 {
			t.update(1, op[1]+1, op[2])
			continue
		}
		to := t[1].to
		x, y := op[1], op[2]
		for i := 0; i < k; i++ {
			var res int
			y := y >> i & 1
			y1 := to[y] >> i & 1 // 穿过 arr 一次
			if y1 == y { // 不变
				res = y
			} else if x == 1 || to[y1]>>i&1 == y1 {
				// 只穿过一次，或者穿过两次和穿过一次相同
				res = y1
			} else {
				res = y ^ x%2 // 奇变偶不变
			}
			ans ^= res << i
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k(n+q\log n))$，其中 $n$ 为 $\textit{arr}$ 的长度，$q$ 为 $\textit{operations}$ 的长度。
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

本题来自 `八、树状数组和线段树 / §8.3 线段树（无区间更新）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.3 线段树（无区间更新）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
