# 2521. 数组乘积中的不同质因数数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distinct-prime-factors-of-product-of-array/
- 题目 slug：`distinct-prime-factors-of-product-of-array`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.3 质因数分解
- 难度分：1413
- 外部题解来源：https://leetcode.cn/problems/distinct-prime-factors-of-product-of-array/solutions/2040069/fen-jie-zhi-yin-shu-by-endlesscheng-x3kz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分解质因数](https://leetcode.cn/problems/distinct-prime-factors-of-product-of-array/solutions/2040069/fen-jie-zhi-yin-shu-by-endlesscheng-x3kz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-jie-zhi-yin-shu-by-endlesscheng-x3kz`
- topic id：`2040069`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

遍历每个数，分解质因数，把质因数加到哈希表中。分解质因数的原理见 [第 324 场周赛视频讲解](https://www.bilibili.com/video/BV1LW4y1T7if/)，可以看看。

答案为哈希表的大小。

```py [sol1-Python3]
class Solution:
    def distinctPrimeFactors(self, nums: List[int]) -> int:
        s = set()
        for x in nums:
            i = 2
            while i * i <= x:
                if x % i == 0:
                    s.add(i)
                    x //= i
                    while x % i == 0:
                        x //= i
                i += 1
            if x > 1:
                s.add(x)
        return len(s)
```

```go [sol1-Go]
func distinctPrimeFactors(nums []int) int {
	set := map[int]struct{}{}
	for _, x := range nums {
		for i := 2; i*i <= x; i++ {
			if x%i == 0 {
				set[i] = struct{}{}
				for x /= i; x%i == 0; x /= i {
				}
			}
		}
		if x > 1 {
			set[x] = struct{}{}
		}
	}
	return len(set)
}
```

#### 复杂度分析

- 时间复杂度：$O(n\sqrt{U})$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$O\left(\dfrac{U}{\log U}\right)$。至多有 $O\left(\dfrac{U}{\log U}\right)$ 个质因数。

#### 相似题目

- [2507. 使用质因数之和替换后可以取到的最小值](https://leetcode.cn/problems/smallest-value-after-replacing-with-sum-of-prime-factors/)
