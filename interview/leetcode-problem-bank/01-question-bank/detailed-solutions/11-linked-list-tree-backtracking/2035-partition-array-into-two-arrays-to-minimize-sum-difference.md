# 2035. 将数组分成两个数组并最小化数组和的差

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partition-array-into-two-arrays-to-minimize-sum-difference/
- 题目 slug：`partition-array-into-two-arrays-to-minimize-sum-difference`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.8 折半搜索
- 难度分：2490
- 外部题解来源：https://leetcode.cn/problems/partition-array-into-two-arrays-to-minimize-sum-difference/solutions/1039419/zhe-ban-mei-ju-pai-xu-er-fen-by-endlessc-04fn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[折半枚举+排序+二分](https://leetcode.cn/problems/partition-array-into-two-arrays-to-minimize-sum-difference/solutions/1039419/zhe-ban-mei-ju-pai-xu-er-fen-by-endlessc-04fn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhe-ban-mei-ju-pai-xu-er-fen-by-endlessc-04fn`
- topic id：`1039419`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

两个数组和之差可以视作从 $\textit{nums}$ 中选 $n$ 个数取正号，其余 $n$ 个数取负号，然后求元素和。

我们可以使用折半枚举的方法，枚举 $\textit{nums}$ 的前 $n$ 个元素取正或取负的所有情况，按取正的个数分组，并按照元素和排序。然后枚举 $\textit{nums}$ 的后 $n$ 个元素取正或取负的所有情况，然后去对应的组里二分找元素和最近的数，答案即为所有情况中最小的差值。

相似题目：

- [1755. 最接近目标值的子序列和](https://leetcode.cn/problems/closest-subsequence-sum/)

```go
func minimumDifference(nums []int) int {
	n := len(nums) / 2
	a := nums[:n]
	res := make([][]int, n+1)
	for i := 0; i < 1<<n; i++ {
		sum, cnt := 0, 0
		for j, v := range a {
			if i>>j&1 > 0 { // 1 视作取正
				sum += v
				cnt++
			} else { // 0 视作取负
				sum -= v
			}
		}
		res[cnt] = append(res[cnt], sum) // 按照取正的个数将元素和分组
	}

	for _, b := range res {
		sort.Ints(b) // 排序，方便下面二分
	}

	ans := math.MaxInt64
	a = nums[n:]
	for i := 0; i < 1<<n; i++ {
		sum, cnt := 0, 0
		for j, v := range a {
			if i>>j&1 > 0 {
				sum += v
				cnt++
			} else {
				sum -= v
			}
		}
		// 在对应的组里二分最近的数
		b := res[cnt]
		j := sort.SearchInts(b, sum)
		if j < len(b) {
			ans = min(ans, b[j]-sum)
		}
		if j > 0 {
			ans = min(ans, sum-b[j-1])
		}
	}
	return ans
}

func min(a, b int) int {
	if a > b {
		return b
	}
	return a
}
```
