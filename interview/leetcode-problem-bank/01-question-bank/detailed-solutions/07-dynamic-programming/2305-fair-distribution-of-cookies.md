# 2305. 公平分发饼干

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fair-distribution-of-cookies/
- 题目 slug：`fair-distribution-of-cookies`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：1887
- 外部题解来源：https://leetcode.cn/problems/fair-distribution-of-cookies/solutions/1596239/by-endlesscheng-80ao/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[子集状压 DP（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/fair-distribution-of-cookies/solutions/1596239/by-endlesscheng-80ao/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-80ao`
- topic id：`1596239`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1aT41157bh) 已出炉，额外介绍了枚举子集的位运算原理，欢迎点赞三连~

---

**阅读提示**：请注意「前 $i$ 个」和「第 $i$ 个」的区别，前者用来表示状态，后者用于参与状态转移。

定义 $f[i][j]$ 表示前 $i$ 个孩子分配的饼干集合为 $j$ 时，前 $i$ 个孩子的不公平程度的最小值。

下文中 $j \setminus s$ 表示从集合 $j$ 中去掉集合 $s$ 的元素后，剩余元素组成的集合。

考虑给第 $i$ 个孩子分配的饼干集合为 $s$，设集合 $s$ 的元素和为 $\textit{sum}[s]$，分类讨论：

- 如果 $\textit{sum}[s] > f[i-1][j \setminus s]$，说明给第 $i$ 个孩子分配的饼干比前面的孩子多，不公平程度变为 $\textit{sum}[s]$；
- 如果 $\textit{sum}[s] \le f[i-1][j \setminus s]$，说明给第 $i$ 个孩子分配的饼干没有比前面的孩子多，不公平程度不变，仍为 $f[i-1][j \setminus s]$。

因此，给第 $i$ 个孩子分配饼干集合 $s$ 后，前 $i$ 个孩子的不公平程度为

$$
\max(f[i-1][j \setminus s], \textit{sum}[s])
$$

枚举 $j$ 的所有子集 $s$，则有

$$
f[i][j]=\min_{s\subseteq j} \max(f[i-1][j \setminus s], \textit{sum}[s])
$$

代码实现时，我们可以用一个二进制数来表示集合，其第 $i$ 位为 $1$ 表示分配了第 $i$ 块饼干，为 $0$ 表示未分配第 $i$ 块饼干。

此外通过倒序枚举 $j$，$f$ 的第一个维度可以省略。$\textit{sum}$ 也可以通过预处理得到。

#### 复杂度分析

- 时间复杂度：$O(k\cdot 3^n)$，其中 $n$ 为 $\textit{cookies}$ 的长度。由于元素个数为 $i$ 的集合有 $C(n,i)$ 个，其子集有 $2^i$ 个，根据二项式定理，$\sum C(n,i)2^i = (2+1)^n = 3^n$，所以枚举所有 $j$ 的所有子集 $s$ 的时间复杂度为 $O(3^n)$。
- 空间复杂度：$O(2^n)$。

注：本题和 [1723. 完成所有工作的最短时间](https://leetcode.cn/problems/find-minimum-time-to-finish-all-jobs/) 是相同的。

注 2：我另写了一篇 [题解](https://leetcode.cn/problems/find-minimum-time-to-finish-all-jobs/solution/by-endlesscheng-d2oa/)，讲述了如何用 Python 的状压 DP 写法通过 1723 题。

```py [sol-Python3]
class Solution:
    def distributeCookies(self, cookies: List[int], k: int) -> int:
        m = 1 << len(cookies)
        SUM = [0] * m
        for i, v in enumerate(cookies):
            bit = 1 << i
            for j in range(bit):
                SUM[bit | j] = SUM[j] + v

        f = SUM.copy()
        for _ in range(1, k):
            for j in range(m - 1, 0, -1):
                s = j
                while s:
                    v = f[j ^ s]
                    if SUM[s] > v: v = SUM[s]  # 不要用 max 和 min，那样会有额外的函数调用开销
                    if v < f[j]: f[j] = v
                    s = (s - 1) & j
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int distributeCookies(int[] cookies, int k) {
        int n = cookies.length;
        int[] sum = new int[1 << n];
        for (int i = 0; i < n; i++) {
            for (int j = 0, bit = 1 << i; j < bit; j++) {
                sum[bit | j] = sum[j] + cookies[i];
            }
        }

        int[] f = sum.clone();
        for (int i = 1; i < k; i++) {
            for (int j = (1 << n) - 1; j > 0; j--) {
                for (int s = j; s > 0; s = (s - 1) & j) {
                    f[j] = Math.min(f[j], Math.max(f[j ^ s], sum[s]));
                }
            }
        }
        return f[(1 << n) - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int distributeCookies(vector<int>& cookies, int k) {
        int n = cookies.size();
        vector<int> sum(1 << n);
        for (int i = 0; i < n; i++) {
            for (int j = 0, bit = 1 << i; j < bit; j++) {
                sum[bit | j] = sum[j] + cookies[i];
            }
        }

        vector<int> f(sum);
        for (int i = 1; i < k; i++) {
            for (int j = (1 << n) - 1; j; j--) {
                for (int s = j; s; s = (s - 1) & j) {
                    f[j] = min(f[j], max(f[j ^ s], sum[s]));
                }
            }
        }
        return f.back();
    }
};
```

```go [sol-Go]
func distributeCookies(cookies []int, k int) int {
	u := 1 << len(cookies)
	sum := make([]int, u)
	for i, v := range cookies {
		highBit := 1 << i
		for j := range highBit {
			sum[highBit|j] = sum[j] + v
		}
	}

	f := slices.Clone(sum)
	for range k - 1 {
		for j := u - 1; j > 0; j-- {
			for s := j; s > 0; s = (s - 1) & j {
				f[j] = min(f[j], max(f[j^s], sum[s]))
			}
		}
	}
	return f[u-1]
}
```

```js [sol-JavaScript]
var distributeCookies = function(cookies, k) {
    const n = cookies.length;
    const sum = Array(1 << n).fill(0);
    for (let i = 0; i < n; i++) {
        for (let j = 0, bit = 1 << i; j < bit; j++) {
            sum[bit | j] = sum[j] + cookies[i];
        }
    }

    const f = sum.slice();
    for (let i = 1; i < k; i++) {
        for (let j = (1 << n) - 1; j; j--) {
            for (let s = j; s; s = (s - 1) & j) {
                f[j] = Math.min(f[j], Math.max(sum[j ^ s], f[s]));
            }
        }
    }
    return f[(1 << n) - 1];
};
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
