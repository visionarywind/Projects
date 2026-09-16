# 2275. 按位与结果大于零的最长组合

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-combination-with-bitwise-and-greater-than-zero/
- 题目 slug：`largest-combination-with-bitwise-and-greater-than-zero`
- 来源专题：位运算
- 来源分类路径：四、拆位 / 贡献法
- 难度分：1642
- 外部题解来源：https://leetcode.cn/problems/largest-combination-with-bitwise-and-greater-than-zero/solutions/1496054/by-endlesscheng-dwja/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计 1 的个数（Python/Java/C++/Go）](https://leetcode.cn/problems/largest-combination-with-bitwise-and-greater-than-zero/solutions/1496054/by-endlesscheng-dwja/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-dwja`
- topic id：`1496054`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

从 $\textit{candidates}$ 中选一个**子序列**，要求子序列所有元素的 AND 大于 $0$。返回这个子序列的最长长度。

## 思路

既然要求 AND 大于 $0$，那么这个 AND 值中，一定有一个比特位是 $1$。

枚举这个比特位：

- 如果 AND 的最低位是 $1$，最多可以从 $\textit{candidates}$ 中选多少个数？
- 如果 AND 的次低位是 $1$，最多可以从 $\textit{candidates}$ 中选多少个数？
- 依此类推。

如果最低位是 $1$，那么从 $\textit{candidates}$ 中选的数，最低位也必须是 $1$。这样问题就变成：

- $\textit{candidates}$ 中有多少个数，最低位是 $1$？

遍历 $\textit{candidates}$ 即可算出。

## 写法一：枚举比特位

```py [sol-Python3]
class Solution:
    def largestCombination(self, candidates: List[int]) -> int:
        m = max(candidates).bit_length()
        return max(sum(x >> i & 1 for x in candidates) for i in range(m))
```

```java [sol-Java]
class Solution {
    public int largestCombination(int[] candidates) {
        int mx = 0;
        for (int x : candidates) {
            mx = Math.max(mx, x);
        }
        int m = 32 - Integer.numberOfLeadingZeros(mx); // mx 的二进制长度

        int ans = 0;
        for (int i = 0; i < m; i++) {
            int cnt = 0;
            for (int x : candidates) {
                cnt += x >> i & 1;
            }
            ans = Math.max(ans, cnt);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestCombination(vector<int>& candidates) {
        int m = bit_width((unsigned) ranges::max(candidates));
        int ans = 0;
        for (int i = 0; i < m; i++) {
            int cnt = 0;
            for (int x : candidates) {
                cnt += x >> i & 1;
            }
            ans = max(ans, cnt);
        }
        return ans;
    }
};
```

```go [sol-Go]
func largestCombination(candidates []int) (ans int) {
    m := bits.Len(uint(slices.Max(candidates)))
    for i := range m {
        cnt := 0
        for _, x := range candidates {
            cnt += x >> i & 1
        }
        ans = max(ans, cnt)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{candidates}$ 的长度，$U=\max(\textit{candidates})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 写法二：一次遍历

方法一需要多次遍历 $\textit{candidates}$ 数组，我们也可以只遍历一次。

创建一个大小为 $24$（$10^7$ 的二进制长度）的 $\textit{cnt}$ 数组，统计每个比特位上的 $1$ 的个数。

最后答案为 $\textit{cnt}$ 的最大值。

```py [sol-Python3]
class Solution:
    def largestCombination(self, candidates: List[int]) -> int:
        cnt = [0] * 24
        for x in candidates:
            i = 0
            while x:
                cnt[i] += x & 1
                x >>= 1
                i += 1
        return max(cnt)
```

```java [sol-Java]
class Solution {
    public int largestCombination(int[] candidates) {
        int[] cnt = new int[24];
        for (int x : candidates) {
            for (int i = 0; x > 0; i++) {
                cnt[i] += x & 1;
                x >>= 1;
            }
        }
        return Arrays.stream(cnt).max().getAsInt();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestCombination(vector<int>& candidates) {
        int cnt[24]{};
        for (int x : candidates) {
            for (int i = 0; x; i++) {
                cnt[i] += x & 1;
                x >>= 1;
            }
        }
        return ranges::max(cnt);
    }
};
```

```go [sol-Go]
func largestCombination(candidates []int) int {
    cnt := [24]int{}
    for _, x := range candidates {
        for i := 0; x > 0; i++ {
            cnt[i] += x & 1
            x >>= 1
        }
    }
    return slices.Max(cnt[:])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{candidates}$ 的长度，$U=\max(\textit{candidates})$。
- 空间复杂度：$\mathcal{O}(\log U)$。

## 思考题

本题选的是 $\textit{candidates}$ 的**子序列**，如果将其改成**子数组**，要怎么做？

欢迎在评论区分享你的思路/代码。

更多相似题目，见下面位运算题单中的「**四、拆位 / 贡献法**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. 【本题相关】[位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
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

本题来自 `四、拆位 / 贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、拆位 / 贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
