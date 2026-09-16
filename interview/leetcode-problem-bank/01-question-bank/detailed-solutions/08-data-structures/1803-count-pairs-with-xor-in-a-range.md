# 1803. 统计异或值在范围内的数对有多少

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/
- 题目 slug：`count-pairs-with-xor-in-a-range`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.4 0-1 字典树（异或字典树）
- 难度分：2479
- 外部题解来源：https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/solutions/2045560/bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[不会字典树？只用哈希表也能做！（Python/Java/C++/Go）](https://leetcode.cn/problems/count-pairs-with-xor-in-a-range/solutions/2045560/bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bu-hui-zi-dian-shu-zhi-yong-ha-xi-biao-y-p2pu`
- topic id：`2045560`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![1803-1.png](https://pic.leetcode.cn/1672839517-EkuLMm-1803-1.png)

代码实现时，可以按照「统计前 $k$ 个比特、统计前 $k-1$ 个比特、统计前 $k-2$ 个比特……统计前 $1$ 个比特」执行。在遍历 $\textit{cnt}$ 的同时，可以把每个键都右移 $1$ 位，从而得到下一次要统计的 $\textit{cnt}$，这样不用重新遍历 $\textit{nums}$，效率也更高。

## 答疑

**问**：$\textit{cnt}[x]\cdot \textit{cnt}[x\oplus t]$ 这里是怎么来的？

**答**：比如 $\textit{nums}$ 中有 $2$ 个 $1$，$3$ 个 $2$，那么有 $2\cdot 3=6$ 个异或值等于 $3$ 的数对。

**问**：你这个统计的逻辑有问题，如果 $t=0$ 就算错了！

**答**：题目数据范围保证 $\textit{low}\ge 1$，即使对于 $t=0$ 计算出了错误的结果，$[0,\textit{high}+1)$ 和 $[0,\textit{low})$ 这两个区间的计算结果相减，恰好可以把 $t=0$ 的结果抵消掉。

**问**：「数对异或值的后两个比特是什么都可以」是什么意思？

**答**：相当于把 $t=10000,10001,10010,10011$ 这 $4$ 个问题一起计算。由于 $\textit{nums}$ 的任意两个数的异或的后两个比特必然为 $00,01,10,11$ 中的一个，因此只需要看这个异或的前三个比特是否为 $100$ 就行了，因此「$t=10000,10001,10010,10011$ 这 $4$ 个问题的数对个数之和」等价于「在只统计 $\textit{nums}[i]$ 的前三个比特的情况下，计算 $t=100$ 的数对个数」。

**问**：感觉这个统计的思想和字典树有点像，可以展开讲讲吗？

**答**：其实这个方法就是字典树自底向上的过程，由于没有像字典树那样逐位统计 $\textit{nums}[i]$ 的每个比特位，所以时间复杂度和空间复杂度都比字典树的写法要优（见复杂度分析）。

```py [sol-Python3]
class Solution:
    def countPairs(self, nums: List[int], low: int, high: int) -> int:
        cnt = Counter(nums)
        ans = 0
        high += 1
        while high:
            nxt = Counter()
            for x, c in cnt.items():
                if high & 1: ans += c * cnt[x ^ (high - 1)]
                if low & 1:  ans -= c * cnt[x ^ (low - 1)]
                nxt[x >> 1] += c
            cnt = nxt
            low >>= 1
            high >>= 1
        return ans // 2
```

```java [sol-Java]
class Solution {
    public int countPairs(int[] nums, int low, int high) {
        int ans = 0;
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            cnt.merge(x, 1, Integer::sum); // cnt[x]++
        }
        for (high++; high > 0; high >>= 1, low >>= 1) {
            Map<Integer, Integer> nxt = new HashMap<>();
            for (var e : cnt.entrySet()) {
                int x = e.getKey(), c = e.getValue();
                if ((high & 1) == 1) ans += c * cnt.getOrDefault(x ^ (high - 1), 0);
                if ((low & 1) == 1)  ans -= c * cnt.getOrDefault(x ^ (low - 1), 0);
                nxt.put(x >> 1, nxt.getOrDefault(x >> 1, 0) + c);
            }
            cnt = nxt;
        }
        return ans / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countPairs(vector<int>& nums, int low, int high) {
        int ans = 0;
        unordered_map<int, int> cnt;
        for (int x : nums) {
            cnt[x]++;
        }
        for (high++; high; high >>= 1, low >>= 1) {
            unordered_map<int, int> nxt;
            for (auto& [x, c] : cnt) {
                if (high & 1 && cnt.count(x ^ (high - 1))) ans += c * cnt[x ^ (high - 1)];
                if (low & 1 && cnt.count(x ^ (low - 1)))   ans -= c * cnt[x ^ (low - 1)];
                nxt[x >> 1] += c;
            }
            cnt = move(nxt);
        }
        return ans / 2;
    }
};
```

```go [sol-Go]
func countPairs(nums []int, low, high int) (ans int) {
    cnt := map[int]int{}
    for _, x := range nums {
        cnt[x]++
    }
    for high++; high > 0; high >>= 1 {
        nxt := map[int]int{}
        for x, c := range cnt {
            if high&1 > 0 { ans += c * cnt[x^(high-1)] }
            if low&1 > 0  { ans -= c * cnt[x^(low-1)] }
            nxt[x>>1] += c
        }
        cnt = nxt
        low >>= 1
    }
    return ans / 2
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，严格来说是 $O\left(n+n\log\dfrac{U}{n}\right)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。如果 $U\le n$，第一次循环是 $O(U)$，第二次循环是 $O(U/2)$，第三次是 $O(U/4)$ …… 累加这个等比数列得到 $O(U)$，但是统计 $\textit{cnt}$ 要 $O(n)$，$U\le n$，所以时间复杂度为 $O(n)$；如果 $U>n$，那么前面 $O\left(\log\dfrac{U}{n}\right)$ 次循环的时间复杂度都是 $O(n)$，其余的加起来是 $O(n)$，所以时间复杂度为 $O\left(n\log\dfrac{U}{n}\right)$。两者取最大值，由于在大 $O$ 记号下最大值和相加是一样的，所以时间复杂度为 $O\left(n+n\log\dfrac{U}{n}\right)$。
- 空间复杂度：$O(n)$。

## 相似题目

- [Codeforces 665E. Beautiful Subarrays](https://codeforces.com/problemset/problem/665/E)
- [Codeforces 817E. Choosing The Commander](https://codeforces.com/problemset/problem/817/E)

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

本题来自 `六、字典树（trie） / §6.4 0-1 字典树（异或字典树）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、字典树（trie） / §6.4 0-1 字典树（异或字典树）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
