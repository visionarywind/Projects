# 1104. 二叉树寻路

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/path-in-zigzag-labelled-binary-tree/
- 题目 slug：`path-in-zigzag-labelled-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.16 其他
- 难度分：1545
- 外部题解来源：https://leetcode.cn/problems/path-in-zigzag-labelled-binary-tree/solutions/536343/di-gui-san-xing-jie-jue-ci-ti-by-endless-sr36/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[递归三行解决此题](https://leetcode.cn/problems/path-in-zigzag-labelled-binary-tree/solutions/536343/di-gui-san-xing-jie-jue-ci-ti-by-endless-sr36/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`di-gui-san-xing-jie-jue-ci-ti-by-endless-sr36`
- topic id：`536343`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

设当前 $label$ 二进制长度为 $n$，则上一排标号最小值加最大值为 $sum=2^{n-2}+2^{n-1}-1$。

另外，注意到在没有“之”字形标记时，对于一颗正常的二叉树，$label$ 的父节点为 $\dfrac{label}{2}$；在进行“之”字形标记后，其父节点为 $sum-\dfrac{label}{2}$。

这样可以得到如下递归代码：

```go
func pathInZigZagTree(label int) []int {
	if label == 1 {
		return []int{1}
	}
	return append(pathInZigZagTree(1<<(bits.Len(uint(label))-2)*3-1-label/2), label)
}
```
