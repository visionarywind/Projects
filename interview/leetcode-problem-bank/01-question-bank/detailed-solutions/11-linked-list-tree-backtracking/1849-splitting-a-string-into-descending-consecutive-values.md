# 1849. 将字符串拆分为递减的连续值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/splitting-a-string-into-descending-consecutive-values/
- 题目 slug：`splitting-a-string-into-descending-consecutive-values`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.3 划分型回溯
- 难度分：1747
- 外部题解来源：https://leetcode.cn/problems/splitting-a-string-into-descending-consecutive-values/solutions/755152/mei-ju-di-yi-ge-shu-by-endlesscheng-5hsp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举第一个数，时间复杂度 O(N^2)](https://leetcode.cn/problems/splitting-a-string-into-descending-consecutive-values/solutions/755152/mei-ju-di-yi-ge-shu-by-endlesscheng-5hsp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-di-yi-ge-shu-by-endlesscheng-5hsp`
- topic id：`755152`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

```go
func splitString(s string) bool {
next:
	for i := 1; i < len(s); i++ {
		v, _ := strconv.Atoi(s[:i]) // 如果 s 很长，会返回最大 int
		v--
		for t := s[i:]; t != ""; v-- {
			// 移除前导零，但不能全部移除，至少保留一位数字
			for len(t) > 1 && t[0] == '0' {
				t = t[1:]
			}
			// 判断 v 是否符合
			tmp := strconv.Itoa(v)
			if !strings.HasPrefix(t, tmp) {
				continue next
			}
			t = t[len(tmp):]
		}
		return true
	}
	return false
}
```
