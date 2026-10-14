# 2165. 重排数字的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-value-of-the-rearranged-number/
- 题目 slug：`smallest-value-of-the-rearranged-number`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1362
- 外部题解来源：https://leetcode.cn/problems/smallest-value-of-the-rearranged-number/solutions/1247662/pai-xu-zheng-shu-cong-xiao-dao-da-fu-shu-ptop/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序：正数从小到大，负数从大到小](https://leetcode.cn/problems/smallest-value-of-the-rearranged-number/solutions/1247662/pai-xu-zheng-shu-cong-xiao-dao-da-fu-shu-ptop/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-zheng-shu-cong-xiao-dao-da-fu-shu-ptop`
- topic id：`1247662`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

```go
func smallestNumber(num int64) int64 {
	s := []byte(strconv.FormatInt(num, 10))
	if num <= 0 {
		t := s[1:]
		sort.Slice(t, func(i, j int) bool { return t[i] > t[j] }) // 负数，从大到小排序（兼容 num=0）
		ans, _ := strconv.ParseInt(string(s), 10, 64)
		return ans
	}
	sort.Slice(s, func(i, j int) bool { return s[i] < s[j] }) // 正数，从小到大排序
	i := 0
	for s[i] == '0' { // 如果有前导零就从后面找一个数字
		i++
	}
	s[i], s[0] = s[0], s[i] // 交换，保证没有前导零
	ans, _ := strconv.ParseInt(string(s), 10, 64)
	return ans
}
```
