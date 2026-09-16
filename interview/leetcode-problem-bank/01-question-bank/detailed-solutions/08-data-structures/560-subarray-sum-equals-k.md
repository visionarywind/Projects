# 560. 和为 K 的子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subarray-sum-equals-k/
- 题目 slug：`subarray-sum-equals-k`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/subarray-sum-equals-k/solutions/2781031/qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和+哈希表：从两次遍历到一次遍历，附变形题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/subarray-sum-equals-k/solutions/2781031/qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr`
- topic id：`2781031`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置题目

本题是连续子数组和问题，可以用**前缀和**处理。

请先完成前缀和**模板题** [303. 区域和检索 - 数组不可变](https://leetcode.cn/problems/range-sum-query-immutable/)，并阅读 [我的题解](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

## 转化

回顾 303 题的前缀和的定义：$s[0]=0,\ s[i] = \textit{nums}[0] + \textit{nums}[1] + \cdots + \textit{nums}[i-1]$。

> 注意 $s$ 是一个长为 $n+1$ 的数组，第一个数是 $0$。

设 $i<j$，如果 $\textit{nums}[i]$ 到 $\textit{nums}[j-1]$ 的元素和等于 $k$，用前缀和表示，就是

$$
s[j] - s[i] = k
$$

问题转化为：

- $s$ 中有多少对下标 $(i,j)$ 满足 $0\le i<j\le n$ 且 $s[j] - s[i] = k$？

写成 $s[j] + (-s[i]) = k$ 就能看得更明白，这是梦开始的地方——[1. 两数之和](https://leetcode.cn/problems/two-sum/)。不过那题只需找到一对下标，而本题需要计算所有满足条件的下标对的个数。

## 枚举右，维护左

以 $\textit{nums}=[1,1,-1,1,-1]$，$k=1$ 为例，其前缀和数组为 $s=[0,1,2,1,2,1]$。画出前缀和数组的折线图，如下：

![lc560-2c.png](https://pic.leetcode.cn/1748326975-svBnhu-lc560-2c.png){:width=500}

如果用二重循环暴力枚举有多少个 $s[j] - s[i] = k$，时间复杂度是 $\mathcal{O}(n^2)$，太慢了。如何加速？

[从两数之和中，我们可以学到什么？](https://leetcode.cn/problems/two-sum/solution/dong-hua-cong-liang-shu-zhi-he-zhong-wo-0yvmj/)我们可以把 $s[j] - s[i] = k$ 移项，得 

$$
s[i] = s[j] - k
$$

枚举当前的前缀和 $s[j]$，看看曾经有多少个前缀和等于 $s[j] - k$（配对）。每当我们在左边找到一个值等于 $s[j] - k$ 的前缀和，就找到了一个和为 $k$ 的子数组（因为 $s[j] - (s[j] - k) = k$）。

比如 $s[j]=2$，那么 $s[i]=s[j]-k=2-1=1$，我们要找的是 $j$ 左边有多少个 $s[i]=1$。在上面的例子中，遍历到 $s[4]=2$ 时，我们知道左边有 $2$ 个 $s[i]=1$，所以新找到了 $2$ 个和为 $1$ 的子数组。

用这个视角，再来算算上图中的那 $6$ 个和为 $1$ 的子数组。

|  $j$ | $s[j]$  | $s[j]-k$  | $s[j]-k$ 的个数 | 解释 |
|:---:|:---:|:---:|:---:|---|
| $0$  | $0$  | $-1$  |  $0$ | 无 |
| $1$  | $1$  | $0$  |  $1$  | $s[0]=0$ |
| $2$  | $2$  | $1$  |  $1$ | $s[1]=1$ |
| $3$  | $1$  | $0$  |  $1$ | $s[0]=0$ |
| $4$  | $2$  | $1$  |  $2$ | $s[1]=s[3]=1$ |
| $5$  | $1$  | $0$  |  $1$ | $s[0]=0$ |

一共有 $0+1+1+1+2+1=6$ 个和为 $k=1$ 的子数组。

具体来说，在遍历 $s[j]$ 的同时，用一个哈希表 $\textit{cnt}$ 统计 $s[j]$ 的个数。哈希表的 key 是 $s[j]$，value 是值为 $s[j]$ 的前缀和的个数。遍历到 $s[j]$ 时，从哈希表中就可以找到有 $\textit{cnt}[s[j] - k]$ 个 $s[i]$，加入答案。请读者动手算算上面的例子，加深理解。

## 答疑

**问**：为什么这样做可以不重不漏地计算？

**答**：暴力做法是，外层循环枚举 $j$，内层循环枚举 $i$，如果 $s[j]-s[i]=k$，那么答案加一。我们保留了「外层循环枚举 $j$」这个过程，把内层循环用哈希表优化成了 $\mathcal{O}(1)$，所以本质是对暴力算法的哈希表优化。既然暴力算法是不重不漏地计算，那么优化做法也是不重不漏地计算。

**问**：为什么要把 $s[0]=0$ 也加到哈希表中？

**答**：举个最简单的例子，$\textit{nums}=[1],\ k=1$。如果不把 $s[0]=0$ 加到哈希表中，按照我们的算法，没法算出这里有 $1$ 个符合要求的子数组。也可以这样理解，要想把任意子数组都表示成两个前缀和的差，必须添加 $s[0]=0$，否则当子数组是前缀时，没法减去一个数，具体见 [前缀和及其扩展](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 中的讲解。

**问**：为什么代码中要先更新 $\textit{ans}$，再更新 $\textit{cnt}$？这两行代码能否交换？

**答**：不行，这会在 $k=0$ 的时候算错。例如 $\textit{nums}=[2],\ k=0$，正确答案应该是 $0$，但如果先把 $\textit{cnt}[2]$ 加一，再把 $\textit{cnt}[2]$ 加到 $\textit{ans}$ 中，最后返回的 $\textit{ans}$ 就不是 $0$ 了。

**问**：为什么这题不适合用**滑动窗口**做？

**答**：滑动窗口需要满足**单调性**，当右端点元素进入窗口时，窗口元素和是不能减少的。本题 $\textit{nums}$ 包含**负数**，当负数进入窗口时，窗口左端点该往哪个方向移动？无法确定。如果没有负数的话，则可以用滑动窗口（恰好型滑动窗口），见 [930. 和相同的二元子数组](https://leetcode.cn/problems/binary-subarrays-with-sum/)。

## 写法一：两次遍历

```py [sol-Python3]
class Solution:
    def subarraySum(self, nums: List[int], k: int) -> int:
        s = [0] * (len(nums) + 1)
        for i, x in enumerate(nums):
            s[i + 1] = s[i] + x

        cnt = defaultdict(int)
        ans = 0
        for sj in s:
            ans += cnt[sj - k]
            cnt[sj] += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int subarraySum(int[] nums, int k) {
        int n = nums.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] = s[i] + nums[i];
        }

        Map<Integer, Integer> cnt = new HashMap<>(n + 1, 1); // 预分配空间
        int ans = 0;
        for (int sj : s) {
            ans += cnt.getOrDefault(sj - k, 0);
            cnt.merge(sj, 1, Integer::sum); // cnt[sj]++
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> s(n + 1);
        for (int i = 0; i < n; i++) {
            s[i + 1] = s[i] + nums[i];
        }

        unordered_map<int, int> cnt;
        int ans = 0;
        for (int sj : s) {
            // 注意不要直接 += cnt[sj-k]，如果 sj-k 不存在，会插入 sj-k
            ans += cnt.contains(sj - k) ? cnt[sj - k] : 0;
            cnt[sj]++;
        }
        return ans;
    }
};
```

```go [sol-Go]
func subarraySum(nums []int, k int) (ans int) {
    s := make([]int, len(nums)+1)
    for i, x := range nums {
        s[i+1] = s[i] + x
    }

    cnt := make(map[int]int, len(s)) // 预分配空间
    for _, sj := range s {
        ans += cnt[sj-k]
        cnt[sj]++
    }
    return
}
```

```js [sol-JavaScript]
var subarraySum = function(nums, k) {
    const n = nums.length;
    const s = Array(n + 1).fill(0);
    for (let i = 0; i < n; i++) {
        s[i + 1] = s[i] + nums[i];
    }

    const cnt = new Map();
    let ans = 0;
    for (const sj of s) {
        ans += cnt.get(sj - k) ?? 0;
        cnt.set(sj, (cnt.get(sj) ?? 0) + 1);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn subarray_sum(nums: Vec<i32>, k: i32) -> i32 {
        let mut s = vec![0; nums.len() + 1];
        for (i, &x) in nums.iter().enumerate() {
            s[i + 1] = s[i] + x;
        }

        let mut cnt = HashMap::with_capacity(s.len()); // 预分配空间
        let mut ans = 0;
        for sj in s {
            if let Some(&c) = cnt.get(&(sj - k)) {
                ans += c;
            }
            *cnt.entry(sj).or_insert(0) += 1;
        }
        return ans;
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二：一次遍历 · 其一

我们可以一边计算前缀和，一边遍历前缀和。

在遍历 $\textit{nums}$ 之前，我们需要先统计 $s[0]=0$，即空前缀的元素和等于 $0$。往 $\textit{cnt}$ 中添加 $\textit{cnt}[0]=1$。

对比一下，两次遍历的代码循环了 $n+1$ 次，下面的代码循环了 $n$ 次，少的那一次是什么？就是对 $s[0]=0$ 的统计。

```py [sol-Python3]
class Solution:
    def subarraySum(self, nums: List[int], k: int) -> int:
        cnt = defaultdict(int)
        cnt[0] = 1  # s[0]=0 单独统计
        ans = s = 0
        for x in nums:
            s += x
            ans += cnt[s - k]
            cnt[s] += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int subarraySum(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>(nums.length + 1, 1); // 预分配空间
        cnt.put(0, 1); // s[0]=0 单独统计
        int s = 0;
        int ans = 0;
        for (int x : nums) {
            s += x;
            ans += cnt.getOrDefault(s - k, 0);
            cnt.merge(s, 1, Integer::sum); // cnt[s]++
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        unordered_map<int, int> cnt = {{0, 1}}; // s[0]=0 单独统计
        int ans = 0, s = 0;
        for (int x : nums) {
            s += x;
            // 注意不要直接 += cnt[s-k]，如果 s-k 不存在，这会插入 s-k，消耗更多空间
            ans += cnt.contains(s - k) ? cnt[s - k] : 0;
            cnt[s]++;
        }
        return ans;
    }
};
```

```go [sol-Go]
func subarraySum(nums []int, k int) (ans int) {
    cnt := make(map[int]int, len(nums)+1) // 预分配空间
    cnt[0] = 1 // s[0]=0 单独统计
    s := 0
    for _, x := range nums {
        s += x
        ans += cnt[s-k]
        cnt[s]++
    }
    return
}
```

```js [sol-JavaScript]
var subarraySum = function(nums, k) {
    const cnt = new Map();
    cnt.set(0, 1); // s[0]=0 单独统计
    let ans = 0, s = 0;
    for (const x of nums) {
        s += x;
        ans += cnt.get(s - k) ?? 0;
        cnt.set(s, (cnt.get(s) ?? 0) + 1);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn subarray_sum(nums: Vec<i32>, k: i32) -> i32 {
        let mut cnt = HashMap::with_capacity(nums.len() + 1); // 预分配空间
        cnt.insert(0, 1); // s[0]=0 单独统计
        let mut s = 0;
        let mut ans = 0;
        for x in nums {
            s += x;
            if let Some(&c) = cnt.get(&(s - k)) {
                ans += c;
            }
            *cnt.entry(s).or_insert(0) += 1;
        }
        return ans;
    }
}
```

## 写法三：一次遍历 · 其二

在同一轮循环中，先把 $s[i-1]$ 加入哈希表，再根据 $s[i]$ 更新答案。

这样写无需初始化 $\textit{cnt}[0]=1$。

```py [sol-Python3]
class Solution:
    def subarraySum(self, nums: List[int], k: int) -> int:
        cnt = defaultdict(int)
        ans = s = 0
        for x in nums:
            cnt[s] += 1
            s += x
            ans += cnt[s - k]
        return ans
```

```java [sol-Java]
class Solution {
    public int subarraySum(int[] nums, int k) {
        Map<Integer, Integer> cnt = new HashMap<>(nums.length, 1); // 预分配空间
        int s = 0;
        int ans = 0;
        for (int x : nums) {
            cnt.merge(s, 1, Integer::sum); // cnt[s]++
            s += x;
            ans += cnt.getOrDefault(s - k, 0);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        unordered_map<int, int> cnt;
        int ans = 0, s = 0;
        for (int x : nums) {
            cnt[s]++;
            s += x;
            // 注意不要直接 += cnt[s-k]，如果 s-k 不存在，这会插入 s-k，消耗更多空间
            ans += cnt.contains(s - k) ? cnt[s - k] : 0;
        }
        return ans;
    }
};
```

```go [sol-Go]
func subarraySum(nums []int, k int) (ans int) {
    cnt := make(map[int]int, len(nums)) // 预分配空间
    s := 0
    for _, x := range nums {
        cnt[s]++
        s += x
        ans += cnt[s-k]
    }
    return
}
```

```js [sol-JavaScript]
var subarraySum = function(nums, k) {
    const cnt = new Map();
    let ans = 0, s = 0;
    for (const x of nums) {
        cnt.set(s, (cnt.get(s) ?? 0) + 1);
        s += x;
        ans += cnt.get(s - k) ?? 0;
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn subarray_sum(nums: Vec<i32>, k: i32) -> i32 {
        let mut cnt = HashMap::with_capacity(nums.len()); // 预分配空间
        let mut s = 0;
        let mut ans = 0;
        for x in nums {
            *cnt.entry(s).or_insert(0) += 1;
            s += x;
            if let Some(&c) = cnt.get(&(s - k)) {
                ans += c;
            }
        }
        return ans;
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$，其中 $m$ 为不同前缀和的个数。如果设置了哈希表的容量，则空间复杂度为 $\mathcal{O}(n)$。

### 变形题

1. 改成计算元素和等于 $k$ 的**最短**子数组，要怎么做？
2. 改成计算元素和等于 $k$ 的**最长**子数组，要怎么做？
3. 改成计算元素和等于 $k$ 的**所有子数组的长度之和**，要怎么做？
4. 改成元素和**至多**为 $k$，要怎么做？见 [363. 矩形区域不超过 K 的最大数值和](https://leetcode.cn/problems/max-sum-of-rectangle-no-larger-than-k/)。
5. 改成计算元素和为**奇数**的子数组个数，要怎么做？

欢迎在评论区分享你的思路/代码。

> 提示：思考题 4 可以枚举上下边界，转成一维数组。

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
