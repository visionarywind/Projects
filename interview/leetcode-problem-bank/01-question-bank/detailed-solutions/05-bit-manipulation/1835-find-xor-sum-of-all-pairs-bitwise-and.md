# 1835. 所有数对按位与结果的异或和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-xor-sum-of-all-pairs-bitwise-and/
- 题目 slug：`find-xor-sum-of-all-pairs-bitwise-and`
- 来源专题：位运算
- 来源分类路径：四、拆位 / 贡献法
- 难度分：1825
- 外部题解来源：https://leetcode.cn/problems/find-xor-sum-of-all-pairs-bitwise-and/solutions/728813/python-yi-xing-by-endlesscheng-007b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Python 一行](https://leetcode.cn/problems/find-xor-sum-of-all-pairs-bitwise-and/solutions/728813/python-yi-xing-by-endlesscheng-007b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`python-yi-xing-by-endlesscheng-007b`
- topic id：`728813`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于 `(a&b)^(a&c) == a&(b^c)`，将两个数组的异或和求出，两个异或和的与就是答案。

```py
class Solution:
    def getXORSum(self, a: List[int], b: List[int]) -> int:
        return reduce(xor, a) & reduce(xor, b)
```
