# 1041. 困于环中的机器人

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/robot-bounded-in-circle/
- 题目 slug：`robot-bounded-in-circle`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1521
- 外部题解来源：https://leetcode.cn/problems/robot-bounded-in-circle/solutions/3940215/jie-lun-ti-yong-fu-shu-zheng-ming-python-lvq4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[结论题，用复数证明（Python/Java/C++/Go）](https://leetcode.cn/problems/robot-bounded-in-circle/solutions/3940215/jie-lun-ti-yong-fu-shu-zheng-ming-python-lvq4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jie-lun-ti-yong-fu-shu-zheng-ming-python-lvq4`
- topic id：`3940215`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

## 定义

把机器人的位置看作复平面上的一个点 $z = x+iy$，其移动方向为复数 $d$。面向东时 $d=1$，面向北时 $d=i$，面向西时 $d=-1$，面向南时 $d=-i$。一开始 $d=i$。

- 直走：把 $z$ 增加 $d$。
- 左转：把 $d$ 乘以 $i$。例如面向北时 $d=i$，由于 $i^2=-1$，所以左转后，$d=-1$，即面向西。
- 右转：把 $d$ 乘以 $-i$。

## 再跑一轮？

设 $\textit{instructions}$ 执行完后，机器人的方向为 $d' = w\cdot d$，其中 $w\in\{1,i,-1,-i\}$。

我们来算算，再跑一轮 $\textit{instructions}$，机器人是如何移动的。

由于机器人的初始方向旋转了 $w$，所以第二轮的每一步都要旋转 $w$：

- 在第一轮朝北走的一步，在第二轮变成了朝「北$\times w$」走一步。
- 在第一轮朝东走的一步，在第二轮变成了朝「东$\times w$」走一步。其余方向同理。

设第一轮走了 $k$ 步，每次移动时增加的复数分别为 $v_1,v_2,\ldots,v_k$。把第一轮的总位移 $z_1$ 用这 $k$ 步之和表示，即：

$$
z_1 = v_1 + v_2 + \cdots + v_k
$$

那么第二轮的总位移 $z_2$ 就是这些被旋转后的复数之和：

$$
\begin{aligned}
z_2 &= wv_1 + wv_2 + \cdots + wv_k         \\
    &= w(v_1 + v_2 + \cdots + v_k)           \\
    &= wz_1           \\
\end{aligned}
$$

依此类推，可以得到第三轮的总位移 $z_3 = wz_2 = w^2z_1$，第四轮的总位移 $z_4 = wz_3 = w^3z_1$。

## 分类讨论

如果 $w=1$（机器人仍然面朝北方）：

- 如果 $z_1 = 0$，说明执行一轮指令后，机器人回到原点，永远无法离开。
- 如果 $z_1\ne 0$，那么执行 $n$ 轮指令后，机器人位于 $nz_1$。由于 $n$ 越大，$|nz_1|$ 也越大，所以机器人会离原点越来越远。

如果 $w\ne 1$ 呢？

累加前四轮的总位移，根据等比数列求和公式，得

$$
z_1+z_2+z_3+z_4 = (1+w+w^2+w^3)z_1 = \dfrac{1-w^4}{1-w}z_1
$$

由于 $w\in\{1,i,-1,-i\}$，所以 $w^4=1$，所以上式等于 $0$。

这意味着，如果 $w\ne 1$，那么机器人执行完四轮指令后，一定位于原点。

## 写法一：复数

```py [sol-Python3]
class Solution:
    def isRobotBounded(self, instructions: str) -> bool:
        z = 0j
        d = 1j  # 初始朝北
        for c in instructions:
            if c == 'G':
                z += d
            elif c == 'L':
                d *= 1j
            else:
                d *= -1j
        return d != 1j or z == 0j
```

```cpp [sol-C++]
class Solution {
public:
    bool isRobotBounded(string instructions) {
        complex z = 0i;
        complex d = 1i; // 初始朝北
        for (char c : instructions) {
            if (c == 'G') {
                z += d;
            } else if (c == 'L') {
                d *= 1i;
            } else {
                d *= -1i;
            }
        }
        return d != 1i || z == 0i;
    }
};
```

```go [sol-Go]
func isRobotBounded(instructions string) bool {
	z := 0i
	d := 1i // 初始朝北
	for _, c := range instructions {
		if c == 'G' {
			z += d
		} else if c == 'L' {
			d *= 1i
		} else {
			d *= -1i
		}
	}
	return d != 1i || z == 0
}
```

## 写法二：方向向量数组

用一个方向向量数组

$$
\textit{dirs} = [(0, 1), (1, 0), (0, -1), (-1, 0)]
$$

分别表示**顺时针**的上右下左（北东南西）四个方向。

用一个下标 $k$ 表示当前机器人的方向为 $\textit{dirs}[k]$，初始 $k=0$，表示初始方向为上。

- **右转**：也就是顺时针转 $90^\circ$，把 $k$ 增加一。如果 $k=4$，则绕回到 $\textit{dirs}$ 数组的最左边，即 $k$ 更新为 $0$。我们可以把 $k$ 统一更新为 $(k+1)\bmod 4$，这样可以兼容 $k=3$ 加一后变成 $0$ 的情况。
- **左转**：也就是逆时针转 $90^\circ$，把 $k$ 减少一。如果 $k=0$，则绕回到 $\textit{dirs}$ 数组的最右边，即 $k$ 更新为 $3$。我们可以把 $k$ 统一更新为 $(k+3)\bmod 4$，这是因为 $\textit{dirs}$ 是个**循环数组**，一个元素的左边相邻元素，相当于往右数 $3$ 个元素。比如 $\textit{dirs}$ 中的 $(1,0)$ 往右数 $3$ 个元素，就是 $(0,1)$。

```py [sol-Python3]
DIRS = (0, 1), (1, 0), (0, -1), (-1, 0)  # 上右下左（顺时针）

class Solution:
    def isRobotBounded(self, instructions: str) -> bool:
        x = y = k = 0
        for c in instructions:
            if c == 'G':
                x += DIRS[k][0]
                y += DIRS[k][1]
            elif c == 'R':
                k = (k + 1) % 4  # 右转
            else:
                k = (k + 3) % 4  # 左转
        return k != 0 or x == y == 0
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 上右下左（顺时针）

    public boolean isRobotBounded(String instructions) {
        int x = 0;
        int y = 0;
        int k = 0;
        for (char c : instructions.toCharArray()) {
            if (c == 'G') {
                x += DIRS[k][0];
                y += DIRS[k][1];
            } else if (c == 'R') {
                k = (k + 1) % 4; // 右转
            } else {
                k = (k + 3) % 4; // 左转
            }
        }
        return k != 0 || x == 0 && y == 0;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 上右下左（顺时针）

public:
    bool isRobotBounded(string instructions) {
        int x = 0, y = 0, k = 0;
        for (char c : instructions) {
            if (c == 'G') {
                x += dirs[k][0];
                y += dirs[k][1];
            } else if (c == 'R') {
                k = (k + 1) % 4; // 右转
            } else {
                k = (k + 3) % 4; // 左转
            }
        }
        return k != 0 || x == 0 && y == 0;
    }
};
```

```go [sol-Go]
type pair struct{ x, y int }
var dirs = [...]pair{{0, 1}, {1, 0}, {0, -1}, {-1, 0}} // 上右下左（顺时针）

func isRobotBounded(instructions string) bool {
	x, y, k := 0, 0, 0
	for _, c := range instructions {
		if c == 'G' {
			x += dirs[k].x
			y += dirs[k].y
		} else if c == 'R' {
			k = (k + 1) % 4 // 右转
		} else {
			k = (k + 3) % 4 // 左转
		}
	}
	return k != 0 || x == 0 && y == 0
}
```

$\texttt{R}$ 的 ASCII 值是 $82$，模 $4$，再加 $3$，得到 $5$。可以代替上面代码 `k = (k + 1) % 4` 中的 $1$。注意在模 $4$ 运算下，加 $1$ 和加 $5$ 是一样的。

$\texttt{L}$ 的 ASCII 值是 $76$，模 $4$，再加 $3$，得到 $3$。可以代替上面代码 `k = (k + 3) % 4` 中的 $3$。

所以左转右转可以统一成一个式子，无需分别判断。

```py [sol-Python3]
DIRS = (0, 1), (1, 0), (0, -1), (-1, 0)  # 上右下左（顺时针）

class Solution:
    def isRobotBounded(self, instructions: str) -> bool:
        x = y = k = 0
        for c in instructions:
            if c == 'G':
                x += DIRS[k][0]
                y += DIRS[k][1]
            else:
                k = (k + ord(c) + 3) % 4
        return k != 0 or x == y == 0
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 上右下左（顺时针）

    public boolean isRobotBounded(String instructions) {
        int x = 0;
        int y = 0;
        int k = 0;
        for (char c : instructions.toCharArray()) {
            if (c == 'G') {
                x += DIRS[k][0];
                y += DIRS[k][1];
            } else {
                k = (k + c + 3) % 4;
            }
        }
        return k != 0 || x == 0 && y == 0;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // 上右下左（顺时针）

public:
    bool isRobotBounded(string instructions) {
        int x = 0, y = 0, k = 0;
        for (char c : instructions) {
            if (c == 'G') {
                x += dirs[k][0];
                y += dirs[k][1];
            } else {
                k = (k + c + 3) % 4;
            }
        }
        return k != 0 || x == 0 && y == 0;
    }
};
```

```go [sol-Go]
type pair struct{ x, y int }
var dirs = [...]pair{{0, 1}, {1, 0}, {0, -1}, {-1, 0}} // 上右下左（顺时针）

func isRobotBounded(instructions string) bool {
	x, y, k := 0, 0, 0
	for _, c := range instructions {
		if c == 'G' {
			x += dirs[k].x
			y += dirs[k].y
		} else {
			k = (k + int(c) + 3) % 4
		}
	}
	return k != 0 || x == 0 && y == 0
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{instructions}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果还有旋转 $60^\circ$ 的指令呢？

如果还有旋转 $\theta^\circ$（其中 $\theta$ 是整数）的指令呢？如果 $\theta$ 是实数呢？

欢迎在评论区分享你的思考。

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

本题来自 `八、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
