# 4015. 树的加权和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/weighted-sum-of-a-tree/
- 题目 slug：`weighted-sum-of-a-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.2 自顶向下 DFS
- 难度分：1534
- 外部题解来源：https://leetcode.cn/problems/weighted-sum-of-a-tree/solutions/4009096/liang-ci-dfspythonjavacgo-by-endlesschen-fcb2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两次 DFS / 一次 DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/weighted-sum-of-a-tree/solutions/4009096/liang-ci-dfspythonjavacgo-by-endlesschen-fcb2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-dfspythonjavacgo-by-endlesschen-fcb2`
- topic id：`4009096`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 写法一

先 DFS 算出树的高度 $h$，做法同 [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/)。晕递归的同学，请先看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)，欢迎点赞~

然后，写一个自顶向下的 DFS，从 $h$ 开始，每往下走一步就把 $h$ 减一，就是题目中的 $h-d+1$ 了。

```py [sol-Python3]
class Solution:
    def weightedSum(self, parent: list[int], nums: list[int]) -> int:
        n = len(parent)
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parent[i]].append(i)

        def get_h(x: int) -> int:
            h = 0
            for y in g[x]:
                h = max(h, get_h(y))
            return h + 1
        h = get_h(0)

        def dfs(x: int, weight: int) -> int:
            ans = nums[x] * weight
            for y in g[x]:
                ans += dfs(y, weight - 1)
            return ans
        return dfs(0, h)
```

```java [sol-Java]
class Solution {
    public long weightedSum(int[] parent, int[] nums) {
        int n = parent.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            g[parent[i]].add(i);
        }

        int h = getH(0, g);
        return dfs(0, h, g, nums);
    }

    private int getH(int x, List<Integer>[] g) {
        int h = 0;
        for (int y : g[x]) {
            h = Math.max(h, getH(y, g));
        }
        return h + 1;
    }

    private long dfs(int x, int weight, List<Integer>[] g, int[] nums) {
        long ans = (long) nums[x] * weight;
        for (int y : g[x]) {
            ans += dfs(y, weight - 1, g, nums);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long weightedSum(vector<int>& parent, vector<int>& nums) {
        int n = parent.size();
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parent[i]].push_back(i);
        }

        auto get_h = [&](this auto&& get_h, int x) -> int {
            int h = 0;
            for (int y : g[x]) {
                h = max(h, get_h(y));
            }
            return h + 1;
        };
        int h = get_h(0);

        long long ans = 0;
        auto dfs = [&](this auto&& dfs, int x, int weight) -> void {
            ans += 1LL * nums[x] * weight;
            for (int y : g[x]) {
                dfs(y, weight - 1);
            }
        };
        dfs(0, h);
        return ans;
    }
};
```

```go [sol-Go]
func weightedSum(parent []int, nums []int) (ans int64) {
	n := len(parent)
	g := make([][]int, n)
	for i := 1; i < n; i++ {
		p := parent[i]
		g[p] = append(g[p], i)
	}

	var getH func(int) int
	getH = func(x int) (h int) {
		for _, y := range g[x] {
			h = max(h, getH(y))
		}
		return h + 1
	}
	h := getH(0)

	var dfs func(int, int)
	dfs = func(x, weight int) {
		ans += int64(nums[x]) * int64(weight)
		for _, y := range g[x] {
			dfs(y, weight-1)
		}
	}
	dfs(0, h)
	return
}
```

## 写法二

在第一次 DFS 中，可以顺带求出每个节点的深度，保存到数组 $\textit{depth}$ 中。这样第二次 DFS 可以用简单的循环代替。

```py [sol-Python3]
class Solution:
    def weightedSum(self, parent: list[int], nums: list[int]) -> int:
        n = len(parent)
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parent[i]].append(i)

        depth = [0] * n
        def dfs(x: int) -> None:
            for y in g[x]:
                depth[y] = depth[x] + 1
                dfs(y)
        dfs(0)

        h = max(depth) + 1
        return sum(x * (h - d) for x, d in zip(nums, depth))
```

```java [sol-Java]
class Solution {
    public long weightedSum(int[] parent, int[] nums) {
        int n = parent.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            g[parent[i]].add(i);
        }

        int[] depth = new int[n];
        int h = dfs(0, g, depth);

        long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += (long) nums[i] * (h - depth[i]);
        }
        return ans;
    }

    private int dfs(int x, List<Integer>[] g, int[] depth) {
        int h = 0;
        for (int y : g[x]) {
            depth[y] = depth[x] + 1;
            h = Math.max(h, dfs(y, g, depth));
        }
        return h + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long weightedSum(vector<int>& parent, vector<int>& nums) {
        int n = parent.size();
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parent[i]].push_back(i);
        }
        vector<int> depth(n);

        auto dfs = [&](this auto&& dfs, int x) -> int {
            int h = 0;
            for (int y : g[x]) {
                depth[y] = depth[x] + 1;
                h = max(h, dfs(y));
            }
            return h + 1;
        };
        int h = dfs(0);

        long long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += 1LL * nums[i] * (h - depth[i]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func weightedSum(parent []int, nums []int) (ans int64) {
	n := len(parent)
	g := make([][]int, n)
	for i := 1; i < n; i++ {
		p := parent[i]
		g[p] = append(g[p], i)
	}

	depth := make([]int, n)
	var dfs func(int) int
	dfs = func(x int) (h int) {
		for _, y := range g[x] {
			depth[y] = depth[x] + 1
			h = max(h, dfs(y))
		}
		return h + 1
	}
	h := dfs(0)

	for i, x := range nums {
		ans += int64(x) * int64(h-depth[i])
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面树题单的「**§3.2 自顶向下 DFS**」和「**§3.3 自底向上 DFS**」。

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

本题来自 `三、一般树 / §3.2 自顶向下 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.2 自顶向下 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
