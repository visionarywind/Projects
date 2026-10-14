# 1947. 最大兼容性评分和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-compatibility-score-sum/
- 题目 slug：`maximum-compatibility-score-sum`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-compatibility-score-sum/solutions/895170/om3-er-fen-tu-zui-da-dai-quan-pi-pei-by-a0wqb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(m^3) 带权二分图最大完美匹配](https://leetcode.cn/problems/maximum-compatibility-score-sum/solutions/895170/om3-er-fen-tu-zui-da-dai-quan-pi-pei-by-a0wqb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`om3-er-fen-tu-zui-da-dai-quan-pi-pei-by-a0wqb`
- topic id：`895170`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
const inf int = 1e9

func KuhnMunkres(wt [][]int) (sum int) {
	match := make([]int, len(wt)) // 右部点匹配了哪一个左部点
	la := make([]int, len(wt))
	for i, row := range wt {
		la[i] = -inf
		for _, v := range row {
			if v > la[i] {
				la[i] = v
			}
		}
	}
	lb := make([]int, len(wt))
	slack := make([]int, len(wt))
	for i := 1; i < len(wt); i++ {
		vb := make([]bool, len(wt))
		for j := 1; j < len(wt); j++ {
			slack[j] = inf
		}
		last := make([]int, len(wt)) // 右部点在交错树中的上一个右部点，用于倒推得到交错路
		y := 0
		match[0] = i // 一开始假设有一条 i-0 的匹配
		for {
			vb[y] = true
			x, nextY := match[y], 0
			delta := inf
			for j := 1; j < len(wt); j++ {
				if !vb[j] {
					if d := la[x] + lb[j] - wt[x][j]; d < slack[j] {
						slack[j] = d
						last[j] = y
					}
					if slack[j] < delta {
						delta = slack[j]
						nextY = j
					}
				}
			}
			// 当 delta=0 时，相当于沿着相等子图向下搜索一层
			// 当 delta>0 时，相当于直接回到最小边（新加入相等子图的边）处开始搜索
			if delta > 0 {
				for j := 0; j < len(wt); j++ {
					if vb[j] {
						la[match[j]] -= delta
						lb[j] += delta
					} else {
						slack[j] -= delta
					}
				}
			}
			y = nextY
			if match[y] == 0 {
				break
			}
		}
		// 倒推更新增广路
		for ; y > 0; y = last[y] {
			match[y] = match[last[y]]
		}
	}
	for w := 1; w < len(wt); w++ {
		sum += wt[match[w]][w]
	}
	return
}

func maxCompatibilitySum(students [][]int, mentors [][]int) int {
	m := len(students)
	wt := make([][]int, m+1)
	wt[0] = make([]int, m+1)
	for i := range wt[0] {
		wt[0][i] = -inf
	}
	for i, st := range students {
		wt[i+1] = make([]int, m+1)
		wt[i+1][0] = -inf
		for j, mt := range mentors {
			for k, v := range st {
				if v == mt[k] {
					wt[i+1][j+1]++
				}
			}
		}
	}
	return KuhnMunkres(wt)
}
```
