# 3776. 使循环数组余额非负的最少移动次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-balance-circular-array/
- 题目 slug：`minimum-moves-to-balance-circular-array`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1740
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-balance-circular-array/solutions/3856585/tan-xin-cong-jin-dao-yuan-o1-kong-jian-x-3tpg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，从近到远，O(1) 空间写法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-moves-to-balance-circular-array/solutions/3856585/tan-xin-cong-jin-dao-yuan-o1-kong-jian-x-3tpg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-cong-jin-dao-yuan-o1-kong-jian-x-3tpg`
- topic id：`3856585`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

设 $\textit{balance}$ 中的负数下标是 $i$（题目保证至多一个负数）。

贪心地，移动离 $i$ 近的数，操作次数更少：

- 先把离 $i$ 最近的 $\textit{balance}[i-1]$ 和 $\textit{balance}[i+1]$ 移到 $i$。每个 $1$ 移动 $1$ 次。
- 再把离 $i$ 为 $2$ 的 $\textit{balance}[i-2]$ 和 $\textit{balance}[i+2]$ 移到 $i$。每个 $1$ 移动 $2$ 次。
- 依此类推。
- 设 $s = \textit{balance}[i-d] + \textit{balance}[i+d]$，如果 $s \ge -\textit{balance}[i]$，那么只需把 $-\textit{balance}[i]$ 个 $1$ 移到下标 $i$，每个 $1$ 移动 $d$ 次，结束操作。否则，继续把更远的数移动到 $i$。

特殊情况：

- 如果 $\textit{balance}$ 的总和是负数，无法让所有数都变成非负数，返回 $-1$。
- 如果 $\textit{balance}$ 没有负数，无需操作，返回 $0$。

[本题视频讲解](https://www.bilibili.com/video/BV1a1meBiETs/?t=5m15s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minMoves(self, balance: List[int]) -> int:
        if sum(balance) < 0:  # 总和必须非负
            return -1

        neg_idx = next((i for i, x in enumerate(balance) if x < 0), -1)
        if neg_idx < 0:  # 没有负数，无需操作
            return 0

        n = len(balance)
        need = -balance[neg_idx]
        ans = 0
        for dis in count(1):  # 把与 neg_idx 相距 dis 的数移到 neg_idx
            s = balance[neg_idx - dis] + balance[(neg_idx + dis) % n]
            if s >= need:
                ans += need * dis  # need 个 1 移动 dis 次
                return ans
            ans += s * dis  # s 个 1 移动 dis 次
            need -= s
```

```java [sol-Java]
class Solution {
    public long minMoves(int[] balance) {
        long total = 0;
        int negIdx = -1;
        for (int i = 0; i < balance.length; i++) {
            int x = balance[i];
            total += x;
            if (x < 0) {
                negIdx = i;
            }
        }

        if (total < 0) { // 总和必须非负
            return -1;
        }
        if (negIdx < 0) { // 没有负数，无需操作
            return 0;
        }

        int n = balance.length;
        int need = -balance[negIdx];
        long ans = 0;
        for (int dis = 1; ; dis++) { // 把与 negIdx 相距 dis 的数移到 negIdx
            int s = balance[(negIdx - dis + n) % n] + balance[(negIdx + dis) % n];
            if (s >= need) {
                ans += (long) need * dis; // need 个 1 移动 dis 次
                return ans;
            }
            ans += (long) s * dis; // s 个 1 移动 dis 次
            need -= s;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minMoves(vector<int>& balance) {
        long long total = 0;
        int neg_idx = -1;
        for (int i = 0; i < balance.size(); i++) {
            int x = balance[i];
            total += x;
            if (x < 0) {
                neg_idx = i;
            }
        }

        if (total < 0) { // 总和必须非负
            return -1;
        }
        if (neg_idx < 0) { // 没有负数，无需操作
            return 0;
        }

        int n = balance.size();
        int need = -balance[neg_idx];
        long long ans = 0;
        for (int dis = 1; ; dis++) { // 把与 neg_idx 相距 dis 的数移到 neg_idx
            int s = balance[(neg_idx - dis + n) % n] + balance[(neg_idx + dis) % n];
            if (s >= need) {
                ans += 1LL * need * dis; // need 个 1 移动 dis 次
                return ans;
            }
            ans += 1LL * s * dis; // s 个 1 移动 dis 次
            need -= s;
        }
    }
};
```

```go [sol-Go]
func minMoves(balance []int) int64 {
	total := 0
	negIdx := -1
	for i, x := range balance {
		total += x
		if x < 0 {
			negIdx = i
		}
	}

	if total < 0 { // 总和必须非负
		return -1
	}
	if negIdx < 0 { // 没有负数，无需操作
		return 0
	}

	n := len(balance)
	need := -balance[negIdx]
	ans := 0
	for dis := 1; ; dis++ { // 把与 negIdx 相距 dis 的数移到 negIdx
		s := balance[(negIdx-dis+n)%n] + balance[(negIdx+dis)%n]
		if s >= need {
			ans += need * dis // need 个 1 移动 dis 次
			return int64(ans)
		}
		ans += s * dis // s 个 1 移动 dis 次
		need -= s
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{balance}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心题单的「**§1.4 从最左/最右开始贪心**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.4 从最左/最右开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.4 从最左/最右开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
