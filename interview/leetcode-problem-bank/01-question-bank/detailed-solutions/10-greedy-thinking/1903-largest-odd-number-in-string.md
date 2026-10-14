# 1903. 字符串中的最大奇数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-odd-number-in-string/
- 题目 slug：`largest-odd-number-in-string`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1249
- 外部题解来源：https://leetcode.cn/problems/largest-odd-number-in-string/solutions/836301/zhao-zui-hou-yi-ge-qi-shu-wei-zhi-by-end-j98d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[找最后一个奇数位置（附一行写法）](https://leetcode.cn/problems/largest-odd-number-in-string/solutions/836301/zhao-zui-hou-yi-ge-qi-shu-wei-zhi-by-end-j98d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhao-zui-hou-yi-ge-qi-shu-wei-zhi-by-end-j98d`
- topic id：`836301`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

由于奇数的最后一个数字是奇数，从后往前找到第一个奇数数字，删掉后面的字符，剩下的就是最大的奇数。

```go
func largestOddNumber(s string) string {
	for s != "" && s[len(s)-1]&1 == 0 {
		s = s[:len(s)-1]
	}
	return s
}
```

一行写法：

```go
func largestOddNumber(s string) string {
    return s[:strings.LastIndexAny(s, "13579")+1]
}
```
