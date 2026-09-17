# 2350. 不可能得到的最短骰子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-impossible-sequence-of-rolls/
- 题目 slug：`shortest-impossible-sequence-of-rolls`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.1 从特殊到一般
- 难度分：1961
- 外部题解来源：https://leetcode.cn/problems/shortest-impossible-sequence-of-rolls/solutions/1693351/by-endlesscheng-diiq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心及其证明（Python/Java/C++/Go）](https://leetcode.cn/problems/shortest-impossible-sequence-of-rolls/solutions/1693351/by-endlesscheng-diiq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-diiq`
- topic id：`1693351`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV16e4y1Q73o?t=15m59s) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

#### 提示 1

考虑包含 $1$ 到 $k$ 的最短前缀 ，无法得到的子序列的**第一个数**必然在里面。

#### 提示 2-1

这个前缀的最后一个数 $x$，在前缀中只会出现一次。

反证：如果 $x$ 出现多次，那么我们可以缩短前缀，同样可以包含 $1$ 到 $k$。

#### 提示 2-2

我们可以取 $x$ 当做子序列的第一个数。

#### 提示 3

去掉这个前缀，考虑**下一个**包含 $1$ 到 $k$ 的最短前缀。在提示 2-2 的前提下，子序列的**第二个数**必然在这个前缀中。同样地，取前缀最后一个数当做子序列的第二个数。

根据提示 2-1，按照这种取法，取到的这两个数组成的子序列，一定不会都位于第一个前缀中（读者可以用这两个数相同和不同来分类讨论）。因此这种取法是正确的。

#### 提示 4

不断重复这一过程，直到剩余部分无法包含 $1$ 到 $k$ 时停止。

设我们取到了 $m$ 个数，对应着 $\textit{rolls}$ 的 $m$ 个子段。由于每一段都包含 $1$ 到 $k$，$\textit{rolls}$ 必然包含长度为 $m$ 的子序列：每一段都选一个元素即可组成这样的子序列。

因此答案至少为 $m+1$。

我们可以构造出一个长为 $m+1$ 的子序列，它不在 $\textit{rolls}$ 中：前 $m$ 个数分别取各个子段的最后一个数，第 $m+1$ 个数取不在剩余部分中的数。因此答案等于 $m+1$。

代码实现时，可以在遍历数组的同时，用一个 $\textit{mark}$ 数组标记当前元素属于哪个子段。这种做法的好处是每次调用时只会申请一次空间，效率更高，在 Java 上尤为明显，左侧是数组写法，右侧是哈希表写法：

![20220728-131906.jpg](https://pic.leetcode.cn/1658985603-KuOvNs-20220728-131906.jpg)

#### 复杂度分析

- 时间复杂度：$O(n+k)$，其中 $n$ 为 $\textit{rolls}$ 的长度。
- 空间复杂度：$O(k)$。

```py [sol1-Python3]
class Solution:
    def shortestSequence(self, rolls: List[int], k: int) -> int:
        mark = [0] * (k + 1)  # mark[v] 标记 v 属于哪个子段
        ans, left = 1, k
        for v in rolls:
            if mark[v] < ans:
                mark[v] = ans
                left -= 1
                if left == 0:
                    left = k
                    ans += 1
        return ans
```

```java [sol1-Java]
class Solution {
    public int shortestSequence(int[] rolls, int k) {
        var mark = new int[k + 1]; // mark[v] 标记 v 属于哪个子段
        int ans = 1, left = k;
        for (var v : rolls)
            if (mark[v] < ans) {
                mark[v] = ans;
                if (--left == 0) {
                    left = k;
                    ++ans;
                }
            }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int shortestSequence(vector<int> &rolls, int k) {
        int ans = 1, left = k, mark[k + 1]; // mark[v] 标记 v 属于哪个子段
        memset(mark, 0, sizeof(mark));
        for (int v : rolls)
            if (mark[v] < ans) {
                mark[v] = ans;
                if (--left == 0) {
                    left = k;
                    ++ans;
                }
            }
        return ans;
    }
};
```

```go [sol1-Go]
func shortestSequence(rolls []int, k int) int {
	mark := make([]int, k+1) // mark[v] 标记 v 属于哪个子段
	ans, left := 1, k
	for _, v := range rolls {
		if mark[v] < ans {
			mark[v] = ans
			if left--; left == 0 {
				left = k
				ans++
			}
		}
	}
	return ans
}
```

#### 思考题

给你 $n$，$k$ 和答案 $\textit{ans}$，请你构造一个长为 $n$ 的 $\textit{rolls}$ 数组，使得 `shortestSequence(rolls, k)` 的输出恰好为 $\textit{ans}$。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.1 从特殊到一般`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.1 从特殊到一般`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
