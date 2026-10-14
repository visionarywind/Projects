# 1932. 合并多棵二叉搜索树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-bsts-to-create-single-bst/
- 题目 slug：`merge-bsts-to-create-single-bst`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.9 二叉搜索树
- 难度分：2484
- 外部题解来源：https://leetcode.cn/problems/merge-bsts-to-create-single-bst/solutions/869912/gen-ju-er-cha-sou-suo-shu-de-xing-zhi-he-v8sv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[根据二叉搜索树的性质合并](https://leetcode.cn/problems/merge-bsts-to-create-single-bst/solutions/869912/gen-ju-er-cha-sou-suo-shu-de-xing-zhi-he-v8sv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gen-ju-er-cha-sou-suo-shu-de-xing-zhi-he-v8sv`
- topic id：`869912`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

```go
const mx int = 5e4 + 1

func canMerge(trees []*TreeNode) *TreeNode {
    isSub := [mx]bool{}      // 对于根节点，需要知道其是否为另一颗二叉搜索树的子节点
    roots := [mx]*TreeNode{} // 对于子节点，我们需要知道其数值所对应的二叉搜索树的根节点是哪个
    for _, rt := range trees {
        if rt.Left != nil {
            if isSub[rt.Left.Val] { // 由于二叉搜索树上不能有两个值相同的节点，所以 trees 中也不能有两个值相同的子节点
                return nil
            }
            isSub[rt.Left.Val] = true
        }
        if rt.Right != nil {
            if isSub[rt.Right.Val] {
                return nil
            }
            isSub[rt.Right.Val] = true
        }
        roots[rt.Val] = rt
    }

    var root *TreeNode
    for _, rt := range trees {
        if !isSub[rt.Val] { // 根节点不应是另一颗二叉搜索树的子节点，否则二叉搜索树上会出现两个值相同的节点
            if root != nil { // 根节点应只有一个，否则会构成森林
                return nil
            }
            root = rt
        }
    }
    if root == nil { // 未找到根节点
        return nil
    }

    cnt := 0
    // 在构建二叉搜索树的同时判断是否合法
    var build func(*TreeNode, int, int) *TreeNode
    build = func(node *TreeNode, l, r int) *TreeNode {
        cnt++
        if node.Left != nil {
            if node.Left.Val <= l {
                return nil
            }
            if lo := roots[node.Left.Val]; lo != nil {
                node.Left = build(lo, l, node.Val)
                if node.Left == nil {
                    return nil
                }
            }
        }
        if node.Right != nil {
            if node.Right.Val >= r {
                return nil
            }
            if ro := roots[node.Right.Val]; ro != nil {
                node.Right = build(ro, node.Val, r)
                if node.Right == nil {
                    return nil
                }
            }
        }
        return node
    }
    root = build(root, 0, mx)
    if cnt == len(trees) { // 所有 trees[i] 均参与构建二叉搜索树
        return root
    }
    return nil
}
```
