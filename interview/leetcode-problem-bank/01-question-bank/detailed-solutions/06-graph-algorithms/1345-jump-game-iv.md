# 1345. 跳跃游戏 IV

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/jump-game-iv/
- 题目 slug：`jump-game-iv`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.3 图论建模 + BFS 最短路
- 难度分：1810
- 外部题解来源：https://leetcode.cn/problems/jump-game-iv/solutions/3963775/bfspythonjavacgo-by-endlesscheng-hyzz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS（Python/Java/C++/Go）](https://leetcode.cn/problems/jump-game-iv/solutions/3963775/bfspythonjavacgo-by-endlesscheng-hyzz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfspythonjavacgo-by-endlesscheng-hyzz`
- topic id：`3963775`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

用图论术语描述，问题等价于：

- 给你一个 $n$ 个节点的无向图，节点编号从 $0$ 到 $n-1$，点权为 $\textit{arr}$。图中的边为 $i\leftrightarrow i+1$。此外，所有点权相同的点之间都有边。
- 计算从 $0$ 到 $n-1$ 的最短路长度（边权均为 $1$）。

求边权均为 $1$ 的最短路，用 **BFS** 实现。

如何实现「所有点权相同的点之间都有边」？用**哈希表**记录相同点权对应的所有节点。哈希表的 key 是 $\textit{arr}[i]$，value 是所有值为 $\textit{arr}[i]$ 的元素的下标组成的列表。

由于所有点权相同的点之间都有边，最坏情况下有 $\mathcal{O}(n^2)$ 条边。遍历所有边太慢了，怎么办？

对于 BFS 算法，访问过的点不能重复入队，所以对于元素 $x$，我们可以在把所有与元素 $x$ 相同的点（下标）入队后，删除哈希表中的 $x$。这样可以保证哈希表中的每个下标至多访问一次，从而做到 $\mathcal{O}(n)$ 时间。

下面用双列表实现 BFS（也可以用队列），原理见[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

```py [sol-Python3]
class Solution:
    def minJumps(self, arr: list[int]) -> int:
        pos = defaultdict(list)  # 元素值 -> [下标]
        for i, x in enumerate(arr):
            pos[x].append(i)

        n = len(arr)
        vis = [False] * n
        vis[0] = True
        q = [0]  # 起点

        for ans in count(0):
            tmp = q
            q = []
            for i in tmp:
                if i == n - 1:  # 到达终点
                    return ans

                # 往右
                if not vis[i + 1]:
                    vis[i + 1] = True
                    q.append(i + 1)

                # 往左
                if i > 0 and not vis[i - 1]:
                    vis[i - 1] = True
                    q.append(i - 1)

                # 访问所有元素值为 arr[i] 的点（下标）
                x = arr[i]
                if x not in pos:  # 之前访问过
                    continue
                for j in pos[x]:
                    if not vis[j]:
                        vis[j] = True
                        q.append(j)
                del pos[x]  # 避免重复访问
```

```java [sol-Java]
class Solution {
    public int minJumps(int[] arr) {
        int n = arr.length;
        Map<Integer, List<Integer>> pos = new HashMap<>(); // 元素值 -> [下标]
        for (int i = 0; i < n; i++) {
            pos.computeIfAbsent(arr[i], _ -> new ArrayList<>()).add(i);
        }

        boolean[] vis = new boolean[n];
        vis[0] = true;
        List<Integer> q = List.of(0); // 起点

        for (int ans = 0; ; ans++) {
            List<Integer> tmp = q;
            q = new ArrayList<>();
            for (int i : tmp) {
                if (i == n - 1) { // 到达终点
                    return ans;
                }

                // 往右
                if (!vis[i + 1]) {
                    vis[i + 1] = true;
                    q.add(i + 1);
                }

                // 往左
                if (i > 0 && !vis[i - 1]) {
                    vis[i - 1] = true;
                    q.add(i - 1);
                }

                // 访问所有元素值为 arr[i] 的点（下标）
                int x = arr[i];
                List<Integer> idx = pos.get(x);
                if (idx == null) { // 之前访问过
                    continue;
                }
                for (int j : idx) {
                    if (!vis[j]) {
                        vis[j] = true;
                        q.add(j);
                    }
                }
                pos.remove(x); // 避免重复访问
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minJumps(vector<int>& arr) {
        int n = arr.size();
        unordered_map<int, vector<int>> pos; // 元素值 -> [下标]
        for (int i = 0; i < n; i++) {
            pos[arr[i]].push_back(i);
        }

        vector<int8_t> vis(n);
        vis[0] = true;
        vector<int> q = {0}; // 起点

        for (int ans = 0; ; ans++) {
            auto tmp = move(q); // move 后 q 为空
            for (int i : tmp) {
                if (i == n - 1) { // 到达终点
                    return ans;
                }

                // 往右
                if (!vis[i + 1]) {
                    vis[i + 1] = true;
                    q.push_back(i + 1);
                }

                // 往左
                if (i > 0 && !vis[i - 1]) {
                    vis[i - 1] = true;
                    q.push_back(i - 1);
                }

                // 访问所有元素值为 arr[i] 的点（下标）
                int x = arr[i];
                auto it = pos.find(x);
                if (it == pos.end()) { // 之前访问过
                    continue;
                }
                for (int j : it->second) {
                    if (!vis[j]) {
                        vis[j] = true;
                        q.push_back(j);
                    }
                }
                pos.erase(it); // 避免重复访问
            }
        }
    }
};
```

```go [sol-Go]
func minJumps(arr []int) (ans int) {
	pos := map[int][]int{} // 元素值 -> [下标]
	for i, x := range arr {
		pos[x] = append(pos[x], i)
	}

	n := len(arr)
	vis := make([]bool, n)
	vis[0] = true
	q := []int{0} // 起点
	for ; ; ans++ {
		tmp := q
		q = nil
		for _, i := range tmp {
			if i == n-1 { // 到达终点
				return
			}

			// 往右
			if !vis[i+1] {
				vis[i+1] = true
				q = append(q, i+1)
			}

			// 往左
			if i > 0 && !vis[i-1] {
				vis[i-1] = true
				q = append(q, i-1)
			}

			// 访问所有元素值为 arr[i] 的点（下标）
			x := arr[i]
			if pos[x] == nil { // 之前访问过
				continue
			}
			for _, j := range pos[x] {
				if !vis[j] {
					vis[j] = true
					q = append(q, j)
				}
			}
			delete(pos, x) // 避免重复访问
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{arr}$ 的长度。每个下标至多入队出队各一次，哈希表中的每个下标至多访问一次。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

[3629. 通过质数传送到达终点的最少跳跃次数](https://leetcode.cn/problems/minimum-jumps-to-reach-end-via-prime-teleportation/)

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

本题来自 `一、图的遍历 / §1.3 图论建模 + BFS 最短路`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.3 图论建模 + BFS 最短路`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
