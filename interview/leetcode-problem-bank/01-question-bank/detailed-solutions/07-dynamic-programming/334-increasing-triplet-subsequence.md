# 334. 递增的三元子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/increasing-triplet-subsequence/
- 题目 slug：`increasing-triplet-subsequence`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/increasing-triplet-subsequence/solutions/3844143/tong-yong-jie-fa-ba-3-gai-cheng-k-ye-nen-t8si/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[通用解法，把 3 改成 k 也能做（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/increasing-triplet-subsequence/solutions/3844143/tong-yong-jie-fa-ba-3-gai-cheng-k-ye-nen-t8si/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-yong-jie-fa-ba-3-gai-cheng-k-ye-nen-t8si`
- topic id：`3844143`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于**递增子序列的子序列一定是递增的**，我们可以先计算 $\textit{nums}$ 的 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)，然后从中取出一个长为 $3$ 的子序列。

设最长递增子序列的长度为 $L$，答案就是判断 $L\ge 3$ 是否成立。

如果把题目的 $3$ 改成 $k$，那么判断条件就是 $L\ge k$。

也可以在循环过程中判断 LIS 的长度是否达到 $k$。

**视频讲解**：[最长递增子序列【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)。

```py [sol-Python3]
class Solution:
    def increasingTriplet(self, nums: List[int]) -> bool:
        g = []
        for x in nums:
            j = bisect_left(g, x)
            if j == 2:  # LIS 长度至少是 3
                return True
            if j == len(g):  # >=x 的 g[j] 不存在
                g.append(x)  
            else:
                g[j] = x
        return False
```

```java [sol-Java]
class Solution {
    public boolean increasingTriplet(int[] nums) {
        List<Integer> g = new ArrayList<>();
        for (int x : nums) {
            int j = lowerBound(g, x);
            if (j == 2) { // LIS 长度至少是 3
                return true;
            }
            if (j == g.size()) { // >=x 的 g[j] 不存在
                g.add(x);
            } else {
                g.set(j, x);
            }
        }
        return false;
    }

    // 开区间写法
    private int lowerBound(List<Integer> g, int target) {
        int left = -1, right = g.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (g.get(mid) < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right; // 或者 left+1
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public boolean increasingTriplet(int[] nums) {
        int ng = 0; // g 的长度
        for (int x : nums) {
            int j = lowerBound(nums, ng, x);
            if (j == 2) { // LIS 长度至少是 3
                return true;
            }
            nums[j] = x;
            if (j == ng) { // >=x 的 g[j] 不存在
                ng++;
            }
        }
        return false;
    }

    // 开区间写法
    private int lowerBound(int[] nums, int right, int target) {
        int left = -1; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right; // 或者 left+1
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool increasingTriplet(vector<int>& nums) {
        vector<int> g;
        for (int x : nums) {
            auto it = ranges::lower_bound(g, x);
            if (it - g.begin() == 2) { // LIS 长度至少是 3
                return true;
            }
            if (it == g.end()) { // >=x 的 g[j] 不存在
                g.push_back(x);
            } else {
                *it = x;
            }
        }
        return false;
    }
};
```

```go [sol-Go]
func increasingTriplet(nums []int) bool {
    g := []int{}
    for _, x := range nums {
        j := sort.SearchInts(g, x)
        if j == 2 { // LIS 长度至少是 3
            return true
        }
        if j == len(g) { // >=x 的 g[j] 不存在
            g = append(g, x)
        } else {
            g[j] = x
        }
    }
    return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log K)$，其中 $n$ 是 $\textit{nums}$ 的长度，$K=3-1=2$。$g$ 数组的长度至多为 $K$，本题二分查找可以视作常数。
- 空间复杂度：$\mathcal{O}(K)$ 或 $\mathcal{O}(1)$。

## 专题训练

见下面动态规划题单的「**§4.2 最长递增子序列（LIS）**」。

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

## 专题训练

见下面动态规划题单的「**§4.2 最长递增子序列（LIS）**」。

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
