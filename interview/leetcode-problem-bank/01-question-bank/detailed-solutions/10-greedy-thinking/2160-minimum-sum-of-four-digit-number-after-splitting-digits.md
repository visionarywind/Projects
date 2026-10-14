# 2160. 拆分数位后四位数字的最小和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-sum-of-four-digit-number-after-splitting-digits/
- 题目 slug：`minimum-sum-of-four-digit-number-after-splitting-digits`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.1 基础
- 难度分：1314
- 外部题解来源：https://leetcode.cn/problems/minimum-sum-of-four-digit-number-after-splitting-digits/solutions/1247103/tan-xin-pai-xu-by-endlesscheng-dkq1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[ 贪心 + 排序](https://leetcode.cn/problems/minimum-sum-of-four-digit-number-after-splitting-digits/solutions/1247103/tan-xin-pai-xu-by-endlesscheng-dkq1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pai-xu-by-endlesscheng-dkq1`
- topic id：`1247103`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

通过比对可以发现，拆分成两位数 + 两位数是最优的。因此要使 $\textit{new1}+\textit{new2}$ 最小，应当最小化十位数字。

我们可以将 $\textit{num}$ 的字符串形式从小到大排序，那么前两个数字为十位数，后两个数字为个位数。

```go
func minimumSum(num int) int {
	s := []byte(strconv.Itoa(num))
	sort.Slice(s, func(i, j int) bool { return s[i] < s[j] })
	return int(s[0]&15+s[1]&15)*10 + int(s[2]&15+s[3]&15)
}
```
