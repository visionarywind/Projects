# 1862. 向下取整数对和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-floored-pairs/
- 题目 slug：`sum-of-floored-pairs`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：2170
- 外部题解来源：https://leetcode.cn/problems/sum-of-floored-pairs/solutions/778535/zhi-yu-qian-zhui-he-mei-ju-fen-mu-he-sha-2143/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[值域前缀和+枚举分母和商](https://leetcode.cn/problems/sum-of-floored-pairs/solutions/778535/zhi-yu-qian-zhui-he-mei-ju-fen-mu-he-sha-2143/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-yu-qian-zhui-he-mei-ju-fen-mu-he-sha-2143`
- topic id：`778535`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

统计每个数的出现次数 $\textit{cnt}$，并计算次数的前缀和 $\textit{sum}$。

然后枚举分母以及商，用前缀和计算对应的分子个数，将分母个数 $c$、商 $d$、分子个数三者相乘即为每部分的结果，累加每部分的结果即为答案。

```go
func sumOfFlooredPairs(a []int) (ans int) {
	cnt := make([]int, 2e5)
	for _, v := range a {
		cnt[v]++
	}
	sum := make([]int, 2e5+1)
	for i, v := range cnt {
		sum[i+1] = sum[i] + v
	}
	for i, c := range cnt {
		if c > 0 {
			for d := 1; d*i <= 1e5; d++ {
				ans += c * d * (sum[(d+1)*i] - sum[d*i])
			}
		}
	}
	return ans % (1e9 + 7)
}
```

时间复杂度：$O(U\log U)$。$U$ 为数组的最大值，枚举分母和商的总次数是一个调和级数，因此时间复杂度为 $O(U\log U)$。
