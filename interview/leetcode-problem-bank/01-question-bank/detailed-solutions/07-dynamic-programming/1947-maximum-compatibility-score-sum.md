# 1947. 最大兼容性评分和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-compatibility-score-sum/
- 题目 slug：`maximum-compatibility-score-sum`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-compatibility-score-sum/solutions/895170/om3-er-fen-tu-zui-da-dai-quan-pi-pei-by-a0wqb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(m^3) 带权二分图最大完美匹配](https://leetcode.cn/problems/maximum-compatibility-score-sum/solutions/895170/om3-er-fen-tu-zui-da-dai-quan-pi-pei-by-a0wqb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`om3-er-fen-tu-zui-da-dai-quan-pi-pei-by-a0wqb`
- topic id：`895170`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
const inf int = 1e9

func KuhnMunkres(wt [][]int) (sum int) {
	match := make([]int, len(wt)) // 右部点匹配了哪一个左部点
	la := make([]int, len(wt))
	for i, row := range wt {
		la[i] = -inf
		for _, v := range row {
			if v > la[i] {
				la[i] = v
			}
		}
	}
	lb := make([]int, len(wt))
	slack := make([]int, len(wt))
	for i := 1; i < len(wt); i++ {
		vb := make([]bool, len(wt))
		for j := 1; j < len(wt); j++ {
			slack[j] = inf
		}
		last := make([]int, len(wt)) // 右部点在交错树中的上一个右部点，用于倒推得到交错路
		y := 0
		match[0] = i // 一开始假设有一条 i-0 的匹配
		for {
			vb[y] = true
			x, nextY := match[y], 0
			delta := inf
			for j := 1; j < len(wt); j++ {
				if !vb[j] {
					if d := la[x] + lb[j] - wt[x][j]; d < slack[j] {
						slack[j] = d
						last[j] = y
					}
					if slack[j] < delta {
						delta = slack[j]
						nextY = j
					}
				}
			}
			// 当 delta=0 时，相当于沿着相等子图向下搜索一层
			// 当 delta>0 时，相当于直接回到最小边（新加入相等子图的边）处开始搜索
			if delta > 0 {
				for j := 0; j < len(wt); j++ {
					if vb[j] {
						la[match[j]] -= delta
						lb[j] += delta
					} else {
						slack[j] -= delta
					}
				}
			}
			y = nextY
			if match[y] == 0 {
				break
			}
		}
		// 倒推更新增广路
		for ; y > 0; y = last[y] {
			match[y] = match[last[y]]
		}
	}
	for w := 1; w < len(wt); w++ {
		sum += wt[match[w]][w]
	}
	return
}

func maxCompatibilitySum(students [][]int, mentors [][]int) int {
	m := len(students)
	wt := make([][]int, m+1)
	wt[0] = make([]int, m+1)
	for i := range wt[0] {
		wt[0][i] = -inf
	}
	for i, st := range students {
		wt[i+1] = make([]int, m+1)
		wt[i+1][0] = -inf
		for j, mt := range mentors {
			for k, v := range st {
				if v == mt[k] {
					wt[i+1][j+1]++
				}
			}
		}
	}
	return KuhnMunkres(wt)
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
