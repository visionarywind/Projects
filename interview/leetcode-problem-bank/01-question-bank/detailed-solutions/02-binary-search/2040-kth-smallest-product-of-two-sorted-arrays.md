# 2040. 两个有序数组的第 K 小乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-smallest-product-of-two-sorted-arrays/
- 题目 slug：`kth-smallest-product-of-two-sorted-arrays`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.6 第 K 小/大
- 难度分：2518
- 外部题解来源：https://leetcode.cn/problems/kth-smallest-product-of-two-sorted-arrays/solutions/1050962/fen-lei-tao-lun-er-fen-da-an-by-endlessc-nl81/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[第 k 小/大问题的通用转换方法（Python/Java/C++/Go）](https://leetcode.cn/problems/kth-smallest-product-of-two-sorted-arrays/solutions/1050962/fen-lei-tao-lun-er-fen-da-an-by-endlessc-nl81/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-er-fen-da-an-by-endlessc-nl81`
- topic id：`1050962`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

第 $k$ 小/大问题的**通用转换方法**：

- 第 $k$ 小等价于：求最小的 $x$，满足 $\le x$ 的数**至少**有 $k$ 个。（注意是至少不是恰好）
- 第 $k$ 大等价于：求最大的 $x$，满足 $\ge x$ 的数**至少**有 $k$ 个。

对于本题，$x$ 越大，$\le x$ 的数越多；$x$ 越小，$\le x$ 的数越少。据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在本题转化成一个判定性问题：

- 给定整数 $\textit{mx}$，统计 $\le \textit{mx}$ 的乘积个数 $\textit{cnt}$，判断是否满足 $\textit{cnt}\ge k$。

如何高效统计 $\textit{cnt}$ 呢？

为方便描述，下文把 $\textit{nums}_1$ 记作 $a$，把 $\textit{nums}_2$ 记作 $b$。

看示例 3，定义 $\textit{matrix}[i][j] = a[i]\cdot b[j]$，得到如下矩阵

$$
\begin{bmatrix}
6 & 2 & -4 & -8 & -10   \\
3 & 1 & -2 & -4 & -5    \\
0 & 0 & 0 & 0 & 0       \\
-3 & -1 & 2 & 4 & 5     \\
-6 & -2 & 4 & 8 & 10    \\
\end{bmatrix}
$$

按照元素正负，把矩阵分成如下四个区域（人为规定 $0$ 分到下面两个区域）

$$
\begin{array}{cc|cc}
6 & 2 & -4 & -8 & -10   \\
3 & 1 & -2 & -4 & -5    \\ \hline
0 & 0 & 0 & 0 & 0       \\
-3 & -1 & 2 & 4 & 5     \\
-6 & -2 & 4 & 8 & 10    \\
\end{array}
$$

其中右下区域

$$
\begin{array}{}
0 & 0 & 0       \\
2 & 4 & 5     \\
4 & 8 & 10    \\
\end{array}
$$

每行每列都是有序的，和 [378. 有序矩阵中第 K 小的元素](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/) 完全一样，都可以用**双指针**解决，见 [图解](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/solutions/3699846/tu-jie-di-k-xiao-da-wen-ti-de-tong-yong-teznd/)。

其余三个区域也都是有序的，只是递增的方向不同，都可以用双指针解决。

最后，确定二分的左右边界。我们需要知道矩阵的最小值和最大值，这一定来自矩阵的四个角，即 $a[0],a[n-1]$ 与 $b[0],b[m-1]$ 的两两乘积，一共 $4$ 种情况。

## 答疑

**问**：为什么二分结束后，答案 $\textit{ans}$ 一定在矩阵中？

**答**：反证法。假设 $\textit{ans}$ 不在矩阵中，这意味着矩阵中第 $k$ 小的数比 $\textit{ans}$ 小，或者说 $\le \textit{ans}-1$。换句话说，$\le \textit{ans}-1$ 的数有 $k$ 个，即 $\text{check}(\textit{ans}-1)=\texttt{true}$。但根据循环不变量，二分结束后 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾。故原命题成立。

```py [sol-Python3]
class Solution:
    def kthSmallestProduct(self, a: List[int], b: List[int], k: int) -> int:
        i0 = bisect_left(a, 0)  # 四个区域的水平分界线
        j0 = bisect_left(b, 0)  # 四个区域的垂直分界线

        def check(mx: int) -> bool:
            if mx < 0:
                cnt = 0

                # 右上区域
                i, j = 0, j0
                while i < i0 and j < m:  # 不判断 cnt < k 更快
                    if a[i] * b[j] > mx:
                        j += 1
                    else:
                        cnt += m - j
                        i += 1

                # 左下区域
                i, j = i0, 0
                while i < n and j < j0:
                    if a[i] * b[j] > mx:
                        i += 1
                    else:
                        cnt += n - i
                        j += 1
            else:
                # 右上区域和左下区域的所有数都 <= 0 <= mx
                cnt = i0 * (m - j0) + (n - i0) * j0

                # 左上区域
                i, j = 0, j0 - 1
                while i < i0 and j >= 0:
                    if a[i] * b[j] > mx:
                        i += 1
                    else:
                        cnt += i0 - i
                        j -= 1

                # 右下区域
                i, j = i0, m - 1
                while i < n and j >= j0:
                    if a[i] * b[j] > mx:
                        j -= 1
                    else:
                        cnt += j - j0 + 1
                        i += 1

            return cnt >= k

        n, m = len(a), len(b)
        corners = (a[0] * b[0], a[0] * b[-1], a[-1] * b[0], a[-1] * b[-1])
        left, right = min(corners), max(corners)
        return left + bisect_left(range(left, right), True, key=check)
```

```java [sol-Java]
class Solution {
    public long kthSmallestProduct(int[] a, int[] b, long k) {
        int i0 = lowerBound(a, 0); // 四个区域的水平分界线
        int j0 = lowerBound(b, 0); // 四个区域的垂直分界线

        int n = a.length;
        int m = b.length;
        List<Long> corners = List.of((long) a[0] * b[0], (long) a[0] * b[m - 1], (long) a[n - 1] * b[0], (long) a[n - 1] * b[m - 1]);
        long left = Collections.min(corners) - 1;
        long right = Collections.max(corners);

        while (left + 1 < right) {
            long mid = left + (right - left) / 2;
            if (check(a, b, i0, j0, k, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int[] a, int[] b, int i0, int j0, long k, long mx) {
        int n = a.length;
        int m = b.length;
        long cnt = 0;

        if (mx < 0) {
            // 右上区域
            int i = 0;
            int j = j0;
            while (i < i0 && j < m) { // 不判断 cnt < k 更快
                if ((long) a[i] * b[j] > mx) {
                    j++;
                } else {
                    cnt += m - j;
                    i++;
                }
            }

            // 左下区域
            i = i0;
            j = 0;
            while (i < n && j < j0) {
                if ((long) a[i] * b[j] > mx) {
                    i++;
                } else {
                    cnt += n - i;
                    j++;
                }
            }
        } else {
            // 右上区域和左下区域的所有数都 <= 0 <= mx
            cnt = (long) i0 * (m - j0) + (long) (n - i0) * j0;

            // 左上区域
            int i = 0;
            int j = j0 - 1;
            while (i < i0 && j >= 0) {
                if ((long) a[i] * b[j] > mx) {
                    i++;
                } else {
                    cnt += i0 - i;
                    j--;
                }
            }

            // 右下区域
            i = i0;
            j = m - 1;
            while (i < n && j >= j0) {
                if ((long) a[i] * b[j] > mx) {
                    j--;
                } else {
                    cnt += j - j0 + 1;
                    i++;
                }
            }
        }

        return cnt >= k;
    }

    // 见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int target) {
        int left = -1;
        int right = nums.length;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long kthSmallestProduct(vector<int>& a, vector<int>& b, long long k) {
        int n = a.size(), m = b.size();
        int i0 = ranges::lower_bound(a, 0) - a.begin(); // 四个区域的水平分界线
        int j0 = ranges::lower_bound(b, 0) - b.begin(); // 四个区域的垂直分界线

        auto check = [&](long long mx) -> bool {
            long long cnt = 0;

            if (mx < 0) {
                // 右上区域
                int i = 0, j = j0;
                while (i < i0 && j < m) { // 注：可以加个 cnt < k 的判断，提前退出
                    if (1LL * a[i] * b[j] > mx) {
                        j++;
                    } else {
                        cnt += m - j;
                        i++;
                    }
                }

                // 左下区域
                i = i0;
                j = 0;
                while (i < n && j < j0) {
                    if (1LL * a[i] * b[j] > mx) {
                        i++;
                    } else {
                        cnt += n - i;
                        j++;
                    }
                }
            } else {
                // 右上区域和左下区域的所有数都 <= 0 <= mx
                cnt = 1LL * i0 * (m - j0) + 1LL * (n - i0) * j0;

                // 左上区域
                int i = 0, j = j0 - 1;
                while (i < i0 && j >= 0) {
                    if (1LL * a[i] * b[j] > mx) {
                        i++;
                    } else {
                        cnt += i0 - i;
                        j--;
                    }
                }

                // 右下区域
                i = i0;
                j = m - 1;
                while (i < n && j >= j0) {
                    if (1LL * a[i] * b[j] > mx) {
                        j--;
                    } else {
                        cnt += j - j0 + 1;
                        i++;
                    }
                }
            }

            return cnt >= k;
        };

        long long corners[4] = {1LL * a[0] * b[0], 1LL * a[0] * b[m - 1], 1LL * a[n - 1] * b[0], 1LL * a[n - 1] * b[m - 1]};
        auto [left, right] = ranges::minmax(corners);
        left--;
        while (left + 1 < right) {
            long long mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func kthSmallestProduct(a, b []int, K int64) int64 {
	n, m, k := len(a), len(b), int(K)
	i0 := sort.SearchInts(a, 0) // 四个区域的水平分界线
	j0 := sort.SearchInts(b, 0) // 四个区域的垂直分界线

	corners := []int{a[0] * b[0], a[0] * b[m-1], a[n-1] * b[0], a[n-1] * b[m-1]}
	left := slices.Min(corners)
	right := slices.Max(corners)
	ans := left + sort.Search(right-left, func(mx int) bool {
		mx += left
		cnt := 0

		if mx < 0 {
			// 右上区域
			i, j := 0, j0
			for i < i0 && j < m { // 注：可以加个 cnt < k 的判断，提前退出
				if a[i]*b[j] > mx {
					j++
				} else {
					cnt += m - j
					i++
				}
			}

			// 左下区域
			i, j = i0, 0
			for i < n && j < j0 {
				if a[i]*b[j] > mx {
					i++
				} else {
					cnt += n - i
					j++
				}
			}
		} else {
			// 右上区域和左下区域的所有数都 <= 0 <= mx
			cnt = i0*(m-j0) + (n-i0)*j0

			// 左上区域
			i, j := 0, j0-1
			for i < i0 && j >= 0 {
				if a[i]*b[j] > mx {
					i++
				} else {
					cnt += i0 - i
					j--
				}
			}

			// 右下区域
			i, j = i0, m-1
			for i < n && j >= j0 {
				if a[i]*b[j] > mx {
					j--
				} else {
					cnt += j - j0 + 1
					i++
				}
			}
		}

		return cnt >= k
	})
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)\log U)$，其中 $n$ 是 $a$ 的长度，$m$ 是 $b$ 的长度，$U$ 为矩阵四个角的最大最小之差。二分 $\mathcal{O}(\log U)$ 次，每次双指针需要 $\mathcal{O}(n+m)$ 的时间。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [668. 乘法表中第 K 小的数](https://leetcode.cn/problems/kth-smallest-number-in-multiplication-table/)
- [378. 有序矩阵中第 K 小的元素](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/)

更多相似题目，见下面二分题单的「**§2.6 第 K 小/大**」。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.6 第 K 小/大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.6 第 K 小/大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
