# 1095. 山脉数组中查找目标值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-in-mountain-array/
- 题目 slug：`find-in-mountain-array`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：1827
- 外部题解来源：https://leetcode.cn/problems/find-in-mountain-array/solutions/1145587/go-ku-han-shu-er-fen-by-endlesscheng-dl6a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 库函数二分](https://leetcode.cn/problems/find-in-mountain-array/solutions/1145587/go-ku-han-shu-er-fen-by-endlesscheng-dl6a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-ku-han-shu-er-fen-by-endlesscheng-dl6a`
- topic id：`1145587`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func findInMountainArray(target int, arr *MountainArray) int {
	n := arr.length()
	peak := sort.Search(n-1, func(i int) bool { return arr.get(i) >= arr.get(i+1) })
	i := sort.Search(peak, func(i int) bool { return arr.get(i) >= target })
	if arr.get(i) != target {
		i = peak + sort.Search(n-1-peak, func(i int) bool { return arr.get(peak+i) <= target })
		if arr.get(i) != target {
			return -1
		}
	}
	return i
}
```
