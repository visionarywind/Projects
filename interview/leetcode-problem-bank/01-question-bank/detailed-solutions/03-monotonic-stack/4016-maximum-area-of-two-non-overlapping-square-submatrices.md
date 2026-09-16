# 4016. 两个不重叠子正方形的最大面积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-area-of-two-non-overlapping-square-submatrices/
- 题目 slug：`maximum-area-of-two-non-overlapping-square-submatrices`
- 来源专题：单调栈
- 来源分类路径：二、矩形
- 难度分：1959
- 外部题解来源：https://leetcode.cn/problems/maximum-area-of-two-non-overlapping-square-submatrices/solutions/4009076/qian-hou-zhui-fen-jie-zhuan-hua-cheng-22-entn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解，转化成 221 题（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-area-of-two-non-overlapping-square-submatrices/solutions/4009076/qian-hou-zhui-fen-jie-zhuan-hua-cheng-22-entn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-zhuan-hua-cheng-22-entn`
- topic id：`4009076`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

两个不重叠的正方形，要么一左一右，要么一上一下。

比如一上一下的情况，假设两个正方形之间有条水平分割线，位于 $i$ 行和 $i+1$ 行之间，那么问题变成两个互相独立的子问题：

- 计算 $\textit{mat}$ 的 $[0,i]$ 行中的 [221. 最大正方形](https://leetcode.cn/problems/maximal-square/)。
- 计算 $\textit{mat}$ 的 $[i+1,m-1]$ 行中的 [221. 最大正方形](https://leetcode.cn/problems/maximal-square/)。

取两个正方形的面积的较小值，即为一上一下情况中的单个正方形的最大面积。

> **注**：如果存在边长为 $k$ 的全 $\texttt{1}$ 正方形，那么也存在边长小于 $k$ 的全 $\texttt{1}$ 正方形。

一左一右的做法类似，枚举垂直分割线的位置。代码实现时，可以直接把 $\textit{mat}$ 转置（或者旋转 $90^\circ$），从而复用同一段代码。

对于 221 题，可以用动态规划，或者单调栈，或者二分答案+二维前缀和等。下面用的动态规划写法（空间优化），来自 [我的题解](https://leetcode.cn/problems/maximal-square/solutions/3704858/he-85-ti-yi-yang-de-zuo-fa-pythonjavaccg-az54/)。

[本题视频讲解](https://www.bilibili.com/video/BV1ryuy6WEDs/?t=9m47s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def calc(self, mat: list[list[int]]) -> int:
        m, n = len(mat), len(mat[0])

        # 221. 最大正方形（空间优化写法）
        # 计算 mat 下半部分的最大正方形的边长
        suf_max = [0] * (m + 1)
        f = [0] * (n + 1)
        for i in range(m - 1, 0, -1):
            last = 0
            for j, x in enumerate(mat[i]):
                if x:
                    tmp = f[j + 1]
                    f[j + 1] = min(last, f[j + 1], f[j]) + 1
                    last = tmp
                else:
                    f[j + 1] = 0
                    last = 0
            suf_max[i] = max(suf_max[i + 1], max(f))

        # 计算 mat 上半部分的最大正方形的边长
        ans = pre_max = 0
        f = [0] * (n + 1)
        for i, row in enumerate(mat):
            last = 0
            for j, x in enumerate(row):
                if x:
                    tmp = f[j + 1]
                    f[j + 1] = min(last, f[j + 1], f[j]) + 1
                    last = tmp
                else:
                    f[j + 1] = 0
                    last = 0
            if suf_max[i + 1] <= ans:
                break  # 最优性优化：继续循环不会让 ans 变大
            pre_max = max(pre_max, max(f))
            ans = max(ans, min(pre_max, suf_max[i + 1]))  # 题目要求两个正方形的边长相等

        return ans * ans

    def maxArea(self, mat: list[list[int]]) -> int:
        return max(self.calc(mat), self.calc(list(zip(*mat))))
```

```java [sol-Java]
class Solution {
    public int maxArea(int[][] mat) {
        return Math.max(calc(mat), calc(transpose(mat)));
    }

    private int calc(int[][] mat) {
        int m = mat.length;
        int n = mat[0].length;

        // 221. 最大正方形（空间优化写法）
        // 计算 mat 下半部分的最大正方形的边长
        int[] sufMax = new int[m];
        int[] f = new int[n + 1];
        int mx = 0;
        for (int i = m - 1; i > 0; i--) {
            int last = 0;
            for (int j = 0; j < n; j++) {
                int x = mat[i][j];
                if (x == 1) {
                    int tmp = f[j + 1];
                    f[j + 1] = Math.min(Math.min(last, f[j + 1]), f[j]) + 1;
                    last = tmp;
                    mx = Math.max(mx, f[j + 1]);
                } else {
                    f[j + 1] = 0;
                    last = 0;
                }
            }
            sufMax[i] = mx;
        }

        int ans = 0;
        // 计算 mat 上半部分的最大正方形的边长
        int preMax = 0;
        Arrays.fill(f, 0);
        for (int i = 0; i < m - 1; i++) {
            int last = 0;
            for (int j = 0; j < n; j++) {
                int x = mat[i][j];
                if (x == 1) {
                    int tmp = f[j + 1];
                    f[j + 1] = Math.min(Math.min(last, f[j + 1]), f[j]) + 1;
                    last = tmp;
                    preMax = Math.max(preMax, f[j + 1]);
                } else {
                    f[j + 1] = 0;
                    last = 0;
                }
            }
            if (sufMax[i + 1] <= ans) {
                break; // 最优性优化：继续循环不会让 ans 变大
            }
            ans = Math.max(ans, Math.min(preMax, sufMax[i + 1])); // 题目要求两个正方形的边长相等
        }

        return ans * ans;
    }

    // 转置矩阵 mat
    private int[][] transpose(int[][] mat) {
        int m = mat.length;
        int n = mat[0].length;
        int[][] a = new int[n][m];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                a[i][j] = mat[j][i];
            }
        }
        return a;
    }
}
```

```cpp [sol-C++]
class Solution {
    int calc(const vector<vector<int>>& mat) {
        int m = mat.size(), n = mat[0].size();

        // 221. 最大正方形（空间优化写法）
        // 计算 mat 下半部分的最大正方形的边长
        vector<int> suf_max(m);
        vector<int> f(n + 1);
        int mx = 0;
        for (int i = m - 1; i > 0; i--) {
            int last = 0;
            for (int j = 0; j < n; j++) {
                if (mat[i][j]) {
                    int tmp = f[j + 1];
                    f[j + 1] = min(min(last, f[j + 1]), f[j]) + 1;
                    last = tmp;
                    mx = max(mx, f[j + 1]);
                } else {
                    f[j + 1] = 0;
                    last = 0;
                }
            }
            suf_max[i] = mx;
        }

        int ans = 0;
        // 计算 mat 上半部分的最大正方形的边长
        int pre_max = 0;
        ranges::fill(f, 0);
        for (int i = 0; i < m - 1; i++) {
            int last = 0;
            for (int j = 0; j < n; j++) {
                if (mat[i][j]) {
                    int tmp = f[j + 1];
                    f[j + 1] = min(min(last, f[j + 1]), f[j]) + 1;
                    last = tmp;
                    pre_max = max(pre_max, f[j + 1]);
                } else {
                    f[j + 1] = 0;
                    last = 0;
                }
            }
            if (suf_max[i + 1] <= ans) {
                break; // 最优性优化：继续循环不会让 ans 变大
            }
            ans = max(ans, min(pre_max, suf_max[i + 1])); // 题目要求两个正方形的边长相等
        }

        return ans * ans;
    }

    // 转置矩阵 mat
    vector<vector<int>> transpose(vector<vector<int>>& mat) {
        int m = mat.size(), n = mat[0].size();
        vector a(n, vector<int>(m));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < mat.size(); j++) {
                a[i][j] = mat[j][i];
            }
        }
        return a;
    }

public:
    int maxArea(vector<vector<int>>& mat) {
        return max(calc(mat), calc(transpose(mat)));
    }
};
```

```go [sol-Go]
func calc(mat [][]int) int {
	m, n := len(mat), len(mat[0])

	// 221. 最大正方形（空间优化写法）
	// 计算 mat 下半部分的最大正方形的边长
	sufMax := make([]int, m)
	f := make([]int, n+1)
	mx := 0
	for i := m - 1; i > 0; i-- {
		last := 0
		for j, x := range mat[i] {
			if x == 1 {
				tmp := f[j+1]
				f[j+1] = min(last, f[j+1], f[j]) + 1
				last = tmp
				mx = max(mx, f[j+1])
			} else {
				f[j+1] = 0
				last = 0
			}
		}
		sufMax[i] = mx
	}

	ans := 0
	// 计算 mat 上半部分的最大正方形的边长
	preMax := 0
	clear(f)
	for i, row := range mat[:m-1] {
		last := 0
		for j, x := range row {
			if x == 1 {
				tmp := f[j+1]
				f[j+1] = min(last, f[j+1], f[j]) + 1
				last = tmp
				preMax = max(preMax, f[j+1])
			} else {
				f[j+1] = 0
				last = 0
			}
		}
		if sufMax[i+1] <= ans {
			break // 最优性优化：继续循环不会让 ans 变大
		}
		ans = max(ans, min(preMax, sufMax[i+1])) // 题目要求两个正方形的边长相等
	}

	return ans * ans
}

// 转置矩阵 mat
func transpose(mat [][]int) [][]int {
	m, n := len(mat), len(mat[0])
	a := make([][]int, n)
	for i := range a {
		a[i] = make([]int, m)
		for j, row := range mat {
			a[i][j] = row[i]
		}
	}
	return a
}

func maxArea(mat [][]int) int {
	return max(calc(mat), calc(transpose(mat)))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{mat}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$ 或 $\mathcal{O}(m+n)$。如果不转置矩阵，则可以做到 $\mathcal{O}(m+n)$ 的空间复杂度。

## 相似题目

[3197. 包含所有 1 的最小矩形面积 II](https://leetcode.cn/problems/find-the-minimum-area-to-cover-all-ones-ii/)

## 专题训练

1. 动态规划题单的「**专题：前后缀分解**」和「**§7.5 子矩形 DP**」。
2. 单调栈题单的「**二、矩形**」

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、矩形`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、矩形`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
