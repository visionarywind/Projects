# 1653. 使字符串平衡的最少删除次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-deletions-to-make-string-balanced/
- 题目 slug：`minimum-deletions-to-make-string-balanced`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1794
- 外部题解来源：https://leetcode.cn/problems/minimum-deletions-to-make-string-balanced/solutions/2149746/qian-hou-zhui-fen-jie-yi-zhang-tu-miao-d-dor2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解，一张图秒懂！（附动态规划）Python/Java/C++/Go](https://leetcode.cn/problems/minimum-deletions-to-make-string-balanced/solutions/2149746/qian-hou-zhui-fen-jie-yi-zhang-tu-miao-d-dor2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-yi-zhang-tu-miao-d-dor2`
- topic id：`2149746`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前后缀分解（两次遍历）

![1653-2-cut3.png](https://pic.leetcode.cn/1678016319-qezsdt-1653-2-cut3.png)

#### 答疑

**问**：为什么把 `if-else` 写成 `(c - 'a') * 2 - 1` 会快很多？

**答**：CPU 在遇到分支（条件跳转指令）时会预测代码要执行哪个分支，如果预测正确，CPU 就会继续按照预测的路径执行程序。但如果预测失败，CPU 就需要回滚之前的指令并加载正确的指令，以确保程序执行的正确性。

对于本题的数据，字符 $\text{`a'}$ 和 $\text{`b'}$ 可以认为是随机出现的，在这种情况下分支预测就会有 $50\%$ 的概率失败。失败导致的回滚和加载操作需要消耗额外的 CPU 周期，如果能用较小的代价去掉分支，对于本题的情况必然可以带来效率上的提升。

> 注：这种优化方法会降低可读性，不建议在业务代码中使用。

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, s: str) -> int:
        ans = delete = s.count('a')
        for c in s:
            delete -= 1 if c == 'a' else -1
            if delete < ans:  # 手动计算 min 会快很多
                ans = delete
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(String S) {
        char[] s = S.toCharArray();
        int del = 0;
        for (char c : s) {
            del += 'b' - c; // 统计 'a' 的个数
        }

        int ans = del;
        for (char c : s) {
            // 'a' -> -1    'b' -> 1
            del += (c - 'a') * 2 - 1;
            ans = Math.min(ans, del);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(string s) {
        int del = 0;
        for (char c : s) {
            del += 'b' - c; // 统计 'a' 的个数
        }

        int ans = del;
        for (char c : s) {
            // 'a' -> -1    'b' -> 1
            del += (c - 'a') * 2 - 1;
            ans = min(ans, del);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumDeletions(s string) int {
	del := strings.Count(s, "a")
	ans := del
	for _, c := range s {
		// 'a' -> -1    'b' -> 1
		del += int((c-'a')*2 - 1)
		ans = min(ans, del)
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：动态规划（一次遍历）

如果你还不熟悉动态规划（包括空间优化），可以先看看 [动态规划入门](https://www.bilibili.com/video/BV1Xj411K7oF/)。

考虑 $s$ 的最后一个字母：

- 如果它是 $\text{`b'}$，则无需删除，问题规模缩小，变成「使 $s$ 的前 $n-1$ 个字母平衡的最少删除次数」。
- 如果它是 $\text{`a'}$：
  - 删除它，则答案为「使 $s$ 的前 $n-1$ 个字母平衡的最少删除次数」加上 $1$。
  - 保留它，那么前面的所有 $\text{`b'}$ 都要删除；

设 $\textit{cntB}$ 为前 $i$ 个字母中 $\text{`b'}$ 的个数。定义 $f[i]$ 表示使 $s$ 的前 $i$ 个字母平衡的最少删除次数：

- 如果第 $i$ 个字母是 $\text{`b'}$，则 $f[i] = f[i-1]$；
- 如果第 $i$ 个字母是 $\text{`a'}$，则 $f[i] = \min(f[i-1]+1,\textit{cntB})$。

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, s: str) -> int:
        f = cnt_b = 0
        for c in s:
            if c == 'b':
                cnt_b += 1  # f 值不变
            else:
                f = min(f + 1, cnt_b)
        return f
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(String s) {
        int f = 0;
        int cntB = 0;
        for (char c : s.toCharArray()) {
            if (c == 'b') {
                cntB++; // f 值不变
            } else {
                f = Math.min(f + 1, cntB);
            }
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(string s) {
        int f = 0, cnt_b = 0;
        for (char c : s) {
            if (c == 'b') {
                cnt_b++; // f 值不变
            } else {
                f = min(f + 1, cnt_b);
            }
        }
        return f;
    }
};
```

```go [sol-Go]
func minimumDeletions(s string) int {
    f, cntB := 0, 0
    for _, c := range s {
        if c == 'b' { // f 值不变
            cntB++
        } else {
            f = min(f+1, cntB)
        }
    }
    return f
}
```

这份代码也可以像方法一那样去掉分支：

- 如果第 $i$ 个字母是 $\text{`b'}$，则 $\textit{cntB}$ 增加 $1$，$f[i] = \min(f[i-1],\textit{cntB})$，这里也考虑全部删除 $\text{`b'}$；
- 如果第 $i$ 个字母是 $\text{`a'}$，则 $\textit{cntB}$ 增加 $0$，$f[i] = \min(f[i-1]+1,\textit{cntB})$。

这两种情况可以合并成：

设当前字母为 $c$，$x=c-\text{`a'}$，则 $\textit{cntB}$ 增加 $x$，$f[i] = \min(f[i-1]+(x\oplus 1),\textit{cntB})$。其中 $\oplus$ 表示异或。

```py [sol-Python3]
class Solution:
    def minimumDeletions(self, s: str) -> int:
        f = cnt_b = 0
        for c in s:
            x = ord(c) - ord('a')  # ord 很慢
            cnt_b += x
            f = min(f + (x ^ 1), cnt_b)
        return f
```

```java [sol-Java]
class Solution {
    public int minimumDeletions(String s) {
        int f = 0;
        int cntB = 0;
        for (char c : s.toCharArray()) {
            int x = c - 'a';
            cntB += x;
            f = Math.min(f + (x ^ 1), cntB);
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumDeletions(string s) {
        int f = 0, cnt_b = 0;
        for (char c : s) {
            int x = c - 'a';
            cnt_b += x;
            f = min(f + (x ^ 1), cnt_b);
        }
        return f;
    }
};
```

```go [sol-Go]
func minimumDeletions(s string) int {
    f, cntB := 0, 0
    for _, c := range s {
        x := int(c - 'a')
        cntB += x
        f = min(f+(x^1), cntB)
    }
    return f
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面动态规划题单的「**专题：前后缀分解**」。

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

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
