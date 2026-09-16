# 1031. 两个无重叠子数组的最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-sum-of-two-non-overlapping-subarrays/
- 题目 slug：`maximum-sum-of-two-non-overlapping-subarrays`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶
- 难度分：2000
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

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
