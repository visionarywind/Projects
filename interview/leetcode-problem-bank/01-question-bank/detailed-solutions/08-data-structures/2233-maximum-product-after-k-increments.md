# 2233. K 次增加后的最大乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-after-k-increments/
- 题目 slug：`maximum-product-after-k-increments`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1686
- 外部题解来源：https://leetcode.cn/problems/maximum-product-after-k-increments/solutions/1408853/tan-xin-mei-ci-gei-zui-xiao-de-yuan-su-j-jxnl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心：每次给最小的元素加一](https://leetcode.cn/problems/maximum-product-after-k-increments/solutions/1408853/tan-xin-mei-ci-gei-zui-xiao-de-yuan-su-j-jxnl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-mei-ci-gei-zui-xiao-de-yuan-su-j-jxnl`
- topic id：`1408853`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

给 $x$ 加一后，让总乘积增加了 $\dfrac{x+1}{x}$ = $1+\dfrac{1}{x}$ 倍，那么 $x$ 越小收益越高，所以要先加 $x$ 最小的。

$x=0$ 的特殊情况显然要先把它加一，仍然满足先加最小的 $x$ 的结论。

用一个小根堆模拟即可。

原地堆化可以做到 $O(1)$ 的空间复杂度。

为避免乘法溢出（Python 是避免大数乘法），需要一边乘一边取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```python [sol-Python3]
class Solution:
    def maximumProduct(self, nums: List[int], k: int) -> int:
        MOD = 10 ** 9 + 7
        heapify(nums)
        while k:
            heapreplace(nums, nums[0] + 1)
            k -= 1

        ans = 1
        for num in nums:
            ans = ans * num % MOD
        return ans
```

```go [sol-Go]
func maximumProduct(nums []int, k int) int {
	h := hp{nums}
	for heap.Init(&h); k > 0; k-- {
		h.IntSlice[0]++ // 每次给最小的加一
		heap.Fix(&h, 0)
	}

	ans := 1
	for _, num := range nums {
		ans = ans * num % (1e9 + 7)
	}
	return ans
}

type hp struct{ sort.IntSlice }
func (hp) Push(any)     {}
func (hp) Pop() (_ any) { return }
```
