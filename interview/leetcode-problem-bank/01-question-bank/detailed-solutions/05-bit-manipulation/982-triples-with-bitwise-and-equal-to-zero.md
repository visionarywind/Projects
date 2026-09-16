# 982. 按位与为零的三元组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/triples-with-bitwise-and-equal-to-zero/
- 题目 slug：`triples-with-bitwise-and-equal-to-zero`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：2085
- 外部题解来源：https://leetcode.cn/problems/triples-with-bitwise-and-equal-to-zero/solutions/2145654/you-ji-qiao-de-mei-ju-chang-shu-you-hua-daxit/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[有技巧的枚举 + 常数优化（Python/Java/C++/Go）](https://leetcode.cn/problems/triples-with-bitwise-and-equal-to-zero/solutions/2145654/you-ji-qiao-de-mei-ju-chang-shu-you-hua-daxit/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`you-ji-qiao-de-mei-ju-chang-shu-you-hua-daxit`
- topic id：`2145654`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

暴力枚举 $i,j,k$ 的话，时间复杂度是 $O(n^3)$ 的，如何优化呢？

对于 $\textit{nums}[k]$ 来说，其实它只需要知道它 $\text{AND}$ 一个数的结果是否等于 $0$，至于这个数具体是由哪些 $\textit{nums}[i]\ \text{AND}\ \textit{nums}[j]$ 得到的，并不重要。

因此，可以先写一个 $O(n^2)$ 的枚举，预处理所有 $\textit{nums}[i]\ \text{AND}\ \textit{nums}[j]$ 的出现次数，存到一个哈希表（或者数组）$\textit{cnt}$ 中。然后枚举 $\textit{nums}[k]$ 和 $x$，如果 $\textit{nums}[k]\ \text{AND}\ x$ 等于 $0$，那就把 $\textit{cnt}[x]$ 加到答案中。

枚举哪些 $x$ 呢？第一种做法是直接枚举。

用哈希表实现的 $\textit{cnt}$ 要枚举所有 key；用数组实现的 $\textit{cnt}$ 要枚举区间 $[0,2^{16})$ 内的所有数（根据题目的数据范围得出）。

```py [sol1-Python3]
class Solution:
    def countTriplets(self, nums: List[int]) -> int:
        cnt = Counter(x & y for x in nums for y in nums)
        return sum(c for x, c in cnt.items() for y in nums if x & y == 0)
```

```java [sol1-Java]
class Solution {
    public int countTriplets(int[] nums) {
        int[] cnt = new int[1 << 16];
        for (int x : nums)
            for (int y : nums)
                ++cnt[x & y];
        int ans = 0;
        for (int x : nums)
            for (int y = 0; y < 1 << 16; ++y)
                if ((x & y) == 0)
                    ans += cnt[y];
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int countTriplets(vector<int> &nums) {
        int cnt[1 << 16]{};
        for (int x : nums)
            for (int y : nums)
                ++cnt[x & y];
        int ans = 0;
        for (int x : nums)
            for (int y = 0; y < 1 << 16; ++y)
                if ((x & y) == 0)
                    ans += cnt[y];
        return ans;
    }
};
```

```go [sol1-Go]
func countTriplets(nums []int) (ans int) {
    cnt := [1 << 16]int{}
    for _, x := range nums {
        for _, y := range nums {
            cnt[x&y]++
        }
    }
    for x, c := range cnt {
        for _, y := range nums {
            if x&y == 0 {
                ans += c
            }
        }
    }
    return
}
```

如果把二进制数看成集合的话，二进制从低到高第 $i$ 位为 $1$ 表示 $i$ 在集合中，为 $0$ 表示 $i$ 不在集合中，例如 $a=1101_{(2)}$ 表示集合 $A=\{0,2,3\}$。

那么 $a\ \text{AND}\ b = 0$ 相当于集合 $A$ 和集合 $B$ 没有交集，也可以理解成 $B$ 是 $\complement_{U}A$ 的子集，这里 $U=\{0,1,2,\cdots,15\}$，对应的数字就是 $\text{0xffff}$。一个数异或 $\text{0xffff}$ 就可以得到这个数的补集了。

因此，上面的代码可以优化成枚举 $m=\textit{nums}[k] \oplus \text{0xffff}$ 的子集。

怎么枚举 $m$ 的子集 $s$ 呢？可以从 $m$ 不断减一，直到 $0$，如果 $s\ \text{AND}\ m = s$ 就表示 $s$ 是 $m$ 的子集。

更高效的做法是直接「跳到」下一个子集，即 $s$ 更新为 $(s-1)\ \text{AND}\ m$。这样做的正确性在于，$s-1$ 仅仅把 $s$ 最低位的 $1$ 改成了 $0$，比这个 $1$ 更低的 $0$ 全部改成了 $1$，因此下一个子集一定是 $s-1$ 的子集，直接 $\text{AND}\ m$，就能得到下一个子集了。

最后，当 $s=0$ 时，由于 $-1$ 的二进制全为 $1$，所以 $(s-1)\ \text{AND}\ m = m$，因此我们可以通过判断下一个子集是否又回到 $m$，来判断是否要退出循环。

> 注：这一技巧经常用于子集状压 DP 中。

```py [sol2-Python3]
class Solution:
    def countTriplets(self, nums: List[int]) -> int:
        cnt = [0] * (1 << 16)
        for x in nums:
            for y in nums:
                cnt[x & y] += 1
        ans = 0
        for m in nums:
            m ^= 0xffff
            s = m
            while True:  # 枚举 m 的子集（包括空集）
                ans += cnt[s]
                s = (s - 1) & m
                if s == m: break
        return ans
```

```java [sol2-Java]
class Solution {
    public int countTriplets(int[] nums) {
        int[] cnt = new int[1 << 16];
        for (int x : nums)
            for (int y : nums)
                ++cnt[x & y];
        int ans = 0;
        for (int m : nums) {
            m ^= 0xffff;
            int s = m;
            do { // 枚举 m 的子集（包括空集）
                ans += cnt[s];
                s = (s - 1) & m;
            } while (s != m);
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int countTriplets(vector<int> &nums) {
        int cnt[1 << 16]{};
        for (int x : nums)
            for (int y : nums)
                ++cnt[x & y];
        int ans = 0;
        for (int m : nums) {
            m ^= 0xffff;
            int s = m;
            do { // 枚举 m 的子集（包括空集）
                ans += cnt[s];
                s = (s - 1) & m;
            } while (s != m);
        }
        return ans;
    }
};
```

```go [sol2-Go]
func countTriplets(nums []int) (ans int) {
    cnt := [1 << 16]int{}
    for _, x := range nums {
        for _, y := range nums {
            cnt[x&y]++
        }
    }
    for _, m := range nums {
        m ^= 0xffff
        s := m
        for { // 枚举 m 的子集（包括空集）
            ans += cnt[s]
            s = (s - 1) & m
            if s == m {
                break
            }
        }
    }
    return
}
```

还可以预处理每个 $\textit{nums}[k]$ 的补集的子集的出现次数 $\textit{cnt}$，最后累加 $\textit{cnt}[\textit{nums}[i]\ \text{AND}\ \textit{nums}[j]]$。

```py [sol3-Python3]
class Solution:
    def countTriplets(self, nums: List[int]) -> int:
        cnt = [0] * (1 << 16)
        cnt[0] = len(nums)  # 直接统计空集
        for m in nums:
            m ^= 0xffff
            s = m
            while s:  # 枚举 m 的非空子集
                cnt[s] += 1
                s = (s - 1) & m
        return sum(cnt[x & y] for x in nums for y in nums)
```

```java [sol3-Java]
class Solution {
    public int countTriplets(int[] nums) {
        int[] cnt = new int[1 << 16];
        cnt[0] = nums.length; // 直接统计空集
        for (int m : nums) {
            m ^= 0xffff;
            for (int s = m; s > 0; s = (s - 1) & m) // 枚举 m 的非空子集
                ++cnt[s];
        }
        int ans = 0;
        for (int x : nums)
            for (int y : nums)
                ans += cnt[x & y];
        return ans;
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    int countTriplets(vector<int> &nums) {
        int cnt[1 << 16]{};
        cnt[0] = nums.size(); // 直接统计空集
        for (int m : nums) {
            m ^= 0xffff;
            for (int s = m; s; s = (s - 1) & m) // 枚举 m 的非空子集
                ++cnt[s];
        }
        int ans = 0;
        for (int x : nums)
            for (int y : nums)
                ans += cnt[x & y];
        return ans;
    }
};
```

```go [sol3-Go]
func countTriplets(nums []int) (ans int) {
    cnt := [1 << 16]int{len(nums)} // 直接统计空集
    for _, m := range nums {
        m ^= 0xffff
        for s := m; s > 0; s = (s - 1) & m { // 枚举 m 的非空子集
            cnt[s]++
        }
    }
    for _, x := range nums {
        for _, y := range nums {
            ans += cnt[x&y]
        }
    }
    return
}
```

再快点：仔细计算 $\textit{cnt}$ 的实际大小 $u$。相应的全集就是 $u-1$。

```py [sol4-Python3]
class Solution:
    def countTriplets(self, nums: List[int]) -> int:
        u = 1
        for x in nums:
            while u <= x:
                u <<= 1
        cnt = [0] * u
        cnt[0] = len(nums)  # 直接统计空集
        for m in nums:
            m ^= u - 1
            s = m
            while s:  # 枚举 m 的非空子集
                cnt[s] += 1
                s = (s - 1) & m
        return sum(cnt[x & y] for x in nums for y in nums)
```

```java [sol4-Java]
class Solution {
    public int countTriplets(int[] nums) {
        int u = 1;
        for (int x : nums)
            while (u <= x)
                u <<= 1;
        int[] cnt = new int[u];
        cnt[0] = nums.length; // 直接统计空集
        for (int m : nums) {
            m ^= u - 1;
            for (int s = m; s > 0; s = (s - 1) & m) // 枚举 m 的非空子集
                ++cnt[s];
        }
        int ans = 0;
        for (int x : nums)
            for (int y : nums)
                ans += cnt[x & y];
        return ans;
    }
}
```

```cpp [sol4-C++]
class Solution {
public:
    int countTriplets(vector<int> &nums) {
        int u = 1;
        for (int x : nums)
            while (u <= x)
                u <<= 1;
        int cnt[u]; memset(cnt, 0, sizeof(cnt));
        cnt[0] = nums.size(); // 直接统计空集
        for (int m : nums) {
            m ^= u - 1;
            for (int s = m; s > 0; s = (s - 1) & m) // 枚举 m 的非空子集
                ++cnt[s];
        }
        int ans = 0;
        for (int x : nums)
            for (int y : nums)
                ans += cnt[x & y];
        return ans;
    }
};
```

```go [sol4-Go]
func countTriplets(nums []int) (ans int) {
    u := 1
    for _, x := range nums {
        for u <= x {
            u <<= 1
        }
    }
    cnt := make([]int, u)
    cnt[0] = len(nums) // 直接统计空集
    for _, m := range nums {
        m ^= u - 1
        for s := m; s > 0; s = (s - 1) & m { // 枚举 m 的非空子集
            cnt[s]++
        }
    }
    for _, x := range nums {
        for _, y := range nums {
            ans += cnt[x&y]
        }
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$O(n(n+U))$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$O(U)$。

---

附：我的 [每日一题题解合集](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)，已分类整理好。

欢迎关注[【biIibiIi@灵茶山艾府】](https://space.bilibili.com/206214)，高质量算法教学，持续更新中~

## 本地原创解析

### 1. 题意重述

本题来自 `九、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
