# 2477. 到达首都的最少油耗

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/
- 题目 slug：`minimum-fuel-cost-to-report-to-the-capital`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：2012
- 外部题解来源：https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/solutions/1981361/kao-lu-mei-tiao-bian-shang-zhi-shao-xu-y-uamv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】没有思路？一张图秒懂！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-fuel-cost-to-report-to-the-capital/solutions/1981361/kao-lu-mei-tiao-bian-shang-zhi-shao-xu-y-uamv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kao-lu-mei-tiao-bian-shang-zhi-shao-xu-y-uamv`
- topic id：`1981361`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

![LC2477-c.png](https://pic.leetcode.cn/1701652114-HoRVil-LC2477-c.png)

```py [sol-Python3]
class Solution:
    def minimumFuelCost(self, roads: List[List[int]], seats: int) -> int:
        g = [[] for _ in range(len(roads) + 1)]
        for x, y in roads:
            g[x].append(y)  # 记录每个点的邻居
            g[y].append(x)

        ans = 0
        def dfs(x: int, fa: int) -> int:
            size = 1
            for y in g[x]:
                if y != fa:  # 递归子节点，不能递归父节点
                    size += dfs(y, x)  # 统计子树大小
            if x:  # x 不是根节点
                nonlocal ans
                ans += (size - 1) // seats + 1  # ceil(size/seats)
            return size
        dfs(0, -1)
        return ans
```

```java [sol-Java]
class Solution {
    public long minimumFuelCost(int[][] roads, int seats) {
        int n = roads.length + 1;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : roads) {
            int x = e[0], y = e[1];
            g[x].add(y); // 记录每个点的邻居
            g[y].add(x);
        }

        dfs(0, -1, g, seats);
        return ans;
    }

    private long ans;

    private int dfs(int x, int fa, List<Integer>[] g, int seats) {
        int size = 1;
        for (int y : g[x]) {
            if (y != fa) { // 递归子节点，不能递归父节点
                size += dfs(y, x, g, seats); // 统计子树大小
            }
        }
        if (x > 0) { // x 不是根节点
            ans += (size - 1) / seats + 1; // ceil(size/seats)
        }
        return size;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumFuelCost(vector<vector<int>> &roads, int seats) {
        vector<vector<int>> g(roads.size() + 1);
        for (auto &e: roads) {
            int x = e[0], y = e[1];
            g[x].push_back(y); // 记录每个点的邻居
            g[y].push_back(x);
        }

        long long ans = 0;
        function<int(int, int)> dfs = [&](int x, int fa) -> int {
            int size = 1;
            for (int y: g[x]) {
                if (y != fa) { // 递归子节点，不能递归父节点
                    size += dfs(y, x); // 统计子树大小
                }
            }
            if (x) { // x 不是根节点
                ans += (size - 1) / seats + 1; // ceil(size/seats)
            }
            return size;
        };
        dfs(0, -1);
        return ans;
    }
};
```

```go [sol-Go]
func minimumFuelCost(roads [][]int, seats int) (ans int64) {
	g := make([][]int, len(roads)+1)
	for _, e := range roads {
		x, y := e[0], e[1]
		g[x] = append(g[x], y) // 记录每个点的邻居
		g[y] = append(g[y], x)
	}

	var dfs func(int, int) int
	dfs = func(x, fa int) int {
		size := 1
		for _, y := range g[x] {
			if y != fa { // 递归子节点，不能递归父节点
				size += dfs(y, x) // 统计子树大小
			}
		}
		if x > 0 { // x 不是根节点
			ans += int64((size-1)/seats + 1) // ceil(size/seats)
		}
		return size
	}
	dfs(0, -1)
	return
}
```

```js [sol-JavaScript]
var minimumFuelCost = function (roads, seats) {
    const g = Array(roads.length + 1).fill(null).map(() => []);
    for (const [x, y] of roads) {
        g[x].push(y); // 记录每个点的邻居
        g[y].push(x);
    }

    let ans = 0;
    function dfs(x, fa) {
        let size = 1;
        for (const y of g[x]) {
            if (y !== fa) { // 递归子节点，不能递归父节点
                size += dfs(y, x); // 统计子树大小
            }
        }
        if (x !== 0) { // x 不是根节点
            ans += Math.ceil(size / seats);
        }
        return size;
    }
    dfs(0, -1);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_fuel_cost(roads: Vec<Vec<i32>>, seats: i32) -> i64 {
        let mut g = vec![vec![]; roads.len() + 1];
        for e in &roads {
            let x = e[0] as usize;
            let y = e[1] as usize;
            g[x].push(y); // 记录每个点的邻居
            g[y].push(x);
        }
        let mut ans = 0i64;
        Self::dfs(0, 0, &g, seats, &mut ans);
        ans
    }

    fn dfs(x: usize, fa: usize, g: &Vec<Vec<usize>>, seats: i32, ans: &mut i64) -> i32 {
        let mut size = 1;
        for &y in &g[x] {
            if y != fa { // 递归子节点，不能递归父节点
                size += Self::dfs(y, x, g, seats, ans); // 统计子树大小
            }
        }
        if x != 0 { // x 不是根节点
            *ans += ((size - 1) / seats + 1) as i64; // ceil(size/seats)
        }
        size
    }
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{roads}$ 的长度。递归这棵树，每个节点至多访问一次。
- 空间复杂度：$O(n)$。

## 相似题目

- [979. 在二叉树中分配硬币](https://leetcode.cn/problems/distribute-coins-in-binary-tree/)，[我的题解](https://leetcode.cn/problems/distribute-coins-in-binary-tree/solution/tu-jie-mei-you-si-lu-jin-lai-miao-dong-p-vrni/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.3 自底向上 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.3 自底向上 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
