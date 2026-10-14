# 2212. 射箭比赛中的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-points-in-an-archery-competition/
- 题目 slug：`maximum-points-in-an-archery-competition`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.2 子集型回溯
- 难度分：1869
- 外部题解来源：https://leetcode.cn/problems/maximum-points-in-an-archery-competition/solutions/1352593/er-jin-zhi-mei-ju-by-endlesscheng-rjul/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二进制枚举](https://leetcode.cn/problems/maximum-points-in-an-archery-competition/solutions/1352593/er-jin-zhi-mei-ju-by-endlesscheng-rjul/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-jin-zhi-mei-ju-by-endlesscheng-rjul`
- topic id：`1352593`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

由于只有 $12$ 个区域，我们可以枚举 Bob 在哪些区域上获胜，这一共有 $2^{12}$ 种不同的情况。

为了节省箭的数量，Bob 在获胜的区域只需要比 Alice 多射一支箭即可，如果有多余的未射出的箭则累加到任意一个区域上。

```go
func maximumBobPoints(numArrows int, aliceArrows []int) (ans []int) {
	for i, maxScore := 0, -1; i < 1<<len(aliceArrows); i++ { // 二进制枚举
		score, arrow, bobArrows := 0, 0, [12]int{}
		for j, v := range aliceArrows {
			if i>>j&1 == 1 {
				score += j
				arrow += v + 1
				bobArrows[j] = v + 1 // Bob 多射一支箭
			}
		}
		if arrow > numArrows { // 超出限制，区域集合不合法
			continue
		}
		if score > maxScore {
			maxScore = score
			bobArrows[0] += numArrows - arrow // 随便找个位置补满至 numArrows
			ans = bobArrows[:]
		}
	}
	return
}
```
