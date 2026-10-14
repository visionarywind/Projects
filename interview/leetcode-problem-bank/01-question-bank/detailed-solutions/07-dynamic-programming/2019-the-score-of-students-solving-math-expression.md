# 2019. 解出数学表达式的学生分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/the-score-of-students-solving-math-expression/
- 题目 slug：`the-score-of-students-solving-math-expression`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.2 区间 DP
- 难度分：2584
- 外部题解来源：https://leetcode.cn/problems/the-score-of-students-solving-math-expression/solutions/1018504/qu-jian-dpha-xi-biao-by-endlesscheng-62fj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[区间 DP+哈希表](https://leetcode.cn/problems/the-score-of-students-solving-math-expression/solutions/1018504/qu-jian-dpha-xi-biao-by-endlesscheng-62fj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qu-jian-dpha-xi-biao-by-endlesscheng-62fj`
- topic id：`1018504`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func scoreOfStudents(s string, answers []int) (ans int) {
	n := len(s)
	// 计算正确答案
	correct := 0
	for i := 0; i < n; {
		mul := int(s[i] & 15)
		for i += 2; i < n && s[i-1] == '*'; i += 2 {
			mul *= int(s[i] & 15)
		}
		correct += mul
	}

	// 区间 DP
	// dp[l][r] 表示 s[l..r] 内的表达式在不同计算顺序下的不超过 1000 的所有可能值
	dp := make([][]map[int]bool, n)
	for i := range dp {
		dp[i] = make([]map[int]bool, n)
	}
	var f func(int, int) map[int]bool
	f = func(l, r int) map[int]bool {
		if l == r {
			return map[int]bool{int(s[l] & 15): true}
		}
		if dp[l][r] != nil {
			return dp[l][r]
		}
		res := map[int]bool{}
		for i := l + 1; i < r; i += 2 { // 枚举分界点
			for v := range f(l, i-1) { // 计算左边表达式的可能值
				for w := range f(i+1, r) { // 计算右边表达式的可能值
					x := v + w
					if s[i] == '*' {
						x = v * w
					}
					if x <= 1000 { // 超过 1000 的结果不计入
						res[x] = true
					}
				}
			}
		}
		dp[l][r] = res
		return res
	}
	res := f(0, n-1)
	for _, v := range answers {
		if v == correct {
			ans += 5
		} else if res[v] {
			ans += 2
		}
	}
	return
}
```
