# 1061. 按字典序排列最小的等效字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lexicographically-smallest-equivalent-string/
- 题目 slug：`lexicographically-smallest-equivalent-string`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/lexicographically-smallest-equivalent-string/solutions/3688682/bing-cha-ji-pythonjavacgo-by-endlesschen-kxv5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/lexicographically-smallest-equivalent-string/solutions/3688682/bing-cha-ji-pythonjavacgo-by-endlesschen-kxv5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bing-cha-ji-pythonjavacgo-by-endlesschen-kxv5`
- topic id：`3688682`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

在 $s_1[i]$ 和 $s_2[i]$ 之间连边，可以得到一个有 $26$ 个节点的图（没有连边的点是孤立点）。

对于 $c = \textit{baseStr}[i]$ 来说，$c$ 所在连通块中的最小字母，就是 $c$ 要变成的字母。

可以建图后，跑 DFS，求每个连通块的最小字母。

用并查集实现更简单：只需在合并的时候，把大的代表元指向小的代表元。

```py [sol-Python3]
class Solution:
    def smallestEquivalentString(self, s1: str, s2: str, baseStr: str) -> str:
        fa = {c: c for c in ascii_lowercase}

        def find(x: str) -> str:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]

        def merge(x: str, y: str) -> None:
            small, big = sorted((find(x), find(y)))
            fa[big] = small  # 把大的代表元指向小的代表元

        for x, y in zip(s1, s2):
            merge(x, y)

        return ''.join(find(c) for c in baseStr)
```

```java [sol-Java]
class Solution {
    public String smallestEquivalentString(String s1, String s2, String baseStr) {
        int[] fa = new int[26];
        for (int i = 0; i < 26; i++) {
            fa[i] = i;
        }

        for (int i = 0; i < s1.length(); i++) {
            merge(fa, s1.charAt(i) - 'a', s2.charAt(i) - 'a');
        }

        char[] s = baseStr.toCharArray();
        for (int i = 0; i < s.length; i++) {
            s[i] = (char) (find(fa, s[i] - 'a') + 'a');
        }
        return new String(s);
    }

    private int find(int[] fa, int x) {
        if (fa[x] != x) {
            fa[x] = find(fa, fa[x]);
        }
        return fa[x];
    }

    private void merge(int[] fa, int x, int y) {
        int fx = find(fa, x);
        int fy = find(fa, y);
        // 把大的代表元指向小的代表元
        if (fx < fy) {
            fa[fy] = fx;
        } else {
            fa[fx] = fy;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string smallestEquivalentString(string s1, string s2, string baseStr) {
        int fa[26];
        ranges::iota(fa, 0);

        auto find = [&](this auto&& find, int x) -> int {
            if (fa[x] != x) {
                fa[x] = find(fa[x]);
            }
            return fa[x];
        };

        auto merge = [&](int x, int y) {
            // 把大的代表元指向小的代表元
            auto [small, big] = minmax(find(x), find(y));
            fa[big] = small;
        };

        for (int i = 0; i < s1.size(); i++) {
            merge(s1[i] - 'a', s2[i] - 'a');
        }

        for (int i = 0; i < baseStr.size(); i++) {
            baseStr[i] = find(baseStr[i] - 'a') + 'a';
        }
        return baseStr;
    }
};
```

```go [sol-Go]
func smallestEquivalentString(s1, s2, baseStr string) string {
    fa := [26]byte{}
    for i := range fa {
        fa[i] = byte(i)
    }

    var find func(byte) byte
    find = func(x byte) byte {
        if fa[x] != x {
            fa[x] = find(fa[x])
        }
        return fa[x]
    }

    merge := func(x, y byte) {
        small, big := find(x), find(y)
        if small > big {
            small, big = big, small
        }
        // 把大的代表元指向小的代表元
        fa[big] = small
    }

    for i, x := range s1 {
        merge(byte(x)-'a', s2[i]-'a')
    }

    s := make([]byte, len(baseStr))
    for i, c := range baseStr {
        s[i] = find(byte(c)-'a') + 'a'
    }
    return string(s)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + m)$，其中 $n$ 是 $s_1$ 的长度，$m$ 是 $\textit{baseStr}$ 的长度。本题只有 $26$ 种字母，并查集的操作可以视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$，其中 $|\Sigma|=26$ 是字符集合的大小。返回值不计入。

更多相似题目，以及完整并查集模板代码，见下面数据结构题单的「**七、并查集**」。

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

本题来自 `七、并查集 / §7.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
