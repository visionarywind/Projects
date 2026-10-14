# 2825. 循环增长使字符串子序列等于另一个字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-string-a-subsequence-using-cyclic-increments/
- 题目 slug：`make-string-a-subsequence-using-cyclic-increments`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.2 判断子序列
- 难度分：1415
- 外部题解来源：https://leetcode.cn/problems/make-string-a-subsequence-using-cyclic-increments/solutions/2396214/shuang-zhi-zhen-by-endlesscheng-h553/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[双指针](https://leetcode.cn/problems/make-string-a-subsequence-using-cyclic-increments/solutions/2396214/shuang-zhi-zhen-by-endlesscheng-h553/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-zhi-zhen-by-endlesscheng-h553`
- topic id：`2396214`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Yu4y1v7H6/) 第二题。

下文将 $\textit{str}_1$ 简记为 $s$，将 $\textit{str}_2$ 简记为 $t$。

想一想，如果 $s[0]$ 可以匹配 $t[0]$，那么此时一定要匹配，不然后面可能没有机会匹配 $t[0]$。

因此，双指针遍历 $s[i]$ 和 $t[j]$，如果 $s[i]$ 可以匹配 $t[j]$，那么 $i$ 和 $j$ 都加一，否则只有 $i$ 加一。

如果 $j$ 等于 $t$ 的长度，则返回 `true`，否则返回 `false`。

```py [sol-Python3]
class Solution:
    def canMakeSubsequence(self, s: str, t: str) -> bool:
        if len(s) < len(t):
            return False
        j = 0
        for b in s:
            c = chr(ord(b) + 1) if b != 'z' else 'a'
            if b == t[j] or c == t[j]:
                j += 1
                if j == len(t):
                    return True
        return False
```

```go [sol-Go]
func canMakeSubsequence(s, t string) bool {
	if len(s) < len(t) {
		return false
	}
	j := 0
	for _, b := range s {
		c := byte(b) + 1
		if b == 'z' {
			c = 'a'
		}
		if byte(b) == t[j] || c == t[j] {
			j++
			if j == len(t) {
				return true
			}
		}
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。
