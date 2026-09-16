# 974. 和可被 K 整除的子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subarray-sums-divisible-by-k/
- 题目 slug：`subarray-sums-divisible-by-k`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：1676
- 外部题解来源：https://leetcode.cn/problems/subarray-sums-divisible-by-k/solutions/3815616/qian-zhui-he-yu-ha-xi-biao-shi-zi-bian-x-qxc5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和与哈希表 + 式子变形（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/subarray-sums-divisible-by-k/solutions/3815616/qian-zhui-he-yu-ha-xi-biao-shi-zi-bian-x-qxc5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-yu-ha-xi-biao-shi-zi-bian-x-qxc5`
- topic id：`3815616`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)，[我的题解](https://leetcode.cn/problems/subarray-sum-equals-k/solutions/2781031/qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr/)。

设 $\textit{nums}$ 的前缀和数组为 $s$。子数组 $[l,r)$ 的元素和为 $s[r] - s[l]$。

题目要求

$$
(s[r] - s[l]) \bmod k = 0
$$

即 $s[r]$ 与 $s[l]$ 关于模 $k$ **同余**。关于同余的概念，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

所以上式等价于

$$
s[l]\bmod k = s[r]\bmod k
$$

把前缀和的每一项都模 $k$，就变成了 560 题（相当于 560 题的 $k=0$）。

注意本题有负数，需要用 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/) 中的技巧，把负数前缀和调整为非负数。

如果不调整负数，会出现同余的数据没有汇总的情况。例如 $k=4$，$-1$ 和 $3$ 关于模 $k$ 同余，如果不调整 $-1$，那么哈希表会分开保存 $-1$ 和 $3$。正确做法是把 $-1$ 的个数汇总到 $3$ 上。

## 写法一：哈希表

```py [sol-Python3]
class Solution:
    def subarraysDivByK(self, nums: List[int], k: int) -> int:
        cnt = defaultdict(int)
        ans = s = 0
        for x in nums:
            cnt[s] += 1
            s = (s + x) % k
            ans += cnt[s]
        return ans
```

```java [sol-Java]
class Solution {
    public int subarraysDivByK(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>(nums.length, 1); // 预分配空间
        int s = 0;
        int ans = 0;
        for (int x : nums) {
            cnt.merge(s, 1, Integer::sum); // cnt[s]++
            s = (s + x % k + k) % k; // 保证 s 非负
            ans += cnt.getOrDefault(s, 0);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int subarraysDivByK(vector<int>& nums, int k) {
        unordered_map<int, int> cnt;
        int ans = 0, s = 0;
        for (int x : nums) {
            cnt[s]++;
            s = (s + x % k + k) % k; // 保证 s 非负
            ans += cnt[s];
        }
        return ans;
    }
};
```

```go [sol-Go]
func subarraysDivByK(nums []int, k int) (ans int) {
    cnt := make(map[int]int, len(nums)) // 预分配空间
    s := 0
    for _, x := range nums {
        cnt[s]++
        s = (s + x%k + k) % k // 保证 s 非负
        ans += cnt[s]
    }
    return
}
```

```js [sol-JavaScript]
var subarraysDivByK = function(nums, k) {
    const cnt = new Map();
    let ans = 0, s = 0;
    for (const x of nums) {
        cnt.set(s, (cnt.get(s) ?? 0) + 1);
        s = (s + x % k + k) % k; // 保证 s 非负
        ans += cnt.get(s) ?? 0;
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn subarrays_div_by_k(nums: Vec<i32>, k: i32) -> i32 {
        let mut cnt = HashMap::with_capacity(nums.len()); // 预分配空间
        let mut s = 0;
        let mut ans = 0;
        for x in nums {
            *cnt.entry(s).or_insert(0) += 1;
            s = (s + x % k + k) % k; // 保证 s 非负
            if let Some(&c) = cnt.get(&s) {
                ans += c;
            }
        }
        return ans;
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(\min(n,k))$。哈希表的元素个数不会超过 $k$。

## 写法二：数组

```py [sol-Python3]
class Solution:
    def subarraysDivByK(self, nums: List[int], k: int) -> int:
        cnt = [0] * k
        ans = s = 0
        for x in nums:
            cnt[s] += 1
            s = (s + x) % k
            ans += cnt[s]
        return ans
```

```java [sol-Java]
class Solution {
    public int subarraysDivByK(int[] nums, int k) {
        int[] cnt = new int[k];
        int s = 0;
        int ans = 0;
        for (int x : nums) {
            cnt[s]++;
            s = (s + x % k + k) % k; // 保证 s 非负
            ans += cnt[s];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int subarraysDivByK(vector<int>& nums, int k) {
        vector<int> cnt(k);
        int ans = 0, s = 0;
        for (int x : nums) {
            cnt[s]++;
            s = (s + x % k + k) % k; // 保证 s 非负
            ans += cnt[s];
        }
        return ans;
    }
};
```

```go [sol-Go]
func subarraysDivByK(nums []int, k int) (ans int) {
    cnt := make([]int, k)
    s := 0
    for _, x := range nums {
        cnt[s]++
        s = (s + x%k + k) % k // 保证 s 非负
        ans += cnt[s]
    }
    return
}
```

```js [sol-JavaScript]
var subarraysDivByK = function(nums, k) {
    const cnt = Array(k).fill(0);
    let ans = 0, s = 0;
    for (const x of nums) {
        cnt[s]++;
        s = (s + x % k + k) % k; // 保证 s 非负
        ans += cnt[s];
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn subarrays_div_by_k(nums: Vec<i32>, k: i32) -> i32 {
        let mut cnt = vec![0; k as usize];
        let mut s = 0;
        let mut ans = 0;
        for x in nums {
            cnt[s as usize] += 1;
            s = (s + x % k + k) % k; // 保证 s 非负
            ans += cnt[s as usize];
        }
        return ans;
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + k)$，其中 $n$ 是 $\textit{nums}$ 的长度。注意创建大小为 $k$ 的数组需要 $\mathcal{O}(k)$ 的时间。
- 空间复杂度：$\mathcal{O}(k)$。

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
