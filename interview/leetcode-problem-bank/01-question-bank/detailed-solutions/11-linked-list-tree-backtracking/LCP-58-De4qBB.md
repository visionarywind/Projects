# LCP 58. 积木拼接

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/De4qBB/
- 题目 slug：`De4qBB`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.7 搜索
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/De4qBB/solutions/1444450/0ms-zui-qiang-jian-zhi-by-endlesscheng-1beq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[0ms 最强剪枝](https://leetcode.cn/problems/De4qBB/solutions/1444450/0ms-zui-qiang-jian-zhi-by-endlesscheng-1beq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`0ms-zui-qiang-jian-zhi-by-endlesscheng-1beq`
- topic id：`1444450`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

![111.png](https://pic.leetcode.cn/1650765018-NOZJjI-111.png)

基本思路是搜索每个面放哪块积木，枚举积木的旋转+翻转状态。放之前先检查下当前放的积木和相邻的积木：

- 边是否严丝合缝；
- 角是否有重叠。

注意第一块积木直接放顶上就行，从第二块积木开始搜索。

下面是比赛中写的代码（合并了一些 `if` 逻辑），比赛中写了 200 行居然一次就过了。

```go
// 每条边压缩成二进制
func encode(a [][]byte) (res [4][2]int) {
	n := len(a)
	for i, b := range a[0] {
		res[0][0] |= int(b&1) << i // 正向
		res[0][1] |= int(b&1) << (n - 1 - i) // 反向
		res[2][0] |= int(a[n-1][i]&1) << i
		res[2][1] |= int(a[n-1][i]&1) << (n - 1 - i)
	}
	for i, r := range a {
		res[1][0] |= int(r[n-1]&1) << i
		res[1][1] |= int(r[n-1]&1) << (n - 1 - i)
		res[3][0] |= int(r[0]&1) << i
		res[3][1] |= int(r[0]&1) << (n - 1 - i)
	}
	return
}

// 顺时针旋转矩阵 90°
func rotate(a [][]byte) [][]byte {
	n, m := len(a), len(a[0])
	b := make([][]byte, m)
	for i := range b {
		b[i] = make([]byte, n)
	}
	for i, r := range a {
		for j, v := range r {
			b[j][n-1-i] = v
		}
	}
	return b
}

func composeCube(shapes [][]string) bool {
	n := len(shapes[0])
	a := [6][8][4][2]int{} // [积木][旋转+翻转][边][0-正向/1-反向]
	for i, shape := range shapes {
		t := make([][]byte, n)
		for j, s := range shape {
			t[j] = []byte(s)
		}
		for j := 0; j < 4; j++ {
			a[i][j] = encode(t)
			t = rotate(t)
		}
		for _, r := range t {
			for j := 0; j < n/2; j++ {
				r[j], r[n-1-j] = r[n-1-j], r[j]
			}
		}
		for j := 4; j < 8; j++ {
			a[i][j] = encode(t)
			t = rotate(t)
		}
	}

	// 判断两条边是否恰好重叠（除了顶角）
	MASK := 1<<(n-1) - 2
	ok := func(v, w int) bool { return v&w == 0 && (v|w)&MASK == MASK }

	type pair struct{ who, rot int }
	fill := [6]pair{} // 枚举每个积木以什么旋转/翻转姿势放在哪个面（0-顶面，1234-侧面，5-底面）
	vis := 0
	var dfs func(int) bool
	dfs = func(p int) bool { // 当前考虑的面
		if p == 6 { return true }
		for cur := 1; cur < 6; cur++ { // 枚举 6 个积木（固定第一个积木放在顶面）
			if vis>>cur&1 > 0 { continue }
			vis ^= 1 << cur
			for rot := 0; rot < 8; rot++ { // 枚举 8 种旋转+翻转的情况
				switch p {
				case 1:
					// 1 和 0 是否有冲突
					if !ok(a[cur][rot][0][0], a[0][0][2][0]) {
						continue
					}
				case 2:
					// 2 和 0 1 是否有冲突
					w, r := fill[p-1].who, fill[p-1].rot
					if !ok(a[cur][rot][0][0], a[0][0][1][1]) || // 边是否冲突
						!ok(a[cur][rot][3][0], a[w][r][1][0]) ||
						a[0][0][2][1]&1 == 0 && a[cur][rot][0][0]&1 == 0 && a[w][r][0][1]&1 == 0 { // 角是否冲突
						continue
					}
				case 3:
					// 3 和 0 2 是否有冲突
					w, r := fill[p-1].who, fill[p-1].rot
					if !ok(a[cur][rot][0][0], a[0][0][0][1]) ||
						!ok(a[cur][rot][3][0], a[w][r][1][0]) ||
						a[0][0][1][0]&1 == 0 && a[cur][rot][0][0]&1 == 0 && a[w][r][0][1]&1 == 0 {
						continue
					}
				case 4:
					// 4 和 0 1 3 是否有冲突
					w, r := fill[p-1].who, fill[p-1].rot
					w1, r1 := fill[1].who, fill[1].rot
					if !ok(a[cur][rot][0][0], a[0][0][3][0]) ||
						!ok(a[cur][rot][3][0], a[w][r][1][0]) ||
						!ok(a[cur][rot][1][0], a[w1][r1][3][0]) ||
						a[0][0][3][0]&1 == 0 && a[cur][rot][0][0]&1 == 0 && a[w][r][0][1]&1 == 0 ||
						a[0][0][2][0]&1 == 0 && a[cur][rot][0][1]&1 == 0 && a[w1][r1][0][0]&1 == 0 {
						continue
					}
				default:
					// 5 和 1 2 3 4 是否有冲突
					w1, r1 := fill[1].who, fill[1].rot
					w2, r2 := fill[2].who, fill[2].rot
					w3, r3 := fill[3].who, fill[3].rot
					w4, r4 := fill[4].who, fill[4].rot
					if !ok(a[cur][rot][0][0], a[w1][r1][2][0]) ||
						!ok(a[cur][rot][1][0], a[w2][r2][2][0]) ||
						!ok(a[cur][rot][2][1], a[w3][r3][2][0]) ||
						!ok(a[cur][rot][3][1], a[w4][r4][2][0]) ||
						a[cur][rot][0][1]&1 == 0 && a[w1][r1][2][1]&1 == 0 && a[w2][r2][2][0]&1 == 0 ||
						a[cur][rot][1][1]&1 == 0 && a[w2][r2][2][1]&1 == 0 && a[w3][r3][2][0]&1 == 0 ||
						a[cur][rot][2][0]&1 == 0 && a[w3][r3][2][1]&1 == 0 && a[w4][r4][2][0]&1 == 0 ||
						a[cur][rot][0][0]&1 == 0 && a[w4][r4][2][1]&1 == 0 && a[w1][r1][2][0]&1 == 0 {
						continue
					}
				}
				fill[p] = pair{cur, rot}
				if dfs(p + 1) { return true }
			}
			vis ^= 1 << cur
		}
		return false
	}
	return dfs(1)
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.7 搜索`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.7 搜索`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
