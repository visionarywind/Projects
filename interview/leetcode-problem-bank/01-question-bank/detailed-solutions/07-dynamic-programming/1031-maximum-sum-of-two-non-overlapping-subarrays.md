# 1031. 两个无重叠子数组的最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-sum-of-two-non-overlapping-subarrays/
- 题目 slug：`maximum-sum-of-two-non-overlapping-subarrays`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1680
- 外部题解来源：https://leetcode.cn/problems/maximum-sum-of-two-non-overlapping-subarrays/solutions/2245647/tu-jie-mei-you-si-lu-yi-zhang-tu-miao-do-3lli/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】没有思路？一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-sum-of-two-non-overlapping-subarrays/solutions/2245647/tu-jie-mei-you-si-lu-yi-zhang-tu-miao-do-3lli/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-mei-you-si-lu-yi-zhang-tu-miao-do-3lli`
- topic id：`2245647`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

[前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)

## 思路

![1031-c.png](https://pic.leetcode.cn/1682327896-NuZacZ-1031-c.png)

## 实现细节

设 $b$ 的下标范围为左闭右开区间 $[i-\textit{secondLen},i)$，其元素和为 $s[i] - s[i-\textit{secondLen}]$。

由于左边要留空间给 $a$，所以 $i$ 要从 $\textit{firstLen}+\textit{secondLen}$ 开始枚举（结合上图理解）。

由于每次枚举一个新的 $i$，都会在 $b$ 的左边产生一个新的子数组 $[i-\textit{secondLen}-\textit{firstLen},i-\textit{secondLen})$，所以用它的元素和 $s[i-\textit{secondLen}] - s[i-\textit{secondLen}-\textit{firstLen}]$ 去更新 $\textit{maxSumA}$。

把这个算法封装成一个函数 $f(\textit{firstLen},\textit{secondLen})$，表示左 $a$ 右 $b$ 的计算。那么 $f(\textit{secondLen},\textit{firstLen})$ 就是左 $b$ 右 $a$ 的计算了。

```py [sol1-Python3]
class Solution:
    def maxSumTwoNoOverlap(self, nums: List[int], firstLen: int, secondLen: int) -> int:
        s = list(accumulate(nums, initial=0))  # nums 的前缀和
        ans = 0
        def f(firstLen: int, secondLen: int) -> None:
            nonlocal ans
            maxSumA = 0
            for i in range(firstLen + secondLen, len(s)):
                maxSumA = max(maxSumA, s[i - secondLen] - s[i - secondLen - firstLen])
                ans = max(ans, maxSumA + s[i] - s[i - secondLen])
        f(firstLen, secondLen)  # 左 a 右 b
        f(secondLen, firstLen)  # 左 b 右 a
        return ans
```

```java [sol1-Java]
class Solution {
    public int maxSumTwoNoOverlap(int[] nums, int firstLen, int secondLen) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + nums[i]; // 计算 nums 的前缀和
        return Math.max(f(s, firstLen, secondLen), f(s, secondLen, firstLen));
    }

    private int f(int[] s, int firstLen, int secondLen) {
        int maxSumA = 0, res = 0;
        for (int i = firstLen + secondLen; i < s.length; ++i) {
            maxSumA = Math.max(maxSumA, s[i - secondLen] - s[i - secondLen - firstLen]);
            res = Math.max(res, maxSumA + s[i] - s[i - secondLen]);
        }
        return res;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int maxSumTwoNoOverlap(vector<int>& nums, int firstLen, int secondLen) {
        int ans = 0, n = nums.size(), s[n + 1];
        s[0] = 0;
        partial_sum(nums.begin(), nums.end(), s + 1); // 计算 nums 的前缀和
        auto f = [&](int firstLen, int secondLen) {
            int maxSumA = 0;
            for (int i = firstLen + secondLen; i <= n; ++i) {
                maxSumA = max(maxSumA, s[i - secondLen] - s[i - secondLen - firstLen]);
                ans = max(ans, maxSumA + s[i] - s[i - secondLen]);
            }
        };
        f(firstLen, secondLen); // 左 a 右 b
        f(secondLen, firstLen); // 左 b 右 a
        return ans;
    }
};
```

```go [sol1-Go]
func maxSumTwoNoOverlap(nums []int, firstLen, secondLen int) (ans int) {
    n := len(nums)
    s := make([]int, n+1)
    for i, x := range nums {
        s[i+1] = s[i] + x // 计算 nums 的前缀和
    }
    f := func(firstLen, secondLen int) {
        maxSumA := 0
        for i := firstLen + secondLen; i <= n; i++ {
            maxSumA = max(maxSumA, s[i-secondLen]-s[i-secondLen-firstLen])
            ans = max(ans, maxSumA+s[i]-s[i-secondLen])
        }
    }
    f(firstLen, secondLen) // 左 a 右 b
    f(secondLen, firstLen) // 左 b 右 a
    return
}
```

## 简化代码

「左 $a$ 右 $b$」和「左 $b$ 右 $a$」可以合并到同一个循环中。

```py [sol2-Python3]
class Solution:
    def maxSumTwoNoOverlap(self, nums: List[int], firstLen: int, secondLen: int) -> int:
        s = list(accumulate(nums, initial=0))  # nums 的前缀和
        ans = maxSumA = maxSumB = 0
        for i in range(firstLen + secondLen, len(s)):
            maxSumA = max(maxSumA, s[i - secondLen] - s[i - firstLen - secondLen])
            maxSumB = max(maxSumB, s[i - firstLen] - s[i - firstLen - secondLen])
            ans = max(ans, maxSumA + s[i] - s[i - secondLen], maxSumB + s[i] - s[i - firstLen])
        return ans
```

```java [sol2-Java]
class Solution {
    public int maxSumTwoNoOverlap(int[] nums, int firstLen, int secondLen) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + nums[i]; // 计算 nums 的前缀和
        int ans = 0, maxSumA = 0, maxSumB = 0;
        for (int i = firstLen + secondLen; i <= n; ++i) {
            maxSumA = Math.max(maxSumA, s[i - secondLen] - s[i - secondLen - firstLen]);
            maxSumB = Math.max(maxSumB, s[i - firstLen] - s[i - firstLen - secondLen]);
            ans = Math.max(ans, Math.max(maxSumA + s[i] - s[i - secondLen],  // 左 a 右 b
                                         maxSumB + s[i] - s[i - firstLen])); // 左 b 右 a
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int maxSumTwoNoOverlap(vector<int>& nums, int firstLen, int secondLen) {
        int n = nums.size(), s[n + 1];
        s[0] = 0;
        partial_sum(nums.begin(), nums.end(), s + 1); // 计算 nums 的前缀和
        int ans = 0, maxSumA = 0, maxSumB = 0;
        for (int i = firstLen + secondLen; i <= n; ++i) {
            maxSumA = max(maxSumA, s[i - secondLen] - s[i - secondLen - firstLen]);
            maxSumB = max(maxSumB, s[i - firstLen] - s[i - firstLen - secondLen]);
            ans = max(ans, max(maxSumA + s[i] - s[i - secondLen],  // 左 a 右 b
                               maxSumB + s[i] - s[i - firstLen])); // 左 b 右 a
        }
        return ans;
    }
};
```

```go [sol2-Go]
func maxSumTwoNoOverlap(nums []int, firstLen, secondLen int) (ans int) {
    n := len(nums)
    s := make([]int, n+1)
    for i, x := range nums {
        s[i+1] = s[i] + x // 计算 nums 的前缀和
    }
    maxSumA, maxSumB := 0, 0
    for i := firstLen + secondLen; i <= n; i++ {
        maxSumA = max(maxSumA, s[i-secondLen]-s[i-secondLen-firstLen])
        maxSumB = max(maxSumB, s[i-firstLen]-s[i-firstLen-secondLen])
        ans = max(ans, max(maxSumA+s[i]-s[i-secondLen], // 左 a 右 b
                           maxSumB+s[i]-s[i-firstLen])) // 左 b 右 a
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

> 注：本题还可以用 $4$ 个滑动窗口实现。可以做到 $\mathcal{O}(1)$ 额外空间，且不修改 $\textit{nums}$。

## 总结

对于有两个变量的题目，通常可以枚举其中一个变量，把它视作常量，从而转化成只有一个变量的问题。

对于本题来说，就是枚举 $b$，把问题转化成计算 $a$ 的最大元素和。

其实这个技巧在 [1. 两数之和](https://leetcode.cn/problems/two-sum/) 中就体现了：枚举第二个数，去左边找第一个数。（用哈希表优化找第一个数的过程。）

## 相似题目

- [2555. 两个线段获得的最多奖品](https://leetcode.cn/problems/maximize-win-from-two-segments/)，[题解](https://leetcode.cn/problems/maximize-win-from-two-segments/solution/tong-xiang-shuang-zhi-zhen-ji-lu-di-yi-t-5hlh/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
