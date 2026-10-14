# 2259. 移除指定数字得到的最大结果

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-digit-from-number-to-maximize-result/
- 题目 slug：`remove-digit-from-number-to-maximize-result`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-digit-from-number-to-maximize-result/solutions/1461634/bao-li-by-endlesscheng-g5qz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[暴力（附 Python 一行）](https://leetcode.cn/problems/remove-digit-from-number-to-maximize-result/solutions/1461634/bao-li-by-endlesscheng-g5qz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bao-li-by-endlesscheng-g5qz`
- topic id：`1461634`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

```python [sol1-Python3]
class Solution:
    def removeDigit(self, s: str, digit: str) -> str:
        return max(s[:i] + s[i + 1:] for i, ch in enumerate(s) if ch == digit)
```

```go [sol1-Go]
func removeDigit(number string, digit byte) (ans string) {
	for i, ch := range number {
		if byte(ch) == digit {
			s := number[:i] + number[i+1:]
			if s > ans {
				ans = s
			}
		}
	}
	return
}
```
