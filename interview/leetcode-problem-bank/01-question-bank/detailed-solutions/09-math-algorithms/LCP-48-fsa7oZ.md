# LCP 48. 无限棋局

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fsa7oZ/
- 题目 slug：`fsa7oZ`
- 来源专题：数学算法
- 来源分类路径：四、博弈论
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/fsa7oZ/solutions/1017041/fen-lei-tao-lun-mei-ju-by-endlesscheng-89hq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论+枚举](https://leetcode.cn/problems/fsa7oZ/solutions/1017041/fen-lei-tao-lun-mei-ju-by-endlesscheng-89hq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-mei-ju-by-endlesscheng-89hq`
- topic id：`1017041`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

分类讨论题：

1. 黑第一手就可以获胜，输出 `"Black"`
2. 黑第一手无法获胜
2.1 白可以一步胜，且获胜位置不止一处，此时黑无法阻止白获胜，输出 `"White"`
2.2 白可以一步胜，但获胜位置只有一处，此时黑第一手下在该处，白无法获胜，进入 3
2.3 白无获胜位置，进入 3
3. 白无法获胜，于是白的策略是防止黑获胜。枚举黑第一手的位置（除了 2.2 中黑只能下在一个位置），然后判断黑第二手能否有超过一处获胜位置，若有超过一处，则白无法阻止黑获胜，输出 `"Black"`，否则输出 `"None"`。

枚举位置的技巧见注释。

```go
const (
	black = "Black"
	white = "White"
	none  = "None"
)

type pair struct{ x, y int }
var dir8 = []pair{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}

func gobang(pieces [][]int) string {
	color := make(map[pair]int, len(pieces)+1)
	for _, p := range pieces {
		p[2]++ // 为方便利用零值，将黑改为 1，白改为 2
		color[pair{p[0], p[1]}] = p[2]
	}

	// 在 (i,j) 落子，颜色为 c，判断落子的一方是否获胜
	checkWin := func(i, j, c int) bool {
		for k, d := range dir8[:4] {
			cnt := 1
			// 检查一个方向
			for x, y := i+d.x, j+d.y; color[pair{x, y}] == c; x, y = x+d.x, y+d.y {
				cnt++
			}
			// 检查相反的另一方向
			d = dir8[k^4]
			for x, y := i+d.x, j+d.y; color[pair{x, y}] == c; x, y = x+d.x, y+d.y {
				cnt++
			}
			if cnt >= 5 {
				return true
			}
		}
		return false
	}

	// 1. 黑第一手就可以获胜
	for _, p := range pieces {
		if p[2] == 2 {
			continue
		}
		i, j := p[0], p[1]
		for _, d := range dir8 { // 黑要想一步获胜只能下在黑子周围
			if x, y := i+d.x, j+d.y; color[pair{x, y}] == 0 && checkWin(x, y, 1) {
				return black
			}
		}
	}

	// 2. 黑第一手无法获胜
	whites := map[pair]bool{}
	posW := pair{}
	for _, p := range pieces {
		if p[2] == 1 {
			continue
		}
		i, j := p[0], p[1]
		for _, d := range dir8 { // 白要想一步获胜只能下在白子周围
			x, y := i+d.x, j+d.y
			q := pair{x, y}
			if color[q] == 0 && checkWin(x, y, 2) {
				// 2.1 白可以一步胜，且获胜位置不止一处
				if whites[q] = true; len(whites) > 1 {
					return white
				}
				posW = q
			}
		}
	}

	// 2.2 白可以一步胜，但获胜位置只有一处
	if len(whites) == 1 {
		color[posW] = 1 // 黑第一手下在该处，阻止白获胜
		blacks := map[pair]bool{}
		// 检查第三步的黑能否获胜
		checkBlackWin := func(i, j int) bool {
			for _, d := range dir8 { // 黑要获胜只能下在黑子周围
				x, y := i+d.x, j+d.y
				p := pair{x, y}
				if color[p] == 0 && checkWin(x, y, 1) {
					if blacks[p] = true; len(blacks) > 1 {
						return true
					}
				}
			}
			return false
		}
		checkBlackWin(posW.x, posW.y)
		for _, p := range pieces {
			if p[2] == 1 && checkBlackWin(p[0], p[1]) {
				return black
			}
		}
		return none
	}

	// 3. 白无法获胜，于是白的策略是防止黑获胜
	// 根据黑第一步的落子位置，在该位置周围枚举黑第三步的落子位置，检查黑能否获胜
	checkBlackWin := func(i0, j0 int) bool {
		blacks := map[pair]bool{}
		for k, d := range dir8 {
			for l, i, j := 0, i0, j0; l < 5; l++ { // 如果黑可以获胜，这两枚黑子的距离不会超过 5
				i += d.x
				j += d.y
				p := pair{i, j}
				if color[p] > 0 {
					continue
				}
				cnt := 1
				// 检查一个方向
				for x, y := i+d.x, j+d.y; color[pair{x, y}] == 1; x, y = x+d.x, y+d.y {
					cnt++
				}
				// 检查相反的另一方向
				d2 := dir8[k^4]
				for x, y := i+d2.x, j+d2.y; color[pair{x, y}] == 1; x, y = x+d2.x, y+d2.y {
					cnt++
				}
				if cnt >= 5 {
					if blacks[p] = true; len(blacks) > 1 {
						return true
					}
				}
			}
		}
		return false
	}
	vis := map[pair]bool{} // 常数优化：避免重复枚举
	for _, p := range pieces {
		if p[2] == 2 {
			continue
		}
		i, j := p[0], p[1]
		// 枚举黑第一步的落子。由于黑要下两手棋，需要枚举黑子周围两圈
		for dx := -2; dx <= 2; dx++ {
			for dy := -2; dy <= 2; dy++ {
				if dx == 0 && dy == 0 {
					continue
				}
				x, y := i+dx, j+dy
				q := pair{x, y}
				if vis[q] || color[q] > 0 {
					continue
				}
				color[q] = 1 // 黑落子
				vis[q] = true
				if checkBlackWin(x, y) {
					return black
				}
				delete(color, q)
			}
		}
	}
	return none
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `四、博弈论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、博弈论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
