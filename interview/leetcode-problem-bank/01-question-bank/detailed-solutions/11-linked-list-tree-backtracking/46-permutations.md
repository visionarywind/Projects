# 46. 全排列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/permutations/
- 题目 slug：`permutations`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.5 排列型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/permutations/solutions/2079585/hui-su-bu-hui-xie-tao-lu-zai-ci-jing-que-6hrh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】回溯不会写？套路在此！精确计算搜索树的节点个数！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/permutations/solutions/2079585/hui-su-bu-hui-xie-tao-lu-zai-ci-jing-que-6hrh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hui-su-bu-hui-xie-tao-lu-zai-ci-jing-que-6hrh`
- topic id：`2079585`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 视频讲解

请看 [回溯算法套路③排列型回溯【基础算法精讲 16】](https://www.bilibili.com/video/BV1mY411D7f6/)。制作不易，欢迎点赞~

```py [sol-Python3]
class Solution:
    def permute(self, nums: List[int]) -> List[List[int]]:
        n = len(nums)
        path = [0] * n  # 所有排列的长度都是一样的 n
        on_path = [False] * n
        ans = []

        # 枚举 path[i] 填 nums 的哪个数
        def dfs(i: int) -> None:
            if i == n:
                ans.append(path.copy())  # 也可以写 path[:]
                return
            for j, on in enumerate(on_path):
                if not on:
                    path[i] = nums[j]  # 从没有选的数字中选一个
                    on_path[j] = True  # 已选上
                    dfs(i + 1)
                    on_path[j] = False  # 恢复现场
                    # 注意 path 无需恢复现场，因为排列长度固定，直接覆盖就行

        dfs(0)
        return ans
```

```py [sol-Python3 set]
class Solution:
    def permute(self, nums: List[int]) -> List[List[int]]:
        n = len(nums)
        path = [0] * n
        ans = []

        # 枚举 path[i] 填 remain（剩余数字）中的哪个数
        def dfs(i: int, remain: Set[int]) -> None:
            if i == n:
                ans.append(path.copy())
                return

            for x in remain:
                path[i] = x
                dfs(i + 1, remain - {x})

        dfs(0, set(nums))
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> permute(int[] nums) {
        int n = nums.length;
        List<Integer> path = Arrays.asList(new Integer[n]); // 所有排列的长度都是一样的 n
        boolean[] onPath = new boolean[n];
        List<List<Integer>> ans = new ArrayList<>();

        dfs(0, nums, ans, path, onPath);
        return ans;
    }

    // 枚举 path[i] 填 nums 的哪个数
    private void dfs(int i, int[] nums, List<List<Integer>> ans, List<Integer> path, boolean[] onPath) {
        if (i == nums.length) {
            ans.add(new ArrayList<>(path));
            return;
        }

        for (int j = 0; j < nums.length; j++) {
            if (!onPath[j]) {
                path.set(i, nums[j]); // 从没有选的数字中选一个
                onPath[j] = true; // 已选上
                dfs(i + 1, nums, ans, path, onPath);
                onPath[j] = false; // 恢复现场
                // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖就行
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> permute(vector<int> &nums) {
        int n = nums.size();
        vector<int> path(n); // 所有排列的长度都是一样的 n
        vector<int8_t> on_path(n);
        vector<vector<int>> ans;

        // 枚举 path[i] 填 nums 的哪个数
        auto dfs = [&](this auto&& dfs, int i) -> void {
            if (i == n) {
                ans.emplace_back(path);
                return;
            }

            for (int j = 0; j < n; j++) {
                if (!on_path[j]) {
                    path[i] = nums[j]; // 从没有选的数字中选一个
                    on_path[j] = true; // 已选上
                    dfs(i + 1);
                    on_path[j] = false; // 恢复现场
                    // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖就行
                }
            }
        };

        dfs(0);
        return ans;
    }
};
```

```c [sol-C]
int** permute(int* nums, int n, int* returnSize, int** returnColumnSizes) {
    // 计算 n!
    int ansSize = 1;
    for (int i = 2; i <= n; i++) {
        ansSize *= i;
    }

    int** ans = malloc(ansSize * sizeof(int*));
    *returnColumnSizes = malloc(ansSize * sizeof(int));
    *returnSize = 0;

    int* path = malloc(n * sizeof(int));
    bool* on_path = calloc(n, sizeof(bool)); // 所有排列的长度都是一样的 n

    // 枚举 path[i] 填什么数字
    void dfs(int i) {
        if (i == n) {
            ans[*returnSize] = malloc(n * sizeof(int));
            memcpy(ans[*returnSize], path, n * sizeof(int));
            (*returnColumnSizes)[*returnSize] = n;
            (*returnSize)++;
            return;
        }

        for (int j = 0; j < n; j++) {
            if (!on_path[j]) {
                path[i] = nums[j]; // 从没有选的数字中选一个
                on_path[j] = true; // 已选上
                dfs(i + 1);
                on_path[j] = false; // 恢复现场
                // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖就行
            }
        }
    }

    dfs(0);

    free(path);
    free(on_path);
    return ans;
}
```

```go [sol-Go]
func permute(nums []int) (ans [][]int) {
    n := len(nums)
    path := make([]int, n)
    onPath := make([]bool, n)

    // 枚举 path[i] 填 nums 的哪个数
    var dfs func(int)
    dfs = func(i int) {
        if i == n {
            ans = append(ans, append([]int(nil), path...))
            return
        }
        for j, on := range onPath {
            if !on {
                path[i] = nums[j]
                onPath[j] = true
                dfs(i + 1)
                onPath[j] = false
            }
        }
    }

    dfs(0)
    return
}
```

```js [sol-JavaScript]
var permute = function(nums) {
    const n = nums.length;
    const path = Array(n).fill(0);
    const onPath = Array(n).fill(false); // 所有排列的长度都是一样的 n
    const ans = [];

    // 枚举 path[i] 填 nums 的哪个数
    function dfs(i) {
        if (i === n) {
            ans.push(path.slice());
            return;
        }

        for (let j = 0; j < n; j++) {
            if (!onPath[j]) {
                path[i] = nums[j]; // 从没有选的数字中选一个
                onPath[j] = true; // 已选上
                dfs(i + 1);
                onPath[j] = false; // 恢复现场
                // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖就行
            }
        }
    };

    dfs(0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn permute(nums: Vec<i32>) -> Vec<Vec<i32>> {
        // 枚举 path[i] 填 nums 的哪个数
        fn dfs(i: usize, nums: &[i32], ans: &mut Vec<Vec<i32>>, path: &mut Vec<i32>, on_path: &mut [bool]) {
            if i == nums.len() {
                ans.push(path.clone());
                return;
            }

            for j in 0..nums.len() {
                if !on_path[j] {
                    path[i] = nums[j]; // 从没有选的数字中选一个
                    on_path[j] = true; // 已选上
                    dfs(i + 1, nums, ans, path, on_path);
                    on_path[j] = false; // 恢复现场
                    // 注意 path 无需恢复现场，因为排列长度固定，直接覆盖就行
                }
            }
        }

        let n = nums.len();
        let mut path = vec![0; n];
        let mut on_path = vec![false; n]; // 所有排列的长度都是一样的 n
        let mut ans = vec![];

        dfs(0, &nums, &mut ans, &mut path, &mut on_path);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot n!)$，其中 $n$ 为 $\textit{nums}$ 的长度。视频中提到，搜索树中的节点个数低于 $3\cdot n!$。实际上，精确值为 $\left\lfloor e\cdot n!\right\rfloor$，其中 $e=2.718\cdots$ 为自然常数。有 $\mathcal{O}(n!)$ 个叶节点，每个叶节点花费 $\mathcal{O}(n)$ 的时间复制 $\textit{path}$ 数组，因此时间复杂度为 $\mathcal{O}(n\cdot n!)$。
- 空间复杂度：$\mathcal{O}(n)$。返回值的空间不计入。

## 专题训练

见下面回溯题单的「**§4.5 排列型回溯**」。

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

本题来自 `四、回溯 / §4.5 排列型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.5 排列型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
