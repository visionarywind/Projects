# 812. 最大三角形面积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-triangle-area/
- 题目 slug：`largest-triangle-area`
- 来源专题：数学算法
- 来源分类路径：五、计算几何 / §5.4 凸包
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/largest-triangle-area/solutions/3793198/liang-chong-fang-fa-mei-ju-tu-bao-xuan-z-1780/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举 / 凸包+旋转卡壳（Python/Java/C++/Go）](https://leetcode.cn/problems/largest-triangle-area/solutions/3793198/liang-chong-fang-fa-mei-ju-tu-bao-xuan-z-1780/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-mei-ju-tu-bao-xuan-z-1780`
- topic id：`3793198`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 三角形面积公式

对于平面上的三个点 $P_1,P_2,P_3$，定义 $\mathbf{a} = \overrightarrow{P_1P_2}$，$\mathbf{b} = \overrightarrow{P_1P_3}$。

根据向量叉积的几何定义，$|| \mathbf{a} \times \mathbf{b} ||$ 是由 $\mathbf{a}$ 和 $\mathbf{b}$ 张成的平行四边形的面积。除以 $2$ 就得到了 $\triangle P_1P_2P_3$ 的面积。

> 严格来说，叉积是三维概念。这里把向量 $\mathbf{a}$ 和 $\mathbf{b}$ 视作 $z$ 方向为 $0$ 的三维向量。

设 $\mathbf{a} = (x_1,y_1)$，$\mathbf{b} = (x_2,y_2)$，根据叉积的计算公式，三角形面积为

$$
\dfrac{1}{2}|x_1y_2 - y_1x_2|
$$

> 上式中的 $(x_1,y_1)$ 来自 $P_1,P_2$ 的横坐标之差，纵坐标之差。$(x_2,y_2)$ 来自 $P_1,P_3$ 的横坐标之差，纵坐标之差。

## 方法一：暴力枚举

```py [sol-Python3]
class Solution:
    def largestTriangleArea(self, points: List[List[int]]) -> float:
        ans = 0
        for p1, p2, p3 in combinations(points, 3):
            x1, y1 = p2[0] - p1[0], p2[1] - p1[1]
            x2, y2 = p3[0] - p1[0], p3[1] - p1[1]
            ans = max(ans, abs(x1 * y2 - y1 * x2))  # 注意这里没有除以 2
        return ans / 2
```

```java [sol-Java]
class Solution {
    public double largestTriangleArea(int[][] points) {
        int n = points.length;
        int ans = 0;
        for (int i = 0; i < n - 2; i++) {
            for (int j = i + 1; j < n - 1; j++) {
                for (int k = j + 1; k < n; k++) {
                    int[] p1 = points[i], p2 = points[j], p3 = points[k];
                    int x1 = p2[0] - p1[0], y1 = p2[1] - p1[1];
                    int x2 = p3[0] - p1[0], y2 = p3[1] - p1[1];
                    ans = Math.max(ans, Math.abs(x1 * y2 - y1 * x2)); // 注意这里没有除以 2
                }
            }
        }
        return ans / 2.0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double largestTriangleArea(vector<vector<int>>& points) {
        int n = points.size();
        int ans = 0;
        for (int i = 0; i < n - 2; i++) {
            auto& p1 = points[i];
            for (int j = i + 1; j < n - 1; j++) {
                auto& p2 = points[j];
                for (int k = j + 1; k < n; k++) {
                    auto& p3 = points[k];
                    int x1 = p2[0] - p1[0], y1 = p2[1] - p1[1];
                    int x2 = p3[0] - p1[0], y2 = p3[1] - p1[1];
                    ans = max(ans, abs(x1 * y2 - y1 * x2)); // 注意这里没有除以 2
                }
            }
        }
        return ans / 2.0;
    }
};
```

```go [sol-Go]
func largestTriangleArea(points [][]int) float64 {
	n := len(points)
	ans := 0
	for i := range n - 2 {
		for j := i + 1; j < n-1; j++ {
			for k := j + 1; k < n; k++ {
				p1, p2, p3 := points[i], points[j], points[k]
				x1, y1 := p2[0]-p1[0], p2[1]-p1[1]
				x2, y2 := p3[0]-p1[0], p3[1]-p1[1]
				ans = max(ans, abs(x1*y2-y1*x2)) // 注意这里没有除以 2
			}
		}
	}
	return float64(ans) / 2
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 是 $\textit{points}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：凸包 + 旋转卡壳

**前置题目**：[587. 安装栅栏](https://leetcode.cn/problems/erect-the-fence/)

若固定三角形的两个顶点，那么第三个顶点在哪？

三角形的高越长越好，所以第三个顶点相比在凸包内部，在凸包上更好（更远）。所以面积最大的三角形，三个顶点都在凸包上。

求出凸包后：

1. 枚举凸包的顶点 $i$ 作为三角形的其中一个顶点。对于另外两个顶点，我们用旋转卡壳（同向双指针）计算。
2. 初始化 $j=i+1$，$k=i+2$。
3. 对于 $\triangle P_iP_jP_k$ 和 $\triangle P_iP_jP_{k+1}$ 的面积，如果后者更大，那么把 $k$ 加一。重复该过程，直到 $k+1$ 越界或者面积没有变大，跳出循环。
4. 跳出循环后，$P_k$ 就移动到了一个在 $\overrightarrow{P_iP_j}$ 左侧且距离 $P_iP_j$ 最远的位置。计算 $\triangle P_iP_jP_k$ 的面积，更新答案的最大值。然后把 $j$ 加一，执行第三步。读者可以在纸上画画，随着 $j$ 的增大，在 $\overrightarrow{P_iP_j}$ 左侧且距离 $P_iP_j$ 最远的 $P_k$ 的下标 $k$ 也在增大，所以可以用同向双指针。

下面代码保证计算 $\mathbf{a} \times \mathbf{b}$ 时，$\mathbf{b}$ 在 $\mathbf{a}$ 的左侧，此时算出来的面积一定大于 $0$，无需取绝对值。

```py [sol-Python3]
def det(x1: int, y1: int, x2: int, y2: int) -> int:
    return x1 * y2 - y1 * x2

def convex_hull(points: List[List[int]]) -> List[List[int]]:
    points.sort()

    # 计算下凸包（从左到右）
    q = []
    for p in points:
        while len(q) > 1 and det(q[-1][0] - q[-2][0], q[-1][1] - q[-2][1], p[0] - q[-1][0], p[1] - q[-1][1]) <= 0:
            q.pop()
        q.append(p)

    # 计算上凸包（从右到左）
    down_size = len(q)
    # 注意下凸包的最后一个点，是上凸包的右边第一个点，所以从 n-2 开始遍历
    for i in range(len(points) - 2, -1, -1):
        p = points[i]
        while len(q) > down_size and det(q[-1][0] - q[-2][0], q[-1][1] - q[-2][1], p[0] - q[-1][0], p[1] - q[-1][1]) <= 0:
            q.pop()
        q.append(p)

    # 此时首尾是同一个点 points[0]，需要去掉
    q.pop()
    return q

class Solution:
    def largestTriangleArea(self, points: List[List[int]]) -> float:
        ch = convex_hull(points)

        def area(i: int, j: int, k: int) -> int:
            return det(ch[j][0] - ch[i][0], ch[j][1] - ch[i][1], ch[k][0] - ch[i][0], ch[k][1] - ch[i][1])

        m = len(ch)
        ans = 0
        # 固定三角形的其中一个顶点 ch[i]
        for i in range(m):
            # 同向双指针
            k = i + 2
            for j in range(i + 1, m - 1):
                while k + 1 < m and area(i, j, k) < area(i, j, k + 1):
                    k += 1
                # 循环结束后，ch[k] 距离 ch[i]ch[j] 最远
                ans = max(ans, area(i, j, k))  # 注意这里没有除以 2
        return ans / 2
```

```py [sol-Python3 SciPy]
from scipy.spatial import ConvexHull

def det(x1: int, y1: int, x2: int, y2: int) -> int:
    return x1 * y2 - y1 * x2

def convex_hull(points: List[List[int]]) -> List[List[int]]:
    # vertices 是凸包顶点的下标（逆时针顺序）
    return [points[i] for i in ConvexHull(points).vertices]

class Solution:
    def largestTriangleArea(self, points: List[List[int]]) -> float:
        ch = convex_hull(points)

        def area(i: int, j: int, k: int) -> int:
            return det(ch[j][0] - ch[i][0], ch[j][1] - ch[i][1], ch[k][0] - ch[i][0], ch[k][1] - ch[i][1])

        m = len(ch)
        ans = 0
        # 固定三角形的其中一个顶点 ch[i]
        for i in range(m):
            # 同向双指针
            k = i + 2
            for j in range(i + 1, m - 1):
                while k + 1 < m and area(i, j, k) < area(i, j, k + 1):
                    k += 1
                # 循环结束后，ch[k] 距离 ch[i]ch[j] 最远
                ans = max(ans, area(i, j, k))  # 注意这里没有除以 2
        return ans / 2
```

```java [sol-Java]
class Solution {
    public double largestTriangleArea(int[][] points) {
        List<int[]> ch = convexHull(points);
        int m = ch.size();
        int ans = 0;
        // 固定三角形的其中一个顶点 ch[i]
        for (int i = 0; i < m; i++) {
            // 同向双指针
            int k = i + 2;
            for (int j = i + 1; j < m - 1; j++) {
                while (k + 1 < m && area(ch, i, j, k) < area(ch, i, j, k + 1)) {
                    k++;
                }
                // 循环结束后，ch[k] 距离 ch[i]ch[j] 最远
                ans = Math.max(ans, area(ch, i, j, k)); // 注意这里没有除以 2
            }
        }
        return ans / 2.0;
    }

    private List<int[]> convexHull(int[][] points) {
        Arrays.sort(points, (a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] - b[1]);

        // 计算下凸包（从左到右）
        List<int[]> q = new ArrayList<>();
        for (int[] p : points) {
            while (q.size() > 1) {
                int[] p1 = q.get(q.size() - 2);
                int[] p2 = q.getLast();
                if (det(p2[0] - p1[0], p2[1] - p1[1], p[0] - p2[0], p[1] - p2[1]) > 0) {
                    break;
                }
                q.removeLast();
            }
            q.add(p);
        }

        // 计算上凸包（从右到左）
        int downSize = q.size();
        // 注意下凸包的最后一个点，是上凸包的右边第一个点，所以从 n-2 开始遍历
        for (int i = points.length - 2; i >= 0; i--) {
            int[] p = points[i];
            while (q.size() > downSize) {
                int[] p1 = q.get(q.size() - 2);
                int[] p2 = q.getLast();
                if (det(p2[0] - p1[0], p2[1] - p1[1], p[0] - p2[0], p[1] - p2[1]) > 0) {
                    break;
                }
                q.removeLast();
            }
            q.add(p);
        }

        // 此时首尾是同一个点 points[0]，需要去掉
        q.removeLast();
        return q;
    }

    private int det(int x1, int y1, int x2, int y2) {
        return x1 * y2 - y1 * x2;
    }

    private int area(List<int[]> ch, int i, int j, int k) {
        return det(ch.get(j)[0] - ch.get(i)[0], ch.get(j)[1] - ch.get(i)[1],
                ch.get(k)[0] - ch.get(i)[0], ch.get(k)[1] - ch.get(i)[1]);
    }
}
```

```cpp [sol-C++]
struct Vec {
    int x, y;

    Vec sub(const Vec& b) const {
        return {x - b.x, y - b.y};
    }

    int det(const Vec& b) const {
        return x * b.y - y * b.x;
    }
};

class Solution {
    vector<Vec> convexHull(vector<Vec>& points) {
        ranges::sort(points, {}, [](auto& p) { return pair(p.x, p.y); });

        vector<Vec> q;
        // 计算下凸包（从左到右）
        for (auto& p : points) {
            while (q.size() > 1 && q[q.size() - 1].sub(q[q.size() - 2]).det(p.sub(q[q.size() - 1])) <= 0) {
                q.pop_back();
            }
            q.push_back(p);
        }

        // 计算上凸包（从右到左）
        int down_size = q.size();
        // 注意下凸包的最后一个点，是上凸包的右边第一个点，所以从 n-2 开始遍历
        for (int i = (int) points.size() - 2; i >= 0; i--) {
            auto& p = points[i];
            while (q.size() > down_size && q[q.size() - 1].sub(q[q.size() - 2]).det(p.sub(q[q.size() - 1])) <= 0) {
                q.pop_back();
            }
            q.push_back(p);
        }

        // 此时首尾是同一个点 points[0]，需要去掉
        q.pop_back();
        return q;
    }

public:
    double largestTriangleArea(vector<vector<int>>& points) {
        vector<Vec> a(points.size());
        for (int i = 0; i < points.size(); i++) {
            a[i] = {points[i][0], points[i][1]};
        }

        vector<Vec> ch = convexHull(a);

        auto area = [&](int i, int j, int k) -> int {
            return ch[j].sub(ch[i]).det(ch[k].sub(ch[i]));
        };

        int m = ch.size();
        int ans = 0;
        // 固定三角形的其中一个顶点 ch[i]
        for (int i = 0; i < m; i++) {
            // 同向双指针
            int k = i + 2;
            for (int j = i + 1; j < m - 1; j++) {
                while (k + 1 < m && area(i, j, k) < area(i, j, k + 1)) {
                    k++;
                }
                // 循环结束后，ch[k] 距离 ch[i]ch[j] 最远
                ans = max(ans, area(i, j, k)); // 注意这里没有除以 2
            }
        }
        return ans / 2.0;
    }
};
```

```go [sol-Go]
type vec struct{ x, y int }

func (a vec) sub(b vec) vec { return vec{a.x - b.x, a.y - b.y} }
func (a vec) det(b vec) int { return a.x*b.y - a.y*b.x }

func convexHull(points []vec) (q []vec) {
	slices.SortFunc(points, func(a, b vec) int { return cmp.Or(a.x-b.x, a.y-b.y) })

	// 计算下凸包（从左到右）
	for _, p := range points {
		for len(q) > 1 && q[len(q)-1].sub(q[len(q)-2]).det(p.sub(q[len(q)-1])) <= 0 {
			q = q[:len(q)-1]
		}
		q = append(q, p)
	}

	// 计算上凸包（从右到左）
	downSize := len(q)
	// 注意下凸包的最后一个点，是上凸包的右边第一个点，所以从 n-2 开始遍历
	for i := len(points) - 2; i >= 0; i-- {
		p := points[i]
		for len(q) > downSize && q[len(q)-1].sub(q[len(q)-2]).det(p.sub(q[len(q)-1])) <= 0 {
			q = q[:len(q)-1]
		}
		q = append(q, p)
	}

	// 此时首尾是同一个点 points[0]，需要去掉
	q = q[:len(q)-1]
	return
}

func largestTriangleArea(points [][]int) float64 {
	a := make([]vec, len(points))
	for i, p := range points {
		a[i] = vec{p[0], p[1]}
	}

	ch := convexHull(a)
	area := func(i, j, k int) int {
		return ch[j].sub(ch[i]).det(ch[k].sub(ch[i]))
	}

	m := len(ch)
	ans := 0
	// 固定三角形的其中一个顶点 ch[i]
	for i := range ch {
		// 同向双指针
		k := i + 2
		for j := i + 1; j < m-1; j++ {
			for k+1 < m && area(i, j, k) < area(i, j, k+1) {
				k++
			}
			// 循环结束后，ch[k] 距离 ch[i]ch[j] 最远
			ans = max(ans, area(i, j, k)) // 注意这里没有除以 2
		}
	}
	return float64(ans) / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{points}$ 的长度。枚举 $i$ 是 $\mathcal{O}(n)$，枚举 $j$ 和 $k$ 的同向双指针也是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

**注**：在 $\mathcal{O}(n\log n)$ 计算出凸包后，计算最大三角形面积有 $\mathcal{O}(n)$ 做法，见论文 [Maximal Area Triangles in a Convex Polygon](https://arxiv.org/pdf/1707.04071v5)。

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

本题来自 `五、计算几何 / §5.4 凸包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、计算几何 / §5.4 凸包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
