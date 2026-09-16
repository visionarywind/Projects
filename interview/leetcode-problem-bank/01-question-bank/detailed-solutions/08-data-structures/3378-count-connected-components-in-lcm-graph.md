# 3378. 统计最小公倍数图中的连通块数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-connected-components-in-lcm-graph/
- 题目 slug：`count-connected-components-in-lcm-graph`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.3 中介并查集
- 难度分：2532
- 外部题解来源：https://leetcode.cn/problems/count-connected-components-in-lcm-graph/solutions/3013720/mei-ju-gcd-bing-cha-ji-pythonjavacgo-by-sq6vd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举 GCD + 并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/count-connected-components-in-lcm-graph/solutions/3013720/mei-ju-gcd-bing-cha-ji-pythonjavacgo-by-sq6vd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-gcd-bing-cha-ji-pythonjavacgo-by-sq6vd`
- topic id：`3013720`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

下文把 $\textit{threshold}$ 简称为 $t$。

$$
\text{LCM}(x,y)\le t
$$

等价于

$$
\dfrac{x\cdot y}{\text{GCD}(x,y)} \le t
$$

设 $g = \text{GCD}(x,y)$。

**核心思路**：枚举 $g=1,2,3,\ldots,t$，以及 $g$ 的倍数 $x$ 和 $y$，对于满足 $x\cdot y\le g\cdot t$ 的 $x$ 和 $y$，把它们的下标用**并查集**连起来。

但是，$g$ 的倍数有 $\mathcal{O}(t/g)$ 个，枚举 $x$ 和 $y$ 需要 $\mathcal{O}((t/g)^2)$ 的时间，会超时。

解决办法：不需要枚举 $x$，而是找到在 $\textit{nums}$ 中的**最小的** $g$ 的倍数，作为 $x$。对于其他的 $g$ 的倍数 $y$，只要它们能和 $x$ 连起来，那么这些 $y$ 就已经在同一个连通块中了。

> 选择最小的 $x$，是因为这样做，满足 $x\cdot y\le g\cdot t$ 的 $y$ 就尽量多，我们可以把能在同一个连通块中的 $y$ 都用并查集合并。

注意题目保证所有元素互不相同。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1YeqHYSEXv/?t=40m53s)，欢迎点赞关注~

## 答疑

**问**：如果枚举的过程中，出现 $g=2,x=4,y=8$ 的情况怎么办？此时 $g\ne \text{GCD}(x,y)$。

**答**：虽然 $\text{GCD}$ 对不上，但不影响正确性。如果此时 $\dfrac{x\cdot y}{g} \le t$ 成立，那么 $\dfrac{x\cdot y}{\text{GCD}(x,y)} \le t$ 也必然成立。

```py [sol-Python3 哈希表]
class Solution:
    def countComponents(self, nums: List[int], threshold: int) -> int:
        n = len(nums)
        fa = list(range(n))
        # 非递归并查集
        def find(x: int) -> int:
            rt = x
            while fa[rt] != rt:
                rt = fa[rt]
            while fa[x] != rt:
                fa[x], x = rt, fa[x]
            return rt

        # 记录每个数的下标
        idx = {x: i for i, x in enumerate(nums)}

        for g in range(1, threshold + 1):
            fi = -1
            for x in range(g, threshold + 1, g):
                if x in idx:
                    fi = find(idx[x])
                    break
            if fi < 0:
                continue
            for y in range(x + g, g * threshold // x + 1, g):
                if y in idx:
                    fj = find(idx[y])
                    if fj != fi:
                        fa[fj] = fi  # 合并 idx[x] 和 idx[y]
                        n -= 1  # 连通块个数减一
        return n
```

```py [sol-Python3 数组]
class Solution:
    def countComponents(self, nums: List[int], threshold: int) -> int:
        n = len(nums)
        fa = list(range(n))
        # 非递归并查集
        def find(x: int) -> int:
            rt = x
            while fa[rt] != rt:
                rt = fa[rt]
            while fa[x] != rt:
                fa[x], x = rt, fa[x]
            return rt

        # 记录每个数的下标
        idx = [-1] * (threshold + 1)
        for i, x in enumerate(nums):
            if x <= threshold:
                idx[x] = i

        for g in range(1, threshold + 1):
            fi = -1
            for x in range(g, threshold + 1, g):
                if idx[x] >= 0:
                    fi = find(idx[x])
                    break
            if fi < 0:
                continue
            for y in range(x + g, g * threshold // x + 1, g):
                if idx[y] >= 0:
                    fj = find(idx[y])
                    if fj != fi:
                        fa[fj] = fi  # 合并 idx[x] 和 idx[y]
                        n -= 1  # 连通块个数减一
        return n
```

```java [sol-Java]
class Solution {
    public int countComponents(int[] nums, int threshold) {
        int n = nums.length;
        // 初始化并查集
        int[] fa = new int[n];
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }

        // 记录每个数的下标
        int[] idx = new int[threshold + 1];
        Arrays.fill(idx, -1);
        for (int i = 0; i < n; i++) {
            if (nums[i] <= threshold) {
                idx[nums[i]] = i;
            }
        }

        for (int g = 1; g <= threshold; g++) {
            int minX = -1;
            for (int x = g; x <= threshold; x += g) {
                if (idx[x] >= 0) {
                    minX = x;
                    break;
                }
            }
            if (minX < 0) {
                continue;
            }
            int fi = find(fa, idx[minX]);
            int upper = (int) ((long) g * threshold / minX);
            for (int y = minX + g; y <= upper; y += g) {
                if (idx[y] >= 0) {
                    int fj = find(fa, idx[y]);
                    if (fj != fi) {
                        fa[fj] = fi; // 合并 idx[x] 和 idx[y]
                        n--; // 连通块个数减一
                    }
                }
            }
        }
        return n;
    }

    private int find(int[] fa, int x) {
        if (fa[x] != x) {
            fa[x] = find(fa, fa[x]);
        }
        return fa[x];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countComponents(vector<int>& nums, int threshold) {
        int n = nums.size();
        vector<int> fa(n);
        iota(fa.begin(), fa.end(), 0);
        // 非递归并查集
        auto find = [&](int x) -> int {
            int rt = x;
            while (fa[rt] != rt) {
                rt = fa[rt];
            }
            while (fa[x] != rt) {
                int tmp = fa[x];
                fa[x] = rt;
                x = tmp;
            }
            return rt;
        };

        // 记录每个数的下标
        vector<int> idx(threshold + 1, -1);
        for (int i = 0; i < n; i++) {
            if (nums[i] <= threshold) {
                idx[nums[i]] = i;
            }
        }

        for (int g = 1; g <= threshold; g++) {
            int min_x = -1;
            for (int x = g; x <= threshold; x += g) {
                if (idx[x] >= 0) {
                    min_x = x;
                    break;
                }
            }
            if (min_x < 0) {
                continue;
            }
            int fi = find(idx[min_x]);
            int upper = (long long) g * threshold / min_x;
            for (int y = min_x + g; y <= upper; y += g) {
                if (idx[y] >= 0) {
                    int fj = find(idx[y]);
                    if (fj != fi) {
                        fa[fj] = fi; // 合并 idx[x] 和 idx[y]
                        n--; // 连通块个数减一
                    }
                }
            }
        }
        return n;
    }
};
```

```go [sol-Go]
func countComponents(nums []int, threshold int) int {
	n := len(nums)
	fa := make([]int, n)
	for i := range fa {
		fa[i] = i
	}
	// 非递归并查集
	find := func(x int) int {
		rt := x
		for fa[rt] != rt {
			rt = fa[rt]
		}
		for fa[x] != rt {
			fa[x], x = rt, fa[x]
		}
		return rt
	}

	// 记录每个数的下标
	idx := make([]int, threshold+1)
	for i, x := range nums {
		if x <= threshold {
			idx[x] = i + 1 // 这里 +1 了，下面减掉
		}
	}

	for g := 1; g <= threshold; g++ {
		minX := -1
		for x := g; x <= threshold; x += g {
			if idx[x] > 0 { // idx[x] == 0 表示不存在
				minX = x
				break
			}
		}
		if minX < 0 {
			continue
		}
		fi := find(idx[minX] - 1)
		for y := minX + g; y <= threshold && y <= g*threshold/minX; y += g {
			if idx[y] > 0 {
				fj := find(idx[y] - 1)
				if fj != fi {
					fa[fj] = fi // 合并 idx[x] 和 idx[y]
					n-- // 连通块个数减一
				}
			}
		}
	}
	return n
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + \alpha t\log t)$，其中 $n$ 是 $\textit{nums}$ 的长度，$t$ 是 $\textit{threshold}$，$\alpha$ 是并查集单次合并的均摊复杂度。根据调和级数，二重循环（不考虑并查集）的循环次数为 $\mathcal{O}(t\log t)$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n + t)$，取决于实现。

## 相似题目

- [1627. 带阈值的图连通性](https://leetcode.cn/problems/graph-connectivity-with-threshold/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.3 中介并查集`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.3 中介并查集`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
