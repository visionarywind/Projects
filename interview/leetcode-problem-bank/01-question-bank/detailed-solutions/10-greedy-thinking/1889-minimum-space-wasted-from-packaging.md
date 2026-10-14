# 1889. 装包裹的最小浪费空间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-space-wasted-from-packaging/
- 题目 slug：`minimum-space-wasted-from-packaging`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：2214
- 外部题解来源：https://leetcode.cn/problems/minimum-space-wasted-from-packaging/solutions/815303/qian-zhui-he-shang-er-fen-by-endlesschen-vmku/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心+二分：使箱子的尺寸之和尽可能地小](https://leetcode.cn/problems/minimum-space-wasted-from-packaging/solutions/815303/qian-zhui-he-shang-er-fen-by-endlesschen-vmku/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-shang-er-fen-by-endlesschen-vmku`
- topic id：`815303`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

由于包裹的尺寸之和是固定的，因此目标转换为最小化箱子的尺寸之和。

将包裹按尺寸排序后，按照箱子尺寸划分包裹，使得每个包裹都装入能装该包裹的最小的箱子。这种贪心策略能使箱子的尺寸总和尽可能地小。

```go
func minWastedSpace(a []int, boxes [][]int) int {
	sort.Ints(a) // 将包裹按尺寸排序，方便后面按照箱子尺寸划分包裹
	ans := math.MaxInt64
	for _, box := range boxes {
		sort.Ints(box)
		if box[len(box)-1] < a[len(a)-1] { // 最大的箱子不够装最大的包裹
			continue
		}
		res, l := 0, 0
		for _, v := range box {
			// 划分包裹：当前箱子 v 可以装入下标在 [l, r) 区间内的包裹
			r := sort.SearchInts(a, v+1)
			res += (r - l) * v // 统计箱子尺寸之和
			l = r
		}
		ans = min(ans, res)
	}
	if ans < math.MaxInt64 {
		for _, v := range a {
			ans -= v // 减去每个包裹尺寸
		}
		return ans % (1e9 + 7)
	}
	return -1
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
```
