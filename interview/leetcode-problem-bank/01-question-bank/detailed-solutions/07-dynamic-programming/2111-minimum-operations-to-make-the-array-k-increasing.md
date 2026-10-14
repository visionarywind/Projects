# 2111. 使数组 K 递增的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-the-array-k-increasing/
- 题目 slug：`minimum-operations-to-make-the-array-k-increasing`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.1 基础
- 难度分：1941
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-the-array-k-increasing/solutions/1165534/zui-chang-fei-jiang-zi-xu-lie-by-endless-x54c/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最长不降子序列，附进阶问题](https://leetcode.cn/problems/minimum-operations-to-make-the-array-k-increasing/solutions/1165534/zui-chang-fei-jiang-zi-xu-lie-by-endless-x54c/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-chang-fei-jiang-zi-xu-lie-by-endless-x54c`
- topic id：`1165534`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

将 $\textit{arr}$ 每 $k$ 个数取一个元素，分为若干组，例如当 $k=3$ 时，分为如下三组：

- $arr[0],arr[3],arr[6],\cdots$
- $arr[1],arr[4],arr[7],\cdots$
- $arr[2],arr[5],arr[8],\cdots$

根据题意，组与组之间是互不影响的，我们需要让每一组内的元素单调不降。

题目要求需要修改的最少元素个数，这可以通过求不需要修改的最多元素个数来求出，也就是该组元素的最长不降子序列的长度。（不了解的同学可以看看第 300 题）

需要注意的是：

- 求最长严格递增子序列需要二分找到大于或等于当前元素的元素位置（即 C++ 中的 `lower_bound`）；
- 求最长不降子序列需要二分找到大于当前元素的元素位置（即 C++ 中的 `upper_bound`）。

累加所有组的最长不降子序列的长度即为最多可以保留的元素个数，用 $\textit{arr}$ 的长度减去该个数即为答案。

```go
func kIncreasing(arr []int, k int) int {
	save := 0
	for i, n := 0, len(arr); i < k && i < n; i++ {
		f := []int{}
		for j := i; j < n; j += k {
			v := arr[j]
			if p := sort.SearchInts(f, v+1); p < len(f) {
				f[p] = v
			} else {
				f = append(f, v)
			}
		}
		save += len(f)
	}
	return len(arr) - save
}
```

**进阶问题**：[如果本题改成严格递增要怎么做？](https://leetcode.cn/discuss/post/3169302/guan-yu-zhe-zhou-zhou-sai-di-si-ti-kdi-z-zjc6/comments/2727336/)
