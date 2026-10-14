# 1834. 单线程 CPU

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/single-threaded-cpu/
- 题目 slug：`single-threaded-cpu`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1798
- 外部题解来源：https://leetcode.cn/problems/single-threaded-cpu/solutions/728856/golang-dui-by-endlesscheng-4sqg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Golang 堆](https://leetcode.cn/problems/single-threaded-cpu/solutions/728856/golang-dui-by-endlesscheng-4sqg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`golang-dui-by-endlesscheng-4sqg`
- topic id：`728856`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

按进入时间排序。遍历任务数组，每次至多执行一个任务，然后将所有进入时间不超过当前时间 $\textit{cur}$ 的任务加入堆中。

```go
type pair struct{ t, i int }
type hp []pair

func (h hp) Len() int            { return len(h) }
func (h hp) Less(i, j int) bool  { a, b := h[i], h[j]; return a.t < b.t || a.t == b.t && a.i < b.i }
func (h hp) Swap(i, j int)       { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v interface{}) { *h = append(*h, v.(pair)) }
func (h *hp) Pop() interface{}   { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
func (h *hp) push(v pair)        { heap.Push(h, v) }
func (h *hp) pop() pair          { return heap.Pop(h).(pair) }

func getOrder(a [][]int) (ans []int) {
	for i := range a {
		a[i] = append(a[i], i)
	}
	sort.Slice(a, func(i, j int) bool { return a[i][0] < a[j][0] })
	h := &hp{}
	for i, cur, n := 0, 0, len(a); i < n; {
		if h.Len() > 0 {
			p := h.pop()
			ans = append(ans, p.i)
			cur += p.t
		}
		if h.Len() == 0 && cur < a[i][0] {
			cur = a[i][0]
		}
		for ; i < n && a[i][0] <= cur; i++ {
			h.push(pair{a[i][1], a[i][2]})
		}
	}
	for h.Len() > 0 {
		ans = append(ans, h.pop().i)
	}
	return
}
```
