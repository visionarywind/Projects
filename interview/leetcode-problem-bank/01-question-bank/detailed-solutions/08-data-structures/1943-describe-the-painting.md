# 1943. 描述绘画结果

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/describe-the-painting/
- 题目 slug：`describe-the-painting`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：1969
- 外部题解来源：https://leetcode.cn/problems/describe-the-painting/solutions/894515/shi-jian-sao-miao-xian-by-endlesscheng-a6v1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[事件扫描线](https://leetcode.cn/problems/describe-the-painting/solutions/894515/shi-jian-sao-miao-xian-by-endlesscheng-a6v1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-jian-sao-miao-xian-by-endlesscheng-a6v1`
- topic id：`894515`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于只有在区间端点处才会改变颜色（增加或减少），我们可以统计所有区间端点并按位置从小到大排序，然后遍历这些端点，在每个端点处累加颜色并记录答案。

```go
func splitPainting(segments [][]int) (ans [][]int64) {
	type event struct{ pos, color int }
	events := make([]event, 0, len(segments)*2)
	for _, seg := range segments {
		events = append(events, event{seg[0], seg[2]}, event{seg[1], -seg[2]}) // 记录每个区间左右端点及颜色
	}
	sort.Slice(events, func(i, j int) bool { return events[i].pos < events[j].pos }) // 按位置排序

	sum := 0
	for i, e := range events[:len(events)-1] {
		sum += e.color
		if sum > 0 && e.pos < events[i+1].pos {
			ans = append(ans, []int64{int64(e.pos), int64(events[i+1].pos), int64(sum)})
		}
	}
	return
}
```
