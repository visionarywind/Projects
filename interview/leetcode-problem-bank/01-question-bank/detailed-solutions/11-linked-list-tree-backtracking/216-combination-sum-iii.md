# 216. 组合总和 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/combination-sum-iii/
- 题目 slug：`combination-sum-iii`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.4 组合型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/combination-sum-iii/solutions/2071013/hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-feme/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频讲解】回溯+剪枝优化（Python/Java/C++/Go）](https://leetcode.cn/problems/combination-sum-iii/solutions/2071013/hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-feme/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-feme`
- topic id：`2071013`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 视频讲解

请看[【基础算法精讲 15】](https://www.bilibili.com/video/BV1xG4y1F7nC/)，制作不易，欢迎点赞关注~

## 方法一：枚举选哪个

**注**：这题如果改成求方案个数（而不是具体方案），就是**恰好装满型 0-1 背包**。

为方便大家与后续 DP 课程内容直接对应上，我们倒着枚举。

```py [sol-Python3]
class Solution:
    def combinationSum3(self, k: int, n: int) -> List[List[int]]:
        ans = []
        path = []

        def dfs(i: int, left_sum: int) -> None:
            d = k - len(path)  # 还要选 d 个数
            if left_sum < 0 or left_sum > (i * 2 - d + 1) * d // 2:  # 剪枝
                return
            if d == 0:  # 找到一个合法组合
                ans.append(path.copy())
                return
            # 枚举的数不能太小，否则后面没有数可以选
            for j in range(i, d - 1, -1):
                path.append(j)
                dfs(j - 1, left_sum - j)
                path.pop()  # 恢复现场

        dfs(9, n)  # 从 i=9 开始倒着枚举
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> combinationSum3(int k, int n) {
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>(k);
        dfs(9, n, k, ans, path); // 从 i=9 开始倒着枚举
        return ans;
    }

    private void dfs(int i, int leftSum, int k, List<List<Integer>> ans, List<Integer> path) {
        int d = k - path.size(); // 还要选 d 个数
        if (leftSum < 0 || leftSum > (i * 2 - d + 1) * d / 2) { // 剪枝
            return;
        }
        if (d == 0) { // 找到一个合法组合
            ans.add(new ArrayList<>(path));
            return;
        }
        // 枚举的数不能太小，否则后面没有数可以选
        for (int j = i; j >= d; j--) {
            path.add(j);
            dfs(j - 1, leftSum - j, k, ans, path);
            path.removeLast(); // path.remove(path.size() - 1);
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> combinationSum3(int k, int n) {
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i, int left_sum) -> void {
            int d = k - path.size(); // 还要选 d 个数
            if (left_sum < 0 || left_sum > (i * 2 - d + 1) * d / 2) { // 剪枝
                return;
            }
            if (d == 0) { // 找到一个合法组合
                ans.emplace_back(path);
                return;
            }
            // 枚举的数不能太小，否则后面没有数可以选
            for (int j = i; j >= d; j--) {
                path.push_back(j);
                dfs(j - 1, left_sum - j);
                path.pop_back(); // 恢复现场
            }
        };

        dfs(9, n); // 从 i=9 开始倒着枚举
        return ans;
    }
};
```

```go [sol-Go]
func combinationSum3(k, n int) (ans [][]int) {
    path := []int{}

    var dfs func(int, int)
    dfs = func(i, leftSum int) {
        d := k - len(path) // 还要选 d 个数
        if leftSum < 0 || leftSum > (i*2-d+1)*d/2 { // 剪枝
            return
        }
        if d == 0 { // 找到一个合法组合
            ans = append(ans, slices.Clone(path))
            return
        }
        // 枚举的数不能太小，否则后面没有数可以选
        for j := i; j >= d; j-- {
            path = append(path, j)
            dfs(j-1, leftSum-j)
            path = path[:len(path)-1] // 恢复现场
        }
    }

    dfs(9, n)
    return
}
```

#### 复杂度分析

- 时间复杂度：分析回溯问题的时间复杂度，有一个简易公式：**路径长度**$\times$**搜索树的叶子数**。对于本题，路径长度始终为 $k$，叶子个数为 $C(9,k)$，所以时间复杂度为 $\mathcal{O}(k\cdot C(9,k))$（去掉剪枝就是 [77. 组合](https://leetcode.cn/problems/combinations/)）。
- 空间复杂度：$\mathcal{O}(k)$。返回值不计入。

## 方法二：选或不选

```py [sol-Python3]
class Solution:
    def combinationSum3(self, k: int, n: int) -> List[List[int]]:
        ans = []
        path = []

        def dfs(i: int, left_sum: int) -> None:
            d = k - len(path)  # 还要选 d 个数
            if left_sum < 0 or left_sum > (i * 2 - d + 1) * d // 2:  # 剪枝
                return
            if d == 0:  # 找到一个合法组合
                ans.append(path.copy())
                return

            # 不选 i
            if i > d:
                dfs(i - 1, left_sum)

            # 选 i
            path.append(i)
            dfs(i - 1, left_sum - i)
            path.pop()

        dfs(9, n)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> combinationSum3(int k, int n) {
        List<List<Integer>> ans = new ArrayList<>();
        List<Integer> path = new ArrayList<>(k);
        dfs(9, n, k, ans, path);
        return ans;
    }

    private void dfs(int i, int leftSum, int k, List<List<Integer>> ans, List<Integer> path) {
        int d = k - path.size(); // 还要选 d 个数
        if (leftSum < 0 || leftSum > (i * 2 - d + 1) * d / 2) { // 剪枝
            return;
        }
        if (d == 0) { // 找到一个合法组合
            ans.add(new ArrayList<>(path));
            return;
        }

        // 不选 i
        if (i > d) {
            dfs(i - 1, leftSum, k, ans, path);
        }

        // 选 i
        path.add(i);
        dfs(i - 1, leftSum - i, k, ans, path);
        path.removeLast(); // path.remove(path.size() - 1);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> combinationSum3(int k, int n) {
        vector<vector<int>> ans;
        vector<int> path;

        auto dfs = [&](this auto&& dfs, int i, int left_sum) -> void {
            int d = k - path.size(); // 还要选 d 个数
            if (left_sum < 0 || left_sum > (i * 2 - d + 1) * d / 2) { // 剪枝
                return;
            }
            if (d == 0) { // 找到一个合法组合
                ans.emplace_back(path);
                return;
            }

            // 不选 i
            if (i > d) {
                dfs(i - 1, left_sum);
            }

            // 选 i
            path.push_back(i);
            dfs(i - 1, left_sum - i);
            path.pop_back();
        };

        dfs(9, n);
        return ans;
    }
};
```

```go [sol-Go]
func combinationSum3(k, n int) (ans [][]int) {
    path := []int{}

    var dfs func(int, int)
    dfs = func(i, leftSum int) {
        d := k - len(path) // 还要选 d 个数
        if leftSum < 0 || leftSum > (i*2-d+1)*d/2 { // 剪枝
            return
        }
        if d == 0 { // 找到一个合法组合
            ans = append(ans, slices.Clone(path))
            return
        }

        // 不选 i
        if i > d {
            dfs(i-1, leftSum)
        }

        // 选 i
        path = append(path, i)
        dfs(i-1, leftSum-i)
        path = path[:len(path)-1]
    }

    dfs(9, n)
    return
}
```

#### 复杂度分析

[见此](https://leetcode.cn/problems/combinations/solutions/1815859/dfs-suan-fa-de-fu-za-du-fen-xi-by-hqztru-14v8/)。

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

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.4 组合型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.4 组合型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
