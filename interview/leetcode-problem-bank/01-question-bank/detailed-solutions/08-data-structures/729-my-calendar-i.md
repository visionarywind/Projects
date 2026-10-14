# 729. 我的日程安排表 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/my-calendar-i/
- 题目 slug：`my-calendar-i`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.5 动态开点线段树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/my-calendar-i/solutions/874115/go-ping-heng-shu-by-endlesscheng-ihq4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 平衡树](https://leetcode.cn/problems/my-calendar-i/solutions/874115/go-ping-heng-shu-by-endlesscheng-ihq4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-ping-heng-shu-by-endlesscheng-ihq4`
- topic id：`874115`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
type MyCalendar struct {
	*redblacktree.Tree
}

func Constructor() MyCalendar {
	t := redblacktree.NewWithIntComparator()
	t.Put(-1, -1) // 哨兵
	return MyCalendar{t}
}

func (c *MyCalendar) Book(start, end int) bool {
	floor, _ := c.Floor(start)
	if floor.Value.(int) > start { // [start,end) 左侧区间的右端点超过了 start
		return false
	}
	if it := c.IteratorAt(floor); it.Next() && it.Key().(int) < end { // [start,end) 右侧区间的左端点小于 end
		return false
	}
	c.Put(start, end) // 可以插入区间 [start,end)
	return true
}
```
