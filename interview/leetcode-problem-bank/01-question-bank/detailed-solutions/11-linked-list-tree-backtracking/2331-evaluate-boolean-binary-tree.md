# 2331. 计算布尔二叉树的值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/evaluate-boolean-binary-tree/
- 题目 slug：`evaluate-boolean-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：1304
- 外部题解来源：https://leetcode.cn/problems/evaluate-boolean-binary-tree/solutions/1658331/by-endlesscheng-391l/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法，自身递归](https://leetcode.cn/problems/evaluate-boolean-binary-tree/solutions/1658331/by-endlesscheng-391l/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-391l`
- topic id：`1658331`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

```go
func evaluateTree(root *TreeNode) bool {
	if root.Left == nil {
		return root.Val == 1
	}
	if root.Val == 2 {
		return evaluateTree(root.Left) || evaluateTree(root.Right)
	}
	return evaluateTree(root.Left) && evaluateTree(root.Right)
}
```
