# 1766. 互质树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/tree-of-coprimes/
- 题目 slug：`tree-of-coprimes`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.2 自顶向下 DFS
- 难度分：2232
- 外部题解来源：https://leetcode.cn/problems/tree-of-coprimes/solutions/2733992/dfs-zhong-ji-lu-jie-dian-zhi-de-shen-du-4v5d2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 中记录节点值的深度和编号，回溯写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/tree-of-coprimes/solutions/2733992/dfs-zhong-ji-lu-jie-dian-zhi-de-shen-du-4v5d2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dfs-zhong-ji-lu-jie-dian-zhi-de-shen-du-4v5d2`
- topic id：`2733992`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

对于节点 $x$，我们需要计算节点值与 $\textit{nums}[x]$ 互质的最近祖先节点是哪个。

最暴力的做法是，枚举 $x$ 的所有祖先节点。但如果这棵树是一条链，枚举 $x$ 的所有祖先节点需要 $\mathcal{O}(n)$ 的时间，每个点都这样枚举的话，总共需要 $\mathcal{O}(n^2)$ 的时间，太慢了。

注意到，所有节点的节点值都不超过 $50$，我们可以枚举 $[1,50]$ 中与 $\textit{nums}[x]$ 互质的数。由于要计算的是「最近」祖先，**对于节点值相同的祖先，只需枚举深度最大的**。因此，对于节点 $x$，我们至多枚举它的 $50$ 个祖先。这样总共只需要 $\mathcal{O}(nU)$ 的时间，其中 $U=50$。

具体来说，我们需要在递归这棵树的同时，维护两组信息：

- $\textit{valDepth}$ 数组。其中 $\textit{valDepth}[j]$ 保存节点值等于 $j$ 的最近祖先的**深度**。
- $\textit{valNodeId}$ 数组。其中 $\textit{valNodeId}[j]$ 保存节点值等于 $j$ 的最近祖先的**节点编号**。

设当前节点值为 $\textit{val}=\textit{nums}[x]$，我们枚举 $[1,50]$ 中与 $\textit{val}$ 互质的数字 $j$，计算出 $\textit{valDepth}[j]$ 的最大值，及其对应的节点编号，即为答案 $\textit{ans}[x]$。

代码实现时，可以**预处理** $[1,50]$ 中有哪些数对是互质的。

### 答疑

**问**：代码中的「恢复现场」是什么意思？

**答**：这是**回溯**中的一个概念（例如 [78. 子集](https://leetcode.cn/problems/subsets/)）。请看示例 1，节点 $1$ 有两个儿子 $2$ 和 $3$。我们先递归节点 $2$，此时会「覆盖」$\textit{valDepth}[3]$ 和 $\textit{valNodeId}[3]$ 的数据。递归完节点 $2$，要在递归节点 $3$ 之前，把 $\textit{valDepth}[3]$ 和 $\textit{valNodeId}[3]$ **恢复**成递归节点 $2$ 之前的数据（即节点 $1$ 的深度和编号），这样在递归节点 $3$ 的时候，节点值为 $3$ 的祖先节点是 $1$。如果不恢复，$\textit{valDepth}[3]$ 和 $\textit{valNodeId}[3]$ 中记录的是节点 $2$ 的信息，这就搞错了，因为节点 $2$ 不是节点 $3$ 的祖先节点。

![lc1766.png](https://pic.leetcode.cn/1712803465-mjpkYd-lc1766.png)

```py [sol-Python3]
# 预处理：coprime[i] 保存 [1, MX) 中与 i 互质的所有元素
MX = 51
coprime = [[j for j in range(1, MX) if gcd(i, j) == 1]
           for i in range(MX)]

class Solution:
    def getCoprimes(self, nums: List[int], edges: List[List[int]]) -> List[int]:
        n = len(nums)
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        ans = [0] * n
        val_depth_id = [(-1, -1)] * MX  # 包含深度和节点编号
        def dfs(x: int, fa: int, depth: int) -> None:
            val = nums[x]  # x 的节点值
            # 计算与 val 互质的祖先节点值中，节点深度最大的节点编号
            ans[x] = max(val_depth_id[j] for j in coprime[val])[1]
            tmp = val_depth_id[val]  # 用于恢复现场
            val_depth_id[val] = (depth, x)  # 保存 val 对应的节点深度和节点编号
            for y in g[x]:
                if y != fa:
                    dfs(y, x, depth + 1)
            val_depth_id[val] = tmp  # 恢复现场
        dfs(0, -1, 0)
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MX = 51;
    private static final int[][] coprime = new int[MX][MX];

    static {
        // 预处理
        // coprime[i] 保存 [1, MX) 中与 i 互质的所有元素
        for (int i = 1; i < MX; i++) {
            int k = 0;
            for (int j = 1; j < MX; j++) {
                if (gcd(i, j) == 1) {
                    coprime[i][k++] = j;
                }
            }
        }
    }

    public int[] getCoprimes(int[] nums, int[][] edges) {
        int n = nums.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        int[] ans = new int[n];
        Arrays.fill(ans, -1);
        int[] valDepth = new int[MX];
        int[] valNodeId = new int[MX];
        dfs(0, -1, 1, g, nums, ans, valDepth, valNodeId);
        return ans;
    }

    private void dfs(int x, int fa, int depth, List<Integer>[] g, int[] nums, int[] ans, int[] valDepth, int[] valNodeId) {
        // x 的节点值
        int val = nums[x];

        // 计算与 val 互质的祖先节点值中，节点深度最大的节点编号
        int maxDepth = 0;
        for (int j : coprime[val]) {
            if (j == 0) {
                break;
            }
            if (valDepth[j] > maxDepth) {
                maxDepth = valDepth[j];
                ans[x] = valNodeId[j];
            }
        }

        // tmpDepth 和 tmpNodeId 用于恢复现场
        int tmpDepth = valDepth[val];
        int tmpNodeId = valNodeId[val];

        // 保存 val 对应的节点深度和节点编号
        valDepth[val] = depth;
        valNodeId[val] = x;

        // 向下递归
        for (int y : g[x]) {
            if (y != fa) {
                dfs(y, x, depth + 1, g, nums, ans, valDepth, valNodeId);
            }
        }

        // 恢复现场
        valDepth[val] = tmpDepth;
        valNodeId[val] = tmpNodeId;
    }

    private static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```cpp [sol-C++]
const int MX = 51;
vector<int> coprime[MX];

auto init = [] {
    // 预处理：coprime[i] 保存 [1, MX) 中与 i 互质的所有元素
    for (int i = 1; i < MX; i++) {
        for (int j = 1; j < MX; j++) {
            if (gcd(i, j) == 1) {
                coprime[i].push_back(j);
            }
        }
    }
    return 0;
}();

class Solution {
    vector<vector<int>> g;
    vector<int> ans;
    pair<int, int> val_depth_id[MX]; // 包含深度和节点编号

    void dfs(int x, int fa, int depth, vector<int> &nums) {
        int val = nums[x]; // x 的节点值
        // 计算与 val 互质的数中，深度最大的节点编号
        int max_depth = 0;
        for (int j : coprime[val]) {
            auto [depth, id] = val_depth_id[j];
            if (depth > max_depth) {
                max_depth = depth;
                ans[x] = id;
            }
        }

        auto tmp = val_depth_id[val]; // 用于恢复现场
        val_depth_id[val] = {depth, x}; // 保存 val 对应的节点深度和节点编号
        for (int y : g[x]) {
            if (y != fa) {
                dfs(y, x, depth + 1, nums);
            }
        }
        val_depth_id[val] = tmp; // 恢复现场
    }

public:
    vector<int> getCoprimes(vector<int> &nums, vector<vector<int>> &edges) {
        int n = nums.size();
        g.resize(n);
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        ans.resize(n, -1);
        dfs(0, -1, 1, nums);
        return ans;
    }
};
```

```go [sol-Go]
const mx = 51
var coprime [mx][]int

func init() {
    // 预处理：coprime[i] 保存 [1, MX) 中与 i 互质的所有元素
    for i := 1; i < mx; i++ {
        for j := 1; j < mx; j++ {
            if gcd(i, j) == 1 {
                coprime[i] = append(coprime[i], j)
            }
        }
    }
}

func getCoprimes(nums []int, edges [][]int) []int {
    n := len(nums)
    g := make([][]int, n)
    for _, e := range edges {
        x, y := e[0], e[1]
        g[x] = append(g[x], y)
        g[y] = append(g[y], x)
    }

    ans := make([]int, n)
    for i := range ans {
        ans[i] = -1
    }
    type pair struct{ depth, id int }
    valDepthId := [mx]pair{}
    var dfs func(int, int, int)
    dfs = func(x, fa, depth int) {
        val := nums[x] // x 的节点值
        // 计算与 val 互质的数中，深度最大的节点编号
        maxDepth := 0
        for _, j := range coprime[val] {
            p := valDepthId[j]
            if p.depth > maxDepth {
                maxDepth = p.depth
                ans[x] = p.id
            }
        }

        tmp := valDepthId[val] // 用于恢复现场
        valDepthId[val] = pair{depth, x} // 保存 val 对应的节点深度和节点编号
        for _, y := range g[x] {
            if y != fa {
                dfs(y, x, depth+1)
            }
        }
        valDepthId[val] = tmp // 恢复现场
    }
    dfs(0, -1, 1)
    return ans
}

func gcd(a, b int) int {
    for a != 0 {
        a, b = b%a, a
    }
    return b
}
```

```js [sol-JavaScript]
// 预处理：coprime[i] 保存 [1, MX) 中与 i 互质的所有元素
const MX = 51;
const coprime = Array.from({length: MX}, () => []);
for (let i = 1; i < MX; i++) {
    for (let j = 1; j < MX; j++) {
        if (gcd(i, j) === 1) {
            coprime[i].push(j);
        }
    }
}

var getCoprimes = function(nums, edges) {
    const n = nums.length;
    const g = Array.from({length: n}, () => []);
    for (const [x, y] of edges) {
        g[x].push(y);
        g[y].push(x);
    }

    const ans = Array(n).fill(-1);
    const valDepthId = Array.from({length: MX}, () => [0, 0]);
    function dfs(x, fa, depth) {
        const val = nums[x]; // x 的节点值
        // 计算与 val 互质的数中，深度最大的节点编号
        let maxDepth = 0;
        for (const j of coprime[val]) {
            const [depth, id] = valDepthId[j];
            if (depth > maxDepth) {
                maxDepth = depth;
                ans[x] = id;
            }
        }

        const tmp = valDepthId[val]; // 用于恢复现场
        valDepthId[val] = [depth, x]; // 保存 val 对应的节点深度和节点编号
        for (const y of g[x]) {
            if (y !== fa) {
                dfs(y, x, depth + 1);
            }
        }
        valDepthId[val] = tmp; // 恢复现场
    }
    dfs(0, -1, 1);
    return ans;
};

function gcd(a, b) {
    return b === 0 ? a : gcd(b, a % b);
}
```

```rust [sol-Rust]
impl Solution {
    pub fn get_coprimes(nums: Vec<i32>, edges: Vec<Vec<i32>>) -> Vec<i32> {
        let n = nums.len();
        let mut g = vec![vec![]; n];
        for e in &edges {
            let x = e[0] as usize;
            let y = e[1] as usize;
            g[x].push(y);
            g[y].push(x);
        }

        fn gcd(a: usize, b: usize) -> usize {
            if b == 0 { a } else { gcd(b, a % b) }
        }
        // 预处理：coprime[i] 保存 [1, MX) 中与 i 互质的所有元素
        const MX: usize = 51;
        let mut coprime = vec![vec![]; MX];
        for i in 1..MX {
            for j in 1..MX {
                if gcd(i, j) == 1 {
                    coprime[i].push(j);
                }
            }
        }

        let mut ans = vec![-1; n];
        let mut val_depth_id = [(0, 0); MX];
        fn dfs(x: usize, fa: usize, depth: i32, g: &Vec<Vec<usize>>, nums: &Vec<i32>, coprime: &Vec<Vec<usize>>, ans: &mut Vec<i32>, val_depth_id: &mut [(i32, usize); MX]) {
            let val = nums[x] as usize; // x 的节点值
            // 计算与 val 互质的数中，深度最大的节点编号
            let mut max_depth = 0;
            for &j in &coprime[val] {
                let (depth, id) = val_depth_id[j];
                if depth > max_depth {
                    max_depth = depth;
                    ans[x] = id as i32;
                }
            }

            let tmp = val_depth_id[val]; // 用于恢复现场
            val_depth_id[val] = (depth, x); // 保存 val 对应的节点深度和节点编号
            for &y in &g[x] {
                if y != fa {
                    dfs(y, x, depth + 1, g, nums, coprime, ans, val_depth_id);
                }
            }
            val_depth_id[val] = tmp; // 恢复现场
        }
        dfs(0, 0, 1, &g, &nums, &coprime, &mut ans, &mut val_depth_id);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nU)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})=50$。
- 空间复杂度：$\mathcal{O}(n+U)$。忽略预处理的时间和空间。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
