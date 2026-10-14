# 2381. 字母移位 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shifting-letters-ii/
- 题目 slug：`shifting-letters-ii`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：1793
- 外部题解来源：https://leetcode.cn/problems/shifting-letters-ii/solutions/1763633/by-endlesscheng-z0ad/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[差分（Python 简洁写法）](https://leetcode.cn/problems/shifting-letters-ii/solutions/1763633/by-endlesscheng-z0ad/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-z0ad`
- topic id：`1763633`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1Kd4y1Z7Fv) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

用差分数组 $\textit{diff}$ 表示一段区间上的更新，即在 $\textit{start}_i$ 变化量增加了 $x$，在 $\textit{end}_i+1$ 变化量减少了 $x$（类比导数的概念）。

这里 $x=2\cdot\textit{direction}_i-1$，把 $0$ 和 $1$ 变成 $-1$ 和 $1$。

然后从小到大遍历 $\textit{diff}$，累加变化量为 $\textit{shift}$（类比积分的概念），这样对于第 $i$ 个字符，其移位值就是 $\textit{shift}$。

```py [sol1-Python3]
c2i = {c: i for i, c in enumerate(ascii_lowercase)}

class Solution:
    def shiftingLetters(self, s: str, shifts: List[List[int]]) -> str:
        diff = [0] * (len(s) + 1)
        for start, end, dir in shifts:
            diff[start] += dir * 2 - 1
            diff[end + 1] -= dir * 2 - 1
        return ''.join(ascii_lowercase[(c2i[c] + shift) % 26] for c, shift in zip(s, accumulate(diff)))
```

```go [sol1-Go]
func shiftingLetters(s string, shifts [][]int) string {
	diff := make([]int, len(s)+1)
	for _, p := range shifts {
		x := p[2]*2 - 1 // 0 和 1 变成 -1 和 1
		diff[p[0]] += x
		diff[p[1]+1] -= x
	}
	t, shift := []byte(s), 0
	for i, c := range t {
		shift = (shift+diff[i])%26 + 26 // 防一手负数
		t[i] = (c-'a'+byte(shift))%26 + 'a'
	}
	return string(t)
}
```

#### 复杂度分析

- 时间复杂度：$O(n+m)$，其中 $n$ 为 $s$ 的长度，$m$ 为 $\textit{shifts}$ 的长度。
- 空间复杂度：$O(n)$。

#### 相似题目

- [1094. 拼车](https://leetcode.cn/problems/car-pooling/)
- [1109. 航班预订统计](https://leetcode.cn/problems/corporate-flight-bookings/)
