# 891. 子序列宽度之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-subsequence-widths/
- 题目 slug：`sum-of-subsequence-widths`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：2183
- 外部题解来源：https://leetcode.cn/problems/sum-of-subsequence-widths/solutions/1977682/by-endlesscheng-upd1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[计算每个元素对答案的贡献，多解法（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-subsequence-widths/solutions/1977682/by-endlesscheng-upd1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-upd1`
- topic id：`1977682`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

![891.png](https://pic.leetcode.cn/1668675423-JpuPGp-891.png)

#### 答疑

**问**：对于数组中有重复元素的情况，这种做法不会重复统计吗？

**答**：不会。例如计算最大值时，如果有多个相同元素 $x$，那么每个 $x$ 都只会考虑在它**左侧**的元素，而不会计入在它**右侧**的元素，所以不会有重复统计发生。

**问**：为什么 $x$ 作为最小值的子序列的个数为 $2^{n-1-i}$？

**答**：因为排序后，$x$ 右侧有 $n-1-i$ 个元素，每个元素都可以选或不选，这有 $2^{n-1-i}$ 种方案，也就对应着 $2^{n-1-i}$ 个子序列。

```py [sol-Python3]
class Solution:
    def sumSubseqWidths(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        nums.sort()
        n = len(nums)
        pow2 = [0] * n
        pow2[0] = 1
        for i in range(1, n):
            pow2[i] = pow2[i - 1] * 2 % MOD  # 预处理 2 的幂次
        return sum((pow2[i] - pow2[-1 - i]) * x
                   for i, x in enumerate(nums)) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int sumSubseqWidths(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        int[] pow2 = new int[n];
        pow2[0] = 1;
        for (int i = 1; i < n; i++) {
            pow2[i] = pow2[i - 1] * 2 % MOD; // 预处理 2 的幂次
        }
        long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += (long) (pow2[i] - pow2[n - 1 - i]) * nums[i]; // 在题目的数据范围下，这不会溢出
        }
        return (int) (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumSubseqWidths(vector<int>& nums) {
        constexpr int MOD = 1'000'000'007;
        ranges::sort(nums);
        int n = nums.size();
        vector<int> pow2(n);
        pow2[0] = 1;
        for (int i = 1; i < n; i++) {
            pow2[i] = pow2[i - 1] * 2 % MOD; // 预处理 2 的幂次
        }
        long long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += long(pow2[i] - pow2[n - 1 - i]) * nums[i]; // 在题目的数据范围下，这不会溢出
        }
        return (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }
};
```

```go [sol-Go]
func sumSubseqWidths(nums []int) (ans int) {
    const mod = 1_000_000_007
    slices.Sort(nums)
    n := len(nums)
    pow2 := make([]int, n)
    pow2[0] = 1
    for i := 1; i < n; i++ {
        pow2[i] = pow2[i-1] * 2 % mod // 预处理 2 的幂次
    }
    for i, x := range nums {
        ans += (pow2[i] - pow2[n-1-i]) * x // 在题目的数据范围下，这不会溢出
    }
    return (ans%mod + mod) % mod // 注意上面有减法，ans 可能为负数
}
```

也可以用 [快速幂](https://leetcode.cn/problems/powx-n/solutions/238559/powx-n-by-leetcode-solution/) 计算 $2$ 的幂次。

```py [sol-Python3]
class Solution:
    def sumSubseqWidths(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        nums.sort()
        return sum((pow(2, i, MOD) - pow(2, len(nums) - 1 - i, MOD)) * x
                   for i, x in enumerate(nums)) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int sumSubseqWidths(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += (pow(2, i) - pow(2, n - 1 - i)) * nums[i]; // 在题目的数据范围下，这不会溢出
        }
        return (int) (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }

    private long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int MOD = 1'000'000'007;

    long long pow(long long x, int n) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }

public:
    int sumSubseqWidths(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size();
        long long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += (pow(2, i) - pow(2, n - 1 - i)) * nums[i]; // 在题目的数据范围下，这不会溢出
        }
        return (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

func sumSubseqWidths(nums []int) (ans int) {
    slices.Sort(nums)
    n := len(nums)
    for i, x := range nums {
        ans += (pow(2, i) - pow(2, n-1-i)) * x // 在题目的数据范围下，这不会溢出
    }
    return (ans%mod + mod) % mod // 注意上面有减法，ans 可能为负数
}

func pow(x, n int) int {
    res := 1
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = res * x % mod
        }
        x = x * x % mod
    }
    return res
}
```

还可以递推算出 $2^i$ 和 $2^{n-1-i}$（后者需要用到 [逆元](https://oi-wiki.org/math/number-theory/inverse/)）。

对于本题而言，可以直接用 $\dfrac{10^9+7+1}{2}=500000004$ 当作 $2$ 关于 $10^9+7$ 的逆元。

```py [sol-Python3]
class Solution:
    def sumSubseqWidths(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        INV2 = (MOD + 1) // 2
        nums.sort()
        ans, pow2, pow2r = 0, 1, pow(2, len(nums) - 1, MOD)
        for x in nums:
            ans += (pow2 - pow2r) * x
            pow2 = pow2 * 2 % MOD
            pow2r = pow2r * INV2 % MOD
        return ans % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int INV2 = (MOD + 1) / 2;

    public int sumSubseqWidths(int[] nums) {
        Arrays.sort(nums);
        long ans = 0, pow2 = 1, pow2r = pow(2, nums.length - 1);
        for (int x : nums) {
            ans += (pow2 - pow2r) * x; // 在题目的数据范围下，这不会溢出
            pow2 = pow2 * 2 % MOD;
            pow2r = pow2r * INV2 % MOD;
        }
        return (int) (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }

    private long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int MOD = 1'000'000'007;
    static constexpr int INV2 = (MOD + 1) / 2;

    long long pow(long long x, int n) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }

public:
    int sumSubseqWidths(vector<int>& nums) {
        ranges::sort(nums);
        long long ans = 0, pow2 = 1, pow2r = pow(2, nums.size() - 1);
        for (int x : nums) {
            ans += (pow2 - pow2r) * x; // 在题目的数据范围下，这不会溢出
            pow2 = pow2 * 2 % MOD;
            pow2r = pow2r * INV2 % MOD;
        }
        return (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const inv2 = (mod + 1) / 2

func sumSubseqWidths(nums []int) (ans int) {
    slices.Sort(nums)
    pow2, pow2r := 1, pow(2, len(nums)-1)
    for _, x := range nums {
        ans += (pow2 - pow2r) * x // 在题目的数据范围下，这不会溢出
        pow2 = pow2 * 2 % mod
        pow2r = pow2r * inv2 % mod
    }
    return (ans%mod + mod) % mod // 注意上面有减法，ans 可能为负数
}

func pow(x, n int) int {
    res := 1
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = res * x % mod
        }
        x = x * x % mod
    }
    return res
}
```

另外一种思路是计算 $2^i$ 对答案的贡献，即 $(\textit{nums}[i]-\textit{nums}[n-1-i])\times 2^i$。

```py [sol-Python3]
class Solution:
    def sumSubseqWidths(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        nums.sort()
        ans, pow2 = 0, 1
        for x, y in zip(nums, reversed(nums)):
            ans += (x - y) * pow2
            pow2 = pow2 * 2 % MOD
        return ans % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007

    public int sumSubseqWidths(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        long ans = 0, pow2 = 1;
        for (int i = 0; i < n; i++) {
            ans += (nums[i] - nums[n - 1 - i]) * pow2; // 在题目的数据范围下，这不会溢出
            pow2 = pow2 * 2 % MOD;
        }
        return (int) (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumSubseqWidths(vector<int>& nums) {
        constexpr int MOD = 1'000'000'007;
        ranges::sort(nums);
        long long ans = 0, pow2 = 1;
        int n = nums.size();
        for (int i = 0; i < n; i++) {
            ans += (nums[i] - nums[n - 1 - i]) * pow2; // 在题目的数据范围下，这不会溢出
            pow2 = pow2 * 2 % MOD;
        }
        return (ans % MOD + MOD) % MOD; // 注意上面有减法，ans 可能为负数
    }
};
```

```go [sol-Go]
func sumSubseqWidths(nums []int) (ans int) {
    const mod = 1_000_000_007
    slices.Sort(nums)
    pow2 := 1
    for i, x := range nums {
        ans += (x - nums[len(nums)-1-i]) * pow2 // 在题目的数据范围下，这不会溢出
        pow2 = pow2 * 2 % mod
    }
    return (ans%mod + mod) % mod // 注意上面有减法，ans 可能为负数
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$，忽略排序时的栈开销，仅用到若干变量。

## 相关题目

如果把子序列改成**子数组**要怎么做？

这题是 [2104. 子数组范围和](https://leetcode.cn/problems/sum-of-subarray-ranges/)。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.5 贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.5 贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
