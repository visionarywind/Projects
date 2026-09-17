# 2836. 在传球游戏中最大化函数值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-value-of-function-in-a-ball-passing-game/
- 题目 slug：`maximize-value-of-function-in-a-ball-passing-game`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法
- 难度分：2769
- 外部题解来源：https://leetcode.cn/problems/maximize-value-of-function-in-a-ball-passing-game/solutions/2413298/shu-shang-bei-zeng-by-endlesscheng-xvsv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】树上倍增（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-value-of-function-in-a-ball-passing-game/solutions/2413298/shu-shang-bei-zeng-by-endlesscheng-xvsv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-shang-bei-zeng-by-endlesscheng-xvsv`
- topic id：`2413298`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 视频讲解

见 [视频讲解](https://www.bilibili.com/video/BV1Em4y1T7Bq/) 第四题。

## 前置知识

[【模板讲解】树上倍增算法](https://leetcode.cn/problems/kth-ancestor-of-a-tree-node/solution/mo-ban-jiang-jie-shu-shang-bei-zeng-suan-v3rw/)

按照这个模板改改就能过。

## 思路

利用倍增算法，预处理每个节点 $x$ 的第 $2^i$ 个祖先节点，以及从 $x$ 的父节点到 $x$ 的第 $2^i$ 个祖先节点的节点编号之和。

最后枚举起点 $x$，一边向上跳一边累加节点编号。

```py [sol-Python3]
class Solution:
    def getMaxFunctionValue(self, receiver: List[int], k: int) -> int:
        n = len(receiver)
        m = k.bit_length() - 1
        pa = [[(p, p)] + [None] * m for p in receiver]
        for i in range(m):
            for x in range(n):
                p, s = pa[x][i]
                pp, ss = pa[p][i]
                pa[x][i + 1] = (pp, s + ss)  # 合并节点值之和

        ans = 0
        for i in range(n):
            x = sum = i
            for j in range(m + 1):
                if (k >> j) & 1:  # k 的二进制从低到高第 j 位是 1
                    x, s = pa[x][j]
                    sum += s
            ans = max(ans, sum)
        return ans
```

```java [sol-Java]
class Solution {
    public long getMaxFunctionValue(List<Integer> receiver, long K) {
        int n = receiver.size();
        int m = 64 - Long.numberOfLeadingZeros(K); // K 的二进制长度
        var pa = new int[m][n];
        var sum = new long[m][n];
        for (int i = 0; i < n; i++) {
            pa[0][i] = receiver.get(i);
            sum[0][i] = receiver.get(i);
        }
        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                int p = pa[i][x];
                pa[i + 1][x] = pa[i][p];
                sum[i + 1][x] = sum[i][x] + sum[i][p]; // 合并节点值之和
            }
        }

        long ans = 0;
        for (int i = 0; i < n; i++) {
            long s = i;
            int x = i;
            for (long k = K; k > 0; k &= k - 1) {
                int ctz = Long.numberOfTrailingZeros(k);
                s += sum[ctz][x];
                x = pa[ctz][x];
            }
            ans = Math.max(ans, s);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long getMaxFunctionValue(vector<int> &receiver, long long K) {
        int n = receiver.size();
        int m = 64 - __builtin_clzll(K); // K 的二进制长度
        vector<vector<pair<int, long long>>> pa(m, vector<pair<int, long long>>(n));
        for (int i = 0; i < n; i++)
            pa[0][i] = {receiver[i], receiver[i]};
        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                auto [p, s] = pa[i][x];
                auto [pp, ss] = pa[i][p];
                pa[i + 1][x] = {pp, s + ss}; // 合并节点值之和
            }
        }

        long long ans = 0;
        for (int i = 0; i < n; i++) {
            long long sum = i;
            int x = i;
            for (long long k = K; k; k &= k - 1) {
                auto [p, s] = pa[__builtin_ctzll(k)][x];
                sum += s;
                x = p;
            }
            ans = max(ans, sum);
        }
        return ans;
    }
};
```

```go [sol-Go]
func getMaxFunctionValue(receiver []int, K int64) int64 {
	type pair struct{ pa, sum int }
	n, m := len(receiver), bits.Len(uint(K))
	pa := make([][]pair, n)
	for i, p := range receiver {
		pa[i] = make([]pair, m)
		pa[i][0] = pair{p, p}
	}
	for i := 0; i+1 < m; i++ {
		for x := range pa {
			p := pa[x][i]
			pp := pa[p.pa][i]
			pa[x][i+1] = pair{pp.pa, p.sum + pp.sum} // 合并节点值之和
		}
	}

	ans := 0
	for i := 0; i < n; i++ {
		x := i
		sum := i // 节点值之和，初始为节点 i
		for k := uint(K); k > 0; k &= k - 1 {
			p := pa[x][bits.TrailingZeros(k)]
			sum += p.sum
			x = p.pa
		}
		ans = max(ans, sum)
	}
	return int64(ans)
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log k)$，其中 $n$ 为 $\textit{receiver}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\log k)$。

## 思考题

做到 $\mathcal{O}(n)$ 时间复杂度。

提示：内向基环树。之前写过一篇 [内向基环树的介绍](https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/solution/nei-xiang-ji-huan-shu-tuo-bu-pai-xu-fen-c1i1b/)。

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.8 最近公共祖先（LCA）、倍增算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
