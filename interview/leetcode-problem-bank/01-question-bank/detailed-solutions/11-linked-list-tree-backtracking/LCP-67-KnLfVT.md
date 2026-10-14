# LCP 67. 装饰树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/KnLfVT/
- 题目 slug：`KnLfVT`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/KnLfVT/solutions/1875286/jijian-by-endlesscheng-4oqo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[极简写法：调用自身](https://leetcode.cn/problems/KnLfVT/solutions/1875286/jijian-by-endlesscheng-4oqo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jijian-by-endlesscheng-4oqo`
- topic id：`1875286`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

[视频讲解](https://www.bilibili.com/video/BV1rT411P7NA) 已出炉，欢迎点赞三连，在评论区分享你对这场力扣杯的看法~

```py [sol1-Python]
class Solution:
    def expandBinaryTree(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        if root.left: root.left = TreeNode(-1, left=self.expandBinaryTree(root.left))
        if root.right: root.right = TreeNode(-1, right=self.expandBinaryTree(root.right))
        return root
```

```go [sol1-Go]
func expandBinaryTree(root *TreeNode) *TreeNode {
	if root.Left != nil {
		root.Left = &TreeNode{-1, expandBinaryTree(root.Left), nil}
	}
	if root.Right != nil {
		root.Right = &TreeNode{-1, nil, expandBinaryTree(root.Right)}
	}
	return root
}
```
