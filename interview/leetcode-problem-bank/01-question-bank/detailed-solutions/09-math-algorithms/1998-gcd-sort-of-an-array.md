# 1998. 数组的最大公因数排序

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/gcd-sort-of-an-array/
- 题目 slug：`gcd-sort-of-an-array`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.3 质因数分解
- 难度分：2429
- 外部题解来源：https://leetcode.cn/problems/gcd-sort-of-an-array/solutions/979224/bing-cha-ji-fen-zu-pai-xu-by-endlesschen-kfav/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[并查集+分组排序](https://leetcode.cn/problems/gcd-sort-of-an-array/solutions/979224/bing-cha-ji-fen-zu-pai-xu-by-endlesschen-kfav/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bing-cha-ji-fen-zu-pai-xu-by-endlesschen-kfav`
- topic id：`979224`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
const mx int = 1e5
var pf [mx + 1][]int

func init() { // 预处理每个数的质因子
	for i := 2; i <= mx; i++ {
		if pf[i] == nil {
			for j := i; j <= mx; j += i {
				pf[j] = append(pf[j], i)
			}
		}
	}
}

func gcdSort(a []int) bool {
	n := len(a)
	u := newUnionFind(n + mx)
	for i, v := range a {
		for _, p := range pf[v] {
			u.merge(i, p+n) // 用并查集将下标 i 和质数 p 合并，为了区分下标集合和质数集合，将 p 加上 n
		}
	}

	// 整理处于同一集合的元素及其下标
	groups := make([]struct{ vs, is []int }, mx)
	for i, v := range a {
		p := u.find(i) - n
		groups[p].vs = append(groups[p].vs, v)
		groups[p].is = append(groups[p].is, i)
	}

	// 每一组内的元素可以直接或间接交换，因此对每组元素直接排序，然后按照在原数组中的顺序填回去
	for _, g := range groups {
		sort.Ints(g.vs)
		for j, v := range g.vs {
			a[g.is[j]] = v
		}
	}
	return sort.IntsAreSorted(a) // 判断数组是否有序
}

type uf struct {
	fa []int
}

func newUnionFind(n int) uf {
	fa := make([]int, n)
	for i := range fa {
		fa[i] = i
	}
	return uf{fa}
}

func (u uf) find(x int) int {
	if u.fa[x] != x {
		u.fa[x] = u.find(u.fa[x])
	}
	return u.fa[x]
}

func (u uf) merge(from, to int) {
	u.fa[u.find(from)] = u.find(to)
}
```
