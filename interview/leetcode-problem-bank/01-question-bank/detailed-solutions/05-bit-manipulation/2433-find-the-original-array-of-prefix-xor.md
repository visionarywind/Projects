# 2433. 找出前缀异或的原始数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-original-array-of-prefix-xor/
- 题目 slug：`find-the-original-array-of-prefix-xor`
- 来源专题：位运算
- 来源分类路径：二、异或（XOR）的性质
- 难度分：1367
- 外部题解来源：https://leetcode.cn/problems/find-the-original-array-of-prefix-xor/solutions/1878928/li-yong-chai-fen-shu-zu-de-si-xiang-by-e-8ve6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[利用差分的思想（Python 一行）](https://leetcode.cn/problems/find-the-original-array-of-prefix-xor/solutions/1878928/li-yong-chai-fen-shu-zu-de-si-xiang-by-e-8ve6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-yong-chai-fen-shu-zu-de-si-xiang-by-e-8ve6`
- topic id：`1878928`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV11d4y1i7Gs) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

问题本质上是给你前缀和，还原回原数组。

由于前缀和的差分是原数组，因此直接两两异或即可。

```
class Solution:
    def findArray(self, pref: List[int]) -> List[int]:
        return [pref[0]] + [x ^ y for x, y in pairwise(pref)]
```
