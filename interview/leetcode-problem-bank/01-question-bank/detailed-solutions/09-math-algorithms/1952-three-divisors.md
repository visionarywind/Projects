# 1952. 三除数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/three-divisors/
- 题目 slug：`three-divisors`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.5 因子
- 难度分：1204
- 外部题解来源：https://leetcode.cn/problems/three-divisors/solutions/908371/go-tong-ji-yin-zi-ge-shu-by-endlesscheng-doxu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 统计因子个数](https://leetcode.cn/problems/three-divisors/solutions/908371/go-tong-ji-yin-zi-ge-shu-by-endlesscheng-doxu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-tong-ji-yin-zi-ge-shu-by-endlesscheng-doxu`
- topic id：`908371`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

```go
func isThree(n int) bool {
	cnt := 0
	for d := 1; d*d <= n; d++ {
		if n%d == 0 {
			cnt++
			if d*d < n {
				cnt++
			}
		}
	}
	return cnt == 3
}
```
