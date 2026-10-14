# 2001. 可互换矩形的组数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-pairs-of-interchangeable-rectangles/
- 题目 slug：`number-of-pairs-of-interchangeable-rectangles`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1436
- 外部题解来源：https://leetcode.cn/problems/number-of-pairs-of-interchangeable-rectangles/solutions/992917/tong-ji-bi-zhi-de-zui-jian-fen-shu-by-en-7p1b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计比值的最简分数](https://leetcode.cn/problems/number-of-pairs-of-interchangeable-rectangles/solutions/992917/tong-ji-bi-zhi-de-zui-jian-fen-shu-by-en-7p1b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-ji-bi-zhi-de-zui-jian-fen-shu-by-en-7p1b`
- topic id：`992917`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

将宽高比 $w/h$ 化简为最简分数，用哈希表统计最简分数的个数。若有 $m$ 个宽高比相同的矩形，则可互换的矩形对有 $C_m^2=\dfrac{m\cdot(m-1)}{2}$ 个。遍历哈希表，累加所有可互换的矩形对的个数即为答案。

相似题目：[149. 直线上最多的点数](https://leetcode.cn/problems/max-points-on-a-line/)

```go
func interchangeableRectangles(rectangles [][]int) (ans int64) {
	cnt := map[[2]int]int64{}
	for _, p := range rectangles {
		// 计算 w/h 的最简分数，计入哈希表
		w, h := p[0], p[1]
		g := gcd(w, h)
		cnt[[2]int{w / g, h / g}]++
	}
	for _, m := range cnt {
		ans += m * (m - 1) / 2
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```
