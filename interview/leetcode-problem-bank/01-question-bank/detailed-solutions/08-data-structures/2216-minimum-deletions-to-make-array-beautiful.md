# 2216. 美化数组的最少删除数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-deletions-to-make-array-beautiful/
- 题目 slug：`minimum-deletions-to-make-array-beautiful`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：1510
- 外部题解来源：https://leetcode.cn/problems/minimum-deletions-to-make-array-beautiful/solutions/1371885/zhan-si-xiang-mo-ni-o1-e-wai-kong-jian-b-mlx0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[栈思想模拟 + O(1) 额外空间](https://leetcode.cn/problems/minimum-deletions-to-make-array-beautiful/solutions/1371885/zhan-si-xiang-mo-ni-o1-e-wai-kong-jian-b-mlx0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhan-si-xiang-mo-ni-o1-e-wai-kong-jian-b-mlx0`
- topic id：`1371885`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**套路**：从前往后遍历 + 需要考虑相邻元素 + 有消除操作 = 栈。

---

遍历数组，用栈来模拟这个过程（实际不需要栈，后面会说明）：

- 如果栈大小为偶数，可以随意加入元素；
- 如果栈大小为奇数，那么加入的元素不能和栈顶相同。

遍历结束后，若栈大小为奇数，则移除栈顶。

最后栈大小就是保留的元素，用数组大小减去栈大小就是删除的元素个数。

实际上不需要栈，用一个变量表示栈的奇偶性即可。

```go
func minDeletion(a []int) (ans int) {
	odd := false // 栈大小的奇偶性
	for i, n := 0, len(a); i < n; {
		start := i
		// 注意这里的 i 和外层循环的 i 是同一个变量，因此时间复杂度为 O(n)
		for i < n && a[i] == a[start] { i++ }
		l := i - start // 连续相同元素个数
		if !odd { // 只能放一个元素
			ans += l - 1
			odd = true
		} else if l == 1 {
			odd = false
		} else { // 放两个元素，奇偶性不变
			ans += l - 2
		}
	}
	if odd { // 栈大小必须为偶数
		ans++
	}
	return
}
```

变形：如果把题面中的 `nums[i] != nums[i + 1]` 改成 `nums[i] == nums[i + 1]` 要怎么做？见[CF1660C](https://codeforces.com/contest/1660/problem/C)。
