# 39. 组合总和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/combination-sum/
- 题目 slug：`combination-sum`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.2 子集型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/combination-sum/solutions/2747858/liang-chong-fang-fa-xuan-huo-bu-xuan-mei-mhf9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：选或不选/枚举选哪个/完全背包预处理+可行性剪枝（Python/Java/C++/Go）](https://leetcode.cn/problems/combination-sum/solutions/2747858/liang-chong-fang-fa-xuan-huo-bu-xuan-mei-mhf9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-xuan-huo-bu-xuan-mei-mhf9`
- topic id：`2747858`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 方法一：选或不选

**前置题目**：[78. 子集](https://leetcode.cn/problems/subsets/)

**视频讲解**：[回溯算法套路①子集型回溯【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)

用 $\textit{dfs}(i,\textit{left})$ 来回溯，设当前枚举到 $\textit{candidates}[i]$，剩余要选的元素之和为 $\textit{left}$，按照选或不选分类讨论：

- 不选 $\textit{candidates}[i]$：递归到 $\textit{dfs}(i+1,\textit{left})$。
- 选 $\textit{candidates}[i]$：递归到 $\textit{dfs}(i,\textit{left}-\textit{candidates}[i])$。注意 $i$ 不变，表示在下次递归中可以**继续**选 $\textit{candidates}[i]$。

注：这个思路类似 [完全背包](https://www.bilibili.com/video/BV16Y411v7Y6/)。

如果递归中发现 $\textit{left}=0$ 则说明找到了一个合法组合，复制一份 $\textit{path}$ 加入答案。

递归边界：如果 $i=n$ 或者 $\textit{left} < 0$ 则返回。

递归入口：$\textit{dfs}(0, \textit{target})$。

```py [sol-Python3]
class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        ans = []
        path = []

        def dfs(i: int, left: int) -> None:
            if left == 0:
                # 找到一个合法组合
                ans.append(path.copy())
                return

            if i == len(candidates) or left < 0:
                return

            # 不选
            dfs(i + 1, left)

            # 选
            path.append(candidates[i])
            dfs(i, left - candidates[i])
            path.pop()  # 恢复现场

        dfs(0, target)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> combinationSum(int[] candidates, int target) {
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        dfs(0, target, candidates, ans, path);
        return ans;
    }

    private void dfs(int i, int left, int[] candidates, List<List<Integer>> ans, List<Integer> path) {
        if (left == 0) {
            // 找到一个合法组合
            ans.add(new ArrayList<>(path));
            return;
        }

        if (i == candidates.length || left < 0) {
            return;
        }

        // 不选
        dfs(i + 1, left, candidates, ans, path);

        // 选
        path.add(candidates[i]);
        dfs(i, left - candidates[i], candidates, ans, path);
        path.remove(path.size() - 1); // 恢复现场
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i, int left) {
            if (left == 0) {
                // 找到一个合法组合
                ans.push_back(path);
                return;
            }

            if (i == candidates.size() || left < 0) {
                return;
            }

            // 不选
            dfs(i + 1, left);

            // 选
            path.push_back(candidates[i]);
            dfs(i, left - candidates[i]);
            path.pop_back(); // 恢复现场
        };

        dfs(0, target);
        return ans;
    }
};
```

```go [sol-Go]
func combinationSum(candidates []int, target int) (ans [][]int) {
    path := []int{}
    var dfs func(int, int)
    dfs = func(i, left int) {
        if left == 0 {
            // 找到一个合法组合
            ans = append(ans, slices.Clone(path))
            return
        }

        if i == len(candidates) || left < 0 {
            return
        }

        // 不选
        dfs(i+1, left)

        // 选
        path = append(path, candidates[i])
        dfs(i, left-candidates[i])
        path = path[:len(path)-1] // 恢复现场
    }
    dfs(0, target)
    return ans
}
```

## 剪枝优化

把 $\textit{candidates}$ 从小到大排序，如果递归中发现 $\textit{left} < \textit{candidates}[i]$，由于后面的数字只会更大，所以无法把 $\textit{left}$ 减小到 $0$，可以直接返回。

```py [sol-Python3]
class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        candidates.sort()
        ans = []
        path = []

        def dfs(i: int, left: int) -> None:
            if left == 0:
                # 找到一个合法组合
                ans.append(path.copy())
                return

            if i == len(candidates) or left < candidates[i]:
                return

            # 不选
            dfs(i + 1, left)

            # 选
            path.append(candidates[i])
            dfs(i, left - candidates[i])
            path.pop()  # 恢复现场

        dfs(0, target)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> combinationSum(int[] candidates, int target) {
        Arrays.sort(candidates);
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        dfs(0, target, candidates, ans, path);
        return ans;
    }

    private void dfs(int i, int left, int[] candidates, List<List<Integer>> ans, List<Integer> path) {
        if (left == 0) {
            // 找到一个合法组合
            ans.add(new ArrayList<>(path));
            return;
        }

        if (i == candidates.length || left < candidates[i]) {
            return;
        }

        // 不选
        dfs(i + 1, left, candidates, ans, path);

        // 选
        path.add(candidates[i]);
        dfs(i, left - candidates[i], candidates, ans, path);
        path.remove(path.size() - 1); // 恢复现场
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        ranges::sort(candidates);
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i, int left) {
            if (left == 0) {
                // 找到一个合法组合
                ans.push_back(path);
                return;
            }

            if (i == candidates.size() || left < candidates[i]) {
                return;
            }

            // 不选
            dfs(i + 1, left);

            // 选
            path.push_back(candidates[i]);
            dfs(i, left - candidates[i]);
            path.pop_back(); // 恢复现场
        };

        dfs(0, target);
        return ans;
    }
};
```

```go [sol-Go]
func combinationSum(candidates []int, target int) (ans [][]int) {
    slices.Sort(candidates)
    path := []int{}
    var dfs func(int, int)
    dfs = func(i, left int) {
        if left == 0 {
            // 找到一个合法组合
            ans = append(ans, slices.Clone(path))
            return
        }

        if i == len(candidates) || left < candidates[i] {
            return
        }

        // 不选
        dfs(i+1, left)

        // 选
        path = append(path, candidates[i])
        dfs(i, left-candidates[i])
        path = path[:len(path)-1] // 恢复现场
    }
    dfs(0, target)
    return ans
}
```

#### 复杂度分析

由如下完全背包代码可知，在 $\textit{candidates}=[2,3,4,\cdots, 31],\ \textit{target}=40$ 的极端数据下，搜索次数的上界为 $37271$。换句话说，即使题目不保证答案个数 $\le 150$，我们也能很快地找到所有答案。

```py
f = [1] + [0] * 40
for i in range(2, 32):
    for j in range(i, 41):
        f[j] += f[j - i]
print(sum(f))  # 37271
```

进一步地，计算 [A002865](https://oeis.org/A002865) 的前 $\textit{target}$ 项之和，即 [A000041](https://oeis.org/A000041)，可得：

- 时间复杂度：$\mathcal{O}\left(n\log n + \dfrac{e^{\pi\sqrt {(2 / 3)\cdot\textit{target}}}}{\textit{target}}\right)$，其中 $n$ 为 $\textit{candidates}$ 的长度。如果你想用这个分式估计搜索次数的话，还要乘上 $\dfrac{1}{4\sqrt 3}$ 的常系数。
- 空间复杂度：$\mathcal{O}(\textit{target})$。返回值不计入。$\textit{path}$ 长度和递归深度至多为 $\mathcal{O}(\textit{target})$。

## 方法二：枚举选哪个

类似 [视频](https://www.bilibili.com/video/BV1mG4y1A7Gu/) 中的「答案视角」。同样用 $\textit{dfs}(i,\textit{left})$ 来回溯，设当前枚举到 $\textit{candidates}[i]$，剩余要选的元素之和为 $\textit{left}$，考虑枚举下个元素是谁：

- 在 $[i,n-1]$ 中枚举要填在 $\textit{path}$ 中的元素 $\textit{candidates}[j]$，然后递归到 $\textit{dfs}(j, \textit{left} - \textit{candidates}[j])$。注意这里是递归到 $j$ 不是 $j+1$，表示 $\textit{candidates}[j]$ 可以重复选取。

```py [sol-Python3]
class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        candidates.sort()
        ans = []
        path = []

        def dfs(i: int, left: int) -> None:
            if left == 0:
                # 找到一个合法组合
                ans.append(path.copy())
                return

            # 枚举选哪个
            for j in range(i, len(candidates)):
                if candidates[j] > left:  # 排序了，后面的数都太大
                    break
                path.append(candidates[j])
                dfs(j, left - candidates[j])
                path.pop()  # 恢复现场

        dfs(0, target)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> combinationSum(int[] candidates, int target) {
        Arrays.sort(candidates);
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        dfs(0, target, candidates, ans, path);
        return ans;
    }

    private void dfs(int i, int left, int[] candidates, List<List<Integer>> ans, List<Integer> path) {
        if (left == 0) {
            // 找到一个合法组合
            ans.add(new ArrayList<>(path));
            return;
        }

        // 枚举选哪个
        for (int j = i; j < candidates.length && candidates[j] <= left; j++) {
            path.add(candidates[j]);
            dfs(j, left - candidates[j], candidates, ans, path);
            path.remove(path.size() - 1); // 恢复现场
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        ranges::sort(candidates);
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i, int left) {
            if (left == 0) {
                // 找到一个合法组合
                ans.push_back(path);
                return;
            }

            // 枚举选哪个
            for (int j = i; j < candidates.size() && candidates[j] <= left; j++) {
                path.push_back(candidates[j]);
                dfs(j, left - candidates[j]);
                path.pop_back(); // 恢复现场
            }
        };

        dfs(0, target);
        return ans;
    }
};
```

```go [sol-Go]
func combinationSum(candidates []int, target int) (ans [][]int) {
    slices.Sort(candidates)
    path := []int{}
    var dfs func(int, int)
    dfs = func(i, left int) {
        if left == 0 {
            // 找到一个合法组合
            ans = append(ans, slices.Clone(path))
            return
        }

        // 枚举选哪个
        for j := i; j < len(candidates) && candidates[j] <= left; j++ {
            path = append(path, candidates[j])
            dfs(j, left-candidates[j])
            path = path[:len(path)-1] // 恢复现场
        }
    }
    dfs(0, target)
    return ans
}
```

#### 复杂度分析

同方法一。

## 方法三：完全背包预处理 + 可行性剪枝

**前置知识**：[完全背包](https://www.bilibili.com/video/BV16Y411v7Y6/)。

例如 $\textit{candidates}=[2,4,6,8,10]$ 都是偶数，但 $\textit{target}=11$ 是奇数，这种情况我们在一开始递归时，就应当判断出无解，不再继续向下递归。

怎么判断？我们可以用**完全背包**预处理出下标在 $[0,i]$ 中的 $\textit{candidates}$ 元素之和能否为 $j$，记作 $f[i+1][j]$。

如果递归中的 $\textit{left}$ 不在可以组合得到的数字中，则可以直接返回。

这一做法可以保证我们是在往正确的方向一步步递归前进的。只要题目保证方案数不超过 $150$，即使 $\textit{target}=1000$ 也能搞定。

```py [sol-Python3]
class Solution:
    def combinationSum(self, candidates: List[int], target: int) -> List[List[int]]:
        n = len(candidates)
        # 完全背包
        f = [[False] * (target + 1) for _ in range(n + 1)]
        f[0][0] = True
        for i, x in enumerate(candidates):
            for j in range(target + 1):
                f[i + 1][j] = f[i][j] or j >= x and f[i + 1][j - x]

        ans = []
        path = []

        def dfs(i: int, left: int) -> None:
            if left == 0:
                # 找到一个合法组合
                ans.append(path.copy())
                return

            # 无法用下标在 [0, i] 中的数字组合出 left
            if left < 0 or not f[i + 1][left]:
                return

            # 不选
            dfs(i - 1, left)

            # 选
            path.append(candidates[i])
            dfs(i, left - candidates[i])
            path.pop()

        # 倒着递归，这样参数符合 f 数组的定义
        dfs(n - 1, target)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> combinationSum(int[] candidates, int target) {
        int n = candidates.length;
        // 完全背包
        boolean[][] f = new boolean[n + 1][target + 1];
        f[0][0] = true;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= target; j++) {
                f[i + 1][j] = f[i][j] || j >= candidates[i] && f[i + 1][j - candidates[i]];
            }
        }

        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>();
        // 倒着递归，这样参数符合 f 数组的定义
        dfs(n - 1, target, candidates, f, ans, path);
        return ans;
    }

    private void dfs(int i, int left, int[] candidates, boolean[][] f, List<List<Integer>> ans, List<Integer> path) {
        if (left == 0) {
            // 找到一个合法组合
            ans.add(new ArrayList<>(path));
            return;
        }

        // 无法用下标在 [0, i] 中的数字组合出 left
        if (left < 0 || !f[i + 1][left]) {
            return;
        }

        // 不选
        dfs(i - 1, left, candidates, f, ans, path);

        // 选
        path.add(candidates[i]);
        dfs(i, left - candidates[i], candidates, f, ans, path);
        path.remove(path.size() - 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        int n = candidates.size();
        // 完全背包
        vector<vector<bool>> f(n + 1, vector<bool>(target + 1));
        f[0][0] = true;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= target; j++) {
                f[i + 1][j] = f[i][j] || j >= candidates[i] && f[i + 1][j - candidates[i]];
            }
        }

        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i, int left) {
            if (left == 0) {
                // 找到一个合法组合
                ans.push_back(path);
                return;
            }

            // 无法用下标在 [0, i] 中的数字组合出 left
            if (left < 0 || !f[i + 1][left]) {
                return;
            }

            // 不选
            dfs(i - 1, left);

            // 选
            path.push_back(candidates[i]);
            dfs(i, left - candidates[i]);
            path.pop_back();
        };

        // 倒着递归，这样参数符合 f 数组的定义
        dfs(n - 1, target);
        return ans;
    }
};
```

```go [sol-Go]
func combinationSum(candidates []int, target int) (ans [][]int) {
    n := len(candidates)
    // 完全背包
    f := make([][]bool, n+1)
    f[0] = make([]bool, target+1)
    f[0][0] = true
    for i, x := range candidates {
        f[i+1] = make([]bool, target+1)
        for j, b := range f[i] {
            f[i+1][j] = b || j >= x && f[i+1][j-x]
        }
    }

    path := []int{}
    var dfs func(int, int)
    dfs = func(i, left int) {
        if left == 0 {
            // 找到一个合法组合
            ans = append(ans, slices.Clone(path))
            return
        }

        // 无法用下标在 [0, i] 中的数字组合出 left
        if left < 0 || !f[i+1][left] {
            return
        }

        // 不选
        dfs(i-1, left)

        // 选
        path = append(path, candidates[i])
        dfs(i, left-candidates[i])
        path = path[:len(path)-1]
    }

    // 倒着递归，这样参数符合 f 数组的定义
    dfs(n-1, target)
    return ans
}
```

- 时间复杂度：$\mathcal{O}\left(\min\left(\dfrac{e^{\pi\sqrt {(2 / 3)\cdot\textit{target}}}}{\textit{target}},\ k\cdot \textit{target}\right) + n\cdot \textit{target}\right)$。其中 $n$ 为 $\textit{candidates}$ 的长度，$k\le 150$ 这是题目保证的。搜索树上至多有 $k$ 条长为 $\mathcal{O}(target)$ 的链，所以搜索树的节点个数为 $\mathcal{O}(k\cdot \textit{target})$。计算完全背包的时间为 $\mathcal{O}(n\cdot \textit{target})$。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{target})$。返回值不计入。

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

本题来自 `四、回溯 / §4.2 子集型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.2 子集型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
