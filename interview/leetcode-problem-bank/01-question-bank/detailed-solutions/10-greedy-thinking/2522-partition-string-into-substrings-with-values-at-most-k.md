# 2522. 将字符串分割成值不超过 K 的子字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partition-string-into-substrings-with-values-at-most-k/
- 题目 slug：`partition-string-into-substrings-with-values-at-most-k`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.5 划分型贪心
- 难度分：1605
- 外部题解来源：https://leetcode.cn/problems/partition-string-into-substrings-with-values-at-most-k/solutions/2040073/bian-li-by-endlesscheng-7ojw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 一次遍历](https://leetcode.cn/problems/partition-string-into-substrings-with-values-at-most-k/solutions/2040073/bian-li-by-endlesscheng-7ojw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bian-li-by-endlesscheng-7ojw`
- topic id：`2040073`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

[视频讲解](https://www.bilibili.com/video/BV1H8411E7hn) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

思考：第一个字符串应该划分在哪？

如果给后面留下的字符串越短，答案显然不会变大，因此第一个字符串越长越好。

```py [sol1-Python3]
class Solution:
    def minimumPartition(self, s: str, k: int) -> int:
        ans, x = 1, 0
        for v in map(int, s):
            if v > k: return -1
            x = x * 10 + v
            if x > k:
                ans += 1
                x = v
        return ans
```

```go [sol1-Go]
// https://www.bilibili.com/video/BV1H8411E7hn
func minimumPartition(s string, k int) int {
	ans, x := 1, 0
	for _, c := range s {
		v := int(c - '0')
		if v > k {
			return -1
		}
		x = x*10 + v
		if x > k {
			ans++
			x = v
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。
