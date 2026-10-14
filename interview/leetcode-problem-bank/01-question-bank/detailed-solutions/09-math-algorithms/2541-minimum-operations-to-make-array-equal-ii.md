# 2541. 使数组中所有元素相等的最小操作数 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-array-equal-ii/
- 题目 slug：`minimum-operations-to-make-array-equal-ii`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1620
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-array-equal-ii/solutions/2073031/zhuan-huan-yi-ci-bian-li-by-endlesscheng-hc0y/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转换+一次遍历](https://leetcode.cn/problems/minimum-operations-to-make-array-equal-ii/solutions/2073031/zhuan-huan-yi-ci-bian-li-by-endlesscheng-hc0y/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-yi-ci-bian-li-by-endlesscheng-hc0y`
- topic id：`2073031`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

令 $a[i] = \textit{nums}_1[i] - \textit{nums}_2[i]$，则问题变成把每个 $a[i]$ 变成 $0$ 的最小操作次数。

那么在 $k=0$ 的时候，无法操作，那么所有 $a[i]$ 必须为 $0$。

对于 $k>0$，由于每个 $a[i]$ 要变成 $0$，它必须是 $k$ 的倍数，如果 $a[i]\bmod k \ne 0$，就无法满足要求。

此外，由于「一个数 $+k$，另一个数 $-k$」这个操作不会影响整个 $a[i]$ 的和，所以如果 $a[i]$ 的和不为 $0$，也无法满足要求。

最后，统计所有正数 $\dfrac{a[i]}{k}$，即为答案（因为负数都同时改成 $0$ 了）。

附：[视频讲解](https://www.bilibili.com/video/BV1jG4y197qD/)

```py [sol1-Python3]
class Solution:
    def minOperations(self, nums1: List[int], nums2: List[int], k: int) -> int:
        ans = sum = 0
        for x, y in zip(nums1, nums2):
            x -= y
            if k:
                if x % k: return -1
                sum += x // k
                if x > 0: ans += x // k
            elif x: return -1
        return -1 if sum else ans
```

```go [sol1-Go]
func minOperations(nums1, nums2 []int, k int) (ans int64) {
	sum := 0
	for i, x := range nums1 {
		x -= nums2[i]
		if k > 0 {
			if x%k != 0 {
				return -1
			}
			sum += x / k
			if x > 0 {
				ans += int64(x / k)
			}
		} else if x != 0 {
			return -1
		}
	}
	if sum != 0 {
		return -1
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}_1$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。
