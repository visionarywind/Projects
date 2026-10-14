# 2554. 从一个范围内选择最多整数 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-integers-to-choose-from-a-range-i/
- 题目 slug：`maximum-number-of-integers-to-choose-from-a-range-i`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1333
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-integers-to-choose-from-a-range-i/solutions/2093248/tan-xin-by-endlesscheng-6q1q/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心](https://leetcode.cn/problems/maximum-number-of-integers-to-choose-from-a-range-i/solutions/2093248/tan-xin-by-endlesscheng-6q1q/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-6q1q`
- topic id：`2093248`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

贪心，优先选小的。

为了快速判断一个数是否在数组 $\textit{banned}$ 中，可以将其转换成哈希表。

附：[视频讲解](https://www.bilibili.com/video/BV1rM4y1X7z9/)

```py [sol1-Python3]
class Solution:
    def maxCount(self, banned: List[int], n: int, maxSum: int) -> int:
        ans, s = 0, set(banned)
        for i in range(1, n + 1):
            if i > maxSum: break
            if i not in s:
                maxSum -= i
                ans += 1
        return ans
```

```go [sol1-Go]
func maxCount(banned []int, n, maxSum int) (ans int) {
	has := map[int]bool{}
	for _, v := range banned {
		has[v] = true
	}
	for i := 1; i <= n && i <= maxSum; i++ {
		if !has[i] {
			maxSum -= i
			ans++
		}
	}
	return
}
```

### 思考题

如果 $\textit{n}$ 和 $\textit{maxSum}$ 都很大呢（比如达到 $10^{18}$）？

欢迎在评论区发表你的做法。

### 复杂度分析

- 时间复杂度：$O(m+n)$，其中 $m$ 为 $\textit{banned}$ 的长度。
- 空间复杂度：$O(m)$。
