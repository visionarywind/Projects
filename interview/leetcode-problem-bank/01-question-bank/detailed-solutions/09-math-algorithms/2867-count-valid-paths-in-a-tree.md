# 2867. 统计树中的合法路径数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-valid-paths-in-a-tree/
- 题目 slug：`count-valid-paths-in-a-tree`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.1 乘法原理
- 难度分：2428
- 外部题解来源：https://leetcode.cn/problems/count-valid-paths-in-a-tree/solutions/2456716/tu-jie-on-xian-xing-zuo-fa-pythonjavacgo-tjz2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】O(n) 线性做法（Python/Java/C++/Go）](https://leetcode.cn/problems/count-valid-paths-in-a-tree/solutions/2456716/tu-jie-on-xian-xing-zuo-fa-pythonjavacgo-tjz2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-on-xian-xing-zuo-fa-pythonjavacgo-tjz2`
- topic id：`2456716`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

[视频讲解](https://www.bilibili.com/video/BV1yu4y1z7sE/) 第四题。

![w364D-2-c.png](https://pic.leetcode.cn/1708991317-yqozUn-w364D-2-c.png)

代码实现时，为避免反复 DFS 同一个非质数连通块，可以把每个非质数所处的连通块的大小记录下来（类似记忆化搜索）。如果之前计算过，就无需再次 DFS。

```py [sol-Python3]
# 标记 10**5 以内的质数
MX = 10 ** 5 + 1
is_prime = [True] * MX
is_prime[1] = False
for i in range(2, isqrt(MX) + 1):
    if is_prime[i]:
        for j in range(i * i, MX, i):
            is_prime[j] = False

class Solution:
    def countPaths(self, n: int, edges: List[List[int]]) -> int:
        g = [[] for _ in range(n + 1)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        nodes = []
        def dfs(x: int, fa: int) -> None:
            nodes.append(x)
            for y in g[x]:
                if y != fa and not is_prime[y]:
                    dfs(y, x)

        ans = 0
        size = [0] * (n + 1)
        for x in range(1, n + 1):
            if not is_prime[x]:  # 跳过非质数
                continue
            s = 0
            for y in g[x]:  # 质数 x 把这棵树分成了若干个连通块
                if is_prime[y]:
                    continue
                if size[y] == 0:  # 尚未计算过
                    nodes.clear()
                    dfs(y, -1)  # 遍历 y 所在连通块，在不经过质数的前提下，统计有多少个非质数
                    for z in nodes:
                        size[z] = len(nodes)
                # 这 size[y] 个非质数与之前遍历到的 s 个非质数，两两之间的路径只包含质数 x
                ans += size[y] * s
                s += size[y]
            ans += s  # 从 x 出发的路径
        return ans
```

```java [sol-Java]
class Solution {
    private final static int MX = (int) 1e5;
    private final static boolean[] np = new boolean[MX + 1]; // 质数=false 非质数=true

    static {
        np[1] = true;
        for (int i = 2; i * i <= MX; i++) {
            if (!np[i]) {
                for (int j = i * i; j <= MX; j += i) {
                    np[j] = true;
                }
            }
        }
    }

    public long countPaths(int n, int[][] edges) {
        List<Integer>[] g = new ArrayList[n + 1];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (var e : edges) {
            int x = e[0], y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        long ans = 0;
        int[] size = new int[n + 1];
        var nodes = new ArrayList<Integer>();
        for (int x = 1; x <= n; x++) {
            if (np[x]) { // 跳过非质数
                continue;
            }
            int sum = 0;
            for (int y : g[x]) { // 质数 x 把这棵树分成了若干个连通块
                if (!np[y]) {
                    continue;
                }
                if (size[y] == 0) { // 尚未计算过
                    nodes.clear();
                    dfs(y, -1, g, nodes); // 遍历 y 所在连通块，在不经过质数的前提下，统计有多少个非质数
                    for (int z : nodes) {
                        size[z] = nodes.size();
                    }
                }
                // 这 size[y] 个非质数与之前遍历到的 sum 个非质数，两两之间的路径只包含质数 x
                ans += (long) size[y] * sum;
                sum += size[y];
            }
            ans += sum; // 从 x 出发的路径
        }
        return ans;
    }

    private void dfs(int x, int fa, List<Integer>[] g, List<Integer> nodes) {
        nodes.add(x);
        for (int y : g[x]) {
            if (y != fa && np[y]) {
                dfs(y, x, g, nodes);
            }
        }
    }
}
```

```cpp [sol-C++]
const int MX = 1e5;
bool np[MX + 1]; // 质数=false 非质数=true
int init = []() {
    np[1] = true;
    for (int i = 2; i * i <= MX; i++) {
        if (!np[i]) {
            for (int j = i * i; j <= MX; j += i) {
                np[j] = true;
            }
        }
    }
    return 0;
}();

class Solution {
public:
    long long countPaths(int n, vector<vector<int>> &edges) {
        vector<vector<int>> g(n + 1);
        for (auto &e: edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        vector<int> size(n + 1);
        vector<int> nodes;
        function<void(int, int)> dfs = [&](int x, int fa) {
            nodes.push_back(x);
            for (int y: g[x]) {
                if (y != fa && np[y]) {
                    dfs(y, x);
                }
            }
        };

        long long ans = 0;
        for (int x = 1; x <= n; x++) {
            if (np[x]) continue; // 跳过非质数
            int sum = 0;
            for (int y: g[x]) { // 质数 x 把这棵树分成了若干个连通块
                if (!np[y]) continue;
                if (size[y] == 0) { // 尚未计算过
                    nodes.clear();
                    dfs(y, -1); // 遍历 y 所在连通块，在不经过质数的前提下，统计有多少个非质数
                    for (int z: nodes) {
                        size[z] = nodes.size();
                    }
                }
                // 这 size[y] 个非质数与之前遍历到的 sum 个非质数，两两之间的路径只包含质数 x
                ans += (long long) size[y] * sum;
                sum += size[y];
            }
            ans += sum; // 从 x 出发的路径
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx int = 1e5 + 1
var np = [mx]bool{1: true}
func init() { // 质数=false 非质数=true
	for i := 2; i*i < mx; i++ {
		if !np[i] {
			for j := i * i; j < mx; j += i {
				np[j] = true
			}
		}
	}
}

func countPaths(n int, edges [][]int) (ans int64) {
	g := make([][]int, n+1)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	size := make([]int, n+1)
	nodes := []int{}
	var dfs func(int, int)
	dfs = func(x, fa int) {
		nodes = append(nodes, x)
		for _, y := range g[x] {
			if y != fa && np[y] {
				dfs(y, x)
			}
		}
	}
	for x := 1; x <= n; x++ {
		if np[x] { // 跳过非质数
			continue
		}
		sum := 0
		for _, y := range g[x] { // 质数 x 把这棵树分成了若干个连通块
			if !np[y] {
				continue
			}
			if size[y] == 0 { // 尚未计算过
				nodes = nodes[:0]
				dfs(y, -1) // 遍历 y 所在连通块，在不经过质数的前提下，统计有多少个非质数
				for _, z := range nodes {
					size[z] = len(nodes)
				}
			}
			// 这 size[y] 个非质数与之前遍历到的 sum 个非质数，两两之间的路径只包含质数 x
			ans += int64(size[y]) * int64(sum)
			sum += size[y]
		}
		ans += int64(sum) // 从 x 出发的路径
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。预处理的时间不计入。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

这题本质上是求一种类似于「非质数-质数-非质数」的路径个数。

这让我想到了另外一道题目 [2242. 节点序列的最大得分](https://leetcode.cn/problems/maximum-score-of-a-node-sequence/)。

这两题的共同点在于「枚举中间」，请读者细细品味。

## 本地原创解析

### 1. 题意重述

本题来自 `二、组合数学 / §2.1 乘法原理`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.1 乘法原理`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
