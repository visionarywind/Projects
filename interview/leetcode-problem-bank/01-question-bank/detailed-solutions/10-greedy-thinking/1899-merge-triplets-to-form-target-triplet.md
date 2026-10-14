# 1899. 合并若干三元组以形成目标三元组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-triplets-to-form-target-triplet/
- 题目 slug：`merge-triplets-to-form-target-triplet`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1636
- 外部题解来源：https://leetcode.cn/problems/merge-triplets-to-form-target-triplet/solutions/825882/pan-duan-man-zu-yao-qiu-de-san-yuan-zu-g-dnom/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[判断满足要求的三元组各部分是否出现过](https://leetcode.cn/problems/merge-triplets-to-form-target-triplet/solutions/825882/pan-duan-man-zu-yao-qiu-de-san-yuan-zu-g-dnom/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pan-duan-man-zu-yao-qiu-de-san-yuan-zu-g-dnom`
- topic id：`825882`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

若 $(a_i,b_i,c_i)$ 各部分均不超过 $(x,y,z)$，则可以执行更新操作。

因此对所有满足要求的 $(a_i,b_i,c_i)$，只要各部分都出现了 $x,y,z$，最终各部分就能更新成 $x,y,z$。

```go
func mergeTriplets(a [][]int, t []int) bool {
	found := [3]bool{}
	for _, p := range a {
		if p[0] <= t[0] && p[1] <= t[1] && p[2] <= t[2] {
			found[0] = found[0] || p[0] == t[0]
			found[1] = found[1] || p[1] == t[1]
			found[2] = found[2] || p[2] == t[2]
		}
	}
	return found[0] && found[1] && found[2]
}
```
