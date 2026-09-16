# 3858. 按位或的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-bitwise-or-from-grid/
- 题目 slug：`minimum-bitwise-or-from-grid`
- 来源专题：位运算
- 来源分类路径：五、试填法
- 难度分：1947
- 外部题解来源：https://leetcode.cn/problems/minimum-bitwise-or-from-grid/solutions/3910784/shi-tian-fa-pythonjavacgo-by-endlesschen-b3zh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[试填法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-bitwise-or-from-grid/solutions/3910784/shi-tian-fa-pythonjavacgo-by-endlesschen-b3zh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-tian-fa-pythonjavacgo-by-endlesschen-b3zh`
- topic id：`3910784`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

要想让答案尽量小，那么答案二进制的高位是 $0$ 比是 $1$ 更好，所以优先判断答案的高位能否是 $0$，即**从高到低**依次判断答案的第 $i$ 位能不能是 $0$。

如果在每一行的**能选的数字**中，都存在二进制第 $i$ 位是 $0$ 的数，那么答案的第 $i$ 位可以是 $0$，否则必须是 $1$。

怎么判断一个数能不能选？

比如答案现在是 $101\texttt{\_}\texttt{\_}$，那么 $\textit{grid}[i][j]$ 绝对不能是 $\texttt{\_}1\texttt{\_}\texttt{\_}\texttt{\_}$，这会导致 OR 的结果是 $\texttt{\_}1\texttt{\_}\texttt{\_}\texttt{\_}$，不符合目前的答案。

一般地，对于答案中的是 $0$ 的比特位，如果 $\textit{grid}[i][j]$ 二进制同一位上是 $1$，那么 $\textit{grid}[i][j]$ 不能选，否则可以选。从集合的角度理解，对于目前已经确定的比特位，$\textit{grid}[i][j]$ 必须是答案的**子集**。具体请看 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

[本题视频讲解](https://www.bilibili.com/video/BV1V4PMzrEYG/?t=9m50s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minimumOR(self, grid: List[List[int]]) -> int:
        # 每行选个最小值，计算 OR，答案 <= OR（答案的二进制长度至多为这些数的最大值的二进制长度）
        mx = max(map(min, grid))  

        ans = 0
        # 试填法：ans 的第 i 位能不能是 0？
        # 如果在每一行的能选的数字中，都存在第 i 位是 0 的数，那么 ans 的第 i 位可以是 0，否则必须是 1
        for i in range(mx.bit_length() - 1, -1, -1):
            mask = ans | ((1 << i) - 1)  # mask 低于 i 的比特位全是 1，表示 grid[i][j] 的低位是 0 还是 1 无所谓
            for row in grid:
                for x in row:
                    # x 的高于 i 的比特位，如果 mask 是 0，那么 x 的这一位必须也是 0（注意 mask 继承了 ans 高位中的 0）
                    # x 的低于 i 的比特位，随意
                    # x 的第 i 个比特位，我们期望它是 0
                    if (x | mask) == mask:  # x 可以选，且第 i 位是 0
                        break
                else:  # 这一行的可选数字中，第 i 位全是 1
                    ans |= 1 << i  # ans 第 i 位必须是 1
                    break  # 填下一位
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumOR(int[][] grid) {
        int or = 0;
        for (int[] row : grid) {
            // 每行选个最小值，计算 OR
            int mn = Integer.MAX_VALUE;
            for (int x : row) {
                mn = Math.min(mn, x);
            }
            or |= mn;
        }
        // 答案 <= or，那么答案的二进制长度也 <= or 的二进制长度
        int bitLength = 32 - Integer.numberOfLeadingZeros(or);

        int ans = 0;
        // 试填法：ans 的第 i 位能不能是 0？
        // 如果在每一行的能选的数字中，都存在第 i 位是 0 的数，那么 ans 的第 i 位可以是 0，否则必须是 1
        for (int i = bitLength - 1; i >= 0; i--) {
            int mask = ans | ((1 << i) - 1); // mask 低于 i 的比特位全是 1，表示 grid[i][j] 的低位是 0 还是 1 无所谓
            next:
            for (int[] row : grid) {
                for (int x : row) {
                    // x 的高于 i 的比特位，如果 mask 是 0，那么 x 的这一位必须也是 0（注意 mask 继承了 ans 高位中的 0）
                    // x 的低于 i 的比特位，随意
                    // x 的第 i 个比特位，我们期望它是 0
                    if ((x | mask) == mask) { // x 可以选，且第 i 位是 0
                        continue next;
                    }
                }
                // 这一行的可选数字中，第 i 位全是 1
                ans |= 1 << i; // ans 第 i 位必须是 1
                break; // 填下一位
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumOR(vector<vector<int>>& grid) {
        int or_ = 0;
        for (auto& row : grid) {
            // 每行选个最小值，计算 OR
            or_ |= ranges::min(row);
        }
        // 答案 <= or，那么答案的二进制长度也 <= or 的二进制长度

        int ans = 0;
        // 试填法：ans 的第 i 位能不能是 0？
        // 如果在每一行的能选的数字中，都存在第 i 位是 0 的数，那么 ans 的第 i 位可以是 0，否则必须是 1
        for (int i = bit_width((uint32_t) or_) - 1; i >= 0; i--) {
            int mask = ans | ((1 << i) - 1); // mask 低于 i 的比特位全是 1，表示 grid[i][j] 的低位是 0 还是 1 无所谓
            for (auto& row : grid) {
                bool found0 = false;
                for (int x : row) {
                    // x 的高于 i 的比特位，如果 mask 是 0，那么 x 的这一位必须也是 0（注意 mask 继承了 ans 高位中的 0）
                    // x 的低于 i 的比特位，随意
                    // x 的第 i 个比特位，我们期望它是 0
                    if ((x | mask) == mask) { // x 可以选，且第 i 位是 0
                        found0 = true;
                        break;
                    }
                }
                if (!found0) { // 这一行的可选数字中，第 i 位全是 1
                    ans |= 1 << i; // ans 第 i 位必须是 1
                    break; // 填下一位
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumOR(grid [][]int) (ans int) {
	or := 0
	for _, row := range grid {
		// 每行选个最小值，计算 OR
		or |= slices.Min(row)
	}
	// 答案 <= or，那么答案的二进制长度也 <= or 的二进制长度

	// 试填法：ans 的第 i 位能不能是 0？
	// 如果在每一行的能选的数字中，都存在第 i 位是 0 的数，那么 ans 的第 i 位可以是 0，否则必须是 1
	for i := bits.Len(uint(or)) - 1; i >= 0; i-- {
		mask := ans | (1<<i - 1) // mask 低于 i 的比特位全是 1，表示 grid[i][j] 的低位是 0 还是 1 无所谓
	next:
		for _, row := range grid {
			for _, x := range row {
				// x 的高于 i 的比特位，如果 mask 是 0，那么 x 的这一位必须也是 0（注意 mask 继承了 ans 高位中的 0）
				// x 的低于 i 的比特位，随意
				// x 的第 i 个比特位，我们期望它是 0
				if x|mask == mask { // x 可以选，且第 i 位是 0
					continue next
				}
			}
			// 这一行的可选数字中，第 i 位全是 1
			ans |= 1 << i // ans 第 i 位必须是 1
			break // 填下一位
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log U)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数，$U$ 是所有 $\textit{grid}[i][j]$ 中的最大值。注意题目保证 $m\cdot n\le 10^5$。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

改成求 OR 的最大值，怎么做？

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面位运算题单的「**五、试填法**」。

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

本题来自 `五、试填法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、试填法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
