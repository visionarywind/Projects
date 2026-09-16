# LCP 43. 十字路口的交通

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/Y1VbOX/
- 题目 slug：`Y1VbOX`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/Y1VbOX/solutions/991709/dong-tai-gui-hua-by-endlesscheng-bcmi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划](https://leetcode.cn/problems/Y1VbOX/solutions/991709/dong-tai-gui-hua-by-endlesscheng-bcmi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-by-endlesscheng-bcmi`
- topic id：`991709`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
var mp = []int{'E': 0, 'S': 1, 'W': 2, 'N': 3} // 将方向字符映射到 0-3 上
var valid = [4][4][4]int{ // 当前车去方向，另一辆车来方向，另一辆车去方向
	{},
	{
		{},
		{1},
		{0, 0, 0, 1},
		{0, 0, 1},
	},
	{
		{},
		{1},
		{1, 1},
	},
	{
		{},
		{1, 0, 1},
		{1, 1},
		{1, 1, 1},
	},
}

// 两辆车 X 和 Y，将 X 旋转至方向 0 上，Y 跟着 X 一起旋转
// 这样上面就不用写一个很长的 valid 数组
func ok(fromX, toX, fromY, toY int) bool {
	toX = (toX - fromX + 4) % 4
	fromY = (fromY - fromX + 4) % 4
	toY = (toY - fromX + 4) % 4
	return valid[toX][fromY][toY] == 1
}

func trafficCommand(ds []string) int {
	n0, n1, n2, n3 := len(ds[0]), len(ds[1]), len(ds[2]), len(ds[3])
	dp := [21][21][21][21]int{}
	vis := [21][21][21][21]bool{}

	var f func(int, int, int, int) int
	f = func(p0, p1, p2, p3 int) (res int) {
		if p0 == n0 && p1 == n1 && p2 == n2 && p3 == n3 {
			return
		}
		dv := &dp[p0][p1][p2][p3]
		if vis[p0][p1][p2][p3] {
			return *dv
		}
		vis[p0][p1][p2][p3] = true
		defer func() { *dv = res }()
		ps := [4]int{p0, p1, p2, p3}
		res = 1e9
	outer:
		for sub := uint(1); sub < 16; sub++ { // 枚举选择哪几个方向的车通行
			a := [][2]int{}
			for s := sub; s > 0; s &= s - 1 {
				from := bits.TrailingZeros(s)
				if ps[from] == len(ds[from]) {
					continue outer
				}
				to := mp[ds[from][ps[from]]]
				for _, q := range a {
					if !ok(from, to, q[0], q[1]) {
						continue outer
					}
				}
				a = append(a, [2]int{from, to})
			}
			for _, p := range a {
				ps[p[0]]++
			}
			res = min(res, f(ps[0], ps[1], ps[2], ps[3])+1)
			for _, p := range a {
				ps[p[0]]--
			}
		}
		return
	}
	return f(0, 0, 0, 0)
}

func min(a, b int) int {
	if a > b {
		return b
	}
	return a
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
