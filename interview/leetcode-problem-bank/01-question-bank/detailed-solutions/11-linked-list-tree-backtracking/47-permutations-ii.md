# 47. 全排列 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/permutations-ii/
- 题目 slug：`permutations-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.6 有重复元素的回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/permutations-ii/solutions/3059690/ru-he-qu-zhong-pythonjavacgojsrust-by-en-zlwl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[如何避免重复的排列？（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/permutations-ii/solutions/3059690/ru-he-qu-zhong-pythonjavacgojsrust-by-en-zlwl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ru-he-qu-zhong-pythonjavacgojsrust-by-en-zlwl`
- topic id：`3059690`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

前置题目：[46. 全排列](https://leetcode.cn/problems/permutations/)，视频讲解：[排列型回溯【基础算法精讲 16】](https://www.bilibili.com/video/BV1mY411D7f6/)。

## 重复来自哪？

以示例 1 $\textit{nums}=[1,1,2]$ 为例。如果按照 46 题的做法，在递归的过程中，会发生如下两种情况：

1. 第一个位置填 $\textit{nums}[0]$，第二个位置填 $\textit{nums}[1]$。
2. 第一个位置填 $\textit{nums}[1]$，第二个位置填 $\textit{nums}[0]$。

这两种填法，最终都会得到排列 $[1,1,2]$，重复了。

## 如何避免重复？

我们要保证第二种情况不会发生：

- 如果在填 $\textit{nums}[1]=1$ 的时候，发现 $\textit{nums}[0]=1$ 还没有填过，那么就不使用这种填法，直接 $\texttt{continue}$。

推广到更多数的时候，这个规则仍然适用吗？

再来看一个例子，$\textit{nums}=[1,1,1,2]$，它有四个排列：

- $[1,1,1,2]$。
- $[1,1,2,1]$。
- $[1,2,1,1]$。
- $[2,1,1,1]$。

第一个位置要么填 $1$，要么填 $2$。

其中第一个位置填 $1$ 的三个排列，会在第一个位置填 $\textit{nums}[0]$ 的时候枚举到。如果第一个位置填 $\textit{nums}[1]$ 或者 $\textit{nums}[2]$，那么必然会产生重复的排列。

所以 $\textit{nums}[1]$ 和 $\textit{nums}[2]$ 绝对不能填到第一个位置上！

这意味着，如果有多个 $\textit{nums}[i]$ 都相同，那么我们只需枚举其中一个 $\textit{nums}[i]$ 填第一个位置的情况，其余所有等于 $\textit{nums}[i]$ 的数都不能填第一个位置。

如果 $\textit{nums}[0]$ 填在了第一个位置，那么问题变成：

- 剩余的 $1,1,2$ 怎么填。

**这是一个和原问题相似的，规模更小的子问题**，处理方式同上：$\textit{nums}[1]$ 可以填在排列的第二个位置，而 $\textit{nums}[2]$ 不能填在排列的第二个位置，否则会导致重复的排列（就像本文开头分析的那样）。怎么判断？如果我们还没有填入 $\textit{nums}[1]$，那么和 $\textit{nums}[1]$ 相等的 $\textit{nums}[2]$ 是不能填入的。

## 如何判断 nums[i] 能不能填？

为方便判断，先把 $\textit{nums}$ 排序（从小到大或者从大到小都可以）。

分类讨论：

- 如果 $\textit{nums}[i]\ne \textit{nums}[i-1]$，那么 $\textit{nums}[i]$ 就是所有等于 $\textit{nums}[i]$ 的数中的第一个数。我们规定：这种情况可以随意填。
- 如果 $\textit{nums}[i] = \textit{nums}[i-1]$，继续讨论：
  - 如果 $\textit{nums}[i-1]$ 没有填入排列，为了避免生成重复的排列，绝对不能填 $\textit{nums}[i]$，直接 $\texttt{continue}$。这会导致后续所有等于 $\textit{nums}[i]$ 的数全部 $\texttt{continue}$，因为对于后面的 $\textit{nums}[i']$ 来说，$\textit{nums}[i']$ 和前面的数 $\textit{nums}[i'-1]$ 相等，并且 $\textit{nums}[i'-1]$ 没有填入排列。
  - 如果 $\textit{nums}[i-1]$ 已经填入排列，那么 $\textit{nums}[i]$ 是剩余元素（子问题）中的第一个等于 $\textit{nums}[i]$ 的数，可以随意填。

```py [sol-Python3]
class Solution:
    def permuteUnique(self, nums: List[int]) -> List[List[int]]:
        nums.sort()

        n = len(nums)
        path = [0] * n  # 所有排列的长度都是 n
        on_path = [False] * n  # on_path[j] 表示 nums[j] 是否已经填入排列
        ans = []

        # i 表示当前要填排列的第几个数
        def dfs(i: int) -> None:
            if i == n:  # 填完了
                ans.append(path.copy())  # 也可以写 path[:]
                return

            # 枚举 nums[j] 填入 path[i]
            for j, on in enumerate(on_path):
                # 如果 nums[j] 已填入排列，continue
                # 如果 nums[j] 和前一个数 nums[j-1] 相等，且 nums[j-1] 没填入排列，continue
                if on or j > 0 and nums[j] == nums[j - 1] and not on_path[j - 1]:
                    continue
                path[i] = nums[j]  # 填入排列
                on_path[j] = True  # nums[j] 已填入排列（注意标记的是下标，不是值）
                dfs(i + 1)  # 填排列的下一个数
                on_path[j] = False  # 恢复现场
                # 注意 path 无需恢复现场，因为排列长度固定，直接覆盖 path[i] 就行

        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> permuteUnique(int[] nums) {
        Arrays.sort(nums);

        int n = nums.length;
        List<Integer> path = Arrays.asList(new Integer[n]); // 所有排列的长度都是 n
        boolean[] onPath = new boolean[n]; // onPath[j] 表示 nums[j] 是否已经填入排列
        List<List<Integer>> ans = new ArrayList<>();

        dfs(0, nums, path, onPath, ans);
        return ans;
    }

    // i 表示当前要填排列的第几个数
    private void dfs(int i, int[] nums, List<Integer> path, boolean[] onPath, List<List<Integer>> ans) {
        if (i == nums.length) { // 填完了
            ans.add(new ArrayList<>(path));
            return;
        }

        // 枚举 nums[j] 填入 path[i]
        for (int j = 0; j < nums.length; j++) {
            // 如果 nums[j] 已填入排列，continue
            // 如果 nums[j] 和前一个数 nums[j-1] 相等，且 nums[j-1] 没填入排列，continue
            if (onPath[j] || j > 0 && nums[j] == nums[j - 1] && !onPath[j - 1]) {
                continue;
            }
            path.set(i, nums[j]); // 填入排列
            onPath[j] = true; // nums[j] 已填入排列（注意标记的是下标，不是值）
            dfs(i + 1, nums, path, onPath, ans); // 填排列的下一个数
            onPath[j] = false; // 恢复现场
            // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖 path[i] 就行
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> permuteUnique(vector<int>& nums) {
        ranges::sort(nums);

        int n = nums.size();
        vector<int> path(n); // 所有排列的长度都是 n
        vector<int> on_path(n); // on_path[j] 表示 nums[j] 是否已经填入排列
        vector<vector<int>> ans;

        // i 表示当前要填排列的第几个数
        auto dfs = [&](this auto&& dfs, int i) -> void {
            if (i == n) { // 填完了
                ans.push_back(path);
                return;
            }

            // 枚举 nums[j] 填入 path[i]
            for (int j = 0; j < n; j++) {
                // 如果 nums[j] 已填入排列，continue
                // 如果 nums[j] 和前一个数 nums[j-1] 相等，且 nums[j-1] 没填入排列，continue
                if (on_path[j] || j > 0 && nums[j] == nums[j - 1] && !on_path[j - 1]) {
                    continue;
                }
                path[i] = nums[j]; // 填入排列
                on_path[j] = true; // nums[j] 已填入排列（注意标记的是下标，不是值）
                dfs(i + 1); // 填排列的下一个数
                on_path[j] = false; // 恢复现场
                // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖 path[i] 就行
            }
        };

        dfs(0);
        return ans;
    }
};
```

```go [sol-Go]
func permuteUnique(nums []int) (ans [][]int) {
    slices.Sort(nums)

    n := len(nums)
    path := make([]int, n)    // 所有排列的长度都是 n
    onPath := make([]bool, n) // onPath[j] 表示 nums[j] 是否已经填入排列

    var dfs func(int)
    dfs = func(i int) { // i 表示当前要填排列的第几个数
        if i == n { // 填完了
            ans = append(ans, slices.Clone(path))
            return
        }

        // 枚举 nums[j] 填入 path[i]
        for j, on := range onPath {
            // 如果 nums[j] 已填入排列，continue
            // 如果 nums[j] 和前一个数 nums[j-1] 相等，且 nums[j-1] 没填入排列，continue
            if on || j > 0 && nums[j] == nums[j-1] && !onPath[j-1] {
                continue
            }
            path[i] = nums[j] // 填入排列
            onPath[j] = true  // nums[j] 已填入排列（注意标记的是下标，不是值）
            dfs(i + 1) // 填排列的下一个数
            onPath[j] = false // 恢复现场
            // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖 path[i] 就行
        }
    }

    dfs(0)
    return
}
```

```js [sol-JavaScript]
var permuteUnique = function(nums) {
    nums.sort((a, b) => a - b);

    const n = nums.length;
    const path = Array(n); // 所有排列的长度都是 n
    const onPath = Array(n).fill(false); // onPath[j] 表示 nums[j] 是否已经填入排列
    const ans = [];

    function dfs(i) { // i 表示当前要填排列的第几个数
        if (i === n) { // 填完了
            ans.push([...path]);
            return;
        }

        // 枚举 nums[j] 填入 path[i]
        for (let j = 0; j < n; j++) {
            // 如果 nums[j] 已填入排列，continue
            // 如果 nums[j] 和前一个数 nums[j-1] 相等，且 nums[j-1] 没填入排列，continue
            if (onPath[j] || j > 0 && nums[j] === nums[j - 1] && !onPath[j - 1]) {
                continue;
            }
            path[i] = nums[j]; // 填入排列
            onPath[j] = true; // nums[j] 已填入排列（注意标记的是下标，不是值）
            dfs(i + 1); // 填排列的下一个数
            onPath[j] = false; // 恢复现场
            // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖 path[i] 就行
        }
    }

    dfs(0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn permute_unique(mut nums: Vec<i32>) -> Vec<Vec<i32>> {
        nums.sort_unstable();

        // i 表示当前要填排列的第几个数
        fn dfs(i: usize, nums: &[i32], path: &mut Vec<i32>, on_path: &mut Vec<bool>, ans: &mut Vec<Vec<i32>>) {
            if i == nums.len() { // 填完了
                ans.push(path.clone());
                return;
            }

            // 枚举 nums[j] 填入 path[i]
            for j in 0..nums.len() {
                // 如果 nums[j] 已填入排列，continue
                // 如果 nums[j] 和前一个数 nums[j-1] 相等，且 nums[j-1] 没填入排列，continue
                if on_path[j] || j > 0 && nums[j] == nums[j - 1] && !on_path[j - 1] {
                    continue;
                }
                path[i] = nums[j]; // 填入排列
                on_path[j] = true; // nums[j] 已填入排列（注意标记的是下标，不是值）
                dfs(i + 1, nums, path, on_path, ans); // 填排列的下一个数
                on_path[j] = false; // 恢复现场
                // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖 path[i] 就行
            }
        }

        let n = nums.len();
        let mut ans = vec![];
        let mut path = vec![0; n]; // 所有排列的长度都是 n
        let mut on_path = vec![false; n]; // on_path[j] 表示 nums[j] 是否已经填入排列
        dfs(0, &nums, &mut path, &mut on_path, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot n!)$，其中 $n$ 为 $\textit{nums}$ 的长度。分析方法同 [46 题的题解](https://leetcode.cn/problems/permutations/solutions/2079585/hui-su-bu-hui-xie-tao-lu-zai-ci-jing-que-6hrh/)。
- 空间复杂度：$\mathcal{O}(n)$。返回值的空间不计入。

## 专题训练

见下面回溯题单的「**§4.6 有重复元素的回溯**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.6 有重复元素的回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.6 有重复元素的回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
