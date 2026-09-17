# 955. 删列造序 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/delete-columns-to-make-sorted-ii/
- 题目 slug：`delete-columns-to-make-sorted-ii`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1876
- 外部题解来源：https://leetcode.cn/problems/delete-columns-to-make-sorted-ii/solutions/3854437/cong-zuo-dao-you-tan-xin-you-hua-pythonj-67o1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从左到右贪心 + 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/delete-columns-to-make-sorted-ii/solutions/3854437/cong-zuo-dao-you-tan-xin-you-hua-pythonj-67o1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-zuo-dao-you-tan-xin-you-hua-pythonj-67o1`
- topic id：`3854437`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

例如 $\textit{strs}=[\texttt{ac},\texttt{ad},\texttt{ba},\texttt{bb}]$，竖着看就是

$$
\begin{aligned}
& \texttt{ac}       \\
& \texttt{ad}       \\
& \texttt{ba}       \\
& \texttt{bb}       \\
\end{aligned}
$$

第一列是升序，可以不删。

- 如果删第一列，那么需要完整地比较第二列的四个字母是不是升序。
- 如果不删第一列，那么对于第二列，由于 $\texttt{d}$ 和 $\texttt{a}$ 前面的字母不同，只看第一列的字母就能确定 $\texttt{ad} < \texttt{ba}$，所以我们不需要比较 $\texttt{d}$ 和 $\texttt{a}$ 的大小。此时第二列分成了两组 $[\texttt{c},\texttt{d}]$ 和 $[\texttt{a},\texttt{b}]$，只需判断组内字母是不是升序，而不是完整地比较第二列的四个字母。

由此可见，当列已经是升序时，不删更好，后面需要比较的字母更少，更容易满足要求，最终删除的列更少。

如果列不是升序，那么一定要删（否则最终得到的数组不是字典序排列）。

## 优化前

```py [sol-Python3]
class Solution:
    def minDeletionSize(self, strs: List[str]) -> int:
        n, m = len(strs), len(strs[0])
        a = [''] * n  # 最终得到的字符串数组
        ans = 0
        for j in range(m):
            for i in range(n - 1):
                if a[i] + strs[i][j] > a[i + 1] + strs[i + 1][j]:
                    # j 列不是升序，必须删
                    ans += 1
                    break
            else:
                # j 列是升序，不删更好
                for i, s in enumerate(strs):
                    a[i] += s[j]
        return ans
```

```java [sol-Java]
class Solution {
    public int minDeletionSize(String[] strs) {
        int n = strs.length;
        int m = strs[0].length();
        String[] a = new String[n]; // 最终得到的字符串数组
        Arrays.fill(a, "");

        int ans = 0;
        next:
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n - 1; i++) {
                if ((a[i] + strs[i].charAt(j)).compareTo(a[i + 1] + strs[i + 1].charAt(j)) > 0) {
                    // j 列不是升序，必须删
                    ans++;
                    continue next;
                }
            }
            // j 列是升序，不删更好
            for (int i = 0; i < n; i++) {
                a[i] += strs[i].charAt(j);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDeletionSize(vector<string>& strs) {
        int n = strs.size(), m = strs[0].size();
        vector<string> a(n); // 最终得到的字符串数组
        int ans = 0;
        for (int j = 0; j < m; j++) {
            bool del = false;
            for (int i = 0; i < n - 1; i++) {
                if (a[i] + strs[i][j] > a[i + 1] + strs[i + 1][j]) {
                    // j 列不是升序，必须删
                    ans++;
                    del = true;
                    break;
                }
            }
            if (!del) {
                // j 列是升序，不删更好
                for (int i = 0; i < n; i++) {
                    a[i] += strs[i][j];
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minDeletionSize(strs []string) (ans int) {
	n, m := len(strs), len(strs[0])
	a := make([]string, n) // 最终得到的字符串数组
next:
	for j := range m {
		for i := range n - 1 {
			if a[i]+string(strs[i][j]) > a[i+1]+string(strs[i+1][j]) {
				// j 列不是升序，必须删
				ans++
				continue next
			}
		}
		// j 列是升序，不删更好
		for i, s := range strs {
			a[i] += string(s[j])
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm^2)$，其中 $n$ 是 $\textit{strs}$ 的长度，$m$ 是 $\textit{strs}[i]$ 的长度。比较 $\mathcal{O}(nm)$ 次大小，每次 $\mathcal{O}(m)$。
- 空间复杂度：$\mathcal{O}(nm)$。

## 优化

回顾前文的例子：

$$
\begin{aligned}
& \texttt{ac}       \\
& \texttt{ad}       \\
& \texttt{ba}       \\
& \texttt{bb}       \\
\end{aligned}
$$

第一列升序，不删。由于 $\textit{strs}[1][0] < \textit{strs}[2][0]$，后续 $a[1] < a[2]$ 必定成立，所以不需要比较这两个字符串。对于其余相邻字符串来说，由于第一列的字母都一样，所以**只需比较第二列的字母，无需比较整个字符串**。

怎么维护需要比较的下标（行号）呢？可以用哈希集合，或者布尔数组，或者创建一个下标列表，删除列表中的无需比较的下标。最后一种方法最高效，我们可以用 [27. 移除元素](https://leetcode.cn/problems/remove-element/) 的方法，原地删除无需比较的下标，见 [我的题解](https://leetcode.cn/problems/remove-element/solutions/2802809/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-72bn/)。

```py [sol-Python3]
class Solution:
    def minDeletionSize(self, strs: List[str]) -> int:
        n, m = len(strs), len(strs[0])
        check_list = list(range(n - 1))

        ans = 0
        for j in range(m):
            for i in check_list:
                if strs[i][j] > strs[i + 1][j]:
                    # j 列不是升序，必须删
                    ans += 1
                    break
            else:
                # j 列是升序，不删更好
                new_size = 0
                for i in check_list:
                    if strs[i][j] == strs[i + 1][j]:
                        # 相邻字母相等，下一列 i 和 i+1 需要继续比大小
                        check_list[new_size] = i  # 原地覆盖
                        new_size += 1
                del check_list[new_size:]
        return ans
```

```java [sol-Java]
class Solution {
    public int minDeletionSize(String[] strs) {
        int n = strs.length;
        int m = strs[0].length();
        int size = n - 1;
        int[] checkList = new int[size];
        for (int i = 0; i < size; i++) {
            checkList[i] = i;
        }

        int ans = 0;
        next:
        for (int j = 0; j < m; j++) {
            for (int t = 0; t < size; t++) {
                int i = checkList[t];
                if (strs[i].charAt(j) > strs[i + 1].charAt(j)) {
                    // j 列不是升序，必须删
                    ans++;
                    continue next;
                }
            }
            // j 列是升序，不删更好
            int newSize = 0;
            for (int t = 0; t < size; t++) {
                int i = checkList[t];
                if (strs[i].charAt(j) == strs[i + 1].charAt(j)) {
                    // 相邻字母相等，下一列 i 和 i+1 需要继续比大小
                    checkList[newSize++] = i; // 原地覆盖
                }
            }
            size = newSize;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDeletionSize(vector<string>& strs) {
        int n = strs.size(), m = strs[0].size();
        vector<int> check_list(n - 1);
        ranges::iota(check_list, 0);

        int ans = 0;
        for (int j = 0; j < m; j++) {
            bool del = false;
            for (int i : check_list) {
                if (strs[i][j] > strs[i + 1][j]) {
                    // j 列不是升序，必须删
                    ans++;
                    del = true;
                    break;
                }
            }
            if (del) {
                continue;
            }
            // j 列是升序，不删更好
            int new_size = 0;
            for (int i : check_list) {
                if (strs[i][j] == strs[i + 1][j]) {
                    // 相邻字母相等，下一列 i 和 i+1 需要继续比大小
                    check_list[new_size++] = i; // 原地覆盖
                }
            }
            check_list.resize(new_size);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minDeletionSize(strs []string) (ans int) {
	n, m := len(strs), len(strs[0])
	checkList := make([]int, n-1)
	for i := range checkList {
		checkList[i] = i
	}

next:
	for j := range m {
		for _, i := range checkList {
			if strs[i][j] > strs[i+1][j] {
				// j 列不是升序，必须删
				ans++
				continue next
			}
		}
		// j 列是升序，不删更好
		newCheckList := checkList[:0] // 原地
		for _, i := range checkList {
			if strs[i][j] == strs[i+1][j] {
				// 相邻字母相等，下一列 i 和 i+1 需要继续比大小
				newCheckList = append(newCheckList, i)
			}
		}
		checkList = newCheckList
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $\textit{strs}$ 的长度，$m$ 是 $\textit{strs}[i]$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
