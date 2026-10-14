# 1992. 找到所有的农场组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-all-groups-of-farmland/
- 题目 slug：`find-all-groups-of-farmland`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1539
- 外部题解来源：https://leetcode.cn/problems/find-all-groups-of-farmland/solutions/978657/go-mo-ni-by-endlesscheng-j42f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 模拟](https://leetcode.cn/problems/find-all-groups-of-farmland/solutions/978657/go-mo-ni-by-endlesscheng-j42f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-mo-ni-by-endlesscheng-j42f`
- topic id：`978657`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

判断是否为矩形左上角，需要满足三个条件：

1. 元素值为 $1$；
2. 左边是边界或者是 $0$；
3. 上边是边界或者是 $0$。

然后遍历找到矩形的右边界和下边界。

```go
func findFarmland(a [][]int) (ans [][]int) {
	n, m := len(a), len(a[0])
	for i, row := range a {
		for j, v := range row {
			if v == 0 || j > 0 && row[j-1] == 1 || i > 0 && a[i-1][j] == 1 {
				continue
			}
			ii := i
			for ; ii+1 < n && a[ii+1][j] == 1; ii++ {}
			jj := j
			for ; jj+1 < m && a[ii][jj+1] == 1; jj++ {}
			ans = append(ans, []int{i, j, ii, jj})
		}
	}
	return
}
```
