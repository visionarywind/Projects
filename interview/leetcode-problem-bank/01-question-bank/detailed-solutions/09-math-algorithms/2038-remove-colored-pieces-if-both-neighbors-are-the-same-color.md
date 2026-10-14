# 2038. 如果相邻两个颜色均相同则删除当前颜色

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/
- 题目 slug：`remove-colored-pieces-if-both-neighbors-are-the-same-color`
- 来源专题：数学算法
- 来源分类路径：四、博弈论
- 难度分：1468
- 外部题解来源：https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/solutions/1050943/tong-ji-lian-xu-xiang-tong-yan-se-de-cha-zynp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计连续相同颜色的长度](https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/solutions/1050943/tong-ji-lian-xu-xiang-tong-yan-se-de-cha-zynp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-ji-lian-xu-xiang-tong-yan-se-de-cha-zynp`
- topic id：`1050943`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于删除操作需要两边都有相同颜色，所以对于每一串连续相同的颜色，最边上的两个颜色是不会被删除的。

因此删除一种颜色不会对另一种颜色产生任何影响，我们只需要统计每一串连续相同颜色的长度 $l$，若 $l>2$，则可以删除 $l-2$ 个颜色。将该值按颜色分别累加，记 $\texttt{A}$ 的累加值为 $a$，$\texttt{B}$ 的累加值为 $b$。

Alice 要想获胜，其操作次数必须比 Bob 多，否则 Bob 获胜，因此若 $a>b$ 则返回 $\texttt{true}$，否则返回 $\texttt{false}$。

```go
func winnerOfGame(colors string) bool {
	cnt := [2]int{}
	for i, n := 0, len(colors); i < n; {
		i0 := i
		c := colors[i0]
		for i < n && colors[i] == c {
			i++ // 注意这里 i 就是外层循环的 i，所以复杂度是 O(n) 的
		}
		if l := i - i0; l > 2 {
			cnt[c-'A'] += l - 2
		}
	}
	return cnt[0] > cnt[1]
}
```
