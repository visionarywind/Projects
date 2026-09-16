# 1590. 使数组和能被 P 整除

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-sum-divisible-by-p/
- 题目 slug：`make-sum-divisible-by-p`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：2039
- 外部题解来源：https://leetcode.cn/problems/make-sum-divisible-by-p/solutions/2158435/tao-lu-qian-zhui-he-ha-xi-biao-pythonjav-rzl0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和与哈希表（Python/Java/C++/Go）](https://leetcode.cn/problems/make-sum-divisible-by-p/solutions/2158435/tao-lu-qian-zhui-he-ha-xi-biao-pythonjav-rzl0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tao-lu-qian-zhui-he-ha-xi-biao-pythonjav-rzl0`
- topic id：`2158435`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 前置知识

[模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)

### 提示 1

例如 $\textit{nums}=[11,2,5,7,8,9]$，$p=10$，把连续子数组 $[5,7]$ 去掉，剩余的数字相加等于 $30$，可以被 $p$ 整除。

$\textit{nums}$ 的元素和 $42\bmod 10=2$，而子数组和 $(5+7)\bmod 10$ 也等于 $2$。

设 $\textit{nums}$ 的元素和为 $x$，去掉的子数组和为 $y$。$x-y$ 能被 $p$ 整除，根据前置知识中**同余**的定义，这等价于

$$
y \equiv x \pmod p
$$

### 提示 2

创建 $\textit{nums}$ 的**前缀和**数组 $s$，方便计算子数组和 $y$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

问题转化成：在 $s$ 中找到两个数 $s[\textit{left}]$ 和 $s[\textit{right}]$，满足 $\textit{right}-\textit{left}$ 最小且

$$
s[\textit{right}]-s[\textit{left}]\equiv x \pmod p
$$

根据前置知识，将上式移项，得

$$
s[\textit{right}]-x \equiv s[\textit{left}]\pmod p
$$

上式相当于（调整成非负数）

$$
((s[\textit{right}]-x)\bmod p+p)\bmod p= s[\textit{left}]\bmod p
$$

也可以写成

$$
(s[\textit{right}]\bmod p-x\bmod p+p)\bmod p= s[\textit{left}]\bmod p
$$

### 提示 3

遍历 $s[i]$ 的同时，用哈希表 $\textit{last}$ 记录 $s[i]\bmod p$ 最近一次出现的下标，如果 $\textit{last}$ 中包含 $(s[i]\bmod p-x\bmod p+p)\bmod p$，设其对应的下标为 $j$，那么 $[j,i)$ 是一个符合题目要求的子数组，用其长度 $i-j$ 更新答案的最小值。如果没有符合要求的子数组，返回 $-1$。

⚠**注意**：本题可以移除空子数组，所以要先更新 $\textit{last}$，再更新答案。

代码实现时，可以把答案初始化成 $\textit{nums}$ 的长度 $n$。因为题目不允许将整个数组都移除，所以如果最后答案等于 $n$，则表示没有符合要求的子数组。

```py [sol-Python3]
class Solution:
    def minSubarray(self, nums: List[int], p: int) -> int:
        s = list(accumulate(nums, initial=0))
        x = s[-1] % p
        if x == 0:
            return 0  # 移除空子数组（这行可以不要）

        ans = n = len(nums)
        last = {}
        for i, v in enumerate(s):
            last[v % p] = i
            j = last.get((v - x) % p, -n)  # 如果不存在，-n 可以保证 i-j >= n
            ans = min(ans, i - j)
        return ans if ans < n else -1
```

```java [sol-Java]
class Solution {
    public int minSubarray(int[] nums, int p) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] = (s[i] + nums[i]) % p;
        }
        int x = s[n];
        if (x == 0) {
            return 0; // 移除空子数组（这行可以不要）
        }

        int ans = n;
        Map<Integer, Integer> last = new HashMap<>(n + 1, 1); // 预分配空间
        for (int i = 0; i <= n; i++) {
            last.put(s[i], i);
            // 如果不存在，-n 可以保证 i-j >= n
            int j = last.getOrDefault((s[i] - x + p) % p, -n);
            ans = Math.min(ans, i - j);
        }
        return ans < n ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSubarray(vector<int> &nums, int p) {
        int n = nums.size(), s[n + 1];
        s[0] = 0;
        for (int i = 0; i < n; i++) {
            s[i + 1] = (s[i] + nums[i]) % p;
        }
        int x = s[n];
        if (x == 0) {
            return 0; // 移除空子数组（这行可以不要）
        }

        int ans = n;
        unordered_map<int, int> last;
        for (int i = 0; i <= n; ++i) {
            last[s[i]] = i;
            auto it = last.find((s[i] - x + p) % p);
            if (it != last.end()) {
                ans = min(ans, i - it->second);
            }
        }
        return ans < n ? ans : -1;
    }
};
```

```go [sol-Go]
func minSubarray(nums []int, p int) int {
    n := len(nums)
    s := make([]int, n+1)
    for i, v := range nums {
        s[i+1] = (s[i] + v) % p
    }
    x := s[n]
    if x == 0 {
        return 0 // 移除空子数组（这个 if 可以不要）
    }

    ans := n
    last := map[int]int{}
    for i, v := range s {
        last[v] = i
        if j, ok := last[(v-x+p)%p]; ok {
            ans = min(ans, i-j)
        }
    }
    if ans < n {
        return ans
    }
    return -1
}
```

也可以不用前缀和数组，一边遍历 $\textit{nums}$ 一边计算前缀和。

```py [sol-Python3]
class Solution:
    def minSubarray(self, nums: List[int], p: int) -> int:
        x = sum(nums) % p
        if x == 0:
            return 0  # 移除空子数组（这行可以不要）

        ans = n = len(nums)
        s = 0
        last = {s: -1}  # 由于下面 i 是从 0 开始的，前缀和下标就要从 -1 开始了
        for i, v in enumerate(nums):
            s += v
            last[s % p] = i
            j = last.get((s - x) % p, -n)  # 如果不存在，-n 可以保证 i-j >= n
            ans = min(ans, i - j)  # 改成手写 min 会再快一些
        return ans if ans < n else -1
```

```java [sol-Java]
class Solution {
    public int minSubarray(int[] nums, int p) {
        long t = 0;
        for (int v : nums) {
            t += v;
        }
        int x = (int) (t % p);
        if (x == 0) {
            return 0; // 移除空子数组（这行可以不要）
        }

        int n = nums.length;
        int ans = n;
        int s = 0;
        Map<Integer, Integer> last = new HashMap<>(n + 1, 1); // 预分配空间
        last.put(s, -1); // 由于下面 i 是从 0 开始的，前缀和下标就要从 -1 开始了
        for (int i = 0; i < n; i++) {
            s = (s + nums[i]) % p;
            last.put(s, i);
            // 如果不存在，-n 可以保证 i-j >= n
            int j = last.getOrDefault((s - x + p) % p, -n);
            ans = Math.min(ans, i - j);
        }
        return ans < n ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSubarray(vector<int> &nums, int p) {
        int x = reduce(nums.begin(), nums.end(), 0LL) % p;
        if (x == 0) {
            return 0; // 移除空子数组（这行可以不要）
        }

        int n = nums.size(), ans = n, s = 0;
        // 由于下面 i 是从 0 开始的，前缀和下标就要从 -1 开始了
        unordered_map<int, int> last{{s, -1}};
        for (int i = 0; i < n; i++) {
            s = (s + nums[i]) % p;
            last[s] = i;
            auto it = last.find((s - x + p) % p);
            if (it != last.end()) {
                ans = min(ans, i - it->second);
            }
        }
        return ans < n ? ans : -1;
    }
};
```

```go [sol-Go]
func minSubarray(nums []int, p int) int {
    x := 0
    for _, v := range nums {
        x += v
    }
    x %= p
    if x == 0 {
        return 0 // 移除空子数组（这个 if 可以不要）
    }

    n := len(nums)
    ans, s := n, 0
    // 由于下面 i 是从 0 开始的，前缀和下标就要从 -1 开始了
    last := map[int]int{s: -1}
    for i, v := range nums {
        s += v
        last[s%p] = i
        if j, ok := last[(s-x+p)%p]; ok {
            ans = min(ans, i-j)
        }
    }
    if ans < n {
        return ans
    }
    return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(\min(n,p))$。

## 专题训练

见下面数据结构题单的「**§1.2 前缀和与哈希表**」。

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

本题来自 `一、前缀和 / §1.2 前缀和与哈希表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.2 前缀和与哈希表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
