# 2037. 使每位学生都有座位的最少移动次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-moves-to-seat-everyone/
- 题目 slug：`minimum-number-of-moves-to-seat-everyone`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：1357
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-moves-to-seat-everyone/solutions/1050936/pai-xu-tan-xin-by-endlesscheng-vv35/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序+贪心](https://leetcode.cn/problems/minimum-number-of-moves-to-seat-everyone/solutions/1050936/pai-xu-tan-xin-by-endlesscheng-vv35/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-tan-xin-by-endlesscheng-vv35`
- topic id：`1050936`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

由于座位和学生数相同，一个萝卜一个坑，将座位和学生位置排序后，第 $i$ 个学生可以对应第 $i$ 个座位。

由于交换任意两个学生对应的座位不会产生更少的移动次数（可以画一画，证明略），所以上述对应关系可以产生最少移动次数，累加位置之差即为答案。

```go
func minMovesToSeat(seats, students []int) (ans int) {
	sort.Ints(seats)
	sort.Ints(students)
	for i, p := range seats {
		ans += abs(p - students[i])
	}
	return
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```
