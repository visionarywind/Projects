# 2486. 追加字符以获得子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/append-characters-to-string-to-make-subsequence/
- 题目 slug：`append-characters-to-string-to-make-subsequence`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.2 判断子序列
- 难度分：1363
- 外部题解来源：https://leetcode.cn/problems/append-characters-to-string-to-make-subsequence/solutions/1993448/tan-xin-pi-pei-by-endlesscheng-d6eq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种双指针写法](https://leetcode.cn/problems/append-characters-to-string-to-make-subsequence/solutions/1993448/tan-xin-pi-pei-by-endlesscheng-d6eq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pi-pei-by-endlesscheng-d6eq`
- topic id：`1993448`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1sD4y1e7pr/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

贪心，双指针遍历 $s$ 和 $t$，$t[j]$ 应匹配 $i$ 尽量小（但大于上一个的匹配的位置）的 $s[i]$。

第一种写法：

```py [sol1-Python3]
class Solution:
    def appendCharacters(self, s: str, t: str) -> int:
        i, n = 0, len(s)
        for j, c in enumerate(t):
            while i < n and s[i] != t[j]: i += 1
            if i == n: return len(t) - j
            i += 1
        return 0
```

```go [sol1-Go]
func appendCharacters(s, t string) int {
	i, n := 0, len(s)
	for j := range t {
		for i < n && s[i] != t[j] {
			i++
		}
		if i == n {
			return len(t) - j
		}
		i++
	}
	return 0
}
```

第二种写法：

```py [sol2-Python3]
class Solution:
    def appendCharacters(self, s: str, t: str) -> int:
        j, m = 0, len(t)
        for c in s:
            if c == t[j]:
                j += 1
                if j == m: return 0
        return m - j
```

```go [sol2-Go]
func appendCharacters(s, t string) int {
	j, m := 0, len(t)
	for _, c := range s {
		if byte(c) == t[j] { // s 的字符肯定匹配的是 t 的前缀
			j++
			if j == m {
				return 0
			}
		}
	}
	return m - j
}
```

#### 复杂度分析

- 时间复杂度：$O(n+m)$，其中 $n$ 为 $s$ 的长度，$m$ 为 $t$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。
