# LCP 48. 无限棋局

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fsa7oZ/
- 题目 slug：`fsa7oZ`
- 来源专题：常用数据结构
- 来源分类路径：编程能力强化训练 / Part C
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/fsa7oZ/solutions/1017041/fen-lei-tao-lun-mei-ju-by-endlesscheng-89hq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论+枚举](https://leetcode.cn/problems/fsa7oZ/solutions/1017041/fen-lei-tao-lun-mei-ju-by-endlesscheng-89hq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-mei-ju-by-endlesscheng-89hq`
- topic id：`1017041`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

分类讨论题：

1. 黑第一手就可以获胜，输出 `"Black"`
2. 黑第一手无法获胜
2.1 白可以一步胜，且获胜位置不止一处，此时黑无法阻止白获胜，输出 `"White"`
2.2 白可以一步胜，但获胜位置只有一处，此时黑第一手下在该处，白无法获胜，进入 3
2.3 白无获胜位置，进入 3
3. 白无法获胜，于是白的策略是防止黑获胜。枚举黑第一手的位置（除了 2.2 中黑只能下在一个位置），然后判断黑第二手能否有超过一处获胜位置，若有超过一处，则白无法阻止黑获胜，输出 `"Black"`，否则输出 `"None"`。

枚举位置的技巧见注释。

```go
const (
	black = "Black"
	white = "White"
	none  = "None"
)

type pair struct{ x, y int }
var dir8 = []pair{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}

func gobang(pieces [][]int) string {
	color := make(map[pair]int, len(pieces)+1)
	for _, p := range pieces {
		p[2]++ // 为方便利用零值，将黑改为 1，白改为 2
		color[pair{p[0], p[1]}] = p[2]
	}

	// 在 (i,j) 落子，颜色为 c，判断落子的一方是否获胜
	checkWin := func(i, j, c int) bool {
		for k, d := range dir8[:4] {
			cnt := 1
			// 检查一个方向
			for x, y := i+d.x, j+d.y; color[pair{x, y}] == c; x, y = x+d.x, y+d.y {
				cnt++
			}
			// 检查相反的另一方向
			d = dir8[k^4]
			for x, y := i+d.x, j+d.y; color[pair{x, y}] == c; x, y = x+d.x, y+d.y {
				cnt++
			}
			if cnt >= 5 {
				return true
			}
		}
		return false
	}

	// 1. 黑第一手就可以获胜
	for _, p := range pieces {
		if p[2] == 2 {
			continue
		}
		i, j := p[0], p[1]
		for _, d := range dir8 { // 黑要想一步获胜只能下在黑子周围
			if x, y := i+d.x, j+d.y; color[pair{x, y}] == 0 && checkWin(x, y, 1) {
				return black
			}
		}
	}

	// 2. 黑第一手无法获胜
	whites := map[pair]bool{}
	posW := pair{}
	for _, p := range pieces {
		if p[2] == 1 {
			continue
		}
		i, j := p[0], p[1]
		for _, d := range dir8 { // 白要想一步获胜只能下在白子周围
			x, y := i+d.x, j+d.y
			q := pair{x, y}
			if color[q] == 0 && checkWin(x, y, 2) {
				// 2.1 白可以一步胜，且获胜位置不止一处
				if whites[q] = true; len(whites) > 1 {
					return white
				}
				posW = q
			}
		}
	}

	// 2.2 白可以一步胜，但获胜位置只有一处
	if len(whites) == 1 {
		color[posW] = 1 // 黑第一手下在该处，阻止白获胜
		blacks := map[pair]bool{}
		// 检查第三步的黑能否获胜
		checkBlackWin := func(i, j int) bool {
			for _, d := range dir8 { // 黑要获胜只能下在黑子周围
				x, y := i+d.x, j+d.y
				p := pair{x, y}
				if color[p] == 0 && checkWin(x, y, 1) {
					if blacks[p] = true; len(blacks) > 1 {
						return true
					}
				}
			}
			return false
		}
		checkBlackWin(posW.x, posW.y)
		for _, p := range pieces {
			if p[2] == 1 && checkBlackWin(p[0], p[1]) {
				return black
			}
		}
		return none
	}

	// 3. 白无法获胜，于是白的策略是防止黑获胜
	// 根据黑第一步的落子位置，在该位置周围枚举黑第三步的落子位置，检查黑能否获胜
	checkBlackWin := func(i0, j0 int) bool {
		blacks := map[pair]bool{}
		for k, d := range dir8 {
			for l, i, j := 0, i0, j0; l < 5; l++ { // 如果黑可以获胜，这两枚黑子的距离不会超过 5
				i += d.x
				j += d.y
				p := pair{i, j}
				if color[p] > 0 {
					continue
				}
				cnt := 1
				// 检查一个方向
				for x, y := i+d.x, j+d.y; color[pair{x, y}] == 1; x, y = x+d.x, y+d.y {
					cnt++
				}
				// 检查相反的另一方向
				d2 := dir8[k^4]
				for x, y := i+d2.x, j+d2.y; color[pair{x, y}] == 1; x, y = x+d2.x, y+d2.y {
					cnt++
				}
				if cnt >= 5 {
					if blacks[p] = true; len(blacks) > 1 {
						return true
					}
				}
			}
		}
		return false
	}
	vis := map[pair]bool{} // 常数优化：避免重复枚举
	for _, p := range pieces {
		if p[2] == 2 {
			continue
		}
		i, j := p[0], p[1]
		// 枚举黑第一步的落子。由于黑要下两手棋，需要枚举黑子周围两圈
		for dx := -2; dx <= 2; dx++ {
			for dy := -2; dy <= 2; dy++ {
				if dx == 0 && dy == 0 {
					continue
				}
				x, y := i+dx, j+dy
				q := pair{x, y}
				if vis[q] || color[q] > 0 {
					continue
				}
				color[q] = 1 // 黑落子
				vis[q] = true
				if checkBlackWin(x, y) {
					return black
				}
				delete(color, q)
			}
		}
	}
	return none
}
```
