# LCP 53. 守护太空城

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/EJvmW4/
- 题目 slug：`EJvmW4`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/EJvmW4/solutions/1426981/by-endlesscheng-pk2q/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n*3^m) 子集状压 DP + 贪心预处理（Python/Java/C++/Go）](https://leetcode.cn/problems/EJvmW4/solutions/1426981/by-endlesscheng-pk2q/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-pk2q`
- topic id：`1426981`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

## 提示 1

考虑计算前 $i$ 个舱室需要的最小能量，由于题目中只有联合屏障会涉及到相邻的舱室，因此可以往 DP 上去思考。

## 提示 2

对于一个固定的舱室，其联合屏障在不同时间点上可能是不一样的，有时会和上一个舱室联合，有时会和下一个舱室联合，有时没有联合。

因此对于 DP，我们不能直接一刀切地划分，而是要细细枚举每个时间点上的联合情况。（或者说划分是一条曲线，可以结合提示 3 思考）

## 提示 3

枚举第 $i-1$ 个舱室在哪些时间点（记作 $\textit{pre}$ 集合）开启了联合屏障，这会对第 $i$ 个舱室产生如下影响（限制）：

- 第 $i$ 个舱室在属于 $\textit{pre}$ 集合的时间点，由于被第 $i-1$ 个舱室的联合屏障保护，无需开启任何屏障；
- 第 $i$ 个舱室在不属于 $\textit{pre}$ 集合的时间点，可以选择开启联合屏障，也可以选择开启单独屏障；若该时间点没有受到陨石雨的冲击，还可以选择不开启屏障。

## 提示 4

将「开启联合屏障的时间点集合」当作状态中的一个维度。

## 思路

定义 $f[i][j]$ 表示考虑前 $i$ 个舱室，且第 $i$ 个舱室与第 $i+1$ 个舱室开启联合屏障的时间点集合为 $j$ 时，所需的最小能量。

考虑 $f[i][j]$ 的转移来源。由于联合屏障的限制，我们需要枚举第 $i-1$ 个舱室开启联合屏障的时间点集合 $\textit{pre}$，满足 $\textit{pre}\cap j=\varnothing$，即集合 $\textit{pre}$ 与集合 $j$ 的交集为空。

剩下需要考虑的就是既不属于集合 $\textit{pre}$ 又不属于集合 $j$ 的时间点了，如果在这些时间点上受到陨石雨的冲击，那么对应的时间点必须单独开启屏障。

记 $\textit{union}[j]$ 表示开启联合屏障的时间点集合**恰好**为 $j$ 时，所需要的能量：如果两个时间点相邻，则可以花费 $1$ 能量继续维持，否则花费 $3$ 能量重新开启。

记 $\textit{single}[j]$ 表示开启单独屏障的时间点集合**恰好**为 $j$ 时，所需要的能量：如果两个时间点相邻，则可以花费 $1$ 能量继续维持，否则花费 $2$ 能量重新开启。

因此有

$$
f[i][j] = \min_{\textit{pre}\cap j=\varnothing} f[i-1][\textit{pre}] + \textit{union}[j] + \textit{single}[k]
$$

其中 $k$ 为既不属于集合 $\textit{pre}$ 又不属于集合 $j$，且第 $i$ 个舱室受到陨石雨冲击的时间点集合。

设 $n=\max(\textit{position})$，最后答案为 $f[n][\varnothing]$，因为最后一个舱室无需开启联合屏障。

代码实现时，$\textit{single}$ 和 $\textit{union}$ 都可以预处理出来，且用二进制表示集合。

```Python [sol1-Python3]
class Solution:
    def defendSpaceCity(self, time: List[int], position: List[int]) -> int:
        n, m = max(position), 1 << max(time)
        rain = [0] * (n + 1)
        for t, p in zip(time, position):
            rain[p] |= 1 << (t - 1)

        union = [0] * m
        single = [0] * m
        for i in range(1, m):
            lb = i & -i
            j = i ^ lb
            lb2 = j & -j
            union[i] = union[j] + (1 if lb == (lb2 >> 1) else 3)  # lb == (lb2 >> 1) 表示两个时间点相邻
            single[i] = single[j] + (1 if lb == (lb2 >> 1) else 2)  # 递推

        f = [[inf] * m for _ in range(n + 1)]
        for j in range(m):
            f[0][j] = union[j] + single[((m - 1) ^ j) & rain[0]]
        for i in range(1, n + 1):
            for j in range(m):
                pre = mask = (m - 1) ^ j
                while True:  # 枚举 j 的补集 mask 中的子集 pre
                    cost = f[i - 1][pre] + union[j] + single[(mask ^ pre) & rain[i]]
                    f[i][j] = min(f[i][j], cost)
                    if pre == 0:
                        break
                    pre = (pre - 1) & mask
        return f[n][0]
```



```java [sol1-Java]
class Solution {
    public int defendSpaceCity(int[] time, int[] position) {
        var n = Arrays.stream(position).max().getAsInt();
        var m = 1 << Arrays.stream(time).max().getAsInt();
        var rain = new int[n + 1];
        for (var i = 0; i < time.length; i++)
            rain[position[i]] |= 1 << (time[i] - 1);

        var union = new int[m];
        var single = new int[m];
        for (var i = 1; i < m; i++) {
            int lb = i & -i, j = i ^ lb, lb2 = j & -j;
            union[i] = union[j] + (lb == (lb2 >> 1) ? 1 : 3); // lb == (lb2 >> 1) 表示两个时间点相邻
            single[i] = single[j] + (lb == (lb2 >> 1) ? 1 : 2); // 递推
        }

        var f = new int[n + 1][m];
        for (var j = 0; j < m; j++)
            f[0][j] = union[j] + single[((m - 1) ^ j) & rain[0]];
        for (var i = 1; i <= n; ++i) {
            Arrays.fill(f[i], Integer.MAX_VALUE / 2);
            for (var j = 0; j < m; ++j)
                // 枚举 j 的补集 mask 中的子集 pre
                for (int mask = (m - 1) ^ j, pre = mask; ; pre = (pre - 1) & mask) {
                    var cost = f[i - 1][pre] + union[j] + single[(mask ^ pre) & rain[i]];
                    f[i][j] = Math.min(f[i][j], cost);
                    if (pre == 0) break;
                }
        }
        return f[n][0];
    }
}
```


```C++ [sol1-C++]
class Solution {
public:
    int defendSpaceCity(vector<int> &time, vector<int> &position) {
        int n = *max_element(position.begin(), position.end());
        int m = 1 << *max_element(time.begin(), time.end());
        int rain[n + 1]; memset(rain, 0, sizeof(rain));
        for (int i = 0; i < time.size(); ++i)
            rain[position[i]] |= 1 << (time[i] - 1);

        int un[m], single[m];
        un[0] = single[0] = 0;
        for (int i = 1; i < m; ++i) {
            int lb = i & -i, j = i ^ lb, lb2 = j & -j;
            un[i] = un[j] + (lb == (lb2 >> 1) ? 1 : 3); // lb == (lb2 >> 1) 表示两个时间点相邻
            single[i] = single[j] + (lb == (lb2 >> 1) ? 1 : 2); // 递推
        }

        int f[n + 1][m]; memset(f, 0x3f, sizeof(f));
        for (int j = 0; j < m; ++j)
            f[0][j] = un[j] + single[((m - 1) ^ j) & rain[0]];
        for (int i = 1; i <= n; ++i)
            for (int j = 0; j < m; ++j)
                // 枚举 j 的补集 mask 中的子集 pre
                for (int mask = (m - 1) ^ j, pre = mask;; pre = (pre - 1) & mask) {
                    int cost = f[i - 1][pre] + un[j] + single[(mask ^ pre) & rain[i]];
                    f[i][j] = min(f[i][j], cost);
                    if (pre == 0) break;
                }
        return f[n][0];
    }
};
```

```go [sol1-Go]
func defendSpaceCity(time, position []int) int {
	const n, m = 100, 1 << 5
	rain := [n + 1]int{}
	for i, t := range time {
		rain[position[i]] |= 1 << (t - 1)
	}

	var union, single [m]int
	for i := 1; i < m; i++ {
		lb := i & -i
		j := i ^ lb
		lb2 := j & -j
		if lb == lb2>>1 { // 两个时间点相邻
			union[i] = union[j] + 1
			single[i] = single[j] + 1 // 递推
		} else {
			// 若 i 的二进制包含 101，对于联合屏障选择继续维持是更优的，
			// 不过下面的 DP 已经枚举了所有的情况，自然会选择更优的方案
			union[i] = union[j] + 3
			single[i] = single[j] + 2
		}
	}

	f := [n + 1][m]int{}
	for j := range f[0] {
		f[0][j] = union[j] + single[(m-1^j)&rain[0]]
	}
	for i := 1; i <= n; i++ {
		for j := range f[i] {
			f[i][j] = math.MaxInt32 / 2
			mask := m - 1 ^ j
			for pre := mask; ; pre = (pre - 1) & mask { // 枚举 j 的补集 mask 中的子集 pre
				cost := f[i-1][pre] + union[j] + single[(mask^pre)&rain[i]]
				f[i][j] = min(f[i][j], cost)
				if pre == 0 { break }
			}
		}
	}
	return f[n][0]
}

func min(a, b int) int { if a > b { return b }; return a }
```

时间复杂度为 $O(n\cdot 3^m)$，其中 $n=\max(\textit{position})$，$m=\max(\textit{time})$。对于每个舱室 $i$，状态转移次数为 $j$ 的每个子集的子集个数之和，由于元素个数为 $k$ 的集合有 $C(m,k)$ 个，其子集有 $2^k$ 个，根据二项式定理，$\sum C(m,k)2^k = (2+1)^m = 3^m$，所以对每个 $i$，状态转移次数为 $O(3^m)$，因此总的时间复杂度为 $O(n\cdot 3^m)$。

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
