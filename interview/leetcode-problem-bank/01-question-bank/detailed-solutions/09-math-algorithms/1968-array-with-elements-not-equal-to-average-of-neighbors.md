# 1968. 构造元素不等于两相邻元素平均值的数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/array-with-elements-not-equal-to-average-of-neighbors/
- 题目 slug：`array-with-elements-not-equal-to-average-of-neighbors`
- 来源专题：数学算法
- 来源分类路径：六、随机算法 / §6.2 随机化技巧
- 难度分：1499
- 外部题解来源：https://leetcode.cn/problems/array-with-elements-not-equal-to-average-of-neighbors/solutions/936634/pai-xu-hou-cuo-kai-yi-wei-he-bing-by-end-mqrk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序后从中间分开，错开一位合并](https://leetcode.cn/problems/array-with-elements-not-equal-to-average-of-neighbors/solutions/936634/pai-xu-hou-cuo-kai-yi-wei-he-bing-by-end-mqrk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-hou-cuo-kai-yi-wei-he-bing-by-end-mqrk`
- topic id：`936634`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

```go
func rearrangeArray(a []int) []int {
	sort.Ints(a)
	i, n := 0, len(a)
	ans := make([]int, n)
	for _, v := range a {
		ans[i] = v
		if i += 2; i >= n {
			i = 1
		}
	}
	return ans
}
```
