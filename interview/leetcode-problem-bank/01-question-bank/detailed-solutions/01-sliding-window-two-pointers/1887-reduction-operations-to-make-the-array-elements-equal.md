# 1887. 使数组元素相等的减少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reduction-operations-to-make-the-array-elements-equal/
- 题目 slug：`reduction-operations-to-make-the-array-elements-equal`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1428
- 外部题解来源：https://leetcode.cn/problems/reduction-operations-to-make-the-array-elements-equal/solutions/815296/pai-xu-hou-bian-li-xiang-dang-yu-qiu-chu-excq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序后遍历，简洁写法](https://leetcode.cn/problems/reduction-operations-to-make-the-array-elements-equal/solutions/815296/pai-xu-hou-bian-li-xiang-dang-yu-qiu-chu-excq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-hou-bian-li-xiang-dang-yu-qiu-chu-excq`
- topic id：`815296`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

相当于求出每个数在数组中是第几小的，排序后遍历一遍即可。

```go
func reductionOperations(a []int) (ans int) {
	sort.Ints(a)
	for i, k, n := 0, 0, len(a); i < n; k++ {
		start := i
		for ; i < n && a[i] == a[start]; i++ {}
		ans += (i - start) * k // [start, i) 区间内的数都是第 k 小的
	}
	return
}
```
