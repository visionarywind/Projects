# 377. 组合总和 Ⅳ

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/combination-sum-iv/
- 题目 slug：`combination-sum-iv`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.1 爬楼梯
- 难度分：1600
- 外部题解来源：https://leetcode.cn/problems/combination-sum-iv/solutions/2706336/ben-zhi-shi-pa-lou-ti-cong-ji-yi-hua-sou-y52j/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[本质是爬楼梯：记忆化搜索 / 递推（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/combination-sum-iv/solutions/2706336/ben-zhi-shi-pa-lou-ti-cong-ji-yi-hua-sou-y52j/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-zhi-shi-pa-lou-ti-cong-ji-yi-hua-sou-y52j`
- topic id：`2706336`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本质上是 [70. 爬楼梯](https://leetcode.cn/problems/climbing-stairs/)，每次从 $\textit{nums}$ 中选一个数，作为往上爬的台阶数，计算爬 $\textit{target}$ 个台阶有多少种方案。70 那题相当于 $\textit{nums}=[1,2]$，因为每次只能爬 $1$ 个或 $2$ 个台阶。

## 一、递归搜索 + 保存递归返回值 = 记忆化搜索

和爬楼梯一样，定义 $\textit{dfs}(i)$ 表示爬 $i$ 个台阶的方案数。

考虑最后一步爬了 $x=\textit{nums}[j]$ 个台阶，那么问题变成爬 $i-x$ 个台阶的方案数，即 $\textit{dfs}(i-x)$。

所以有

$$
\textit{dfs}(i) = \sum_{j=0}^{n-1} \textit{dfs}(i-\textit{nums}[j])
$$

注：如果 $\textit{nums}[j] > i$ 则跳过。 

回顾一下，70 那题可以看作 $\textit{nums}=[1,2]$，有

$$
\textit{dfs}(i) = \textit{dfs}(i-1) + \textit{dfs}(i-2)
$$

递归边界：$\textit{dfs}(0)=1$。爬 $0$ 个台阶的方案数是 $1$。也可以这样理解，我们从 $\textit{target}$ 开始往下爬，刚好爬到底部（递归边界）此时就找到了一个合法的方案，返回 $1$。

递归入口：$\textit{dfs}(\textit{target})$，也就是答案。

按照视频中讲的，用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def combinationSum4(self, nums: List[int], target: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            if i == 0:  # 爬完了
                return 1
            return sum(dfs(i - x) for x in nums if x <= i)  # 枚举所有可以爬的台阶数
        return dfs(target)
```

```java [sol-Java]
class Solution {
    public int combinationSum4(int[] nums, int target) {
        int[] memo = new int[target + 1];
        Arrays.fill(memo, -1); // -1 表示没有计算过
        return dfs(target, nums, memo);
    }

    private int dfs(int i, int[] nums, int[] memo) {
        if (i == 0) { // 爬完了
            return 1;
        }
        if (memo[i] != -1) { // 之前计算过
            return memo[i];
        }
        int res = 0;
        for (int x : nums) { // 枚举所有可以爬的台阶数
            if (x <= i) {
                res += dfs(i - x, nums, memo);
            }
        }
        return memo[i] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int combinationSum4(vector<int>& nums, int target) {
        vector<int> memo(target + 1, -1); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i) {
            if (i == 0) { // 爬完了
                return 1;
            }
            int &res = memo[i]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = 0;
            for (int x : nums) {
                if (x <= i) {
                    res += dfs(i - x);
                }
            }
            return res;
        };
        return dfs(target);
    }
};
```

```c [sol-C]
int dfs(int i, int* nums, int numsSize, int* memo) {
    if (i == 0) { // 爬完了
        return 1;
    }
    if (memo[i] != -1) { // 之前计算过
        return memo[i];
    }
    int res = 0;
    for (int j = 0; j < numsSize; j++) { // 枚举所有可以爬的台阶数
        int x = nums[j];
        if (x <= i) {
            res += dfs(i - x, nums, numsSize, memo);
        }
    }
    return memo[i] = res; // 记忆化
}

int combinationSum4(int* nums, int numsSize, int target) {
    int* memo = malloc((target + 1) * sizeof(int));
    memset(memo, -1, (target + 1) * sizeof(int)); // -1 表示没有计算过
    int ans = dfs(target, nums, numsSize, memo);
    free(memo);
    return ans;
}
```

```go [sol-Go]
func combinationSum4(nums []int, target int) int {
    memo := make([]int, target+1)
    for i := range memo {
        memo[i] = -1 // -1 表示没有计算过
    }
    var dfs func(int) int
    dfs = func(i int) (res int) {
        if i == 0 { // 爬完了
            return 1
        }
        p := &memo[i]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        for _, x := range nums { // 枚举所有可以爬的台阶数
            if x <= i {
                res += dfs(i - x)
            }
        }
        return
    }
    return dfs(target)
}
```

```js [sol-JavaScript]
var combinationSum4 = function(nums, target) {
    const memo = Array(target + 1).fill(-1); // -1 表示没有计算过
    function dfs(i) {
        if (i === 0) { // 爬完了
            return 1;
        }
        if (memo[i] !== -1) { // 之前计算过
            return memo[i];
        }
        let res = 0;
        for (const x of nums) { // 枚举所有可以爬的台阶数
            if (x <= i) {
                res += dfs(i - x);
            }
        }
        return memo[i] = res; // 记忆化
    }
    return dfs(target);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn combination_sum4(nums: Vec<i32>, target: i32) -> i32 {
        fn dfs(i: usize, nums: &[i32], memo: &mut [i32]) -> i32 {
            if i == 0 { // 爬完了
                return 1;
            }
            if memo[i] != -1 { // 之前计算过
                return memo[i];
            }
            let mut res = 0;
            for &x in nums { // 枚举所有可以爬的台阶数
                let x = x as usize;
                if x <= i {
                    res += dfs(i - x, nums, memo);
                }
            }
            memo[i] = res; // 记忆化
            res
        }
        let t = target as usize;
        let mut memo = vec![-1; t + 1]; // -1 表示没有计算过
        dfs(t, &nums, &mut memo)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\textit{target}\cdot n)$，其中 $n$ 为 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(\textit{target})$，单个状态的计算时间为 $\mathcal{O}(n)$，所以动态规划的时间复杂度为 $\mathcal{O}(\textit{target}\cdot n)$。
- 空间复杂度：$\mathcal{O}(\textit{target})$。有多少个状态，$\textit{memo}$ 数组的大小就是多少。

## 二、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是一样的，都表示表示爬 $i$ 个台阶的方案数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \sum_{j=0}^{n-1} f[i-\textit{nums}[j]]
$$

注：如果 $\textit{nums}[j] > i$ 则跳过。

回顾一下，70 那题可以看作 $\textit{nums}=[1,2]$，状态转移方程为

$$
f[i] = f[i-1] + f[i-2]
$$

初始值 $f[0]=1$，翻译自递归边界 $\textit{dfs}(0)=1$。

答案为 $f[\textit{target}]$，翻译自递归入口 $\textit{dfs}(\textit{target})$。

### 答疑

**问**：我能理解 $\textit{dfs}(0)=1$，但我无法直接理解 $f[0]=1$，这里的 $1$ 是什么意思？

**答**：用爬楼梯理解的话，就是「原地不动也算一种方案」的意思。用原始题意理解的话，就是「什么也不选，元素和为 $0$，也算一种方案」的意思。好比让你去拿一个装着 $i$ 升水的瓶子，如果 $i=0$，就表示拿一个空瓶子。另一个角度是，如果初始化 $f[0]=0$ 的话，看代码，我们计算的每个 $f[i]$ 都会是 $0$，这显然是错误的。

**问**：这个代码和完全背包（例如 [518. 零钱兑换 II](https://leetcode.cn/problems/coin-change-ii/)）的代码很像，二者区别在哪？

**答**：本题计算的是**排列**，完全背包计算的是**组合**。比如 $1,2$ 和 $2,1$ 这两个排列，在本题是有区别的，是两种方案；但在完全背包中这两个排列没有区别，只算一种方案。从代码上看，计算排列（本题）需要外层循环枚举体积，内层循环枚举物品；计算组合（完全背包）需要外层循环枚举物品，内层循环枚举体积。

```py [sol-Python3]
class Solution:
    def combinationSum4(self, nums: List[int], target: int) -> int:
        f = [1] + [0] * target
        for i in range(1, target + 1):
            f[i] = sum(f[i - x] for x in nums if x <= i)
        return f[target]
```

```java [sol-Java]
class Solution {
    public int combinationSum4(int[] nums, int target) {
        int[] f = new int[target + 1];
        f[0] = 1;
        for (int i = 1; i <= target; i++) {
            for (int x : nums) {
                if (x <= i) {
                    f[i] += f[i - x];
                }
            }
        }
        return f[target];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int combinationSum4(vector<int>& nums, int target) {
        // 使用 unsigned 可以让溢出不报错
        // 对于溢出的数据，不会影响答案的正确性（题目保证）
        vector<unsigned> f(target + 1);
        f[0] = 1;
        for (int i = 1; i <= target; i++) {
            for (int x : nums) {
                if (x <= i) {
                    f[i] += f[i - x];
                }
            }
        }
        return f[target];
    }
};
```

```c [sol-C]
int combinationSum4(int* nums, int numsSize, int target) {
    // 使用 unsigned 可以让溢出不报错
    // 对于溢出的数据，不会影响答案的正确性（题目保证）
    unsigned* f = calloc(target + 1, sizeof(unsigned));
    f[0] = 1;
    for (int i = 1; i <= target; i++) {
        for (int j = 0; j < numsSize; j++) {
            int x = nums[j];
            if (x <= i) {
                f[i] += f[i - x];
            }
        }
    }
    int ans = f[target];
    free(f);
    return ans;
}
```

```go [sol-Go]
func combinationSum4(nums []int, target int) int {
    f := make([]int, target+1)
    f[0] = 1
    for i := 1; i <= target; i++ {
        for _, x := range nums {
            if x <= i {
                f[i] += f[i-x]
            }
        }
    }
    return f[target]
}
```

```js [sol-JavaScript]
var combinationSum4 = function(nums, target) {
    const f = Array(target + 1).fill(0);
    f[0] = 1;
    for (let i = 1; i <= target; i++) {
        for (const x of nums) {
            if (x <= i) {
                f[i] += f[i - x];
            }
        }
    }
    return f[target];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn combination_sum4(nums: Vec<i32>, target: i32) -> i32 {
        let t = target as usize;
        let mut f = vec![0; t + 1];
        f[0] = 1;
        for i in 1..=t {
            for &x in &nums {
                let x = x as usize;
                if x <= i {
                    f[i] += f[i - x];
                }
            }
        }
        f[t]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\textit{target}\cdot n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{target})$。

## 进阶问题

**问**：如果给定的数组中含有负数会发生什么？问题会产生何种变化？如果允许负数出现，需要向题目中添加哪些限制条件？

**答**：负数可能会导致有无穷个方案。例如 $\textit{nums}=[1,-1]$，要组成 $1$，只要选的个 $1$ 的个数比 $-1$ 多一个，就可以满足要求，这有无穷种选法。为了不让答案为无穷大，可以限制负数的选择个数上限。

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

本题来自 `一、入门 DP / §1.1 爬楼梯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.1 爬楼梯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
