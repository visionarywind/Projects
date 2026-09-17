# 90. 子集 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subsets-ii/
- 题目 slug：`subsets-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.6 有重复元素的回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/subsets-ii/solutions/3036436/liang-chong-fang-fa-xuan-huo-bu-xuan-mei-v0js/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：选或不选/枚举选哪个（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/subsets-ii/solutions/3036436/liang-chong-fang-fa-xuan-huo-bu-xuan-mei-v0js/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-xuan-huo-bu-xuan-mei-v0js`
- topic id：`3036436`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

**前置题目**：请先完成没有重复元素的版本 [78. 子集](https://leetcode.cn/problems/subsets/)。

**视频讲解**：[回溯算法套路①子集型回溯【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)。

## 方法一：选或不选

和 78 题一样，用「选或不选」解决。需要注意的是，在不选 $\textit{nums}[i]$ 时，要**跳过后续所有等于 $\textit{nums}[i]$ 的数**。如果不跳过这些数，比如 $\textit{nums}[i] = \textit{nums}[i+1] = 2$，那么「选第一个 $2$ 不选第二个 $2$」和「不选第一个 $2$ 选第二个 $2$」这两种情况都相当于「选了一个 $2$」，这就重复了。

为了方便跳过相同元素，在递归前，把 $\textit{nums}$ 排序（从小到大或者从大到小都可以）。

```py [sol-Python3]
class Solution:
    def subsetsWithDup(self, nums: List[int]) -> List[List[int]]:
        nums.sort()
        n = len(nums)
        ans = []
        path = []

        def dfs(i: int) -> None:
            if i == n:
                ans.append(path.copy())  # 也可以写 path[:]
                return

            # 选 x
            x = nums[i]
            path.append(x)
            dfs(i + 1)
            path.pop()  # 恢复现场

            # 不选 x，那么后面所有等于 x 的数都不选
            # 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
            i += 1
            while i < n and nums[i] == x:
                i += 1
            dfs(i)

        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> subsetsWithDup(int[] nums) {
        Arrays.sort(nums);
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        dfs(0, nums, ans, path);
        return ans;
    }

    private void dfs(int i, int[] nums, List<List<Integer>> ans, List<Integer> path) {
        int n = nums.length;
        if (i == n) {
            ans.add(new ArrayList<>(path));
            return;
        }

        // 选 x
        int x = nums[i];
        path.add(x);
        dfs(i + 1, nums, ans, path);
        path.remove(path.size() - 1); // 恢复现场

        // 不选 x，那么后面所有等于 x 的数都不选
        // 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
        i++;
        while (i < n && nums[i] == x) {
            i++;
        }
        dfs(i, nums, ans, path);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size();
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i) -> void {
            if (i == n) {
                ans.push_back(path);
                return;
            }

            // 选 x
            int x = nums[i];
            path.push_back(x);
            dfs(i + 1);
            path.pop_back(); // 恢复现场

            // 不选 x，那么后面所有等于 x 的数都不选
            // 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
            i++;
            while (i < n && nums[i] == x) {
                i++;
            }
            dfs(i);
        };

        dfs(0);
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int** subsetsWithDup(int* nums, int n, int* returnSize, int** columnSizes) {
    qsort(nums, n, sizeof(int), cmp);

    int m = 1 << n; // 至多有 2^n 个子集
    int** ans = malloc(m * sizeof(int*));
    *columnSizes = malloc(m * sizeof(int));
    *returnSize = 0;
    int* path = malloc(n * sizeof(int));

    void dfs(int i, int pathSize) {
        if (i == n) {
            ans[*returnSize] = malloc(pathSize * sizeof(int));
            memcpy(ans[*returnSize], path, pathSize * sizeof(int));
            (*columnSizes)[(*returnSize)++] = pathSize;
            return;
        }
    
        // 选 x
        int x = nums[i];
        path[pathSize] = x;
        dfs(i + 1, pathSize + 1);
    
        // 不选 x，那么后面所有等于 x 的数都不选
        // 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
        i++;
        while (i < n && nums[i] == x) {
            i++;
        }
        dfs(i, pathSize);
    }
    
    dfs(0, 0);

    free(path);
    return ans;
}
```

```go [sol-Go]
func subsetsWithDup(nums []int) (ans [][]int) {
    slices.Sort(nums)
    n := len(nums)
    path := []int{}
    var dfs func(int)
    dfs = func(i int) {
        if i == n {
            ans = append(ans, slices.Clone(path))
            return
        }

        // 选 x
        x := nums[i]
        path = append(path, x)
        dfs(i + 1)
        path = path[:len(path)-1] // 恢复现场

        // 不选 x，那么后面所有等于 x 的数都不选
        // 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
        i++
        for i < n && nums[i] == x {
            i++
        }
        dfs(i)
    }
    dfs(0)
    return
}
```

```js [sol-JavaScript]
var subsetsWithDup = function(nums) {
    nums.sort((a, b) => a - b);
    const n = nums.length;
    const ans = [];
    const path = [];

    function dfs(i) {
        if (i === n) {
            ans.push([...path]);
            return;
        }

        // 选 x
        const x = nums[i];
        path.push(x);
        dfs(i + 1);
        path.pop(); // 恢复现场

        // 不选 x，那么后面所有等于 x 的数都不选
        // 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
        i++;
        while (i < n && nums[i] === x) {
            i++;
        }
        dfs(i);
    }

    dfs(0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn subsets_with_dup(mut nums: Vec<i32>) -> Vec<Vec<i32>> {
        nums.sort_unstable();

        fn dfs(mut i: usize, nums: &[i32], ans: &mut Vec<Vec<i32>>, path: &mut Vec<i32>) {
            if i == nums.len() {
                ans.push(path.clone());
                return;
            }

            // 选 x
            let x = nums[i];
            path.push(x);
            dfs(i + 1, nums, ans, path);
            path.pop(); // 恢复现场

            // 不选 x，那么后面所有等于 x 的数都不选
            // 如果不跳过这些数，会导致「选 x 不选 x'」和「不选 x 选 x'」这两种情况都会加到 ans 中，这就重复了
            i += 1;
            while i < nums.len() && nums[i] == x {
                i += 1;
            }
            dfs(i, nums, ans, path);
        }

        let mut ans = vec![];
        let mut path = vec![];
        dfs(0, &nums, &mut ans, &mut path);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n2^n)$，其中 $n$ 为 $\textit{nums}$ 的长度。如果所有元素互不相同，那么每次都是选或不选，递归次数为一个满二叉树的节点个数，一共会递归 $\mathcal{O}(2^n)$ 次（等比数列和）。由于 $n$ 个数可以组成 $2^n$ 个子集，所以至多有 $2^n$ 个叶结点，再算上加入答案时复制 $\textit{path}$ 需要 $\mathcal{O}(n)$ 的时间，所以时间复杂度为 $\mathcal{O}(n2^n)$。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 方法二：枚举选哪个

在 $[i,n-1]$ 中枚举要加到 $\textit{path}$ 中的数 $\textit{nums}[j]$，然后递归到 $\textit{dfs}(j+1)$。注意，如果 $j>i$，说明我们没选 $\textit{nums}[j-1]$，那么根据方法一的跳过规则，如果此时 $\textit{nums}[j]=\textit{nums}[j-1]$，则跳过，枚举下一个 $j$。

比如考虑第一个数选谁，选 $\textit{nums}[0]$ 还是 $\textit{nums}[1]$ 还是 $\textit{nums}[2]$ 还是 ……，如果我们第一个数选了 $\textit{nums}[1]$，这间接说明我们没有选 $\textit{nums}[0]$。

注意不需要判断 $i=n$，因为此时一定不会进入循环。

```py [sol-Python3]
class Solution:
    def subsetsWithDup(self, nums: List[int]) -> List[List[int]]:
        nums.sort()
        n = len(nums)
        ans = []
        path = []

        def dfs(i: int) -> None:
            ans.append(path.copy())  # 也可以写 path[:]

            # 在 [i,n-1] 中选一个 nums[j]
            # 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
            for j in range(i, n):
                # 如果 j>i，说明 nums[j-1] 没有选
                # 同方法一，所有等于 nums[j-1] 的数都不选
                if j > i and nums[j] == nums[j - 1]:
                    continue
                path.append(nums[j])
                dfs(j + 1)
                path.pop()  # 恢复现场

        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> subsetsWithDup(int[] nums) {
        Arrays.sort(nums);
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        dfs(0, nums, ans, path);
        return ans;
    }

    private void dfs(int i, int[] nums, List<List<Integer>> ans, List<Integer> path) {
        ans.add(new ArrayList<>(path));

        // 在 [i,n-1] 中选一个 nums[j]
        // 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
        for (int j = i; j < nums.length; j++) {
            // 如果 j>i，说明 nums[j-1] 没有选
            // 同方法一，所有等于 nums[j-1] 的数都不选
            if (j > i && nums[j] == nums[j - 1]) {
                continue;
            }
            path.add(nums[j]);
            dfs(j + 1, nums, ans, path);
            path.remove(path.size() - 1); // 恢复现场
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size();
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i) -> void {
            ans.push_back(path);

            // 在 [i,n-1] 中选一个 nums[j]
            // 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
            for (int j = i; j < n; j++) {
                // 如果 j>i，说明 nums[j-1] 没有选
                // 同方法一，所有等于 nums[j-1] 的数都不选
                if (j > i && nums[j] == nums[j - 1]) {
                    continue;
                }
                path.push_back(nums[j]);
                dfs(j + 1);
                path.pop_back(); // 恢复现场
            }
        };

        dfs(0);
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int** subsetsWithDup(int* nums, int n, int* returnSize, int** columnSizes) {
    qsort(nums, n, sizeof(int), cmp);

    int m = 1 << n; // 至多有 2^n 个子集
    int** ans = malloc(m * sizeof(int*));
    *columnSizes = malloc(m * sizeof(int));
    *returnSize = 0;
    int* path = malloc(n * sizeof(int));
    
    void dfs(int i, int pathSize) {
        ans[*returnSize] = malloc(pathSize * sizeof(int));
        memcpy(ans[*returnSize], path, pathSize * sizeof(int));
        (*columnSizes)[(*returnSize)++] = pathSize;
    
        // 在 [i,n-1] 中选一个 nums[j]
        // 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
        for (int j = i; j < n; j++) {
            // 如果 j>i，说明 nums[j-1] 没有选
            // 同方法一，所有等于 nums[j-1] 的数都不选
            if (j > i && nums[j] == nums[j - 1]) {
                continue;
            }
            path[pathSize] = nums[j];
            dfs(j + 1, pathSize + 1);
        }
    }

    dfs(0, 0);

    free(path);
    return ans;
}
```

```go [sol-Go]
func subsetsWithDup(nums []int) (ans [][]int) {
    slices.Sort(nums)
    n := len(nums)
    path := []int{}
    var dfs func(int)
    dfs = func(i int) {
        ans = append(ans, slices.Clone(path))

        // 在 [i,n-1] 中选一个 nums[j]
        // 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
        for j := i; j < n; j++ {
            // 如果 j>i，说明 nums[j-1] 没有选
            // 同方法一，所有等于 nums[j-1] 的数都不选
            if j > i && nums[j] == nums[j-1] {
                continue
            }
            path = append(path, nums[j])
            dfs(j + 1)
            path = path[:len(path)-1] // 恢复现场
        }
    }
    dfs(0)
    return
}
```

```js [sol-JavaScript]
var subsetsWithDup = function(nums) {
    nums.sort((a, b) => a - b);
    const n = nums.length;
    const ans = [];
    const path = [];

    function dfs(i) {
        ans.push([...path]);

        // 在 [i,n-1] 中选一个 nums[j]
        // 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
        for (let j = i; j < n; j++) {
            // 如果 j>i，说明 nums[j-1] 没有选
            // 同方法一，所有等于 nums[j-1] 的数都不选
            if (j > i && nums[j] === nums[j - 1]) {
                continue;
            }
            path.push(nums[j]);
            dfs(j + 1);
            path.pop(); // 恢复现场
        }
    }

    dfs(0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn subsets_with_dup(mut nums: Vec<i32>) -> Vec<Vec<i32>> {
        nums.sort_unstable();

        fn dfs(i: usize, nums: &[i32], ans: &mut Vec<Vec<i32>>, path: &mut Vec<i32>) {
            ans.push(path.clone());

            // 在 [i,n-1] 中选一个 nums[j]
            // 注意选 nums[j] 意味着 [i,j-1] 中的数都没有选
            for j in i..nums.len() {
                // 如果 j>i，说明 nums[j-1] 没有选
                // 同方法一，所有等于 nums[j-1] 的数都不选
                if j > i && nums[j] == nums[j - 1] {
                    continue;
                }
                path.push(nums[j]);
                dfs(j + 1, nums, ans, path);
                path.pop(); // 恢复现场
            }
        }

        let mut ans = vec![];
        let mut path = vec![];
        dfs(0, &nums, &mut ans, &mut path);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n2^n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

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
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
