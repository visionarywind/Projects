# 2556. 二进制矩阵中翻转最多一次使路径不连通

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/disconnect-path-in-a-binary-matrix-by-at-most-one-flip/
- 题目 slug：`disconnect-path-in-a-binary-matrix-by-at-most-one-flip`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：2369
- 外部题解来源：https://leetcode.cn/problems/disconnect-path-in-a-binary-matrix-by-at-most-one-flip/solutions/2093243/zhuan-huan-cheng-qiu-lun-kuo-shi-fou-xia-io8x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转换成求轮廓是否相交（Python/Java/C++/Go）](https://leetcode.cn/problems/disconnect-path-in-a-binary-matrix-by-at-most-one-flip/solutions/2093243/zhuan-huan-cheng-qiu-lun-kuo-shi-fou-xia-io8x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-cheng-qiu-lun-kuo-shi-fou-xia-io8x`
- topic id：`2093243`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

### 提示 1

如果让你把所有**从起点到终点**的路径上的格子都做个标记，这些标记的「轮廓」会是什么样的？

如果可以使矩阵不连通，「轮廓」应该有什么特点？

### 提示 2

如果可以使矩阵不连通，那么你翻转的那个格子必然会使「轮廓」也不连通（断开）。

如何找到「轮廓」？

### 提示 3

从 $(0,0)$ 出发，优先向下走，其次向右走，得到下轮廓。

从 $(0,0)$ 出发，优先向右走，其次向下走，得到上轮廓。

如果两个轮廓有交集（除了起点终点），那么翻转交集中的任意一个格子，都可以使矩阵不连通。

代码实现时，可以直接把下轮廓的格子值修改成 $0$，如果再从 $(0,0)$ 出发，无法到达终点，则说明可以使矩阵不连通。也就是说，得到下轮廓后，无需求上轮廓，只需要看 $(0,0)$ 和终点之间是否有通路即可。

附：[视频讲解](https://www.bilibili.com/video/BV1rM4y1X7z9/)

```py [sol1-Python3]
class Solution:
    def isPossibleToCutPath(self, g: List[List[int]]) -> bool:
        m, n = len(g), len(g[0])
        def dfs(x: int, y: int) -> bool:  # 返回能否到达终点
            if x == m - 1 and y == n - 1: return True
            g[x][y] = 0  # 直接修改
            return x < m - 1 and g[x + 1][y] and dfs(x + 1, y) or \
                   y < n - 1 and g[x][y + 1] and dfs(x, y + 1)
        return not dfs(0, 0) or not dfs(0, 0)
```

```java [sol1-Java]
class Solution {
    private int[][] g;
    private int m, n;

    public boolean isPossibleToCutPath(int[][] grid) {
        g = grid; m = g.length; n = g[0].length;
        return !dfs(0, 0) || !dfs(0, 0);
    }

    private boolean dfs(int x, int y) { // 返回能否到达终点
        if (x == m - 1 && y == n - 1) return true;
        g[x][y] = 0; // 直接修改
        return x < m - 1 && g[x + 1][y] > 0 && dfs(x + 1, y) ||
               y < n - 1 && g[x][y + 1] > 0 && dfs(x, y + 1);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    bool isPossibleToCutPath(vector<vector<int>> &g) {
        int m = g.size(), n = g[0].size();
        function<bool(int, int)> dfs = [&](int x, int y) -> bool { // 返回能否到达终点
            if (x == m - 1 && y == n - 1) return true;
            g[x][y] = 0; // 直接修改
            return x < m - 1 && g[x + 1][y] && dfs(x + 1, y) ||
                   y < n - 1 && g[x][y + 1] && dfs(x, y + 1);
        };
        return !dfs(0, 0) || !dfs(0, 0);
    }
};
```

```go [sol1-Go]
func isPossibleToCutPath(g [][]int) bool {
    m, n := len(g), len(g[0])
    var dfs func(int, int) bool
    dfs = func(x, y int) bool { // 返回能否到达终点
        if x == m-1 && y == n-1 {
            return true
        }
        g[x][y] = 0 // 直接修改，同时保证每个点至多访问一次
        return x < m-1 && g[x+1][y] > 0 && dfs(x+1, y) ||
               y < n-1 && g[x][y+1] > 0 && dfs(x, y+1)
    }
    return !dfs(0, 0) || !dfs(0, 0)
}
```

### 复杂度分析

- 时间复杂度：$O(mn)$，其中 $m$ 为 $\textit{grid}$ 的长度，$n$ 为 $\textit{grid}[i]$ 的长度。
- 空间复杂度：$O(m+n)$。递归需要 $O(m+n)$ 的栈空间。

### 思考题

如果题目还允许向上和向左，要如何做呢？

欢迎在评论区发表你的做法。

---

如果你觉得自己的思维能力有些薄弱，可以做做 [从周赛中学算法 - 2022 年周赛题目总结（下篇）](https://leetcode.cn/circle/discuss/WR1MJP/) 中的「思维题」这节，所有题目我都写了题解。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
