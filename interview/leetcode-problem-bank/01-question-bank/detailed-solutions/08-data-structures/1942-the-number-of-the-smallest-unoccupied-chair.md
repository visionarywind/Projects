# 1942. 最小未被占据椅子的编号

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/the-number-of-the-smallest-unoccupied-chair/
- 题目 slug：`the-number-of-the-smallest-unoccupied-chair`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1695
- 外部题解来源：https://leetcode.cn/problems/the-number-of-the-smallest-unoccupied-chair/solutions/894516/shi-jian-sao-miao-xian-dui-by-endlessche-5tzj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：Bitset 或堆](https://leetcode.cn/problems/the-number-of-the-smallest-unoccupied-chair/solutions/894516/shi-jian-sao-miao-xian-dui-by-endlessche-5tzj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-jian-sao-miao-xian-dui-by-endlessche-5tzj`
- topic id：`894516`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### Bitset

用 $0$ 表示空椅子，$1$ 表示椅子被占用，这样可以在 bitset 内暴力找第一个空椅子。

```go
const wLog = 5 + bits.UintSize>>6
const wMask = bits.UintSize - 1

func smallestChair(times [][]int, targetFriend int) int {
	// 按时间顺序，记录每个到达事件和离开事件相对应的朋友编号
	events := make([][2][]int, 1e5+1)
	for i, t := range times {
		l, r := t[0], t[1]
		events[l][1] = append(events[l][1], i) // 朋友到达
		events[r][0] = append(events[r][0], i) // 朋友离开
	}

	// 初始化未被占据的椅子
	n := len(times)
	bitset := make([]uint, n>>wLog+1)
	flip := func(p int) { bitset[p>>wLog] ^= 1 << (p & wMask) }

	// 按时间顺序扫描每个事件
	belong := make([]int, n)
	for _, e := range events {
		for _, id := range e[0] { // 朋友离开
			flip(belong[id]) // 返还椅子
		}
		for _, id := range e[1] { // 朋友到达
			for i, mask := range bitset { // 暴力找未被占据的椅子
				if ^mask != 0 {
					belong[id] = i<<wLog | bits.TrailingZeros(^mask) // 记录占据该椅子的朋友编号
					break
				}
			}
			if id == targetFriend {
				return belong[id]
			}
			flip(belong[id])
		}
	}
	return 0
}
```

#### 堆

```go
func smallestChair(times [][]int, targetFriend int) int {
	// 按时间顺序，记录每个到达事件和离开事件相对应的朋友编号
	events := make([][2][]int, 1e5+1)
	for i, t := range times {
		l, r := t[0], t[1]
		events[l][1] = append(events[l][1], i) // 朋友到达
		events[r][0] = append(events[r][0], i) // 朋友离开
	}

	// 初始化未被占据的椅子
	n := len(times)
	unoccupied := hp{make([]int, n)}
	for i := range unoccupied.IntSlice {
		unoccupied.IntSlice[i] = i
	}

	// 按时间顺序扫描每个事件
	belong := make([]int, n)
	for _, e := range events {
		for _, id := range e[0] { // 朋友离开
			heap.Push(&unoccupied, belong[id]) // 返还椅子
		}
		for _, id := range e[1] { // 朋友到达
			belong[id] = heap.Pop(&unoccupied).(int) // 记录占据该椅子的朋友编号
			if id == targetFriend {
				return belong[id]
			}
		}
	}
	return 0
}

type hp struct{ sort.IntSlice }

func (h *hp) Push(v interface{}) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() interface{}   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```
