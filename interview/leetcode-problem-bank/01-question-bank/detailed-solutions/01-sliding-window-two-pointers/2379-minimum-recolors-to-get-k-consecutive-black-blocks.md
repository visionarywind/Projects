# 2379. 得到 K 个黑块的最少涂色次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-recolors-to-get-k-consecutive-black-blocks/
- 题目 slug：`minimum-recolors-to-get-k-consecutive-black-blocks`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.1 基础
- 难度分：1360
- 外部题解来源：https://leetcode.cn/problems/minimum-recolors-to-get-k-consecutive-black-blocks/solutions/1763639/on-hua-dong-chuang-kou-by-endlesscheng-s4fx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口+避免分支预测（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-recolors-to-get-k-consecutive-black-blocks/solutions/1763639/on-hua-dong-chuang-kou-by-endlesscheng-s4fx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-hua-dong-chuang-kou-by-endlesscheng-s4fx`
- topic id：`1763639`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：滑动窗口

滑动窗口是数组/字符串问题中的常用概念。**窗口**通常是指在数组/字符串中由开始和结束索引定义的一系列元素的集合，即闭区间 $[i,j]$。而**滑动窗口**是指可以将两个边界向某一方向「滑动」的窗口。

例如，将窗口 $[i,j]$ 向右滑动 $1$ 个位置，变为 $[i+1,j+1]$。滑动后，窗口内少了元素 $i$，多了元素 $j+1$。

## 思路

本题可以看成一个长度固定为 $k$ 的滑动窗口，我们需要计算窗口内 $\text{`W'}$ 的出现次数的最小值。

窗口初始位于 $\textit{blocks}$ 的长为 $k$ 的前缀上，那么初始化 $\textit{cntW}$ 为这个前缀的 $\text{`W'}$ 的个数。然后不断向右滑动窗口，如果窗口内少了 $\text{`W'}$，则 $\textit{cntW}$ 减一；如果窗口内多了 $\text{`W'}$，则 $\textit{cntW}$ 加一。

取滑动中的 $\textit{cntW}$ 的最小值，即为答案。

[本题视频讲解](https://www.bilibili.com/video/BV1Kd4y1Z7Fv)

## 答疑

**问**：`s[i] & 1` 是什么意思？

**答**：对于本题，可以把 $\text{`W'}$ 看成 $1$，把 $\text{`B'}$ 看成 $0$，这恰好是它们 ASCII 码二进制最低位的值。

**问**：为什么把 `if-else`（或者 `?:` 三目运算符）写成 `s[i] & 1` 就会快很多？（本题数据范围小看不出区别）

**答**：CPU 在遇到分支（条件跳转指令）时会预测代码要执行哪个分支，如果预测正确，CPU 就会继续按照预测的路径执行程序。但如果预测失败，CPU 就需要回滚之前的指令并加载正确的指令，以确保程序执行的正确性。

对于本题的数据，字符 $\text{`W'}$ 和 $\text{`B'}$ 可以认为是随机出现的，在这种情况下分支预测就会有 $50\%$ 的概率失败。失败导致的回滚和加载操作需要消耗额外的 CPU 周期，如果能用较小的代价去掉分支，对于本题的情况必然可以带来效率上的提升。

注意：这种优化方法往往会降低可读性，最好不要在业务代码中使用。

```py [sol-Python3]
class Solution:
    def minimumRecolors(self, blocks: str, k: int) -> int:
        ans = cnt_w = blocks[:k].count('W')
        for in_, out in zip(blocks[k:], blocks):
            cnt_w += (in_ == 'W') - (out == 'W')
            ans = min(ans, cnt_w)
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumRecolors(String blocks, int k) {
        char[] s = blocks.toCharArray();
        int cntW = 0;
        for (int i = 0; i < k; i++) {
            cntW += s[i] & 1;
        }
        int ans = cntW;
        for (int i = k; i < s.length; i++) {
            cntW += (s[i] & 1) - (s[i - k] & 1);
            ans = Math.min(ans, cntW);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumRecolors(string blocks, int k) {
        int cnt_w = 0;
        for (int i = 0; i < k; i++) {
            cnt_w += blocks[i] & 1;
        }
        int ans = cnt_w;
        for (int i = k; i < blocks.length(); i++) {
            cnt_w += (blocks[i] & 1) - (blocks[i - k] & 1);
            ans = min(ans, cnt_w);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumRecolors(blocks string, k int) int {
	cntW := strings.Count(blocks[:k], "W")
	ans := cntW
	for i := k; i < len(blocks); i++ {
		cntW += int(blocks[i]&1) - int(blocks[i-k]&1)
		ans = min(ans, cntW)
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{blocks}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

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

本题来自 `一、定长滑动窗口 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
