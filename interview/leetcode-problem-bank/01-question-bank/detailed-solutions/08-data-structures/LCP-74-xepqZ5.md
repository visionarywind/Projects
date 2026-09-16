# LCP 74. 最强祝福力场

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/xepqZ5/
- 题目 slug：`xepqZ5`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.2 二维差分
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/xepqZ5/solutions/2240170/chi-san-hua-er-wei-chai-fen-by-endlessch-q43z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[离散化+二维差分（Python/Java/C++/Go）](https://leetcode.cn/problems/xepqZ5/solutions/2240170/chi-san-hua-er-wei-chai-fen-by-endlessch-q43z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chi-san-hua-er-wei-chai-fen-by-endlessch-q43z`
- topic id：`2240170`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 本题视频讲解

见[【力扣杯2023春·个人赛】](https://www.bilibili.com/video/BV1dg4y1j78A/)第三题。

### 思路

1. 统计所有左下和右上坐标，由于会出现 $0.5$，可以将坐标乘 $2$。
2. 离散化横纵坐标。
3. 二维差分，具体见视频讲解。
4. 用二维前缀和复原，计算最大值。

```py [sol1-Python3]
class Solution:
    def fieldOfGreatestBlessing(self, forceField: List[List[int]]) -> int:
        # 1. 统计所有左下和右上坐标
        x_set = set()
        y_set = set()
        for i, j, side in forceField:
            x_set.add(2 * i - side)
            x_set.add(2 * i + side)
            y_set.add(2 * j - side)
            y_set.add(2 * j + side)

        # 2. 离散化
        xs = sorted(x_set)
        ys = sorted(y_set)
        n, m = len(xs), len(ys)

        # 3. 二维差分：快速地把一个矩形范围内的数都 +1
        diff = [[0] * (m + 2) for _ in range(n + 2)]
        for i, j, side in forceField:
            r1 = bisect_left(xs, 2 * i - side)
            r2 = bisect_left(xs, 2 * i + side)
            c1 = bisect_left(ys, 2 * j - side)
            c2 = bisect_left(ys, 2 * j + side)
            # 将区域 r1<=r<=r2 && c1<=c<=c2 上的数都加上 x
            # 多 +1 是为了方便求后面用二维前缀和复原
            diff[r1 + 1][c1 + 1] += 1
            diff[r1 + 1][c2 + 2] -= 1
            diff[r2 + 2][c1 + 1] -= 1
            diff[r2 + 2][c2 + 2] += 1

        # 4. 直接在 diff 上复原（二维前缀和），计算最大值
        ans = 0
        for i in range(1, n + 1):
            for j in range(1, m + 1):
                diff[i][j] += diff[i][j - 1] + diff[i - 1][j] - diff[i - 1][j - 1]
                ans = max(ans, diff[i][j])
        return ans
```

```java [sol1-Java]
class Solution {
    public int fieldOfGreatestBlessing(int[][] forceField) {
        // 1. 统计所有左下和右上坐标
        int nf = forceField.length, k = 0;
        long[] xs = new long[nf * 2], ys = new long[nf * 2];
        for (var f : forceField) {
            long i = f[0], j = f[1], side = f[2];
            xs[k] = 2 * i - side;
            xs[k + 1] = 2 * i + side;
            ys[k++] = 2 * j - side;
            ys[k++] = 2 * j + side;
        }

        // 2. 排序去重
        xs = unique(xs);
        ys = unique(ys);

        // 3. 二维差分
        int n = xs.length, m = ys.length;
        var diff = new int[n + 2][m + 2];
        for (var f : forceField) {
            long i = f[0], j = f[1], side = f[2];
            int r1 = Arrays.binarySearch(xs, 2 * i - side);
            int r2 = Arrays.binarySearch(xs, 2 * i + side);
            int c1 = Arrays.binarySearch(ys, 2 * j - side);
            int c2 = Arrays.binarySearch(ys, 2 * j + side);
            // 将区域 r1<=r<=r2 && c1<=c<=c2 上的数都加上 x
            // 多 +1 是为了方便求后面复原
            ++diff[r1 + 1][c1 + 1];
            --diff[r1 + 1][c2 + 2];
            --diff[r2 + 2][c1 + 1];
            ++diff[r2 + 2][c2 + 2];
        }

        // 4. 直接在 diff 上复原，计算最大值
        int ans = 0;
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= m; ++j) {
                diff[i][j] += diff[i - 1][j] + diff[i][j - 1] - diff[i - 1][j - 1];
                ans = Math.max(ans, diff[i][j]);
            }
        }
        return ans;
    }

    private long[] unique(long[] a) {
        Arrays.sort(a);
        int k = 0;
        for (int i = 1; i < a.length; i++)
            if (a[k] != a[i])
                a[++k] = a[i];
        return Arrays.copyOf(a, k + 1);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int fieldOfGreatestBlessing(vector<vector<int>> &forceField) {
        // 1. 统计所有左下和右上坐标
        vector<long long> xs, ys;
        for (auto &f: forceField) {
            long long i = f[0], j = f[1], side = f[2];
            xs.push_back(2 * i - side);
            xs.push_back(2 * i + side);
            ys.push_back(2 * j - side);
            ys.push_back(2 * j + side);
        }

        // 2. 排序去重
        sort(xs.begin(), xs.end());
        xs.erase(unique(xs.begin(), xs.end()), xs.end());
        sort(ys.begin(), ys.end());
        ys.erase(unique(ys.begin(), ys.end()), ys.end());

        // 3. 二维差分
        int n = xs.size(), m = ys.size(), diff[n + 2][m + 2];
        memset(diff, 0, sizeof(diff));
        for (auto &f: forceField) {
            long long i = f[0], j = f[1], side = f[2];
            int r1 = lower_bound(xs.begin(), xs.end(), 2 * i - side) - xs.begin();
            int r2 = lower_bound(xs.begin(), xs.end(), 2 * i + side) - xs.begin();
            int c1 = lower_bound(ys.begin(), ys.end(), 2 * j - side) - ys.begin();
            int c2 = lower_bound(ys.begin(), ys.end(), 2 * j + side) - ys.begin();
            // 将区域 r1<=r<=r2 && c1<=c<=c2 上的数都加上 x
            // 多 +1 是为了方便求后面复原
            ++diff[r1 + 1][c1 + 1];
            --diff[r1 + 1][c2 + 2];
            --diff[r2 + 2][c1 + 1];
            ++diff[r2 + 2][c2 + 2];
        }

        // 4. 直接在 diff 上复原，计算最大值
        int ans = 0;
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= m; ++j) {
                diff[i][j] += diff[i - 1][j] + diff[i][j - 1] - diff[i - 1][j - 1];
                ans = max(ans, diff[i][j]);
            }
        }
        return ans;
    }
};
```

```go [sol1-Golang]
func fieldOfGreatestBlessing(forceField [][]int) (ans int) {
	// 1. 统计所有左下和右上坐标
	var xs, ys []int
	for _, f := range forceField {
		i, j, side := f[0], f[1], f[2]
		xs = append(xs, 2*i-side, 2*i+side)
		ys = append(ys, 2*j-side, 2*j+side)
	}

	// 2. 排序去重
	unique := func(a []int) []int {
		sort.Ints(a)
		k := 0
		for _, x := range a[1:] {
			if a[k] != x {
				k++
				a[k] = x
			}
		}
		return a[:k+1]
	}
	xs = unique(xs)
	ys = unique(ys)

	// 3. 二维差分
	n, m := len(xs), len(ys)
	diff := make([][]int, n+2)
	for i := range diff {
		diff[i] = make([]int, m+2)
	}
	for _, f := range forceField {
		i, j, side := f[0], f[1], f[2]
		r1 := sort.SearchInts(xs, 2*i-side)
		r2 := sort.SearchInts(xs, 2*i+side)
		c1 := sort.SearchInts(ys, 2*j-side)
		c2 := sort.SearchInts(ys, 2*j+side)
		// 将区域 r1<=r<=r2 && c1<=c<=c2 上的数都加上 x
		// 多 +1 是为了方便求后面复原
		diff[r1+1][c1+1]++
		diff[r1+1][c2+2]--
		diff[r2+2][c1+1]--
		diff[r2+2][c2+2]++
	}

	// 4. 直接在 diff 上复原，计算最大值
	for i := 1; i <= n; i++ {
		for j := 1; j <= m; j++ {
			diff[i][j] += diff[i][j-1] + diff[i-1][j] - diff[i-1][j-1]
			ans = max(ans, diff[i][j])
		}
	}
	return
}

func max(a, b int) int { if a < b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{forceField}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 本地原创解析

### 1. 题意重述

本题来自 `二、差分 / §2.2 二维差分`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、差分 / §2.2 二维差分`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
