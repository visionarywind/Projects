# 2358. 分组的最大数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-groups-entering-a-competition/
- 题目 slug：`maximum-number-of-groups-entering-a-competition`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.5 划分型贪心
- 难度分：1503
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-groups-entering-a-competition/solutions/1710832/pai-xu-tan-xin-mo-ni-by-endlesscheng-zq8m/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：排序 + 贪心模拟 / 脑筋急转弯](https://leetcode.cn/problems/maximum-number-of-groups-entering-a-competition/solutions/1710832/pai-xu-tan-xin-mo-ni-by-endlesscheng-zq8m/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-tan-xin-mo-ni-by-endlesscheng-zq8m`
- topic id：`1710832`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1Ba411N78j) 已出炉，包含文章最后的**思考题**的讲解，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

由于元素都是正数，第一个组的元素越小越好。那么排序后，我们可以按照题目要求模拟。

注意内外层共用同一个循环变量 $i$，二重循环部分的时间复杂度是线性的。

```go
func maximumGroups(grades []int) (ans int) {
	sort.Ints(grades)
	for i, preSum, k, n := 0, 0, 0, len(grades); i+k < n; ans++ {
		i0, sum := i, 0 // i0 保存本次循环的起始下标
		for ; i < n && (sum <= preSum || i <= i0+k); i++ { // 必须满足题目的两个要求
			sum += grades[i]
		}
		if sum <= preSum { // 未能满足要求
			break
		}
		preSum, k = sum, i-i0
	}
	return
}
```

注意到排序后，后面组的元素之和必然比前面的组大，那么第 $i$ 组的人数必然是 $i$（最后一组如果不足 $i$，与前一组合并）。

设组数为 $x$，那么有 $\dfrac{x(x+1)}{2}\le n$，最大的 $x$ 即为

$$
\dfrac{-1+\sqrt{1+8n}}{2}
$$

取其整数部分作为答案。

```go
func maximumGroups(grades []int) int {
	return int((math.Sqrt(float64(1+8*len(grades))) - 1) / 2)
}
```

如果担心精度问题，比如本来答案为 $6$，实际由于浮点误差导致结果为 $\lfloor 5.999\cdots \rfloor = 5$，那么可以改成

```go
func maximumGroups(grades []int) int {
	n := len(grades)
	x := int((math.Sqrt(float64(1+8*n)) - 1) / 2)
	if (x+1)*(x+2)/2 <= n {
		x++
	}
	return x
}
```

#### 思考题

1. 如果 $\textit{grades}$ 中有 $0$ 要怎么做？
   解答：设除了 $0$ 以外的正数，能组成 $x$ 组。如果 $0$ 的个数足够，我们可以将 $0$ 与多出的 $n-\dfrac{x(x+1)}{2}$ 个正数组成一个新的长度至少为 $x+1$ 的组。如果还有至少 $x+2$ 个 $0$，我们可以给每个组都加上一个 $0$，并在最前面加上一个只包含一个 $0$ 的组，这样可以得到 $x+2$ 个组。
2. 如果不能打乱 $\textit{grades}$ 的顺序，直接分组，要如何做呢？（其实就是最上面的代码去掉排序）
3. 如果要求满足最大分组下的方案数呢？
