# 2507. 使用质因数之和替换后可以取到的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-value-after-replacing-with-sum-of-prime-factors/
- 题目 slug：`smallest-value-after-replacing-with-sum-of-prime-factors`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.3 质因数分解
- 难度分：1500
- 外部题解来源：https://leetcode.cn/problems/smallest-value-after-replacing-with-sum-of-prime-factors/solutions/2024532/bao-li-by-endlesscheng-xh0b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[暴力](https://leetcode.cn/problems/smallest-value-after-replacing-with-sum-of-prime-factors/solutions/2024532/bao-li-by-endlesscheng-xh0b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bao-li-by-endlesscheng-xh0b`
- topic id：`2024532`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1LW4y1T7if/) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

不断循环，计算 $n$ 的质因数之和 $s$。如果 $s=n$ 说明无法再继续减小 $n$ 了，返回 $n$；否则更新 $n$ 为 $s$，继续循环。

```py [sol1-Python3]
class Solution:
    def smallestValue(self, n: int) -> int:
        while True:
            x, s, i = n, 0, 2
            while i * i <= x:
                while x % i == 0:
                    s += i
                    x //= i
                i += 1
            if x > 1: s += x
            if s == n: return n
            n = s
```

```go [sol1-Go]
func smallestValue(n int) int {
	for {
		x, s := n, 0
		for i := 2; i*i <= x; i++ {
			for ; x%i == 0; x /= i {
				s += i
			}
		}
		if x > 1 {
			s += x
		}
		if s == n {
			return n
		}
		n = s
	}
}
```

#### 复杂度分析

- 时间复杂度：$O(\sqrt n)$。最坏情况下每次循环 $n$ 更新为 $2+n/2$，近似看成是 $n$ 减半，那么时间复杂度为 $O\left(\sqrt n + \sqrt\dfrac{n}{2} + \sqrt\dfrac{n}{4} + \cdots \right)=O\left(\sqrt n\cdot \left(1 +  \sqrt\dfrac{1}{2} + \sqrt\dfrac{1}{4} + \cdots\right)\right) = O(\sqrt n)$。
- 空间复杂度：$O(1)$，仅用到若干变量。
