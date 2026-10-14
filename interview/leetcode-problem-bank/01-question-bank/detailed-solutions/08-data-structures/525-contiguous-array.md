# 525. 连续数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/contiguous-array/
- 题目 slug：`contiguous-array`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/contiguous-array/solutions/3805089/shi-zi-bian-xing-mei-ju-you-wei-hu-zuo-p-x9q2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[式子变形 + 枚举右维护左（Python/Java/C++/Go）](https://leetcode.cn/problems/contiguous-array/solutions/3805089/shi-zi-bian-xing-mei-ju-you-wei-hu-zuo-p-x9q2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-zi-bian-xing-mei-ju-you-wei-hu-zuo-p-x9q2`
- topic id：`3805089`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**一句话思路**：把 $0$ 看成 $-1$，计算和为 $0$ 的最长子数组。

这是怎么想出来的？

**前置知识**：[前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

设 $0$ 在的 $\textit{nums}$ 中的**个数前缀和**数组为 $S_0$：如果 $\textit{nums}[i] = 0$ 则视作 $1$，否则视作 $0$，计算这个序列的前缀和数组。

同理，设 $1$ 在的 $\textit{nums}$ 中的个数前缀和数组为 $S_1$。

子数组 $[l,r)$ 中的 $1$ 和 $0$ 的出现次数相等，即

$$
S_1[r] - S_1[l] = S_0[r] - S_0[l]
$$

移项得

$$
S_1[r] - S_0[r] = S_1[l] - S_0[l]
$$

定义数组 $\textit{sum}[i] = S_1[i] - S_0[i]$，问题变成：

- 计算数组 $\textit{sum}$ 中的一对相等元素的最远距离。（注意子数组 $[l,r)$ 的长度是 $r-l$，无需加一。）

枚举右，维护左。维护 $\textit{sum}[i]$ 首次出现的下标，再次遇到 $\textit{sum}[i]$ 时，用 $i$ 减去 $\textit{sum}[i]$ 首次出现的下标，即为子数组长度，更新答案的最大值。

从 $\textit{sum}[i] = S_1[i] - S_0[i]$ 这个定义可以看出来，如果把原数组中的 $0$ 视作 $-1$，我们计算的就是和为 $0$ 的最长子数组。

## 优化前

```py [sol-Python3]
class Solution:
    def findMaxLength(self, nums: List[int]) -> int:
        # 把 0 当作 -1
        nums = [1 if x else -1 for x in nums]

        # 计算 nums 的前缀和
        s = list(accumulate(nums, initial=0))

        pos = {}
        ans = 0
        for i, x in enumerate(s):
            if x in pos:
                # 找到一对相等元素，下标分别为 pos[x] 和 i，距离为 i - pos[x]
                ans = max(ans, i - pos[x])
            else:
                pos[x] = i
        return ans
```

```java [sol-Java]
class Solution {
    public int findMaxLength(int[] nums) {
        // 计算前缀和
        int n = nums.length;
        int[] sum = new int[n + 1];
        for (int i = 0; i < n; i++) {
            int x = nums[i] == 0 ? -1 : 1; // 把 0 当作 -1
            sum[i + 1] = sum[i] + x;
        }

        Map<Integer, Integer> pos = new HashMap<>();
        int ans = 0;
        for (int i = 0; i <= n; i++) {
            int s = sum[i];
            if (pos.containsKey(s)) {
                // 找到一对相等元素，下标分别为 pos[s] 和 i，距离为 i - pos[s]
                ans = Math.max(ans, i - pos.get(s));
            } else {
                pos.put(s, i);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMaxLength(vector<int>& nums) {
        for (int& x : nums) {
            x = x ? 1 : -1; // 把 0 当作 -1
        }

        // 计算 nums 的前缀和
        int n = nums.size();
        vector<int> sum(n + 1);
        partial_sum(nums.begin(), nums.end(), sum.begin() + 1);

        unordered_map<int, int> pos;
        int ans = 0;
        for (int i = 0; i <= n; i++) {
            int s = sum[i];
            if (pos.contains(s)) {
                // 找到一对相等元素，下标分别为 pos[s] 和 i，距离为 i - pos[s]
                ans = max(ans, i - pos[s]);
            } else {
                pos[s] = i;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findMaxLength(nums []int) (ans int) {
    // 计算前缀和
    sum := make([]int, len(nums)+1)
    for i, x := range nums {
        // 把 0 当作 -1
        if x == 0 {
            x = -1
        }
        sum[i+1] = sum[i] + x
    }

    pos := map[int]int{}
    for i, s := range sum {
        if j, ok := pos[s]; ok {
            // 找到一对相等元素，下标分别为 j=pos[s] 和 i，距离为 i-j
            ans = max(ans, i-j)
        } else {
            pos[s] = i
        }
    }
    return
}
```

## 优化

合并两个循环，把 $\textit{sum}$ 数组简化成一个变量 $s$。

上面代码的第二个循环，我们循环了 $n+1$ 次（注意 $\textit{sum}$ 的长度是 $n+1$），而在下面的代码中，我们循环了 $n$ 次。

少了那一次去哪了？我们单独处理了 $\textit{sum}$ 的第一个数。

```py [sol-Python3]
class Solution:
    def findMaxLength(self, nums: List[int]) -> int:
        # 前缀和数组的首项 0 相当于在 -1 下标
        pos = {0: -1}
        ans = s = 0
        for i, x in enumerate(nums):
            s += 1 if x else -1
            if s in pos:
                ans = max(ans, i - pos[s])
            else:
                pos[s] = i
        return ans
```

```java [sol-Java]
class Solution {
    public int findMaxLength(int[] nums) {
        // 前缀和数组的首项 0 相当于在 -1 下标
        Map<Integer, Integer> pos = new HashMap<>();
        pos.put(0, -1);
        int ans = 0, s = 0;
        for (int i = 0; i < nums.length; i++) {
            s += nums[i] == 0 ? -1 : 1;
            if (pos.containsKey(s)) {
                ans = Math.max(ans, i - pos.get(s));
            } else {
                pos.put(s, i);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMaxLength(vector<int>& nums) {
        // 前缀和数组的首项 0 相当于在 -1 下标
        unordered_map<int, int> pos = {{0, -1}};
        int ans = 0, s = 0;
        for (int i = 0; i < nums.size(); i++) {
            s += nums[i] ? 1 : -1;
            if (pos.contains(s)) {
                ans = max(ans, i - pos[s]);
            } else {
                pos[s] = i;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findMaxLength(nums []int) (ans int) {
    // 前缀和数组的首项 0 相当于在 -1 下标
    pos := map[int]int{0: -1}
    s := 0
    for i, x := range nums {
        s += x*2 - 1 // 1 则加一，0 则减一
        if j, ok := pos[s]; ok {
            ans = max(ans, i-j)
        } else {
            pos[s] = i
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§1.2 前缀和与哈希表**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
