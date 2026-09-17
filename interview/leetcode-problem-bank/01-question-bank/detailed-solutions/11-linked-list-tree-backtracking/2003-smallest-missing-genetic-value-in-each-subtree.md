# 2003. 每棵子树内缺失的最小基因值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-missing-genetic-value-in-each-subtree/
- 题目 slug：`smallest-missing-genetic-value-in-each-subtree`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.10 树上启发式合并
- 难度分：2415
- 外部题解来源：https://leetcode.cn/problems/smallest-missing-genetic-value-in-each-subtree/solutions/2505883/tu-jie-yi-zhang-tu-miao-dong-duo-chong-x-q095/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一张图秒懂！多种写法！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/smallest-missing-genetic-value-in-each-subtree/solutions/2505883/tu-jie-yi-zhang-tu-miao-dong-duo-chong-x-q095/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-yi-zhang-tu-miao-dong-duo-chong-x-q095`
- topic id：`2505883`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

![lc2003-c.png](https://pic.leetcode.cn/1698675788-vTZtZN-lc2003-c.png)

## 实现细节

在从下往上走的过程中，由于上面的节点（对应的子树）一定包含下面的节点，所以下面节点的基因值集合，一定是上面节点的基因值集合的子集，例如图中 $\{1\}\subset\{1,2,3\}\subset \{1,2,3,4,5,6\}$。所以上面节点的 $\textit{ans}$ 值一定大于等于下面节点的 $\textit{ans}$ 值。

这意味着，在计算 $\textit{ans}[i]$ 时，不需要从 $1$ 开始枚举，而是从 $\textit{ans}[j]$ 开始枚举（假设从 $j$ 往上走到了 $i$）。这可以将时间复杂度优化至 $\mathcal{O}(n)$，具体见文末的「复杂度分析」。

此外，「遍历子树」可以用 DFS 递归遍历，也可以用栈或者队列保存接下来需要遍历的节点，这引出了下面几种不同的写法。

#### 写法一：DFS

```py [sol-Python3]
class Solution:
    def smallestMissingValueSubtree(self, parents: List[int], nums: List[int]) -> List[int]:
        n = len(parents)
        ans = [1] * n
        if 1 not in nums:  # 不存在基因值为 1 的节点
            return ans

        # 建树
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parents[i]].append(i)

        vis = set()
        def dfs(x: int) -> None:
            vis.add(nums[x])  # 标记基因值
            for son in g[x]:
                if nums[son] not in vis:
                    dfs(son)

        mex = 2  # 缺失的最小基因值
        node = nums.index(1)  # 出发点
        while node >= 0:
            dfs(node)
            while mex in vis:  # node 子树包含这个基因值
                mex += 1
            ans[node] = mex  # 缺失的最小基因值
            node = parents[node]  # 往上走
        return ans
```

```java [sol-Java]
class Solution {
    public int[] smallestMissingValueSubtree(int[] parents, int[] nums) {
        int n = parents.length;
        int[] ans = new int[n];
        Arrays.fill(ans, 1);
        int node = -1;
        for (int i = 0; i < n; i++) {
            if (nums[i] == 1) {
                node = i; // 出发点
                break;
            }
        }
        if (node < 0) { // 不存在基因值为 1 的节点
            return ans;
        }

        // 建树
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int i = 1; i < n; ++i) {
            g[parents[i]].add(i);
        }

        Set<Integer> vis = new HashSet<>();
        int mex = 2; // 缺失的最小基因值
        while (node >= 0) {
            dfs(node, g, vis, nums);
            while (vis.contains(mex)) { // node 子树包含这个基因值
                mex++;
            }
            ans[node] = mex; // 缺失的最小基因值
            node = parents[node]; // 往上走
        }
        return ans;
    }

    // 遍历 x 子树
    private void dfs(int x, List<Integer>[] g, Set<Integer> vis, int[] nums) {
        vis.add(nums[x]); // 标记基因值
        for (int son : g[x]) {
            if (!vis.contains(nums[son])) {
                dfs(son, g, vis, nums);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> smallestMissingValueSubtree(vector<int> &parents, vector<int> &nums) {
        int n = parents.size();
        vector<int> ans(n, 1);
        auto it = find(nums.begin(), nums.end(), 1);
        if (it == nums.end()) { // 不存在基因值为 1 的节点
            return ans;
        }

        // 建树
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parents[i]].push_back(i);
        }

        unordered_set<int> vis;
        function<void(int)> dfs = [&](int x) -> void {
            vis.insert(nums[x]); // 标记基因值
            for (int son: g[x]) {
                if (!vis.contains(nums[son])) {
                    dfs(son);
                }
            }
        };

        int mex = 2; // 缺失的最小基因值
        int node = it - nums.begin();
        while (node >= 0) {
            dfs(node);
            while (vis.contains(mex)) { // node 子树包含这个基因值
                mex++;
            }
            ans[node] = mex; // 缺失的最小基因值
            node = parents[node]; // 往上走
        }
        return ans;
    }
};
```

```go [sol-Go]
func smallestMissingValueSubtree(parents []int, nums []int) []int {
    n := len(parents)
    ans := make([]int, n)
    for i := range ans {
        ans[i] = 1
    }
    node := slices.Index(nums, 1)
    if node < 0 { // 不存在基因值为 1 的节点
        return ans
    }

    // 建树
    g := make([][]int, n)
    for i := 1; i < n; i++ {
        p := parents[i]
        g[p] = append(g[p], i)
    }

    vis := make(map[int]bool, n)
    var dfs func(int)
    dfs = func(x int) {
        vis[nums[x]] = true // 标记基因值
        for _, son := range g[x] {
            if !vis[nums[son]] { // 避免重复访问节点
                dfs(son)
            }
        }
    }

    for mex := 2; node >= 0; node = parents[node] {
        dfs(node)
        for vis[mex] { // node 子树包含这个基因值
            mex++
        }
        ans[node] = mex // 缺失的最小基因值
    }
    return ans
}
```

```js [sol-JavaScript]
var smallestMissingValueSubtree = function (parents, nums) {
    const n = parents.length;
    const ans = Array(n).fill(1);
    let node = nums.indexOf(1);
    if (node < 0) { // 不存在基因值为 1 的节点
        return ans;
    }

    // 建树
    const g = Array(n).fill(null).map(() => []);
    for (let i = 1; i < n; i++) {
        g[parents[i]].push(i);
    }

    const vis = new Set();
    function dfs(x) {
        vis.add(nums[x]);
        for (const son of g[x]) {
            if (!vis.has(nums[son])) {
                dfs(son);
            }
        }
    }

    let mex = 2; // 缺失的最小基因值
    while (node >= 0) {
        dfs(node);
        while (vis.has(mex)) { // node 子树包含这个基因值
            mex++;
        }
        ans[node] = mex; // 缺失的最小基因值
        node = parents[node]; // 往上走
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn smallest_missing_value_subtree(parents: Vec<i32>, nums: Vec<i32>) -> Vec<i32> {
        let n = parents.len();
        let mut ans = vec![1; n];
        let mut node = match nums.iter().position(|&x| x == 1) {
            Some(i) => i as i32,
            None => return ans, // 不存在基因值为 1 的节点
        };

        // 建树
        let mut g = vec![vec![]; n];
        for i in 1..n {
            g[parents[i] as usize].push(i);
        }

        // 遍历 x 子树
        fn dfs(x: usize, g: &Vec<Vec<usize>>, vis: &mut HashSet<i32>, nums: &Vec<i32>) {
            vis.insert(nums[x]); // 标记基因值
            for &son in &g[x] {
                if !vis.contains(&nums[son]) {
                    dfs(son, g, vis, nums);
                }
            }
        }

        let mut vis = HashSet::new();
        let mut mex = 2; // 缺失的最小基因值
        while node >= 0 {
            dfs(node as usize, &g, &mut vis, &nums);
            while vis.contains(&mex) { // node 子树包含这个基因值
                mex += 1;
            }
            ans[node as usize] = mex; // 缺失的最小基因值
            node = parents[node as usize]; // 往上走
        }
        ans
    }
}
```

#### 写法二：非递归

改为手动记录接下来要访问的点。

此外，假设 $\textit{pre}$ 是下面的点（从 $\textit{pre}$ 往上走到当前节点），那么遍历子树的时候可以跳过 $\textit{pre}$ 子树。

```py [sol-Python3]
class Solution:
    def smallestMissingValueSubtree(self, parents: List[int], nums: List[int]) -> List[int]:
        n = len(parents)
        ans = [1] * n
        if 1 not in nums:  # 不存在基因值为 1 的节点
            return ans

        # 建树
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parents[i]].append(i)

        vis = set()
        mex = 2  # 缺失的最小基因值
        pre = -1
        node = nums.index(1)  # 出发点
        while node >= 0:
            vis.add(nums[node])  # 标记基因值
            nodes = [son for son in g[node] if son != pre]
            while nodes:
                x = nodes.pop()
                vis.add(nums[x])  # 标记基因值
                nodes.extend(g[x])  # 保存接下来需要遍历的点
            while mex in vis:  # node 子树包含这个基因值
                mex += 1
            ans[node] = mex  # 缺失的最小基因值
            pre = node  # 下一轮循环不会遍历 pre 子树
            node = parents[node]  # 往上走
        return ans
```

```java [sol-Java]
class Solution {
    public int[] smallestMissingValueSubtree(int[] parents, int[] nums) {
        int n = parents.length;
        int[] ans = new int[n];
        Arrays.fill(ans, 1);
        int node = -1;
        for (int i = 0; i < n; i++) {
            if (nums[i] == 1) {
                node = i; // 出发点
                break;
            }
        }
        if (node < 0) { // 不存在基因值为 1 的节点
            return ans;
        }

        // 建树
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            g[parents[i]].add(i);
        }

        Set<Integer> vis = new HashSet<>();
        List<Integer> nodes = new ArrayList<>(); // 保存接下来需要遍历的点
        int mex = 2; // 缺失的最小基因值
        int pre = -1;
        while (node >= 0) {
            vis.add(nums[node]); // 标记基因值
            for (int son : g[node]) {
                if (son != pre) { // pre 子树已经遍历过了
                    nodes.add(son); // 保存接下来需要遍历的点
                }
            }
            while (!nodes.isEmpty()) {
                int x = nodes.remove(nodes.size() - 1);
                vis.add(nums[x]); // 标记基因值
                nodes.addAll(g[x]); // 保存接下来需要遍历的点
            }
            while (vis.contains(mex)) { // node 子树包含这个基因值
                mex++;
            }
            ans[node] = mex; // 缺失的最小基因值
            pre = node; // 下一轮循环不会遍历 pre 子树
            node = parents[node]; // 往上走
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> smallestMissingValueSubtree(vector<int> &parents, vector<int> &nums) {
        int n = parents.size();
        vector<int> ans(n, 1);
        auto it = find(nums.begin(), nums.end(), 1);
        if (it == nums.end()) { // 不存在基因值为 1 的节点
            return ans;
        }

        // 建树
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parents[i]].push_back(i);
        }

        unordered_set<int> vis;
        stack<int> nodes; // 保存接下来需要遍历的点
        int mex = 2; // 缺失的最小基因值
        int pre = -1;
        int node = it - nums.begin();
        while (node >= 0) {
            vis.insert(nums[node]); // 标记基因值
            for (int son: g[node]) {
                if (son != pre) { // pre 子树已经遍历过了
                    nodes.push(son); // 保存接下来需要遍历的点
                }
            }
            while (!nodes.empty()) {
                int x = nodes.top();
                nodes.pop();
                vis.insert(nums[x]); // 标记基因值
                for (int son: g[x]) {
                    nodes.push(son); // 保存接下来需要遍历的点
                }
            }
            while (vis.contains(mex)) { // node 子树包含这个基因值
                mex++;
            }
            ans[node] = mex; // 缺失的最小基因值
            pre = node; // 下一轮循环不会遍历 pre 子树
            node = parents[node]; // 往上走
        }
        return ans;
    }
};
```

```go [sol-Go]
func smallestMissingValueSubtree(parents []int, nums []int) []int {
    n := len(parents)
    ans := make([]int, n)
    for i := range ans {
        ans[i] = 1
    }
    node := slices.Index(nums, 1)
    if node < 0 { // 不存在基因值为 1 的节点
        return ans
    }

    // 建树
    g := make([][]int, n)
    for i := 1; i < n; i++ {
        p := parents[i]
        g[p] = append(g[p], i)
    }

    vis := make(map[int]bool, n)
    nodes := []int{} // 保存接下来需要遍历的点
    for mex, pre := 2, -1; node >= 0; node = parents[node] {
        vis[nums[node]] = true // 标记基因值
        for _, son := range g[node] {
            if son != pre { // pre 子树已经遍历过了
                nodes = append(nodes, son) // 保存接下来需要遍历的点
            }
        }
        for len(nodes) > 0 {
            x := nodes[len(nodes)-1]
            nodes = nodes[:len(nodes)-1]
            vis[nums[x]] = true // 标记基因值
            nodes = append(nodes, g[x]...) // 保存接下来需要遍历的点
        }
        for vis[mex] { // node 子树包含这个基因值
            mex++
        }
        ans[node] = mex // 缺失的最小基因值
        pre = node // 下一轮循环不会遍历 pre 子树
    }
    return ans
}
```

```js [sol-JavaScript]
var smallestMissingValueSubtree = function (parents, nums) {
    const n = parents.length;
    const ans = Array(n).fill(1);
    let node = nums.indexOf(1);
    if (node < 0) { // 不存在基因值为 1 的节点
        return ans;
    }

    // 建树
    const g = Array(n).fill(null).map(() => []);
    for (let i = 1; i < n; i++) {
        g[parents[i]].push(i);
    }

    const vis = new Set();
    const nodes = []; // 保存接下来需要遍历的点
    let mex = 2; // 缺失的最小基因值
    let pre = -1;
    while (node >= 0) {
        vis.add(nums[node]); // 标记基因值
        for (const son of g[node]) {
            if (son !== pre) { // pre 子树已经遍历过了
                nodes.push(son); // 保存接下来需要遍历的点
            }
        }
        while (nodes.length) {
            const x = nodes.pop();
            vis.add(nums[x]); // 标记基因值
            nodes.push(...g[x]); // 保存接下来需要遍历的点
        }
        while (vis.has(mex)) { // node 子树包含这个基因值
            mex++;
        }
        ans[node] = mex; // 缺失的最小基因值
        pre = node; // 下一轮循环不会遍历 pre 子树
        node = parents[node]; // 往上走
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn smallest_missing_value_subtree(parents: Vec<i32>, nums: Vec<i32>) -> Vec<i32> {
        let n = parents.len();
        let mut ans = vec![1; n];
        let mut node = match nums.iter().position(|&x| x == 1) {
            Some(i) => i as i32,
            None => return ans, // 不存在基因值为 1 的节点
        };

        // 建树
        let mut g = vec![vec![]; n];
        for i in 1..n {
            g[parents[i] as usize].push(i);
        }

        let mut vis = HashSet::new();
        let mut nodes = Vec::new(); // 保存接下来需要遍历的点
        let mut mex = 2; // 缺失的最小基因值
        let mut pre = n;
        while node >= 0 {
            vis.insert(nums[node as usize]); // 标记基因值
            for &son in &g[node as usize] {
                if son != pre { // pre 子树已经遍历过了
                    nodes.push(son); // 保存接下来需要遍历的点
                }
            }
            while let Some(x) = nodes.pop() {
                vis.insert(nums[x]); // 标记基因值
                nodes.append(&mut g[x]); // 保存接下来需要遍历的点
            }
            while vis.contains(&mex) { // node 子树包含这个基因值
                mex += 1;
            }
            ans[node as usize] = mex; // 缺失的最小基因值
            pre = node as usize; // 下一轮循环不会遍历 pre 子树
            node = parents[node as usize]; // 往上走
        }
        ans
    }
}
```

#### 写法三：非递归+数组

注意到，即使 $\textit{nums}$ 包含了 $1$ 到 $n$ 中的所有数，$\textit{ans}[i]$ 最大也不会超过 $n+1$，所以对于超过 $n+1$ 的 $\textit{nums}[i]$ 来说，可以视作 $n+2$。

进一步地，其实可以视作 $n+1$，因为只要存在一个超过 $n$ 的 $\textit{nums}[i]$，那么 $\textit{ans}[i]$ 最大不超过 $n$。例如 $\textit{nums}=[1,2,99]$，$\textit{ans}[i]$ 最大为 $3$，也就是 $n$。

所以可以把哈希表替换成一个长为 $n+2$ 的布尔数组，记录各个节点是否访问过。

> 注：还可以用链式前向星优化建树过程，感兴趣的读者可查阅相关资料。

```py [sol-Python3]
class Solution:
    def smallestMissingValueSubtree(self, parents: List[int], nums: List[int]) -> List[int]:
        n = len(parents)
        ans = [1] * n
        if 1 not in nums:  # 不存在基因值为 1 的节点
            return ans

        # 建树
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parents[i]].append(i)

        vis = [False] * (n + 2)
        mex = 2  # 缺失的最小基因值
        pre = -1
        node = nums.index(1)  # 出发点
        while node >= 0:
            vis[min(nums[node], n + 1)] = True  # 标记基因值
            nodes = [son for son in g[node] if son != pre]
            while nodes:
                x = nodes.pop()
                vis[min(nums[x], n + 1)] = True  # 标记基因值
                nodes.extend(g[x])  # 保存接下来需要遍历的点
            while vis[mex]:  # node 子树包含这个基因值
                mex += 1
            ans[node] = mex  # 缺失的最小基因值
            pre = node  # 下一轮循环不会遍历 pre 子树
            node = parents[node]  # 往上走
        return ans
```

```java [sol-Java]
class Solution {
    public int[] smallestMissingValueSubtree(int[] parents, int[] nums) {
        int n = parents.length;
        int[] ans = new int[n];
        Arrays.fill(ans, 1);
        int node = -1;
        for (int i = 0; i < n; i++) {
            if (nums[i] == 1) {
                node = i; // 出发点
                break;
            }
        }
        if (node < 0) { // 不存在基因值为 1 的节点
            return ans;
        }

        // 建树
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            g[parents[i]].add(i);
        }

        boolean[] vis = new boolean[n + 2];
        List<Integer> nodes = new ArrayList<>(); // 保存接下来需要遍历的点
        int mex = 2; // 缺失的最小基因值
        int pre = -1;
        while (node >= 0) {
            vis[Math.min(nums[node], n + 1)] = true; // 标记基因值
            for (int son : g[node]) {
                if (son != pre) { // pre 子树已经遍历过了
                    nodes.add(son); // 保存接下来需要遍历的点
                }
            }
            while (!nodes.isEmpty()) {
                int x = nodes.remove(nodes.size() - 1);
                vis[Math.min(nums[x], n + 1)] = true; // 标记基因值
                nodes.addAll(g[x]); // 保存接下来需要遍历的点
            }
            while (vis[mex]) { // node 子树包含这个基因值
                mex++;
            }
            ans[node] = mex; // 缺失的最小基因值
            pre = node; // 下一轮循环不会遍历 pre 子树
            node = parents[node]; // 往上走
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> smallestMissingValueSubtree(vector<int> &parents, vector<int> &nums) {
        int n = parents.size();
        vector<int> ans(n, 1);
        auto it = find(nums.begin(), nums.end(), 1);
        if (it == nums.end()) { // 不存在基因值为 1 的节点
            return ans;
        }

        // 建树
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            g[parents[i]].push_back(i);
        }

        vector<int> vis(n + 2);
        stack<int> nodes; // 保存接下来需要遍历的点
        int mex = 2; // 缺失的最小基因值
        int pre = -1;
        int node = it - nums.begin();
        while (node >= 0) {
            vis[min(nums[node], n + 1)] = true; // 标记基因值
            for (int son: g[node]) {
                if (son != pre) { // pre 子树已经遍历过了
                    nodes.push(son); // 保存接下来需要遍历的点
                }
            }
            while (!nodes.empty()) {
                int x = nodes.top();
                nodes.pop();
                vis[min(nums[x], n + 1)] = true; // 标记基因值
                for (int son: g[x]) {
                    nodes.push(son); // 保存接下来需要遍历的点
                }
            }
            while (vis[mex]) { // node 子树包含这个基因值
                mex++;
            }
            ans[node] = mex; // 缺失的最小基因值
            pre = node; // 下一轮循环不会遍历 pre 子树
            node = parents[node]; // 往上走
        }
        return ans;
    }
};
```

```go [sol-Go]
func smallestMissingValueSubtree(parents []int, nums []int) []int {
    n := len(parents)
    ans := make([]int, n)
    for i := range ans {
        ans[i] = 1
    }
    node := slices.Index(nums, 1)
    if node < 0 { // 不存在基因值为 1 的节点
        return ans
    }

    // 建树
    g := make([][]int, n)
    for i := 1; i < n; i++ {
        p := parents[i]
        g[p] = append(g[p], i)
    }

    vis := make([]bool, n+2)
    nodes := []int{} // 保存接下来需要遍历的点
    for mex, pre := 2, -1; node >= 0; node = parents[node] {
        vis[min(nums[node], n+1)] = true // 标记基因值
        for _, son := range g[node] {
            if son != pre { // pre 子树已经遍历过了
                nodes = append(nodes, son) // 保存接下来需要遍历的点
            }
        }
        for len(nodes) > 0 {
            x := nodes[len(nodes)-1]
            nodes = nodes[:len(nodes)-1]
            vis[min(nums[x], n+1)] = true  // 标记基因值
            nodes = append(nodes, g[x]...) // 保存接下来需要遍历的点
        }
        for vis[mex] { // node 子树包含这个基因值
            mex++
        }
        ans[node] = mex // 缺失的最小基因值
        pre = node      // 下一轮循环不会遍历 pre 子树
    }
    return ans
}
```

```js [sol-JavaScript]
var smallestMissingValueSubtree = function (parents, nums) {
    const n = parents.length;
    const ans = Array(n).fill(1);
    let node = nums.indexOf(1);
    if (node < 0) { // 不存在基因值为 1 的节点
        return ans;
    }

    // 建树
    const g = Array(n).fill(null).map(() => []);
    for (let i = 1; i < n; i++) {
        g[parents[i]].push(i);
    }

    const vis = Array(n + 2).fill(false);
    const nodes = []; // 保存接下来需要遍历的点
    let mex = 2; // 缺失的最小基因值
    let pre = -1;
    while (node >= 0) {
        vis[Math.min(nums[node], n + 1)] = true; // 标记基因值
        for (const son of g[node]) {
            if (son !== pre) { // pre 子树已经遍历过了
                nodes.push(son); // 保存接下来需要遍历的点
            }
        }
        while (nodes.length) {
            const x = nodes.pop();
            vis[Math.min(nums[x], n + 1)] = true; // 标记基因值
            nodes.push(...g[x]); // 保存接下来需要遍历的点
        }
        while (vis[mex]) { // node 子树包含这个基因值
            mex++;
        }
        ans[node] = mex; // 缺失的最小基因值
        pre = node; // 下一轮循环不会遍历 pre 子树
        node = parents[node]; // 往上走
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn smallest_missing_value_subtree(parents: Vec<i32>, nums: Vec<i32>) -> Vec<i32> {
        let n = parents.len();
        let mut ans = vec![1; n];
        let mut node = match nums.iter().position(|&x| x == 1) {
            Some(i) => i as i32,
            None => return ans, // 不存在基因值为 1 的节点
        };

        // 建树
        let mut g = vec![vec![]; n];
        for i in 1..n {
            g[parents[i] as usize].push(i);
        }

        let mut vis = vec![false; n + 2];
        let mut nodes = Vec::new(); // 保存接下来需要遍历的点
        let mut mex = 2; // 缺失的最小基因值
        let mut pre = n;
        while node >= 0 {
            vis[(nums[node as usize] as usize).min(n + 1)] = true; // 标记基因值
            for &son in &g[node as usize] {
                if son != pre { // pre 子树已经遍历过了
                    nodes.push(son); // 保存接下来需要遍历的点
                }
            }
            while let Some(x) = nodes.pop() {
                vis[(nums[x] as usize).min(n + 1)] = true; // 标记基因值
                nodes.append(&mut g[x]); // 保存接下来需要遍历的点
            }
            while vis[mex as usize] { // node 子树包含这个基因值
                mex += 1;
            }
            ans[node as usize] = mex; // 缺失的最小基因值
            pre = node as usize; // 下一轮循环不会遍历 pre 子树
            node = parents[node as usize]; // 往上走
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。由于不会访问之前访问过的节点，所以每个节点只会访问一次。此外，内层循环中的 `mex++` 这句话至多执行 $n-1$ 次（因为 $\textit{mex}$ 初始值为 $2$ 且不会超过 $n+1$，注意内层循环没有重置 $\textit{mex}=2$），所以二重循环的循环次数是 $\mathcal{O}(n)$ 的。所以时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.10 树上启发式合并`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.10 树上启发式合并`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
