# 2501. 数组中最长的方波

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-square-streak-in-an-array/
- 题目 slug：`longest-square-streak-in-an-array`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：1480
- 外部题解来源：https://leetcode.cn/problems/longest-square-streak-in-an-array/solutions/2015977/by-endlesscheng-koht/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力枚举 / 记忆化搜索](https://leetcode.cn/problems/longest-square-streak-in-an-array/solutions/2015977/by-endlesscheng-koht/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-koht`
- topic id：`2015977`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

[视频讲解](https://www.bilibili.com/video/BV1QK41167cr/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

# 方法一：暴力枚举

由于数组元素至少为 $2$，平方 $k$ 次后，元素至少为 $2^{2^{k-1}}$。

因此只要暴力枚举初始项，不断平方即可，至多循环 $\log\log U$ 次，这里 $U=max(\textit{nums})$。

检查元素是否在数组中，可以用哈希表预处理。

```py [sol1-Python3]
class Solution:
    def longestSquareStreak(self, nums: List[int]) -> int:
        ans, s = 0, set(nums)
        for x in s:
            cnt = 0
            while x in s:
                cnt += 1
                x *= x
            ans = max(ans, cnt)
        return ans if ans > 1 else -1
```

```go [sol1-Go]
func longestSquareStreak(nums []int) (ans int) {
	set := map[int]bool{}
	for _, x := range nums {
		set[x] = true
	}
	for x := range set {
		cnt := 1
		for x *= x; set[x]; x *= x {
			cnt++
		}
		ans = max(ans, cnt)
	}
	if ans == 1 {
		return -1
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n\log\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=max(\textit{nums})$。
- 空间复杂度：$O(n)$。

# 方法二：记忆化搜索

```py [sol2-Python3]
class Solution:
    def longestSquareStreak(self, nums: List[int]) -> int:
        s = set(nums)
        @cache
        def dfs(x: int) -> int:
            if x not in s: return 0
            return 1 + dfs(x * x)
        ans = max(map(dfs, s))
        return ans if ans > 1 else -1
```

```go [sol2-Go]
func longestSquareStreak(nums []int) (ans int) {
	set := map[int]bool{}
	for _, x := range nums {
		set[x] = true
	}
	dp := map[int]int{}
	var f func(int) int
	f = func(x int) int {
		if !set[x] {
			return 0
		}
		if v, ok := dp[x]; ok {
			return v
		}
		dp[x] = 1 + f(x*x)
		return dp[x]
	}
	for x := range set {
		ans = max(ans, f(x))
	}
	if ans == 1 {
		return -1
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。至多有 $O(n)$ 个状态。
- 空间复杂度：$O(n)$。
