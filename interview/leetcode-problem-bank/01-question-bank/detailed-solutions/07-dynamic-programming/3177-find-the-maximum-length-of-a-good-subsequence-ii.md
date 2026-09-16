# 3177. 求出最长好子序列 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-maximum-length-of-a-good-subsequence-ii/
- 题目 slug：`find-the-maximum-length-of-a-good-subsequence-ii`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2365
- 外部题解来源：https://leetcode.cn/problems/find-the-maximum-length-of-a-good-subsequence-ii/solutions/2805263/dong-tai-gui-hua-you-hua-pythonjavacgo-b-jqn2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划+优化（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-maximum-length-of-a-good-subsequence-ii/solutions/2805263/dong-tai-gui-hua-you-hua-pythonjavacgo-b-jqn2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-you-hua-pythonjavacgo-b-jqn2`
- topic id：`2805263`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 引入

以示例 1 为例，$\textit{nums}=[1,2,1,1,3]$。

假如好子序列的最后一个数是 $\textit{nums}[3]=1$，分类讨论：

- $1$ 单独作为一个子序列。
- $1$ 和好子序列的倒数第二个数相同。
- $1$ 和好子序列的倒数第二个数不同。由于 $1$ 左边只有 $2$，那么就需要知道，最后一个数是 $2$ 的好子序列的最长长度。

在这个过程中，我们需要跟踪如下关键信息：

- 子序列的最后一个数 $x$。下文把该子序列称作「以 $x$ 结尾的子序列」。
- 子序列中有多少个 `seq[i] != seq[i + 1]`，也就是「相邻不同」数对的个数。

## 思路

遍历到 $x=\textit{nums}[i]$ 时，维护以 $x$ 结尾的、有**至多** $j$ 个「相邻不同」数对的子序列的最大长度，定义为 $f[x][j]$，初始值全为 $0$。

对于 $x$，有三种决策：

1. 不选：$f[x][j]$ 不变。但其实这种情况无需考虑，因为直接把 $x$ 加到以 $x$ 结尾的好子序列的末尾，并不会增加「相邻不同」数对的个数。为了让子序列尽量长，选肯定是更优的。
2. 选，且 $x$ 与子序列末尾相同，或者作为子序列的第一个数：$f[x][j]$ 增加 $1$。
3. 选，且 $x$ 与子序列末尾不同：设末尾元素为 $y$，我们需要知道最大的 $f[y][j-1]$。

对于第三种决策，如果暴力枚举 $y$，可以通过 [3176. 求出最长好子序列 I](https://leetcode.cn/problems/find-the-maximum-length-of-a-good-subsequence-i/)，但无法通过本题。

如何更快地计算最大的 $f[y][j-1]$ 呢？能否不暴力枚举 $y$？

## 优化

当 $j$ 为定值时，设：

- $\textit{mx} = f[\textit{num}][j-1]$ 是最大值。
- $\textit{mx}_2 = f[\textit{num}_2][j-1]$ 是次大值，其中 $\textit{num}_2\ne \textit{num}$。

于是：

- 如果 $x\ne \textit{num}$，那么最大的 $f[y][j-1]$ 就是 $\textit{mx}$。
- 如果 $x = \textit{num}$，那么最大的 $f[y][j-1]$ 就是 $\textit{mx}_2$。

这样我们就无需暴力枚举 $y$ 了。

把最大的 $f[y][j-1]$ 记作 $m$，则 $f[x][j]$ 更新为

$$
\max(f[x][j] + 1, m + 1)
$$

对于不同的 $j$，我们需要维护对应的 $\textit{mx},\textit{mx}_2,\textit{num}$。用一个长为 $k+1$ 的数组 $\textit{records}$ 记录。

由于在计算 $f[x][j]$ 时会用到 $\textit{records}[j-1]$，然后更新 $\textit{records}[j]$，可以倒序枚举 $j$，以避免使用覆盖后的数据。

```py [sol-Python3]
class Solution:
    def maximumLength(self, nums: List[int], k: int) -> int:
        fs = {}
        records = [[0] * 3 for _ in range(k + 1)]
        for x in nums:
            if x not in fs:
                fs[x] = [0] * (k + 1)
            f = fs[x]
            for j in range(k, -1, -1):
                f[j] += 1
                if j > 0:
                    mx, mx2, num = records[j - 1]
                    f[j] = max(f[j], (mx if x != num else mx2) + 1)

                # records[j] 维护 fs[.][j] 的 mx, mx2, num
                v = f[j]
                p = records[j]
                if v > p[0]:
                    if x != p[2]:
                        p[2] = x
                        p[1] = p[0]
                    p[0] = v
                elif x != p[2] and v > p[1]:
                    p[1] = v
        return records[k][0]
```

```java [sol-Java]
class Solution {
    public int maximumLength(int[] nums, int k) {
        Map<Integer, int[]> fs = new HashMap<>();
        int[][] records = new int[k + 1][3];
        for (int x : nums) {
            int[] f = fs.computeIfAbsent(x, i -> new int[k + 1]);
            for (int j = k; j >= 0; j--) {
                f[j]++;
                if (j > 0) {
                    int mx = records[j - 1][0], mx2 = records[j - 1][1], num = records[j - 1][2];
                    f[j] = Math.max(f[j], (x != num ? mx : mx2) + 1);
                }

                // records[j] 维护 fs[.][j] 的 mx, mx2, num
                int v = f[j];
                int[] p = records[j];
                if (v > p[0]) {
                    if (x != p[2]) {
                        p[2] = x;
                        p[1] = p[0];
                    }
                    p[0] = v;
                } else if (x != p[2] && v > p[1]) {
                    p[1] = v;
                }
            }
        }
        return records[k][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumLength(vector<int>& nums, int k) {
        unordered_map<int, vector<int>> fs;
        vector<array<int, 3>> records(k + 1);
        for (int x : nums) {
            auto& f = fs[x];
            f.resize(k + 1);
            for (int j = k; j >= 0; j--) {
                f[j]++;
                if (j) {
                    auto& r = records[j - 1];
                    int mx = r[0], mx2 = r[1], num = r[2];
                    f[j] = max(f[j], (x != num ? mx : mx2) + 1);
                }

                // records[j] 维护 fs[.][j] 的 mx, mx2, num
                int v = f[j];
                auto& p = records[j];
                if (v > p[0]) {
                    if (x != p[2]) {
                        p[2] = x;
                        p[1] = p[0];
                    }
                    p[0] = v;
                } else if (x != p[2] && v > p[1]) {
                    p[1] = v;
                }
            }
        }
        return records[k][0];
    }
};
```

```go [sol-Go]
func maximumLength(nums []int, k int) int {
	fs := map[int][]int{}
	records := make([]struct{ mx, mx2, num int }, k+1)
	for _, x := range nums {
		if fs[x] == nil {
			fs[x] = make([]int, k+1)
		}
		f := fs[x]
		for j := k; j >= 0; j-- {
			f[j]++
			if j > 0 {
				p := records[j-1]
				m := p.mx
				if x == p.num {
					m = p.mx2
				}
				f[j] = max(f[j], m+1)
			}

			// records[j] 维护 fs[.][j] 的 mx,mx2,num
			v := f[j]
			p := &records[j]
			if v > p.mx {
				if x != p.num {
					p.num = x
					p.mx2 = p.mx
				}
				p.mx = v
			} else if x != p.num && v > p.mx2 {
				p.mx2 = v
			}
		}
	}
	return records[k].mx
}
```

## 进一步优化

只需要维护 $\textit{mx}$，无需判断 $x$ 和子序列的最后一个数是否相等。因为：

- 如果 $x\ne \textit{num}$，那么转移和前文是一样的，用 $\textit{mx} + 1$ 更新 $f[x][j]$ 的最大值。
- 如果 $x = \textit{num}$，强行使用 $\textit{mx} + 1$，相当于用 $f[x][j-1] + 1$ 更新 $f[x][j]$ 的最大值。注意这个转移是不符合状态定义的（应该用 $f[x][j]+1$），但由于 $j$ 越大，能选的数越多，所以 $f[x][j]\ge f[x][j-1]$，考虑到第二种决策会用 $f[x][j] + 1$ 更新 $f[x][j]$，这不会低于 $f[x][j-1] + 1$。所以用 $f[x][j-1] + 1$ 更新 $f[x][j]$ 的最大值其实不会改变 $f[x][j]$。

综上所述，可以直接用 $\textit{mx}+1$ 更新 $f[x][j]$ 的最大值，无需考虑次大值 $\textit{mx}_2$。

上面代码中的 $\textit{records}$ 数组可以简化为 $\textit{mx}$ 数组。

上面代码需要判断 $j$ 和 $0$ 的大小关系，为避免判断，可以往 $\textit{mx}$ 数组的最左边插入一个 $0$，把 $\textit{mx}$ 的下标加一。此时 $\textit{mx}[j+1]$ 表示 $f[x][j]$ 的最大值。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1Tx4y1b7wk/) 第四题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def maximumLength(self, nums: List[int], k: int) -> int:
        fs = {}
        mx = [0] * (k + 2)
        for x in nums:
            if x not in fs:
                fs[x] = [0] * (k + 1)
            f = fs[x]
            for j in range(k, -1, -1):
                f[j] = max(f[j], mx[j]) + 1
                mx[j + 1] = max(mx[j + 1], f[j])
        return mx[-1]
```

```java [sol-Java]
class Solution {
    public int maximumLength(int[] nums, int k) {
        Map<Integer, int[]> fs = new HashMap<>();
        int[] mx = new int[k + 2];
        for (int x : nums) {
            int[] f = fs.computeIfAbsent(x, i -> new int[k + 1]);
            for (int j = k; j >= 0; j--) {
                f[j] = Math.max(f[j], mx[j]) + 1;
                mx[j + 1] = Math.max(mx[j + 1], f[j]);
            }
        }
        return mx[k + 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumLength(vector<int>& nums, int k) {
        unordered_map<int, vector<int>> fs;
        vector<int> mx(k + 2);
        for (int x : nums) {
            auto& f = fs[x];
            f.resize(k + 1);
            for (int j = k; j >= 0; j--) {
                f[j] = max(f[j], mx[j]) + 1;
                mx[j + 1] = max(mx[j + 1], f[j]);
            }
        }
        return mx[k + 1];
    }
};
```

```go [sol-Go]
func maximumLength(nums []int, k int) int {
	fs := map[int][]int{}
	mx := make([]int, k+2)
	for _, x := range nums {
		if fs[x] == nil {
			fs[x] = make([]int, k+1)
		}
		f := fs[x]
		for j := k; j >= 0; j-- {
			f[j] = max(f[j], mx[j]) + 1
			mx[j+1] = max(mx[j+1], f[j])
		}
	}
	return mx[k+1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(nk)$。

#### 相似题目

- [CF264C. Choosing Balls](https://codeforces.com/problemset/problem/264/C)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
