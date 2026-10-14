# 719. 找出第 K 小的数对距离

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-k-th-smallest-pair-distance/
- 题目 slug：`find-k-th-smallest-pair-distance`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.6 第 K 小/大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-k-th-smallest-pair-distance/solutions/3700873/di-k-xiao-da-wen-ti-de-tong-yong-zhuan-h-jmg0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[第 k 小/大问题的通用转化方法（Python/Java/C++/Go）](https://leetcode.cn/problems/find-k-th-smallest-pair-distance/solutions/3700873/di-k-xiao-da-wen-ti-de-tong-yong-zhuan-h-jmg0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`di-k-xiao-da-wen-ti-de-tong-yong-zhuan-h-jmg0`
- topic id：`3700873`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 转化

第 $k$ 小/大问题的**通用转化方法**：

- 第 $k$ 小等价于：求最小的 $x$，满足绝对差 $\le x$ 的数对**至少**有 $k$ 个。（注意是至少不是恰好）
- 第 $k$ 大等价于：求最大的 $x$，满足绝对差 $\ge x$ 的数对**至少**有 $k$ 个。

对于本题，$x$ 越大，越能找到 $k$ 个数对；$x$ 越小，越不能找到 $k$ 个数对。据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在本题转化成一个判定性问题：

- 给定整数 $\textit{mx}$，统计绝对差 $\le \textit{mx}$ 的数对个数 $\textit{cnt}$，判断是否满足 $\textit{cnt}\ge k$。

如何高效统计 $\textit{cnt}$ 呢？

## 同向双指针

为方便计算，把 $\textit{nums}$ 从小到大排序。

排序后，$\textit{nums}[j]$ 越大，那么满足 $i<j$ 且 $\textit{nums}[j] - \textit{nums}[i] \le \textit{mx}$ 的最小的 $i$ 也越大，我们可以用同向双指针解决这个问题。外层循环枚举 $\textit{nums}[j]$，内层循环若 $i$ 不满足上式，就把 $i$ 加一。

内层循环结束后，下标对 $(i,j)$ 是满足要求的。由于 $i$ 越大越能满足要求，所以除了 $(i,j)$，还有 $(i+1,j),(i+2,j),\ldots,(j-1,j)$ 都是满足要求的。也就是说，当 $j$ 固定时，$i,i+1,i+2,\ldots,j-1$ 都是满足要求的，这一共有 $j-i$ 个，加到 $\textit{cnt}$ 中。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$-1$。绝对差不能是负数，也就是有 $0$ 个数对 $\le -1$，由于 $0 < k$，所以无法满足要求。
- 开区间右端点初始值：排序后的 $\textit{nums}[n-1]-\textit{nums}[0]$。所有数对都满足要求，这有 $\dfrac{n(n-1)}{2}$ 个，由于 $\dfrac{n(n-1)}{2}\ge k$，所以一定满足要求。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

## 答疑

**问**：为什么二分结束后，答案 $\textit{ans}$ 一定是 $\textit{nums}$ 中的某两个数的绝对差？

**答**：反证法。假设 $\textit{ans}$ 不是某两个数的绝对差，这意味着第 $k$ 小的绝对差比 $\textit{ans}$ 小，或者说 $\le \textit{ans}-1$。换句话说，$\le \textit{ans}-1$ 的数对有 $k$ 个，即 $\text{check}(\textit{ans}-1)=\texttt{true}$。但根据循环不变量，二分结束后 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾。故原命题成立。

```py [sol-Python3]
class Solution:
    def smallestDistancePair(self, nums: List[int], k: int) -> int:
        nums.sort()

        def check(mx: int) -> bool:
            cnt = i = 0
            for j, x in enumerate(nums):
                while x - nums[i] > mx:
                    i += 1
                cnt += j - i
            return cnt >= k

        left, right = -1, nums[-1] - nums[0]
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def smallestDistancePair(self, nums: List[int], k: int) -> int:
        nums.sort()

        def check(mx: int) -> bool:
            cnt = i = 0
            for j, x in enumerate(nums):
                while x - nums[i] > mx:
                    i += 1
                cnt += j - i
            return cnt >= k

        return bisect_left(range(nums[-1] - nums[0]), True, key=check)
```

```java [sol-Java]
class Solution {
    public int smallestDistancePair(int[] nums, int k) {
        Arrays.sort(nums);
        int left = -1;
        int right = nums[nums.length - 1] - nums[0];
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (check(nums, k, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int[] nums, int k, int mx) {
        int cnt = 0;
        int i = 0;
        for (int j = 0; j < nums.length; j++) {
            while (nums[j] - nums[i] > mx) {
                i++;
            }
            cnt += j - i;
        }
        return cnt >= k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int smallestDistancePair(vector<int>& nums, int k) {
        ranges::sort(nums);

        auto check = [&](int mx) -> bool {
            int cnt = 0, i = 0;
            for (int j = 0; j < nums.size(); j++) {
                while (nums[j] - nums[i] > mx) {
                    i++;
                }
                cnt += j - i;
            }
            return cnt >= k;
        };

        int left = -1, right = nums.back() - nums[0];
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func smallestDistancePair(nums []int, k int) int {
    slices.Sort(nums)
    ans := sort.Search(nums[len(nums)-1]-nums[0], func(mx int) bool {
        cnt := 0
        i := 0
        for j, x := range nums {
            for x-nums[i] > mx {
                i++
            }
            cnt += j - i
        }
        return cnt >= k
    })
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums}) - \min(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 相似题目

见下面二分题单的「**§2.6 第 K 小/大**」。

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
