# 870. 优势洗牌

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/advantage-shuffle/
- 题目 slug：`advantage-shuffle`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：1648
- 外部题解来源：https://leetcode.cn/problems/advantage-shuffle/solutions/1875994/tian-ji-sai-ma-by-endlesscheng-yxm6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[田忌赛马（Python/Java/C++/Go）](https://leetcode.cn/problems/advantage-shuffle/solutions/1875994/tian-ji-sai-ma-by-endlesscheng-yxm6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tian-ji-sai-ma-by-endlesscheng-yxm6`
- topic id：`1875994`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

> “今以君之下驷与彼上驷，取君上驷与彼中驷，取君中驷与彼下驷。”既驰三辈毕，而田忌一不胜而再胜，卒得王千金。

把 $\textit{nums}_1$ 当成是田忌的马，$\textit{nums}_2$ 当成是齐威王的马。

讨论田忌的下等马（$\textit{nums}_1$ 的最小值）：

- 如果它能比过齐威王的下等马（$\textit{nums}_2$ 的最小值），那这一分田忌直接拿下；
- 如果它比不过齐威王的下等马，则用田忌的下等马比齐威王的上等马（$\textit{nums}_2$ 的最大值）。

去掉这两匹马，问题变成一个规模更小（$n-1$）的子问题。重复上述过程，即得到了所有马的对应关系。

代码实现时，由于 $\textit{nums}_2$ 不能排序，我们可以创建一个下标数组 $\textit{idx}$，对$\textit{idx}$ 排序，即 $\textit{idx}[0]$ 对应 $\textit{nums}_2$ 中最小值的下标，$\textit{idx}[1]$ 对应 $\textit{nums}_2$ 中第二小值的下标，……。用双指针操作 $\textit{idx}$，从而知道每个下标所要对应的 $\textit{nums}_1$ 的元素，也就找到了所要求的 $\textit{nums}_1$ 的排列。

```py [sol-Python3]
class Solution:
    def advantageCount(self, nums1: List[int], nums2: List[int]) -> List[int]:
        nums1.sort()

        n = len(nums1)
        idx = sorted(range(n), key=lambda i: nums2[i])

        ans = [0] * n
        left, right = 0, n - 1
        for x in nums1:
            if x > nums2[idx[left]]:
                ans[idx[left]] = x  # 用下等马比下等马
                left += 1
            else:
                ans[idx[right]] = x  # 用下等马比上等马
                right -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] advantageCount(int[] nums1, int[] nums2) {
        Arrays.sort(nums1);

        int n = nums1.length;
        Integer[] idx = new Integer[n];
        Arrays.setAll(idx, i -> i);
        Arrays.sort(idx, (i, j) -> nums2[i] - nums2[j]);

        int[] ans = new int[n];
        int left = 0;
        int right = n - 1;
        for (int x : nums1) {
            int i = x > nums2[idx[left]] ? idx[left++] : idx[right--];
            ans[i] = x;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> advantageCount(vector<int>& nums1, vector<int>& nums2) {
        ranges::sort(nums1);

        int n = nums1.size();
        vector<int> idx(n);
        ranges::iota(idx, 0);
        ranges::sort(idx, {}, [&](int i) { return nums2[i]; });

        vector<int> ans(n);
        int left = 0, right = n - 1;
        for (int x : nums1) {
            int i = x > nums2[idx[left]] ? idx[left++] : idx[right--];
            ans[i] = x;
        }
        return ans;
    }
};
```

```go [sol-Go]
func advantageCount(nums1, nums2 []int) []int {
    slices.Sort(nums1)

    n := len(nums1)
    idx := make([]int, n)
    for i := range idx {
        idx[i] = i
    }
    slices.SortFunc(idx, func(i, j int) int { return nums2[i] - nums2[j] })

    ans := make([]int, n)
    left, right := 0, n-1
    for _, x := range nums1 {
        if x > nums2[idx[left]] {
            ans[idx[left]] = x // 用下等马比下等马
            left++
        } else {
            ans[idx[right]] = x // 用下等马比上等马
            right--
        }
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}_1$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
