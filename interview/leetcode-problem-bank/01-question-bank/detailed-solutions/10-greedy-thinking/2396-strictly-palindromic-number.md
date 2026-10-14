# 2396. 严格回文的数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/strictly-palindromic-number/
- 题目 slug：`strictly-palindromic-number`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1329
- 外部题解来源：https://leetcode.cn/problems/strictly-palindromic-number/solutions/1798736/shu-xue-zheng-ming-by-endlesscheng-8ozj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数学证明](https://leetcode.cn/problems/strictly-palindromic-number/solutions/1798736/shu-xue-zheng-ming-by-endlesscheng-8ozj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-xue-zheng-ming-by-endlesscheng-8ozj`
- topic id：`1798736`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1na41137jv) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

上联：[return true](https://leetcode.cn/problems/stone-game/)

下联：[return false](https://leetcode.cn/problems/strictly-palindromic-number/)

横批：脑筋急转弯

---

在题目的条件下，答案一定为 `false`，证明如下：

根据带余除法，$n=qb+r$，其中 $0\le r < b$。

取 $b=n-2$，那么当 $n>4$ 时，上式的 $q=1$，$r=2$，也就是说 $n$ 在 $n-2$ 进制下的数值为 $12$，不是回文数。

而对于 $n=4$，在 $b=2$ 进制下的数值为 $100$，也不是回文数。

因此直接返回 `false` 即可。

```py [sol1-Python3]
class Solution:
    def isStrictlyPalindromic(self, n: int) -> bool:
        return False
```

```go [sol1-Go]
func isStrictlyPalindromic(int) bool {
	return false
}
```
