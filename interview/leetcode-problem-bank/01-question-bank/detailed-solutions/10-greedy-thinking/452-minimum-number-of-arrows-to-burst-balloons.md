# 452. 用最少数量的箭引爆气球

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-arrows-to-burst-balloons/
- 题目 slug：`minimum-number-of-arrows-to-burst-balloons`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.3 区间选点
- 难度分：1700
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-arrows-to-burst-balloons/solutions/2974741/qu-jian-xuan-dian-wen-ti-pythonjavaccgoj-w9am/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你如何思考这题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-arrows-to-burst-balloons/solutions/2974741/qu-jian-xuan-dian-wen-ti-pythonjavaccgoj-w9am/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qu-jian-xuan-dian-wen-ti-pythonjavaccgoj-w9am`
- topic id：`2974741`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

## 题意

输入一些区间，把这些区间画在数轴上。在数轴上**最少**要放置多少个点，使得每个区间都包含至少一个点？

## 思路

从左到右思考，想一想，第一个点（**最左边的点**）放在哪最好？

例如 $\textit{points}=[[1,6],[6,7],[2,8],[7,10]]$。第一个点放在 $x=1$ 处，只能满足一个区间 $[1,6]$，而如果放在 $x=2$ 处，则可以满足区间 $[1,6],[2,8]$。进一步地，如果放在更靠右的 $x=6$ 处，则可以满足三个区间 $[1,6],[6,7],[2,8]$。注意不能放在 $x=7$ 处，这样没法满足区间 $[1,6]$ 了。

在 $x=6$ 放点后，问题变成剩下 $n-3$ 个区间，在数轴上最少要放置多少个点，使得每个区间都包含至少一个点。这是一个规模更小的子问题，可以用同样的方法解决。

把区间按照右端点从小到大排序，这样**第一个点就放在第一个区间的右端点处**。去掉包含第一个点的区间后，第二个点就放在剩余区间的第一个区间的右端点处。依此类推。

**总结**：想清楚第一个点怎么放，是破解这道题的关键。

## 算法

1. 把区间按照右端点从小到大排序。
2. 初始化答案 $\textit{ans}=0$，上一个放点的位置 $\textit{pre}=-\infty$。
3. 遍历区间，如果 $\textit{start}\le \textit{pre}$，那么这个区间已经包含点，跳过。
4. 如果 $\textit{start} > \textit{pre}$，那么必须放一个点，把 $\textit{ans}$ 加一。根据上面的讨论，当前区间的右端点就是放点的位置，更新 $\textit{pre}=\textit{end}$。
5. 遍历结束后，返回 $\textit{ans}$。

## 细节

对于 C++ 等语言，需要注意 $\textit{start}$ 可能等于 $32$ 位 $\text{int}$ 的最小值。代码实现时，可以把 $\textit{pre}$ 初始化为 $64$ 位 $\text{int}$ 的最小值，或者把 $\textit{pre}$ 初始化成第一个区间的右端点，然后从第二个区间开始遍历。

```py [sol-Python3]
class Solution:
    def findMinArrowShots(self, points: List[List[int]]) -> int:
        points.sort(key=lambda p: p[1])  # 按照右端点从小到大排序
        ans = 0
        pre = -inf
        for start, end in points:
            if start > pre:  # 上一个点在区间左边
                ans += 1
                pre = end  # 在区间的最右边放一个点
        return ans
```

```java [sol-Java]
class Solution {
    public int findMinArrowShots(int[][] points) {
        Arrays.sort(points, Comparator.comparingInt(p -> p[1])); // 按照右端点从小到大排序
        int ans = 0;
        long pre = Long.MIN_VALUE;
        for (int[] p : points) {
            if (p[0] > pre) { // 上一个放的点在区间左边
                ans++;
                pre = p[1]; // 在区间的最右边放一个点
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMinArrowShots(vector<vector<int>>& points) {
        ranges::sort(points, {}, [](auto& p) { return p[1]; }); // 按照右端点从小到大排序
        int ans = 0;
        long long pre = LLONG_MIN;
        for (auto& p : points) {
            if (p[0] > pre) { // 上一个点在区间左边
                ans++;
                pre = p[1]; // 在区间的最右边放一个点
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) { 
    int end_a = (*(int**)a)[1], end_b = (*(int**)b)[1];
    return (end_a > end_b) - (end_a < end_b);
}

int findMinArrowShots(int** points, int pointsSize, int* pointsColSize) {
    qsort(points, pointsSize, sizeof(int*), cmp);
    int ans = 1;
    int pre = points[0][1];
    for (int i = 1; i < pointsSize; i++) {
        if (points[i][0] > pre) { // 上一个放的点在区间左边
            ans++;
            pre = points[i][1]; // 在区间的最右边放一个点
        }
    }
    return ans;
}
```

```go [sol-Go]
func findMinArrowShots(points [][]int) (ans int) {
    slices.SortFunc(points, func(a, b []int) int { return a[1] - b[1] }) // 按照右端点从小到大排序
    pre := math.MinInt
    for _, p := range points {
        if p[0] > pre { // 上一个放的点在区间左边
            ans++
            pre = p[1] // 在区间的最右边放一个点
        }
    }
    return
}
```

```js [sol-JavaScript]
var findMinArrowShots = function(points) {
    points.sort((a, b) => a[1] - b[1]); // 按照右端点从小到大排序
    let ans = 0;
    let pre = -Infinity;
    for (const [start, end] of points) {
        if (start > pre) { // 上一个放的点在区间左边
            ans++;
            pre = end; // 在区间的最右边放一个点
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_min_arrow_shots(mut points: Vec<Vec<i32>>) -> i32 {
        points.sort_unstable_by(|a, b| a[1].cmp(&b[1])); // 按照右端点从小到大排序
        let mut ans = 1;
        let mut pre = points[0][1];
        for p in &points[1..] {
            if p[0] > pre { // 上一个放的点在区间左边
                ans += 1;
                pre = p[1]; // 在区间的最右边放一个点
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{points}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

更多相似题目，见下面贪心题单中的「**区间贪心**」。

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
