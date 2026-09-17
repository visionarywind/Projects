# 2580. 统计将重叠区间合并成组的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-ways-to-group-overlapping-ranges/
- 题目 slug：`count-ways-to-group-overlapping-ranges`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.5 合并区间
- 难度分：1632
- 外部题解来源：https://leetcode.cn/problems/count-ways-to-group-overlapping-ranges/solutions/2147717/tiao-yue-you-xi-bian-xing-by-endlesschen-hatn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[合并区间，附题单（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/count-ways-to-group-overlapping-ranges/solutions/2147717/tiao-yue-you-xi-bian-xing-by-endlesschen-hatn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tiao-yue-you-xi-bian-xing-by-endlesschen-hatn`
- topic id：`2147717`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

题目说「有交集的区间必须在同一个组内」，我们可以先把有交集的区间，合并成一个大区间。

设合并后有 $m$ 个大区间，那么每个大区间都可以分到第一个组或者第二个组，每个大区间都有 $2$ 个方案。由于不同的大区间之间互相独立，根据**乘法原理**，方案数为 $2^m$。

怎么求出 $m$ 呢？

初始化 $m=0,\ \textit{maxR}=-1$。把区间按照**左端点**从小到大排序，遍历区间，同时维护当前合并的大区间右端点 $\textit{maxR}$：

- 如果当前区间的左端点 $l$ 大于 $\textit{maxR}$，由于我们已经按照左端点排序了，那么后面任何区间都不会和之前的区间有交集，换句话说，产生了一个新的大区间，把 $m$ 加一，同时 $\textit{maxR}$ 更新为当前区间右端点 $r$。
- 否则，当前区间要合并到大区间内，用当前区间右端点 $r$ 更新 $\textit{maxR}$ 的最大值。

代码实现时，也可以在遍历的同时，直接计算答案 $\textit{ans}=2^m$，把「$m$ 加一」改成「$\textit{ans}$ 乘 $2$」。在计算中对 $10^9+7$ 取模。

附：[视频讲解](https://www.bilibili.com/video/BV1dY4y1C77x/) 第三题。

```py [sol-Python3]
class Solution:
    def countWays(self, ranges: List[List[int]]) -> int:
        ranges.sort(key=lambda p: p[0])
        m, max_r = 0, -1
        for l, r in ranges:
            if l > max_r:  # 无法合并
                m += 1  # 新区间
            max_r = max(max_r, r)  # 合并
        return pow(2, m, 1_000_000_007)
```

```java [sol-Java]
class Solution {
    public int countWays(int[][] ranges) {
        Arrays.sort(ranges, (a, b) -> a[0] - b[0]);
        int ans = 1;
        int maxR = -1;
        for (int[] p : ranges) {
            if (p[0] > maxR) { // 无法合并
                ans = ans * 2 % 1_000_000_007; // 新区间
            }
            maxR = Math.max(maxR, p[1]); // 合并
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countWays(vector<vector<int>> &ranges) {
        ranges::sort(ranges, [](auto &a, auto &b) { return a[0] < b[0]; });
        int ans = 1, max_r = -1;
        for (auto &p : ranges) {
            if (p[0] > max_r) { // 无法合并
                ans = ans * 2 % 1'000'000'007; // 新区间
            }
            max_r = max(max_r, p[1]); // 合并
        }
        return ans;
    }
};
```

```go [sol-Go]
func countWays(ranges [][]int) int {
	slices.SortFunc(ranges, func(p, q []int) int { return p[0] - q[0] })
	ans, maxR := 1, -1
	for _, p := range ranges {
		if p[0] > maxR { // 无法合并
			ans = ans * 2 % 1_000_000_007 // 新区间
		}
		maxR = max(maxR, p[1]) // 合并
	}
	return ans
}
```

```js [sol-JavaScript]
var countWays = function(ranges) {
    ranges.sort((a, b) => a[0] - b[0]);
    let ans = 1, maxR = -1;
    for (const [l, r] of ranges) {
        if (l > maxR) { // 无法合并
            ans = ans * 2 % 1_000_000_007; // 新区间
        }
        maxR = Math.max(maxR, r); // 合并
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn count_ways(mut ranges: Vec<Vec<i32>>) -> i32 {
        ranges.sort_unstable_by(|a, b| a[0].cmp(&b[0]));
        let mut ans = 1;
        let mut max_r = -1;
        for p in &ranges {
            if p[0] > max_r { // 无法合并
                ans = ans * 2 % 1_000_000_007; // 新区间
            }
            max_r = max_r.max(p[1]); // 合并
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{ranges}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序时的栈开销，仅用到若干额外变量。

## 题单：合并区间

#### 练习 A

- [56. 合并区间](https://leetcode.cn/problems/merge-intervals/)
- [55. 跳跃游戏](https://leetcode.cn/problems/jump-game/)
- [2963. 统计好分割方案的数目](https://leetcode.cn/problems/count-the-number-of-good-partitions/) 1985
- [2584. 分割数组使乘积互质](https://leetcode.cn/problems/split-the-array-to-make-coprime-products/) 2159
- [2655. 寻找最大长度的未覆盖区间](https://leetcode.cn/problems/find-maximal-uncovered-ranges/)（会员题）

#### 练习 B

- [45. 跳跃游戏 II](https://leetcode.cn/problems/jump-game-ii/)
- [1024. 视频拼接](https://leetcode.cn/problems/video-stitching/) 1746
- [1326. 灌溉花园的最少水龙头数目](https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/) 1885

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.5 合并区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.5 合并区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
