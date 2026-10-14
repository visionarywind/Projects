# 2425. 所有数对的异或和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/bitwise-xor-of-all-pairings/
- 题目 slug：`bitwise-xor-of-all-pairings`
- 来源专题：位运算
- 来源分类路径：四、拆位 / 贡献法
- 难度分：1622
- 外部题解来源：https://leetcode.cn/problems/bitwise-xor-of-all-pairings/solutions/1863486/tao-lun-mei-ge-yuan-su-zai-da-an-zhong-d-uutg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贡献法：讨论每个元素在答案中的出现次数](https://leetcode.cn/problems/bitwise-xor-of-all-pairings/solutions/1863486/tao-lun-mei-ge-yuan-su-zai-da-an-zhong-d-uutg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tao-lun-mei-ge-yuan-su-zai-da-an-zhong-d-uutg`
- topic id：`1863486`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1tW4y1e7rb) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

由于答案是一大堆数字的异或和，根据**贡献法**的思想，我们可以讨论每个数字在这一大堆数字中出现了多少次，对答案的贡献是多少。

设 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度。

对于 $\textit{nums}_1[i]$，由于它要与 $\textit{nums}_2$ 的每个元素异或一次，因此它一共出现了 $m$ 次。由于一个元素异或它自己等于 $0$，因此如果 $m$ 是偶数，则 $\textit{nums}_1[i]$ 对答案的贡献是 $0$，否则是 $\textit{nums}_1[i]$。

对于 $\textit{nums}_2$ 的元素，分析同理。

```py [sol1-Python3]
class Solution:
    def xorAllNums(self, nums1: List[int], nums2: List[int]) -> int:
        ans = 0
        if len(nums2) % 2: ans ^= reduce(xor, nums1)
        if len(nums1) % 2: ans ^= reduce(xor, nums2)
        return ans
```

```go [sol1-Go]
func xorAllNums(nums1, nums2 []int) (ans int) {
	if len(nums2)%2 > 0 {
		for _, x := range nums1 {
			ans ^= x
		}
	}
	if len(nums1)%2 > 0 {
		for _, x := range nums2 {
			ans ^= x
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$O(n+m)$，其中 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干变量。
