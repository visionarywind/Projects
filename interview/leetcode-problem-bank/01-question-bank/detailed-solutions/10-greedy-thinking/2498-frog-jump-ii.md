# 2498. 青蛙过河 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/frog-jump-ii/
- 题目 slug：`frog-jump-ii`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1759
- 外部题解来源：https://leetcode.cn/problems/frog-jump-ii/solutions/2015696/tan-xin-by-endlesscheng-wj2k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[间隔跳跃+简单证明](https://leetcode.cn/problems/frog-jump-ii/solutions/2015696/tan-xin-by-endlesscheng-wj2k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-wj2k`
- topic id：`2015696`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

[视频讲解](https://www.bilibili.com/video/BV1kR4y1r7Df/) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

首先每个石头都需要选，这样可以尽量让最大距离最小。

如何跳呢？比如四块石头 $a-b-c-d$：

- 不采用间隔跳，路径为 $a-d-c-b-a$；
- 采用间隔跳，路径为 $a-c-d-b-a$；

由于 $a-d$ 必然比 $a-c$ 和 $b-d$ 大，从这点可以看出，间隔跳是最优的。

该结论可以推广至更多的石头的情况。

```py [sol1-Python3]
class Solution:
    def maxJump(self, stones: List[int]) -> int:
        ans = stones[1] - stones[0]
        for i in range(2, len(stones)):
            ans = max(ans, stones[i] - stones[i - 2])
        return ans
```

```go [sol1-Go]
func maxJump(stones []int) int {
	ans := stones[1] - stones[0]
	for i := 2; i < len(stones); i++ {
		ans = max(ans, stones[i]-stones[i-2])
	}
	return ans
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{stones}$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。
