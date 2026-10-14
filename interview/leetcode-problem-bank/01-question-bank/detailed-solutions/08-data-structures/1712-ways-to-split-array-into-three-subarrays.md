# 1712. 将数组分成三个子数组的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/ways-to-split-array-into-three-subarrays/
- 题目 slug：`ways-to-split-array-into-three-subarrays`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：2079
- 外部题解来源：https://leetcode.cn/problems/ways-to-split-array-into-three-subarrays/solutions/544682/golang-jian-ji-xie-fa-by-endlesscheng-xaad/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Golang 简洁写法](https://leetcode.cn/problems/ways-to-split-array-into-three-subarrays/solutions/544682/golang-jian-ji-xie-fa-by-endlesscheng-xaad/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`golang-jian-ji-xie-fa-by-endlesscheng-xaad`
- topic id：`544682`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

记两个分割位置为 $l$ 和 $r$，利用前缀和可以得到不等式 $S_l\le S_r-S_l\le S_n-S_r$.

变形为 $S_l\ge 2S_r-S_n$ 和 $2S_l\le S_r$.

枚举 $r$，通过二分可以得到使不等式成立的 $l$ 的范围，累加这个范围的长度即为答案。

代码实现时，应注意 $r$ 的范围需要满足 $2\le r<n$ 和 $2\cdot(2S_r-S_n)\le S_r$，即 $3S_r\le 2S_n$；$l$ 的范围需要满足 $1\le l<r$.

```go
func waysToSplit(a []int) (ans int) {
	n := len(a)
	sum := make([]int, n+1)
	for i, v := range a {
		sum[i+1] = sum[i] + v
	}
	for r := 2; r < n && 3*sum[r] <= 2*sum[n]; r++ {
		l1 := sort.SearchInts(sum[1:r], 2*sum[r]-sum[n]) + 1
		ans += sort.SearchInts(sum[l1:r], sum[r]/2+1)
	}
	return ans % (1e9 + 7)
}
```
