# 3609. 到达目标点的最小移动次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-reach-target-in-grid/
- 题目 slug：`minimum-moves-to-reach-target-in-grid`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：2419
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-reach-target-in-grid/solutions/3716440/ni-xiang-si-wei-fen-lei-tao-lun-yan-ge-z-m5cc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维 + 分类讨论，严格证明（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-moves-to-reach-target-in-grid/solutions/3716440/ni-xiang-si-wei-fen-lei-tao-lun-yan-ge-z-m5cc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-fen-lei-tao-lun-yan-ge-z-m5cc`
- topic id：`3716440`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

正着走，不知道该执行哪个操作，但倒着走就知道了。为什么？且听我细说~

从终点 $(x,y)$ 倒着走到起点 $(\textit{sx}, \textit{sy})$。

不失一般性，假设 $x\ge y$ 且 $x>0$。如果 $x<y$，则交换 $x$ 和 $y$，以及交换 $\textit{sx}$ 和 $\textit{sy}$。假设 $x>0$ 是因为对于 $x=y=0$ 的情况我们可以特判。

> **注**：为什么可以交换？因为 $x$ 与 $y$ 是**无序**的，计算 $\max(x,y)$ 是无序的，操作 $(x+m,y)$ 还是 $(x,y+m)$ 都是把其中一个数增加 $m$，也是无序的。所以 $(x,y)$ 与 $(y,x)$ 是等价的，计算出的答案是一样的。如果 $x<y$，把每个点 $(x,y)$ 变换到直线 $y=x$ 的镜像位置 $(y,x)$。 

从 $(x,y)$ 倒退一步，上一步在哪？上一步执行了什么操作？

有四种情况：

- 执行了操作一（横坐标加横坐标，或者横坐标加纵坐标）：
   - 上一步把横坐标乘以 $2$，换句话说，上一步在 $(x/2, y)$。前提条件：$x$ 是偶数且 $\max(x/2,y)=x/2$，即 $x\ge 2y$。
   - 上一步把横坐标加上了 $y$，换句话说，上一步在 $(x-y, y)$。前提条件：$\max(x-y,y)=y$，即 $x\le 2y$。
- 执行了操作二（纵坐标加纵坐标，或者纵坐标加横坐标），类似地：
   - 上一步在 $(x, y/2)$。前提条件：$y$ 是偶数且 $\max(x,y/2)=y/2$，即 $x \le y/2$，结合 $x\ge y$ 的假设，得 $y\le y/2$，这只在 $y=0$ 时才成立（不考虑负数）。代入 $y\le x \le y/2$，得 $0\le x\le 0$，即 $x=0$。这与 $x>0$ 的假设矛盾，所以无需考虑这种情况。
   - 上一步在 $(x, y-x)$。由于 $y-x\le 0$ 且本题不需要考虑负数，所以只有当 $x=y$ 时，才能这么操作。由于 $x>0$，此时 $x=y>0$。

根据上述讨论，从 $(x,y)$ 倒退一步的方法如下：

- 如果 $x=y$：
    - 当 $\textit{sy}>0$ 的时候，只能把 $x$ 变成 $0$（把 $y$ 变成 $0$ 无法回到起点）。
    - 当 $\textit{sx}>0$ 的时候，只能把 $y$ 变成 $0$（把 $x$ 变成 $0$ 无法回到起点）。
    - 无需判断 $\textit{sx}=\textit{sy}=0$ 的情况，$x$ 或 $y$ 在减半的过程中一定会变成奇数，我们会返回 $-1$。
- 否则 $x>y$，此时只能执行操作一：
    - 如果 $x> 2y$，只能把 $x$ 减半。
    - 如果 $x< 2y$，只能把 $x$ 减少 $y$。
    - 如果 $x=2y$，二者效果一样，都会把 $x$ 变成 $y$。

按照上述规则模拟，每倒退一步，把答案加一。**注意这不是贪心，是纯模拟**。换句话说，如果我们没有返回 $-1$，那么起点与终点之间的走法是**唯一**的。

边界情况：

- 如果 $x<\textit{sx}$ 或 $y<\textit{sy}$，走过头了，返回 $-1$。
- 如果 $x=\textit{sx}$ 且 $y=\textit{sy}$，返回答案。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1GF3qzMEni/?t=32m18s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minMoves(self, sx: int, sy: int, x: int, y: int) -> int:
        ans = 0
        while x != sx or y != sy:
            if x < sx or y < sy:
                return -1
            ans += 1
            if x == y:
                if sy > 0:
                    x = 0
                else:
                    y = 0
                continue
            # 保证 x > y
            if x < y:
                x, y = y, x
                sx, sy = sy, sx
            if x > y * 2:
                if x % 2 > 0:
                    return -1
                x //= 2
            else:
                x -= y
        return ans
```

```java [sol-Java]
class Solution {
    public int minMoves(int sx, int sy, int x, int y) {
        int ans = 0;
        for (; x != sx || y != sy; ans++) {
            if (x < sx || y < sy) {
                return -1;
            }
            if (x == y) {
                if (sy > 0) {
                    x = 0;
                } else {
                    y = 0;
                }
                continue;
            }
            // 保证 x > y
            if (x < y) {
                int tmp = x;
                x = y;
                y = tmp;

                tmp = sx;
                sx = sy;
                sy = tmp;
            }
            if (x > y * 2) {
                if (x % 2 > 0) {
                    return -1;
                }
                x /= 2;
            } else {
                x -= y;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minMoves(int sx, int sy, int x, int y) {
        int ans = 0;
        for (; x != sx || y != sy; ans++) {
            if (x < sx || y < sy) {
                return -1;
            }
            if (x == y) {
                if (sy > 0) {
                    x = 0;
                } else {
                    y = 0;
                }
                continue;
            }
            // 保证 x > y
            if (x < y) {
                swap(x, y);
                swap(sx, sy);
            }
            if (x > y * 2) {
                if (x % 2 > 0) {
                    return -1;
                }
                x /= 2;
            } else {
                x -= y;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minMoves(sx, sy, x, y int) (ans int) {
	for ; x != sx || y != sy; ans++ {
		if x < sx || y < sy {
			return -1
		}
		if x == y {
			if sy > 0 {
				x = 0
			} else {
				y = 0
			}
			continue
		}
		// 保证 x > y
		if x < y {
			x, y = y, x
			sx, sy = sy, sx
		}
		if x > y*2 {
			if x%2 > 0 {
				return -1
			}
			x /= 2
		} else {
			x -= y
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log(\textit{tx}+\textit{ty}))$。$x>2y$ 时把 $x$ 减半；$x\le 2y$ 时把 $x$ 减去 $y$，减去的 $y \ge x/2$。所以 $x$ 至少减半。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [780. 到达终点](https://leetcode.cn/problems/reaching-points/)
- [2543. 判断一个点是否可以到达](https://leetcode.cn/problems/check-if-point-is-reachable/)

另见下面贪心与思维题单的「**§5.3 逆向思维**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.4 逆向思维`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.4 逆向思维`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
