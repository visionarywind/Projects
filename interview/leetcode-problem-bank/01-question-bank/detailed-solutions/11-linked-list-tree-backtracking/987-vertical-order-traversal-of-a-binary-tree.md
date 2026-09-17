# 987. 二叉树的垂序遍历

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/vertical-order-traversal-of-a-binary-tree/
- 题目 slug：`vertical-order-traversal-of-a-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.16 其他
- 难度分：1676
- 外部题解来源：https://leetcode.cn/problems/vertical-order-traversal-of-a-binary-tree/solutions/2638913/si-chong-xie-fa-dfsha-xi-biao-shuang-shu-tg6q/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[四种写法：DFS+哈希表/双数组/单数组（Python/Java/C++/Go）](https://leetcode.cn/problems/vertical-order-traversal-of-a-binary-tree/solutions/2638913/si-chong-xie-fa-dfsha-xi-biao-shuang-shu-tg6q/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`si-chong-xie-fa-dfsha-xi-biao-shuang-shu-tg6q`
- topic id：`2638913`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

![lc987.jpg](https://pic.leetcode.cn/1707784579-XlBlbd-lc987.jpg)

看示例 3，我们需要知道每个节点的行号 $\textit{row}$、列号 $\textit{col}$ 以及节点值 $\textit{val}$。

如果知道了这些信息，那么问题相当于，给你这些三元组： 

$$
(2,-2,4),(1,-1,2),(0,0,1),(2,0,6),(2,0,5),(1,1,3),(2,2,7)
$$

每个三元组表示 $(\textit{row},\textit{col},\textit{val})$，你需要把这些三元组按照 $\textit{col}$ 分组，也就是把 $\textit{col}$ 相同的分到同一组，每组只保留 $\textit{val}$，每组的 $\textit{val}$ 按照 $\textit{row}$ 从小到大排序，$\textit{row}$ 相同的按照 $\textit{val}$ 从小到大排序。分组后的结果就是答案 $[[4],[2],[1,5,6],[3],[7]]$。

为了获取每个节点的信息，我们可以用 DFS，除了参数 $\textit{node}$ 外，还需要参数 $\textit{row}$ 和 $\textit{col}$ 表示当前节点的行号和列号。每往下递归一层，就把 $\textit{row}$ 加一。如果往左儿子递归，就把 $\textit{col}$ 减一；如果往右儿子递归，就把 $\textit{col}$ 加一。

在 DFS 的同时，用一个哈希表（或者有序字典）来记录这些数据。哈希表的 key 是 $\textit{col}$，哈希表的 value 是一个列表，列表中保存 $(\textit{row},\textit{val})$ 二元组。

DFS 结束后，按照 key 从小到大遍历哈希表，对于哈希表的每个 value，把 value 中的二元组排序，最后取出 value 中的 $\textit{val}$ 加入答案。

晕递归的同学可以看 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

## 写法一

```py [sol-Python3]
class Solution:
    def verticalTraversal(self, root: Optional[TreeNode]) -> List[List[int]]:
        groups = defaultdict(list)
        def dfs(node: Optional[TreeNode], row: int, col: int):
            if node is None:
                return
            groups[col].append((row, node.val))  # col 相同的分到同一组
            dfs(node.left, row + 1, col - 1)
            dfs(node.right, row + 1, col + 1)
        dfs(root, 0, 0)

        ans = []
        for _, g in sorted(groups.items()):
            g.sort()  # 按照 row 排序，row 相同按照 val 排序
            ans.append([val for _, val in g])
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> verticalTraversal(TreeNode root) {
        Map<Integer, List<int[]>> groups = new TreeMap<>();
        dfs(root, 0, 0, groups);

        List<List<Integer>> ans = new ArrayList<>(groups.size());
        for (List<int[]> g : groups.values()) {
            g.sort((a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] - b[1]);
            List<Integer> vals = new ArrayList<>(g.size());
            for (int[] p : g) {
                vals.add(p[1]);
            }
            ans.add(vals);
        }
        return ans;
    }

    private void dfs(TreeNode node, int row, int col, Map<Integer, List<int[]>> groups) {
        if (node == null) {
            return;
        }
        // col 相同的分到同一组
        groups.computeIfAbsent(col, k -> new ArrayList<>()).add(new int[]{row, node.val});
        dfs(node.left, row + 1, col - 1, groups);
        dfs(node.right, row + 1, col + 1, groups);
    }
}
```

```cpp [sol-C++]
class Solution {
    map<int, vector<pair<int, int>>> groups;

    void dfs(TreeNode *node, int row, int col) {
        if (node == nullptr) {
            return;
        }
        // col 相同的分到同一组
        groups[col].emplace_back(row, node->val);
        dfs(node->left, row + 1, col - 1);
        dfs(node->right, row + 1, col + 1);
    }

public:
    vector<vector<int>> verticalTraversal(TreeNode *root) {
        dfs(root, 0, 0);
        vector<vector<int>> ans;
        for (auto &[_, g] : groups) {
            ranges::sort(g);
            vector<int> vals;
            for (auto &[_, val] : g) {
                vals.push_back(val);
            }
            ans.push_back(vals);
        }
        return ans;
    }
};
```

```go [sol-Go]
func verticalTraversal(root *TreeNode) [][]int {
    type pair struct{ row, val int }
    groups := map[int][]pair{}
    var dfs func(*TreeNode, int, int)
    dfs = func(node *TreeNode, row, col int) {
        if node == nil {
            return
        }
        groups[col] = append(groups[col], pair{row, node.Val})
        dfs(node.Left, row+1, col-1)
        dfs(node.Right, row+1, col+1)
    }
    dfs(root, 0, 0)

    keys := make([]int, 0, len(groups))
    for k := range groups {
        keys = append(keys, k)
    }
    slices.Sort(keys)

    ans := make([][]int, len(keys))
    for i, key := range keys {
        g := groups[key]
        slices.SortFunc(g, func(a, b pair) int {
            if a.row != b.row {
                return a.row - b.row
            }
            return a.val - b.val
        })
        ans[i] = make([]int, len(g))
        for j, p := range g {
            ans[i][j] = p.val
        }
    }
    return ans
}
```

## 写法二

也可以在 DFS 的同时记录 $\textit{col}$ 的最小值，这样无需对 key 排序，也无需使用有序字典。

```py [sol-Python3]
class Solution:
    def verticalTraversal(self, root: Optional[TreeNode]) -> List[List[int]]:
        groups = defaultdict(list)
        min_col = 0
        def dfs(node: Optional[TreeNode], row: int, col: int):
            if node is None:
                return
            nonlocal min_col
            min_col = min(min_col, col)
            groups[col].append((row, node.val))  # col 相同的分到同一组
            dfs(node.left, row + 1, col - 1)
            dfs(node.right, row + 1, col + 1)
        dfs(root, 0, 0)

        ans = []
        for col in range(min_col, min_col + len(groups)):
            g = groups[col]
            g.sort()  # 按照 row 排序，row 相同按照 val 排序
            ans.append([val for _, val in g])
        return ans
```

```java [sol-Java]
class Solution {
    private int minCol;

    public List<List<Integer>> verticalTraversal(TreeNode root) {
        Map<Integer, List<int[]>> groups = new HashMap<>();
        dfs(root, 0, 0, groups);

        List<List<Integer>> ans = new ArrayList<>(groups.size());
        for (int col = minCol; col < minCol + groups.size(); col++) {
            List<int[]> g = groups.get(col);
            g.sort((a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] - b[1]);
            List<Integer> vals = new ArrayList<>(g.size());
            for (int[] p : g) {
                vals.add(p[1]);
            }
            ans.add(vals);
        }
        return ans;
    }

    private void dfs(TreeNode node, int row, int col, Map<Integer, List<int[]>> groups) {
        if (node == null) {
            return;
        }
        minCol = Math.min(minCol, col);
        groups.computeIfAbsent(col, k -> new ArrayList<>()).add(new int[]{row, node.val});
        dfs(node.left, row + 1, col - 1, groups);
        dfs(node.right, row + 1, col + 1, groups);
    }
}
```

```cpp [sol-C++]
class Solution {
    unordered_map<int, vector<pair<int, int>>> groups;
    int min_col = 0;

    void dfs(TreeNode *node, int row, int col) {
        if (node == nullptr) {
            return;
        }
        min_col = min(min_col, col);
        groups[col].emplace_back(row, node->val);
        dfs(node->left, row + 1, col - 1);
        dfs(node->right, row + 1, col + 1);
    }

public:
    vector<vector<int>> verticalTraversal(TreeNode *root) {
        dfs(root, 0, 0);
        vector<vector<int>> ans;
        for (int i = min_col; i < min_col + (int) groups.size(); i++) {
            auto &g = groups[i];
            ranges::sort(g);
            vector<int> vals;
            for (auto &[_, val] : g) {
                vals.push_back(val);
            }
            ans.push_back(vals);
        }
        return ans;
    }
};
```

```go [sol-Go]
func verticalTraversal(root *TreeNode) [][]int {
    type pair struct{ row, val int }
    groups := map[int][]pair{}
    minCol := 0
    var dfs func(*TreeNode, int, int)
    dfs = func(node *TreeNode, row, col int) {
        if node == nil {
            return
        }
        groups[col] = append(groups[col], pair{row, node.Val})
        minCol = min(minCol, col)
        dfs(node.Left, row+1, col-1)
        dfs(node.Right, row+1, col+1)
    }
    dfs(root, 0, 0)

    ans := make([][]int, len(groups))
    for i := range ans {
        g := groups[minCol+i]
        slices.SortFunc(g, func(a, b pair) int {
            if a.row != b.row {
                return a.row - b.row
            }
            return a.val - b.val
        })
        ans[i] = make([]int, len(g))
        for j, p := range g {
            ans[i][j] = p.val
        }
    }
    return ans
}
```

## 写法三

也可以用两个列表记录数据，一个列表 $\textit{left}$ 负责统计负数 $\textit{col}$，另一个列表 $\textit{right}$ 负责统计非负数 $\textit{col}$。

> 注：相当于用两个列表来模拟双端队列。

```py [sol-Python3]
class Solution:
    def verticalTraversal(self, root: Optional[TreeNode]) -> List[List[int]]:
        left, right = [], []
        def dfs(node: Optional[TreeNode], row: int, col: int):
            if node is None:
                return
            if col < -len(left):
                left.append([])
            elif col == len(right):
                right.append([])
            (left[-col - 1] if col < 0 else right[col]).append((row, node.val))
            dfs(node.left, row + 1, col - 1)
            dfs(node.right, row + 1, col + 1)
        dfs(root, 0, 0)

        ans = []
        for g in left[::-1] + right:
            g.sort()
            ans.append([val for _, val in g])
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> verticalTraversal(TreeNode root) {
        List<List<int[]>> left = new ArrayList<>();
        List<List<int[]>> right = new ArrayList<>();
        dfs(root, 0, 0, left, right);

        List<List<Integer>> ans = new ArrayList<>(left.size() + right.size());
        Collections.reverse(left);
        add(ans, left);
        add(ans, right);
        return ans;
    }

    private void dfs(TreeNode node, int row, int col, List<List<int[]>> left, List<List<int[]>> right) {
        if (node == null) {
            return;
        }
        if (col < -left.size()) {
            left.add(new ArrayList<>());
        } else if (col == right.size()) {
            right.add(new ArrayList<>());
        }
        (col < 0 ? left.get(-col - 1) : right.get(col)).add(new int[]{row, node.val});
        dfs(node.left, row + 1, col - 1, left, right);
        dfs(node.right, row + 1, col + 1, left, right);
    }

    private void add(List<List<Integer>> ans, List<List<int[]>> a) {
        for (List<int[]> g : a) {
            Collections.sort(g, (p, q) -> p[0] != q[0] ? p[0] - q[0] : p[1] - q[1]);
            List<Integer> vals = new ArrayList<>(g.size());
            for (int[] p : g) {
                vals.add(p[1]);
            }
            ans.add(vals);
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> verticalTraversal(TreeNode *root) {
        vector<vector<pair<int, int>>> left, right;
        function<void(TreeNode*, int, int)> dfs = [&](TreeNode *node, int row, int col) {
            if (node == nullptr) {
                return;
            }
            if (col < -(int) left.size()) {
                left.push_back({});
            } else if (col == right.size()) {
                right.push_back({});
            }
            (col < 0 ? left[-col - 1] : right[col]).emplace_back(row, node->val);
            dfs(node->left, row + 1, col - 1);
            dfs(node->right, row + 1, col + 1);
        };
        dfs(root, 0, 0);

        vector<vector<int>> ans;
        auto add = [&](vector<vector<pair<int, int>>> &a) {
            for (auto &g : a) {
                ranges::sort(g);
                vector<int> vals;
                for (auto &[_, val] : g) {
                    vals.push_back(val);
                }
                ans.push_back(vals);
            }
        };
        ranges::reverse(left);
        add(left);
        add(right);
        return ans;
    }
};
```

```go [sol-Go]
func verticalTraversal(root *TreeNode) [][]int {
    type pair struct{ row, val int }
    var left, right [][]pair
    var dfs func(*TreeNode, int, int)
    dfs = func(node *TreeNode, row, col int) {
        if node == nil {
            return
        }
        if col < 0 {
            if col < -len(left) {
                left = append(left, []pair{})
            }
            left[-col-1] = append(left[-col-1], pair{row, node.Val})
        } else {
            if col == len(right) {
                right = append(right, []pair{})
            }
            right[col] = append(right[col], pair{row, node.Val})
        }
        dfs(node.Left, row+1, col-1)
        dfs(node.Right, row+1, col+1)
    }
    dfs(root, 0, 0)

    ans := make([][]int, 0, len(left)+len(right))
    add := func(a [][]pair) {
        for _, g := range a {
            slices.SortFunc(g, func(a, b pair) int {
                if a.row != b.row {
                    return a.row - b.row
                }
                return a.val - b.val
            })
            vals := make([]int, len(g))
            for j, p := range g {
                vals[j] = p.val
            }
            ans = append(ans, vals)
        }
    }
    slices.Reverse(left)
    add(left)
    add(right)
    return ans
}
```

## 写法四

大道至简，把所有 $(\textit{col},\textit{row},\textit{val})$ 全部丢到同一个列表中，排序后按照 $\textit{col}$ 分组。

```py [sol-Python3]
class Solution:
    def verticalTraversal(self, root: Optional[TreeNode]) -> List[List[int]]:
        data = []
        def dfs(node: Optional[TreeNode], row: int, col: int):
            if node is None:
                return
            data.append((col, row, node.val))
            dfs(node.left, row + 1, col - 1)
            dfs(node.right, row + 1, col + 1)
        dfs(root, 0, 0)

        ans = []
        last_col = inf
        data.sort()
        for col, _, val in data:
            if col != last_col:
                last_col = col
                ans.append([])
            ans[-1].append(val)
        return ans
```

```py [sol-Python3 groupby]
class Solution:
    def verticalTraversal(self, root: Optional[TreeNode]) -> List[List[int]]:
        data = []
        def dfs(node: Optional[TreeNode], row: int, col: int):
            if node is None:
                return
            data.append((col, row, node.val))
            dfs(node.left, row + 1, col - 1)
            dfs(node.right, row + 1, col + 1)
        dfs(root, 0, 0)

        data.sort()
        return [[d[2] for d in g] for _, g in groupby(data, key=lambda d: d[0])]
```

```java [sol-Java]
class Solution {
    public List<List<Integer>> verticalTraversal(TreeNode root) {
        List<int[]> data = new ArrayList<>();
        dfs(root, 0, 0, data);

        List<List<Integer>> ans = new ArrayList<>();
        data.sort((a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] != b[1] ? a[1] - b[1] : a[2] - b[2]);
        int lastCol = Integer.MIN_VALUE;
        for (int[] d : data) {
            if (d[0] != lastCol) {
                lastCol = d[0];
                ans.add(new ArrayList<>());
            }
            ans.get(ans.size() - 1).add(d[2]);
        }
        return ans;
    }

    private void dfs(TreeNode node, int row, int col, List<int[]> data) {
        if (node == null) {
            return;
        }
        data.add(new int[]{col, row, node.val});
        dfs(node.left, row + 1, col - 1, data);
        dfs(node.right, row + 1, col + 1, data);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> verticalTraversal(TreeNode *root) {
        vector<tuple<int, int, int>> data;
        function<void(TreeNode*, int, int)> dfs = [&](TreeNode *node, int row, int col) {
            if (node == nullptr) {
                return;
            }
            data.emplace_back(col, row, node->val);
            dfs(node->left, row + 1, col - 1);
            dfs(node->right, row + 1, col + 1);
        };
        dfs(root, 0, 0);

        vector<vector<int>> ans;
        ranges::sort(data);
        int last_col = INT_MIN;
        for (auto &[col, _, val]: data) {
            if (col != last_col) {
                last_col = col;
                ans.push_back({});
            }
            ans.back().push_back(val);
        }
        return ans;
    }
};
```

```go [sol-Go]
func verticalTraversal(root *TreeNode) (ans [][]int) {
    type tuple struct{ col, row, val int }
    data := []tuple{}
    var dfs func(*TreeNode, int, int)
    dfs = func(node *TreeNode, row, col int) {
        if node == nil {
            return
        }
        data = append(data, tuple{col, row, node.Val})
        dfs(node.Left, row+1, col-1)
        dfs(node.Right, row+1, col+1)
    }
    dfs(root, 0, 0)

    slices.SortFunc(data, func(a, b tuple) int {
        if a.col != b.col {
            return a.col - b.col
        }
        if a.row != b.row {
            return a.row - b.row
        }
        return a.val - b.val
    })

    lastCol := math.MinInt
    for _, d := range data {
        if d.col != lastCol {
            lastCol = d.col
            ans = append(ans, []int{})
        }
        ans[len(ans)-1] = append(ans[len(ans)-1], d.val)
    }
    return
}
```

#### 复杂度分析

最坏情况下，有多少个节点在同一个坐标 $(\textit{row},\textit{col})$？

从 $(0,0)$ 出发，向左再向右可以到达 $(2,0)$，向右再向左也可以到达 $(2,0)$。这 $2$ 个在 $(2,0)$ 的节点，按照同样的方法可以得到 $4$ 个在 $(4,0)$ 的节点。依此类推。如果当前有 $2^k$ 个节点在同一个坐标，那么增加 $2^{k+2}$ 个节点，就可以得到 $2^{k+1}$ 个在同一坐标的节点。

- $n=5$ 时，最多有 $2$ 个节点在同一个坐标。
- $n=5+8=13$ 时，最多有 $4$ 个节点在同一个坐标。
- $n=5+8+16=29$ 时，最多有 $8$ 个节点在同一个坐标。
- $n=5+8+16+32=61$ 时，最多有 $16$ 个节点在同一个坐标。
- ……
- $n=2^{k+2}-3$ 时，最多有 $2^k$ 个节点在同一个坐标。

所以最坏情况下有大约 $\dfrac{n}{4}$ 个节点在同一个坐标，无论用何种方式遍历二叉树，基于比较的排序都需要 $\mathcal{O}(n\log n)$ 的时间。

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为二叉树的节点个数。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `二、二叉树 / §2.16 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.16 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
