# 462. 最小操作次数使数组元素相等 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/
- 题目 slug：`minimum-moves-to-equal-array-elements-ii`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.5 中位数贪心
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/solutions/3711552/zhong-wei-shu-tan-xin-ji-qi-zheng-ming-p-6cxi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[中位数贪心及其证明（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/solutions/3711552/zhong-wei-shu-tan-xin-ji-qi-zheng-ming-p-6cxi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhong-wei-shu-tan-xin-ji-qi-zheng-ming-p-6cxi`
- topic id：`3711552`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

给定数组 $\textit{nums}$，每次操作，可以把其中一个数加一或者减一。把 $\textit{nums}$ 的所有数都变成一样的，最少要操作多少次？

**定理**：把所有数都变成 $\textit{nums}$ 的**中位数**是最优的。

[证明](https://zhuanlan.zhihu.com/p/1922938031687595039)。

无论 $n$ 是奇数还是偶数，中位数的下标都可以取 $\left\lfloor\dfrac{n}{2}\right\rfloor$。

**算法**：

1. 计算 $\textit{nums}$ 的中位数 $\textit{median}$。可以排序，也可以用快速选择算法（见 C++）。
2. 计算 $\textit{nums}_i - |\textit{median}|$ 的总和，即为答案。

> 注意题目保证答案在 $32$ 位整数范围内，无需使用 $64$ 位整数。

```py [sol-Python3]
class Solution:
    def minMoves2(self, nums: List[int]) -> int:
        nums.sort()
        median = nums[len(nums) // 2]
        return sum(abs(x - median) for x in nums)
```

```java [sol-Java]
class Solution {
    public int minMoves2(int[] nums) {
        Arrays.sort(nums);
        int median = nums[nums.length / 2];

        int ans = 0;
        for (int x : nums) {
            ans += Math.abs(x - median);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minMoves2(vector<int>& nums) {
        int m = nums.size() / 2;
        ranges::nth_element(nums, nums.begin() + m);
        int median = nums[m];

        int ans = 0;
        for (int x : nums) {
            ans += abs(x - median);
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int minMoves2(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int median = nums[numsSize / 2];

    int ans = 0;
    for (int i = 0; i < numsSize; i++) {
        ans += abs(nums[i] - median);
    }
    return ans;
}
```

```go [sol-Go]
func minMoves2(nums []int) (ans int) {
    slices.Sort(nums)
    median := nums[len(nums)/2]

    for _, x := range nums {
        ans += abs(x - median)
    }
    return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
var minMoves2 = function(nums) {
    nums.sort((a, b) => a - b);
    const median = nums[Math.floor(nums.length / 2)];
    return nums.reduce((s, x) => s + Math.abs(x - median), 0);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_moves2(mut nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let median = *nums.select_nth_unstable(n / 2).1;
        nums.into_iter().map(|x| (x - median).abs()).sum()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。用快速选择算法可以做到 $\mathcal{O}(n)$，见 C++、Rust 代码。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序时的栈开销。

## 专题训练

见下面贪心题单的「**§4.5 中位数贪心**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
