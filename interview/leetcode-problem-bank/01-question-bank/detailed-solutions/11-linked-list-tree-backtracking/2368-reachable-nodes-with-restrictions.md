# 2368. 受限条件下可到达节点的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reachable-nodes-with-restrictions/
- 题目 slug：`reachable-nodes-with-restrictions`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.1 遍历
- 难度分：1477
- 外部题解来源：https://leetcode.cn/problems/reachable-nodes-with-restrictions/solutions/2662538/shu-shang-dfspythonjavacgojsrust-by-endl-0r3a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树上 DFS（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/reachable-nodes-with-restrictions/solutions/2662538/shu-shang-dfspythonjavacgojsrust-by-endl-0r3a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-shang-dfspythonjavacgojsrust-by-endl-0r3a`
- topic id：`2662538`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

用哈希集合（或者布尔数组）记录哪些节点是受限的。

然后建树，只有当两个节点都不是受限的（都不在哈希集合中）才连边。

从 $0$ 出发 DFS 这棵树，统计能访问到的节点个数，即为答案。

```py [sol-Python3]
class Solution:
    def reachableNodes(self, n: int, edges: List[List[int]], restricted: List[int]) -> int:
        r = set(restricted)
        g = [[] for _ in range(n)]
        for x, y in edges:
            if x not in r and y not in r:
                g[x].append(y)  # 都不受限才连边
                g[y].append(x)
        def dfs(x: int, fa: int) -> int:
            cnt = 1
            for y in g[x]:
                if y != fa:
                    cnt += dfs(y, x)
            return cnt
        return dfs(0, -1)
```

```java [sol-Java]
class Solution {
    public int reachableNodes(int n, int[][] edges, int[] restricted) {
        boolean[] isRestricted = new boolean[n];
        for (int x : restricted) {
            isRestricted[x] = true;
        }
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0], y = e[1];
            if (!isRestricted[x] && !isRestricted[y]) {
                g[x].add(y); // 都不受限才连边
                g[y].add(x);
            }
        }
        return dfs(0, -1, g);
    }

    private int dfs(int x, int fa, List<Integer>[] g) {
        int cnt = 1;
        for (int y : g[x]) {
            if (y != fa) {
                cnt += dfs(y, x, g);
            }
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<vector<int>> g;

    int dfs(int x, int fa) {
        int cnt = 1;
        for (int y : g[x]) {
            if (y != fa) {
                cnt += dfs(y, x);
            }
        }
        return cnt;
    };

public:
    int reachableNodes(int n, vector<vector<int>> &edges, vector<int> &restricted) {
        unordered_set<int> r(restricted.begin(), restricted.end());
        g.resize(n);
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            if (!r.contains(x) && !r.contains(y)) {
                g[x].push_back(y); // 都不受限才连边
                g[y].push_back(x);
            }
        }
        return dfs(0, -1);
    }
};
```

```go [sol-Go]
func reachableNodes(n int, edges [][]int, restricted []int) (ans int) {
	r := make(map[int]bool, len(restricted))
	for _, x := range restricted {
		r[x] = true
	}
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		if !r[x] && !r[y] { // 都不受限才连边
			g[x] = append(g[x], y)
			g[y] = append(g[y], x)
		}
	}
	var dfs func(int, int)
	dfs = func(x, fa int) {
		ans++
		for _, y := range g[x] {
			if y != fa {
				dfs(y, x)
			}
		}
	}
	dfs(0, -1)
	return
}
```

```js [sol-JavaScript]
var reachableNodes = function(n, edges, restricted) {
    const r = new Set(restricted);
    const g = Array(n).fill(null).map(() => []);
    for (const [x, y] of edges) {
        if (!r.has(x) && !r.has(y)) {
            g[x].push(y); // 都不受限才连边
            g[y].push(x);
        }
    }
    let ans = 0;
    function dfs(x, fa) {
        ans++;
        for (const y of g[x]) {
            if (y !== fa) {
                dfs(y, x);
            }
        }
    }
    dfs(0, -1);
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn reachable_nodes(n: i32, edges: Vec<Vec<i32>>, restricted: Vec<i32>) -> i32 {
        let r = restricted.into_iter().collect::<HashSet<_>>();
        let mut g = vec![vec![]; n as usize];
        for e in &edges {
            let x = e[0];
            let y = e[1];
            if !r.contains(&x) && !r.contains(&y) {
                g[x as usize].push(y as usize); // 都不受限才连边
                g[y as usize].push(x as usize);
            }
        }
        fn dfs(x: usize, fa: usize, g: &Vec<Vec<usize>>) -> i32 {
            let mut cnt = 1;
            for &y in &g[x] {
                if y != fa {
                    cnt += dfs(y, x, g);
                }
            }
            cnt
        }
        dfs(0, 0, &g)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.1 遍历`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.1 遍历`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
