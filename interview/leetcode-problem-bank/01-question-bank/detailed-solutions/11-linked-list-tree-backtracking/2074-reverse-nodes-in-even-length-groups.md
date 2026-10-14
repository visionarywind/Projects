# 2074. 反转偶数长度组的节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-nodes-in-even-length-groups/
- 题目 slug：`reverse-nodes-in-even-length-groups`
- 来源专题：链表、树与回溯
- 来源分类路径：一、链表 / §1.4 反转链表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/reverse-nodes-in-even-length-groups/solutions/1100918/zhi-jie-jiao-huan-yuan-su-zhi-by-endless-hsfq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[交换元素值的写法](https://leetcode.cn/problems/reverse-nodes-in-even-length-groups/solutions/1100918/zhi-jie-jiao-huan-yuan-su-zhi-by-endless-hsfq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-jie-jiao-huan-yuan-su-zhi-by-endless-hsfq`
- topic id：`1100918`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

```go
func reverseEvenLengthGroups(head *ListNode) *ListNode {
	var nodes []*ListNode
	for node, size := head, 1; node != nil; node = node.Next {
		nodes = append(nodes, node)
		if len(nodes) == size || node.Next == nil { // 统计到 size 个节点，或到达链表末尾
			if n := len(nodes); n%2 == 0 { // 有偶数个节点
				for i := 0; i < n/2; i++ {
					nodes[i].Val, nodes[n-1-i].Val = nodes[n-1-i].Val, nodes[i].Val // 直接交换元素值
				}
			}
			nodes = nil
			size++
		}
	}
	return head
}
```
