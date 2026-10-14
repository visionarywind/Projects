# 2261. 含最多 K 个可整除元素的子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-divisible-elements-subarrays/
- 题目 slug：`k-divisible-elements-subarrays`
- 来源专题：字符串
- 来源分类路径：四、字符串哈希
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/k-divisible-elements-subarrays/solutions/1461621/ha-xi-biao-mo-ni-by-endlesscheng-wrc7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[哈希表模拟](https://leetcode.cn/problems/k-divisible-elements-subarrays/solutions/1461621/ha-xi-biao-mo-ni-by-endlesscheng-wrc7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ha-xi-biao-mo-ni-by-endlesscheng-wrc7`
- topic id：`1461621`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

```python [sol1-Python3]
class Solution:
    def countDistinct(self, nums: List[int], k: int, p: int) -> int:
        s, n = set(), len(nums)
        for i in range(n):
            cnt = 0
            for j in range(i, n):
                if nums[j] % p == 0:
                    cnt += 1
                    if cnt > k:
                        break
                s.add(tuple(nums[i: j + 1]))
        return len(s)
```

```go [sol1-Go]
func countDistinct(nums []int, k, p int) int {
	set := map[[200]int]struct{}{}
	for i := range nums {
		arr, idx, cnt := [200]int{}, 0, 0
		for _, v := range nums[i:] { // 从 i 开始向右扩展子数组
			if v%p == 0 {
				if cnt++; cnt > k {
					break
				}
			}
			arr[idx] = v
			idx++
			set[arr] = struct{}{}
		}
	}
	return len(set)
}
```
