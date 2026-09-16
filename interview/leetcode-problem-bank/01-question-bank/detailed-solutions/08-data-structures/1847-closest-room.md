# 1847. 最近的房间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/closest-room/
- 题目 slug：`closest-room`
- 来源专题：常用数据结构
- 来源分类路径：专题：离线算法
- 难度分：2082
- 外部题解来源：https://leetcode.cn/problems/closest-room/solutions/2996902/chi-xian-you-xu-ji-he-shuang-zhi-zhen-py-jch8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[离线+有序集合+双指针（Python/Java/C++/Go/Rust）](https://leetcode.cn/problems/closest-room/solutions/2996902/chi-xian-you-xu-ji-he-shuang-zhi-zhen-py-jch8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chi-xian-you-xu-ji-he-shuang-zhi-zhen-py-jch8`
- topic id：`2996902`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

把询问**排序**，通过改变回答询问的顺序，使问题更容易处理。

比如有两个询问，其中 $\textit{minSize}$ 分别为 $3$ 和 $6$。

我们可以先回答 $\textit{minSize}=6$ 的询问，再回答 $\textit{minSize}=3$ 的询问。

也就是先把面积 $\ge 6$ 的房间号添加到一个有序集合中，回答 $\textit{minSize}=6$ 的询问；然后把面积 $\ge 3$ 的房间号添加到有序集合中，回答 $\textit{minSize}=3$ 的询问。

**这里的关键是，由于面积 $\ge 6$ 的房间编号已经添加到有序集合中了，所以后续只需把面积在 $[3,5]$ 中的房间号添加到有序集合中，不需要重复处理面积 $\ge 6$ 的房间。**

## 具体思路

直接对 $\textit{queries}$ 排序是不行的，因为返回的答案必须按照询问的顺序。

解决办法：设 $\textit{q}$ 是 $\textit{queries}$ 的长度，创建一个下标数组 $\textit{queryIds}=[0,1,2,\ldots,q-1]$，把下标根据 $\textit{queries}$ 的 $\textit{minSize}$ 从大到小排序，这样就避免直接对 $\textit{queries}$ 排序了。

把 $\textit{rooms}$ 按照 $\textit{size}$ 从小到大排序（也可以从大到小）。

然后创建一个有序集合 $\textit{roomIds}$。用**双指针**遍历 $\textit{queryIds}$ 和 $\textit{rooms}$，把房间面积 $\ge \textit{minSize}$ 的房间号添加到 $\textit{roomIds}$ 中。然后在 $\textit{roomIds}$ 中搜索离 $\textit{preferred}$ 最近的左右两个房间号，其中离 $\textit{preferred}$ 最近的房间号就是答案。

```py [sol-Python3]
class Solution:
    def closestRoom(self, rooms: List[List[int]], queries: List[List[int]]) -> List[int]:
        rooms.sort(key=lambda r: r[1])  # 按照 size 从小到大排序
        q = len(queries)
        ans = [-1] * q
        room_ids = SortedList()  # sortedcontainers
        j = len(rooms) - 1
        for i in sorted(range(q), key=lambda i: -queries[i][1]):  # 按照 minSize 从大到小排序
            preferred_id, min_size = queries[i]
            while j >= 0 and rooms[j][1] >= min_size:
                room_ids.add(rooms[j][0])
                j -= 1

            diff = inf
            k = room_ids.bisect_left(preferred_id)
            if k:
                diff = preferred_id - room_ids[k - 1]  # 左边的差
                ans[i] = room_ids[k - 1]
            if k < len(room_ids) and room_ids[k] - preferred_id < diff:  # 右边的差更小
                ans[i] = room_ids[k]
        return ans
```

```java [sol-Java]
class Solution {
    public int[] closestRoom(int[][] rooms, int[][] queries) {
        // 按照 size 从大到小排序
        Arrays.sort(rooms, (a, b) -> (b[1] - a[1]));

        int q = queries.length;
        Integer[] queryIds = new Integer[q];
        Arrays.setAll(queryIds, i -> i);
        // 按照 minSize 从大到小排序
        Arrays.sort(queryIds, (i, j) -> queries[j][1] - queries[i][1]);

        int[] ans = new int[q];
        Arrays.fill(ans, -1);
        TreeSet<Integer> roomIds = new TreeSet<>();
        int j = 0;
        for (int i : queryIds) {
            int preferredId = queries[i][0];
            int minSize = queries[i][1];
            while (j < rooms.length && rooms[j][1] >= minSize) {
                roomIds.add(rooms[j][0]);
                j++;
            }

            int diff = Integer.MAX_VALUE;
            Integer floor = roomIds.floor(preferredId);
            if (floor != null) {
                diff = preferredId - floor; // 左边的差
                ans[i] = floor;
            }
            Integer ceiling = roomIds.ceiling(preferredId);
            if (ceiling != null && ceiling - preferredId < diff) { // 右边的差更小
                ans[i] = ceiling;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> closestRoom(vector<vector<int>>& rooms, vector<vector<int>>& queries) {
        // 按照 size 从大到小排序
        ranges::sort(rooms, {}, [](auto& a) { return -a[1]; });

        int q = queries.size();
        vector<int> query_ids(q);
        iota(query_ids.begin(), query_ids.end(), 0);
        // 按照 minSize 从大到小排序
        ranges::sort(query_ids, {}, [&](int i) { return -queries[i][1]; });

        vector<int> ans(q, -1);
        set<int> room_ids;
        int j = 0;
        for (int i : query_ids) {
            int preferred_id = queries[i][0], min_size = queries[i][1];
            while (j < rooms.size() && rooms[j][1] >= min_size) {
                room_ids.insert(rooms[j][0]);
                j++;
            }

            int diff = INT_MAX;
            auto it = room_ids.lower_bound(preferred_id);
            if (it != room_ids.begin()) {
                auto p = prev(it);
                diff = preferred_id - *p; // 左边的差
                ans[i] = *p;
            }
            if (it != room_ids.end() && *it - preferred_id < diff) { // 右边的差更小
                ans[i] = *it;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func closestRoom(rooms [][]int, queries [][]int) []int {
    // 按照 size 从大到小排序
    slices.SortFunc(rooms, func(a, b []int) int { return b[1] - a[1] })

    q := len(queries)
    queryIds := make([]int, q)
    for i := range queryIds {
        queryIds[i] = i
    }
    // 按照 minSize 从大到小排序
    slices.SortFunc(queryIds, func(i, j int) int { return queries[j][1] - queries[i][1] })

    ans := make([]int, q)
    for i := range ans {
        ans[i] = -1
    }
    roomIds := redblacktree.New[int, struct{}]() // import "github.com/emirpasic/gods/v2/trees/redblacktree"
    j := 0
    for _, i := range queryIds {
        preferredId, minSize := queries[i][0], queries[i][1]
        for j < len(rooms) && rooms[j][1] >= minSize {
            roomIds.Put(rooms[j][0], struct{}{})
            j++
        }

        diff := math.MaxInt
        // 左边的差
        if node, ok := roomIds.Floor(preferredId); ok {
            diff = preferredId - node.Key
            ans[i] = node.Key
        }
        // 右边的差
        if node, ok := roomIds.Ceiling(preferredId); ok && node.Key-preferredId < diff {
            ans[i] = node.Key
        }
    }
    return ans
}
```

```rust [sol-Rust]
use std::collections::BTreeSet;

impl Solution {
    pub fn closest_room(mut rooms: Vec<Vec<i32>>, queries: Vec<Vec<i32>>) -> Vec<i32> {
        rooms.sort_unstable_by_key(|r| -r[1]); // 按照 size 从大到小排序

        let q = queries.len();
        let mut query_ids = (0..q).collect::<Vec<_>>();
        query_ids.sort_unstable_by_key(|&i| -queries[i][1]); // 按照 minSize 从大到小排序

        let mut ans = vec![-1; q];
        let mut room_ids = BTreeSet::new();
        let mut j = 0;
        for i in query_ids {
            let preferred_id = queries[i][0];
            let min_size = queries[i][1];
            while j < rooms.len() && rooms[j][1] >= min_size {
                room_ids.insert(rooms[j][0]);
                j += 1;
            }

            let mut diff = i32::MAX;
            if let Some(&prev) = room_ids.range(..preferred_id).next_back() {
                diff = preferred_id - prev; // 左边的差
                ans[i] = prev;
            }
            if let Some(&next) = room_ids.range(preferred_id..).next() {
                if next - preferred_id < diff { // 右边的差更小
                    ans[i] = next;
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + q\log q + q\log n)$，其中 $n$ 是 $\textit{rooms}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。排序需要 $\mathcal{O}(n\log n + q\log q)$ 的时间，每次询问在 $\textit{roomIds}$ 上查找需要 $\mathcal{O}(\log n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n+q)$。

更多相似题目，见下面数据结构题单的「**专题：离线算法**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `专题：离线算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：离线算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
