# 646. 最长数对链

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-length-of-pair-chain/
- 题目 slug：`maximum-length-of-pair-chain`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.1 不相交区间
- 难度分：435
- 外部题解来源：https://leetcode.cn/problems/maximum-length-of-pair-chain/solutions/3077521/tan-xin-zheng-ming-pythonjavaccgojsrust-a20dx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心+证明（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-length-of-pair-chain/solutions/3077521/tan-xin-zheng-ming-pythonjavaccgojsrust-a20dx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-zheng-ming-pythonjavaccgojsrust-a20dx`
- topic id：`3077521`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

考虑所选区间中，最左边的那个区间。

## 最左边的区间选哪个？

我们可以选所有区间中右端点最小的区间（记作 $A$）。为什么？因为**在任意一种选法中，都可以把最左边的区间，替换成** $A$。由于 $A$ 是一个更靠左的区间，所以 $A$ 不会与其他已选区间相交，所以这个替换操作是合法的。

所以，在所有最优选法中，一定存在一种选法，选择了右端点最小的区间 $A$。

去掉区间 $A$ 以及与 $A$ 相交的区间（这些区间的左端点都小于 $A$ 的右端点），问题变成：

- 剩余区间中最多可以选多少个互不重叠的区间？

这是一个规模更小的子问题，可以用递归/迭代解决。

## 为什么要排序？为什么要按照右端点排序？

选择右端点最小的区间 $A$ 后，左端点小于 $A$ 的右端点的区间都与 $A$ 相交，都不能选。

因此，为了方便计算**下一个可以选的区间**，按照右端点从小到大排序。

排序后，$\textit{pairs}[0]$ 一定可以选，并且下一个可以选的区间是第一个左端点 $\ge \textit{pairs}[0][1]$ 的区间。

## 算法

1. 把 $\textit{pairs}$ 按照右端点从小到大排序。
2. 初始化计数器 $\textit{ans}=0$，上一个选的区间的右端点 $\textit{preR}=-\infty$。
3. 遍历 $\textit{pairs}$，如果发现 $\textit{pairs}[i][0]\ge \textit{preR}$，那么选 $\textit{pairs}[i]$，把 $\textit{ans}$ 加一，更新 $\textit{preR} = \textit{pairs}[i][1]$。
4. 遍历结束后，$\textit{ans}$ 就是不重叠区间个数的最大值。

```py [sol-Python3]
class Solution:
    def findLongestChain(self, pairs: List[List[int]]) -> int:
        pairs.sort(key=lambda x: x[1])
        ans = 0
        pre_r = -inf
        for l, r in pairs:
            if l > pre_r:
                ans += 1
                pre_r = r
        return ans
```

```java [sol-Java]
class Solution {
    public int findLongestChain(int[][] pairs) {
        Arrays.sort(pairs, (a, b) -> a[1] - b[1]);
        int ans = 0;
        int preR = Integer.MIN_VALUE;
        for (int[] p : pairs) {
            if (p[0] > preR) {
                ans++;
                preR = p[1];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findLongestChain(vector<vector<int>>& pairs) {
        ranges::sort(pairs, {}, [](auto& a) { return a[1]; });
        int ans = 0;
        int pre_r = INT_MIN;
        for (auto& p : pairs) {
            if (p[0] > pre_r) {
                ans++;
                pre_r = p[1];
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return (*(int**)a)[1] - (*(int**)b)[1];
}

int findLongestChain(int** pairs, int pairsSize, int* pairsColSize) {
    qsort(pairs, pairsSize, sizeof(int*), cmp);
    int ans = 0;
    int pre_r = INT_MIN;
    for (int i = 0; i < pairsSize; i++) {
        if (pairs[i][0] > pre_r) {
            ans++;
            pre_r = pairs[i][1];
        }
    }
    return ans;
}
```

```go [sol-Go]
func findLongestChain(pairs [][]int) (ans int) {
    slices.SortFunc(pairs, func(a, b []int) int { return a[1] - b[1] })
    preR := math.MinInt
    for _, p := range pairs {
        if p[0] > preR {
            ans++
            preR = p[1]
        }
    }
    return
}
```

```js [sol-JavaScript]
var findLongestChain = function(pairs) {
    pairs.sort((a, b) => a[1] - b[1]);
    let ans = 0;
    let preR = -Infinity;
    for (const [l, r] of pairs) {
        if (l > preR) {
            ans++;
            preR = r;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_longest_chain(mut pairs: Vec<Vec<i32>>) -> i32 {
        pairs.sort_unstable_by_key(|a| a[1]);
        let mut ans = 0;
        let mut pre_r = i32::MIN;
        for p in pairs {
            if p[0] > pre_r {
                ans += 1;
                pre_r = p[1];
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{pairs}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。不计入排序的栈开销。

更多相似题目，见下面贪心题单中的「**§2.1 不相交区间**」。

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
10. 【本题相关】[贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
