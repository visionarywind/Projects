# 1986. 完成任务的最少工作时间段

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-work-sessions-to-finish-the-tasks/
- 题目 slug：`minimum-number-of-work-sessions-to-finish-the-tasks`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：1995
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-work-sessions-to-finish-the-tasks/solutions/964986/zi-ji-dong-tai-gui-hua-by-endlesscheng-wtua/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[子集动态规划](https://leetcode.cn/problems/minimum-number-of-work-sessions-to-finish-the-tasks/solutions/964986/zi-ji-dong-tai-gui-hua-by-endlesscheng-wtua/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zi-ji-dong-tai-gui-hua-by-endlesscheng-wtua`
- topic id：`964986`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

定义状态 $f[s]$ 表示完成任务集合 $s$ 的所有任务所需要的最少数目的工作时间段。

枚举 $s$ 的子集 $\textit{sub}$，若完成 $\textit{sub}$ 的所有任务耗时不超过 $\textit{sessionTime}$，则可以将 $\textit{sub}$ 的所有任务用一个工作时间段来完成，也就是将 $f[s\setminus\textit{sub}]+1$ 转移到 $f[s]$ 上，二者取最小值。这里 $s\setminus\textit{sub}$ 表示从 $s$ 中去掉 $\textit{sub}$ 的剩余集合。

通过预处理 $\textit{tasks}$ 的每个子集的子集和，可以做到 $O(1)$ 判断 $\textit{sub}$ 的所有任务耗时不超过 $\textit{sessionTime}$。

时间复杂度：$O(2^n+3^n)$。预处理子集和耗时 $O(2^n)$；状态转移次数为 $s$ 的每个子集的子集个数之和，由于元素个数为 $k$ 的集合有 $C(n,k)$ 个，其子集有 $2^k$ 个，根据二项式定理，$\sum C(n,k)2^k = (2+1)^n = 3^n$，故状态转移耗时 $O(3^n)$。

相似题目：

- [1494. 并行课程 II](https://leetcode.cn/problems/parallel-courses-ii/)
- [1655. 分配重复整数](https://leetcode.cn/problems/distribute-repeating-integers)

```C++ [sol-C++]
class Solution {
public:
    int minSessions(vector<int> &tasks, int sessionTime) {
        int n = tasks.size();
        int m = 1 << n;
        // 预处理所有子集的子集和，复杂度 O(1+2+4+...+2^(n-1)) = O(2^n)
        vector<int> sum(m);
        for (int i = 0; i < n; ++i) {
            for (int j = 0, k = 1 << i; j < k; ++j) {
                sum[j | k] = sum[j] + tasks[i];
            }
        }
        vector<int> f(m, n);
        f[0] = 0;
        for (int s = 0; s < m; ++s) {
            // 枚举 s 的所有子集 sub，若 sub 耗时不超过 sessionTime，则将 f[s^sub]+1 转移到 f[s] 上
            for (int sub = s; sub; sub = (sub - 1) & s) {
                if (sum[sub] <= sessionTime) {
                    f[s] = min(f[s], f[s ^ sub] + 1);
                }
            }
        }
        return f[m - 1];
    }
};
```

```go [sol-Go]
func minSessions(tasks []int, sessionTime int) (ans int) {
	n := len(tasks)
	m := 1 << n
	// 预处理所有子集的子集和，复杂度 O(1+2+4+...+2^(n-1)) = O(2^n)
	sum := make([]int, m)
	for i, t := range tasks {
		for j, k := 0, 1<<i; j < k; j++ {
			sum[j|k] = sum[j] + t
		}
	}
	f := make([]int, m)
	for i := range f {
		f[i] = n
	}
	f[0] = 0
	for s := range f {
		// 枚举 s 的所有子集 sub，若 sub 耗时不超过 sessionTime，则将 f[s^sub]+1 转移到 f[s] 上
		for sub := s; sub > 0; sub = (sub - 1) & s {
			if sum[sub] <= sessionTime && f[s^sub]+1 < f[s] {
				f[s] = f[s^sub] + 1
			}
		}
	}
	return f[m-1]
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
