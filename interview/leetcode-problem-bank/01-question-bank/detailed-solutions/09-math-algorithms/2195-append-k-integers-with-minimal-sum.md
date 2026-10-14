# 2195. 向数组中追加 K 个整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/append-k-integers-with-minimal-sum/
- 题目 slug：`append-k-integers-with-minimal-sum`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1659
- 外部题解来源：https://leetcode.cn/problems/append-k-integers-with-minimal-sum/solutions/1314517/pai-xu-bian-li-by-endlesscheng-81er/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[只用到排序的做法](https://leetcode.cn/problems/append-k-integers-with-minimal-sum/solutions/1314517/pai-xu-bian-li-by-endlesscheng-81er/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-bian-li-by-endlesscheng-81er`
- topic id：`1314517`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

排序后，在相邻数字之间填充数字。

通过加入两个哨兵来简化代码。

```py [sol1-Python3]
class Solution:
    def minimalKSum(self, nums: List[int], k: int) -> int:
        ans = 0
        nums.extend([0, inf])  # 加入两个哨兵
        nums.sort()
        for x, y in pairwise(nums):
            fill = y - x - 1  # 可以填充的数字个数
            if fill <= 0:  # 没有可以填充的位置
                continue
            if fill >= k:  # 填充剩余的 k 个数
                return ans + (x * 2 + 1 + k) * k // 2  # 等差数列求和
            ans += (x + y) * fill // 2  # 填充 fill 个数：等差数列求和
            k -= fill  # 更新剩余要填充的数字个数
```

```go [sol1-Go]
func minimalKSum(nums []int, k int) int64 {
	ans := 0
	nums = append(nums, 0, math.MaxInt32) // 加入两个哨兵
	sort.Ints(nums)
	for i := 1; ; i++ {
		fill := nums[i] - nums[i-1] - 1 // 可以填充的数字个数
		if fill <= 0 { // 没有可以填充的位置
			continue
		}
		if fill >= k { // 填充剩余的 k 个数
			return int64(ans + (nums[i-1]*2+1+k)*k/2) // 等差数列求和
		}
		ans += (nums[i-1] + nums[i]) * fill / 2 // 填充 fill 个数：等差数列求和
		k -= fill // 更新剩余要填充的数字个数
	}
}
```
