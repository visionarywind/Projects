# 2151. 基于陈述统计最多好人数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-good-people-based-on-statements/
- 题目 slug：`maximum-good-people-based-on-statements`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.2 子集型回溯
- 难度分：1980
- 外部题解来源：https://leetcode.cn/problems/maximum-good-people-based-on-statements/solutions/1227442/er-jin-zhi-mei-ju-by-endlesscheng-ttix/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二进制枚举（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-good-people-based-on-statements/solutions/1227442/er-jin-zhi-mei-ju-by-endlesscheng-ttix/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-jin-zhi-mei-ju-by-endlesscheng-ttix`
- topic id：`1227442`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

由于至多有 $n=15$ 个人，我们可以枚举这 $n$ 个人谁是好人，谁是坏人，这一共有 $2^n$ 种不同的情况。

根据 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/) 中的「枚举子集」的技巧，枚举好人子集。

用二进制数表示这 $n$ 个人中谁好谁坏，其中 $1$ 表示好人，$0$ 表示坏人。这样就可以枚举 $i\in [0, 2^n-1]$ 中的所有数字，然后判断 $i$ 中好人的陈述是否与实际情况矛盾，若不矛盾则 $i$ 为一种合法的情况。所有合法情况中的好人个数的最大值即为答案。

代码实现时，可以从 $i=1$ 开始枚举。

```py [sol-Python3]
class Solution:
    def maximumGood(self, statements: List[List[int]]) -> int:
        def check(i: int) -> int:
            cnt = 0  # i 中好人个数
            for j, row in enumerate(statements):  # 枚举 i 中的好人 j
                if i >> j & 1:
                    if any(st < 2 and st != i >> k & 1 for k, st in enumerate(row)):
                        return 0  # 好人 j 的某个陈述 st 与实际情况矛盾
                    cnt += 1
            return cnt

        return max(check(i) for i in range(1, 1 << len(statements)))
```

```java [sol-Java]
class Solution {
    public int maximumGood(int[][] statements) {
        int n = statements.length;
        int ans = 0;
        next:
        for (int i = 1; i < 1 << n; i++) {
            int cnt = 0; // i 中好人个数
            for (int j = 0; j < n; j++) {
                if ((i >> j & 1) > 0) { // 枚举 i 中的好人 j
                    for (int k = 0; k < n; k++) { // 枚举 j 的所有陈述
                        if (statements[j][k] < 2 && statements[j][k] != (i >> k & 1)) { // 该陈述与实际情况矛盾
                            continue next;
                        }
                    }
                    cnt++;
                }
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
    int maximumGood(vector<vector<int>>& statements) {
        int ans = 0, n = statements.size();
        for (int i = 1; i < 1 << n; i++) {
            int cnt = 0; // i 中好人个数
            for (int j = 0; j < n; j++) {
                if (i >> j & 1) { // 枚举 i 中的好人 j
                    for (int k = 0; k < n; k++) { // 枚举 j 的所有陈述
                        if (statements[j][k] < 2 && statements[j][k] != (i >> k & 1)) { // 该陈述与实际情况矛盾
                            goto next;
                        }
                    }
                    ++cnt;
                }
            }
            ans = max(ans, cnt);
            next:;
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumGood(statements [][]int) (ans int) {
next:
	for i := 1; i < 1<<len(statements); i++ {
		cnt := 0 // i 中好人个数
		for j, row := range statements {
			if i>>j&1 > 0 { // 枚举 i 中的好人 j
				for k, st := range row { // 枚举 j 的所有陈述 st
					if st < 2 && st != i>>k&1 { // 该陈述与实际情况矛盾
						continue next
					}
				}
				cnt++
			}
		}
		ans = max(ans, cnt)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^22^n)$，其中 $n$ 是 $\textit{statements }$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果坏人一定说假话呢？

这题是 [CF1594D](https://codeforces.com/problemset/problem/1594/D)。

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

本题来自 `四、回溯 / §4.2 子集型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.2 子集型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
