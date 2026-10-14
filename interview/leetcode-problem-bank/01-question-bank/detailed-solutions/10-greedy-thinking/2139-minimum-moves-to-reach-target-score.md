# 2139. 得到目标值的最少行动次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-reach-target-score/
- 题目 slug：`minimum-moves-to-reach-target-score`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：1417
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-reach-target-score/solutions/1213925/ni-xiang-si-wei-tan-xin-by-endlesscheng-rwfl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维 + 贪心](https://leetcode.cn/problems/minimum-moves-to-reach-target-score/solutions/1213925/ni-xiang-si-wei-tan-xin-by-endlesscheng-rwfl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-tan-xin-by-endlesscheng-rwfl`
- topic id：`1213925`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

由于加倍次数有限，我们应该将其留在数字更大的时候加倍，这样可以节省更多的行动次数。

从 $v=\textit{target}$ 倒推思考：

- 若 $v$ 为奇数，则其只能由 $v-1$ 递增得到。
- 若 $v$ 为偶数且还有加倍次数，则其可以由 $\dfrac{v}{2}$ 加倍得到。由于我们是从大往小考虑，因此若能加倍则必加倍。若此时无加倍次数，则后面都是递增操作，此时可以直接算出答案并返回。

```go
func minMoves(v, maxDoubles int) (ans int) {
	for v > 1 {
		if maxDoubles == 0 {
			return ans + v - 1
		}
		if v%2 > 0 {
			v--
			ans++
		}
		maxDoubles--
		v /= 2
		ans++
	}
	return
}
```
