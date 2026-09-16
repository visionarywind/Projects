# 2127. 参加会议的最多员工数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/
- 题目 slug：`maximum-employees-to-be-invited-to-a-meeting`
- 来源专题：图论算法
- 来源分类路径：二、拓扑排序 / §2.3 基环树
- 难度分：2449
- 外部题解来源：https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/solutions/1187830/nei-xiang-ji-huan-shu-tuo-bu-pai-xu-fen-c1i1b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[内向基环树：拓扑排序 + 分类讨论（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/solutions/1187830/nei-xiang-ji-huan-shu-tuo-bu-pai-xu-fen-c1i1b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nei-xiang-ji-huan-shu-tuo-bu-pai-xu-fen-c1i1b`
- topic id：`1187830`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

从 $i$ 向 $\textit{favorite}[i]$ 连边，可以得到一张有向图。由于每个大小为 $k$ 的连通块都有 $k$ 个点和 $k$ 条边，所以每个连通块必定有且仅有一个环，且由于每个点的出度均为 $1$，这样的有向图又叫做内向**基环树 (pseudotree)**，由基环树组成的森林叫**基环树森林 (pseudoforest)**。

每一个内向基环树（连通块）都由一个**基环**和其余指向基环的**树枝**组成。例如示例 $3$ 可以得到如下内向基环树，其基环由节点 $0$、$1$、$3$ 和 $4$ 组成，节点 $2$ 为其树枝：

![1.png](https://pic.leetcode.cn/1641096462-IsWZUX-1.png)

特别地，基环可能只包含两个节点。例如示例 $1$ 可以得到如下内向基环树，其基环只包含节点 $1$ 和 $2$，而节点 $0$ 和 $3$ 组成其树枝：

![2.png](https://pic.leetcode.cn/1641096467-KCwxMo-2.png)

对于本题来说，这两类基环树在组成圆桌时会有明显区别，下文会说明这一点。

先来看看基环长度大于 $2$ 的情况。基环上的节点是可以组成一个圆桌的；而树枝上的点，若插入圆桌上 $x\rightarrow y$ 这两人中间，会导致节点 $x$ 无法和其喜欢的员工坐在一起，因此**树枝上的点是无法插入圆桌的**；此外，树枝上的点也不能单独组成圆桌，因为这样会存在一个出度为 $0$ 的节点，无法和其喜欢的员工坐在一起。对于其余内向基环树（连通块）上的节点，和树枝同理，也无法插入该基环组成的圆桌。

因此，对于基环长度大于 $2$ 的情况，圆桌的最大员工数目即为最大的基环长度，记作 $\textit{maxRingSize}$。

下面来分析基环长度等于 $2$ 的情况。

以如下基环树为例，$0$ 和 $1$ 组成基环，其余节点组成树枝：

![3.png](https://pic.leetcode.cn/1641096473-JtGBgY-3.png)

可以先让 $0$ 和 $1$ 坐在圆桌旁（假设 $0$ 坐在 $1$ 左侧），那么 $0$ 这一侧的树枝只能坐在 $0$ 的左侧，而 $1$ 这一侧的树枝只能坐在 $1$ 的右侧。

$2$ 可以紧靠着坐在 $0$ 的左侧，而 $3$ 和 $4$ 只能选一个坐在 $2$ 的左侧（如果 $4$ 紧靠着坐在 $2$ 的左侧，那么 $3$ 是无法紧靠着坐在 $4$ 的左侧的，反之亦然）。

这意味着从 $0$ 出发倒着找树枝上的点（即沿着反图上的边），每个点只能在其反图上选择一个儿子，因此 $0$ 这一侧的节点必须组成一条链，那么可以找**最长的那条链**，即上图加粗的节点。

对于 $1$ 这一侧也同理。将这两条最长链拼起来，即为该基环树能组成的圆桌的最大员工数。

对于多个基环长度等于 $2$ 的基环树，每个基环树所对应的链，都可以拼在其余链的末尾，因此可以**将这些链全部拼成一个圆桌**，其大小记作 $\textit{sumChainSize}$。

答案即为 $\max(\textit{maxRingSize},\textit{sumChainSize})$。

## 实现细节

通过一次拓扑排序，可以「剪掉」所有树枝。因为拓扑排序后，树枝节点的入度均为 $0$，基环节点的入度均为 $1$。这样就可以将基环和树枝区分开，从而简化后续处理流程：

- 如果要遍历基环，可以从入度为 $1$ 的节点出发，遍历其余入度为 $1$ 的节点。
- 如果要遍历树枝，可以以基环与树枝的连接处为起点，顺着反图来遍历树枝，从而将问题转化成一个树形问题。

注意创建反图的过程可以在拓扑排序中完成，这样创建的反图是不包含基环的，遍历的时候更方便。

对于本题，可以遍历所有基环，并按基环长度分类计算：

- 对于长度大于 $2$ 的基环，取基环长度的最大值；
- 对于长度等于 $2$ 的基环，可以从基环上的点出发，在反图上找到最大的树枝节点深度。

```py [sol-Python3]
class Solution:
    def maximumInvitations(self, favorite: List[int]) -> int:
        n = len(favorite)
        deg = [0] * n
        for f in favorite:
            deg[f] += 1  # 统计基环树每个节点的入度

        rg = [[] for _ in range(n)]  # 反图
        q = deque(i for i, d in enumerate(deg) if d == 0)
        while q:  # 拓扑排序，剪掉图上所有树枝
            x = q.popleft()
            y = favorite[x]  # x 只有一条出边
            rg[y].append(x)
            deg[y] -= 1
            if deg[y] == 0:
                q.append(y)

        # 通过反图 rg 寻找树枝上最深的链
        def rdfs(x: int) -> int:
            max_depth = 1
            for son in rg[x]:
                max_depth = max(max_depth, rdfs(son) + 1)
            return max_depth

        max_ring_size = sum_chain_size = 0
        for i, d in enumerate(deg):
            if d == 0: continue

            # 遍历基环上的点
            deg[i] = 0  # 将基环上的点的入度标记为 0，避免重复访问
            ring_size = 1  # 基环长度
            x = favorite[i]
            while x != i:
                deg[x] = 0  # 将基环上的点的入度标记为 0，避免重复访问
                ring_size += 1
                x = favorite[x]

            if ring_size == 2:  # 基环长度为 2
                sum_chain_size += rdfs(i) + rdfs(favorite[i])  # 累加两条最长链的长度
            else:
                max_ring_size = max(max_ring_size, ring_size)  # 取所有基环长度的最大值
        return max(max_ring_size, sum_chain_size)
```

```java [sol-Java]
class Solution {
    public int maximumInvitations(int[] favorite) {
        int n = favorite.length;
        int[] deg = new int[n];
        for (int f : favorite) {
            deg[f]++; // 统计基环树每个节点的入度
        }

        List<Integer>[] rg = new List[n]; // 反图
        Arrays.setAll(rg, e -> new ArrayList<>());
        Deque<Integer> q = new ArrayDeque<>();
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) {
                q.add(i);
            }
        }
        while (!q.isEmpty()) { // 拓扑排序，剪掉图上所有树枝
            int x = q.poll();
            int y = favorite[x]; // x 只有一条出边
            rg[y].add(x);
            if (--deg[y] == 0) {
                q.add(y);
            }
        }

        int maxRingSize = 0, sumChainSize = 0;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) continue;

            // 遍历基环上的点
            deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            int ringSize = 1; // 基环长度
            for (int x = favorite[i]; x != i; x = favorite[x]) {
                deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
                ringSize++;
            }

            if (ringSize == 2) { // 基环长度为 2
                sumChainSize += rdfs(i, rg) + rdfs(favorite[i], rg); // 累加两条最长链的长度
            } else {
                maxRingSize = Math.max(maxRingSize, ringSize); // 取所有基环长度的最大值
            }
        }
        return Math.max(maxRingSize, sumChainSize);
    }

    // 通过反图 rg 寻找树枝上最深的链
    private int rdfs(int x, List<Integer>[] rg) {
        int maxDepth = 1;
        for (int son : rg[x]) {
            maxDepth = Math.max(maxDepth, rdfs(son, rg) + 1);
        }
        return maxDepth;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumInvitations(vector<int> &favorite) {
        int n = favorite.size();
        vector<int> deg(n);
        for (int f: favorite) {
            deg[f]++; // 统计基环树每个节点的入度
        }

        vector<vector<int>> rg(n); // 反图
        queue<int> q;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) {
                q.push(i);
            }
        }
        while (!q.empty()) { // 拓扑排序，剪掉图上所有树枝
            int x = q.front();
            q.pop();
            int y = favorite[x]; // x 只有一条出边
            rg[y].push_back(x);
            if (--deg[y] == 0) {
                q.push(y);
            }
        }

        // 通过反图 rg 寻找树枝上最深的链
        function<int(int)> rdfs = [&](int x) -> int {
            int max_depth = 1;
            for (int son: rg[x]) {
                max_depth = max(max_depth, rdfs(son) + 1);
            }
            return max_depth;
        };

        int max_ring_size = 0, sum_chain_size = 0;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) continue;

            // 遍历基环上的点
            deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            int ring_size = 1; // 基环长度
            for (int x = favorite[i]; x != i; x = favorite[x]) {
                deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
                ring_size++;
            }

            if (ring_size == 2) { // 基环长度为 2
                sum_chain_size += rdfs(i) + rdfs(favorite[i]); // 累加两条最长链的长度
            } else {
                max_ring_size = max(max_ring_size, ring_size); // 取所有基环长度的最大值
            }
        }
        return max(max_ring_size, sum_chain_size);
    }
};
```

```go [sol-Go]
func maximumInvitations(favorite []int) int {
    n := len(favorite)
    deg := make([]int, n)
    for _, f := range favorite {
        deg[f]++ // 统计基环树每个节点的入度
    }

    rg := make([][]int, n) // 反图
    q := []int{}
    for i, d := range deg {
        if d == 0 {
            q = append(q, i)
        }
    }
    for len(q) > 0 { // 拓扑排序，剪掉图上所有树枝
        x := q[0]
        q = q[1:]
        y := favorite[x] // x 只有一条出边
        rg[y] = append(rg[y], x)
        if deg[y]--; deg[y] == 0 {
            q = append(q, y)
        }
    }

    // 通过反图 rg 寻找树枝上最深的链
    var rdfs func(int) int
    rdfs = func(x int) int {
        maxDepth := 1
        for _, son := range rg[x] {
            maxDepth = max(maxDepth, rdfs(son)+1)
        }
        return maxDepth
    }

    maxRingSize, sumChainSize := 0, 0
    for i, d := range deg {
        if d == 0 {
            continue
        }

        // 遍历基环上的点
        deg[i] = 0 // 将基环上的点的入度标记为 0，避免重复访问
        ringSize := 1 // 基环长度
        for x := favorite[i]; x != i; x = favorite[x] {
            deg[x] = 0 // 将基环上的点的入度标记为 0，避免重复访问
            ringSize++
        }

        if ringSize == 2 { // 基环长度为 2
            sumChainSize += rdfs(i) + rdfs(favorite[i]) // 累加两条最长链的长度
        } else {
            maxRingSize = max(maxRingSize, ringSize) // 取所有基环长度的最大值
        }
    }
    return max(maxRingSize, sumChainSize)
}
```

```js [sol-JavaScript]
var maximumInvitations = function (favorite) {
    const n = favorite.length;
    const deg = Array(n).fill(0);
    for (const f of favorite) {
        deg[f]++; // 统计基环树每个节点的入度
    }

    const rg = Array(n).fill(null).map(() => []); // 反图
    const q = new Queue();
    for (let i = 0; i < n; i++) {
        if (deg[i] === 0) {
            q.enqueue(i);
        }
    }
    while (!q.isEmpty()) { // 拓扑排序，剪掉图上所有树枝
        const x = q.dequeue();
        const y = favorite[x]; // x 只有一条出边
        rg[y].push(x);
        if (--deg[y] === 0) {
            q.enqueue(y);
        }
    }

    // 通过反图 rg 寻找树枝上最深的链
    function rdfs(x) {
        let maxDepth = 1;
        for (const son of rg[x]) {
            maxDepth = Math.max(maxDepth, rdfs(son) + 1);
        }
        return maxDepth;
    }

    let maxRingSize = 0, sumChainSize = 0;
    for (let i = 0; i < n; i++) {
        if (deg[i] === 0) continue;

        // 遍历基环上的点
        deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
        let ringSize = 1; // 基环长度
        for (let x = favorite[i]; x !== i; x = favorite[x]) {
            deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            ringSize++;
        }

        if (ringSize === 2) { // 基环长度为 2
            sumChainSize += rdfs(i) + rdfs(favorite[i]); // 累加两条最长链的长度
        } else {
            maxRingSize = Math.max(maxRingSize, ringSize); // 取所有基环长度的最大值
        }
    }
    return Math.max(maxRingSize, sumChainSize);
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn maximum_invitations(favorite: Vec<i32>) -> i32 {
        let n = favorite.len();
        let mut deg = vec![0; n];
        for &f in &favorite {
            deg[f as usize] += 1; // 统计基环树每个节点的入度
        }

        let mut rg = vec![vec![]; n]; // 反图
        let mut q = VecDeque::new();
        for (i, &d) in deg.iter().enumerate() {
            if d == 0 {
                q.push_back(i);
            }
        }
        while let Some(x) = q.pop_front() { // 拓扑排序，剪掉图上所有树枝
            let y = favorite[x] as usize; // x 只有一条出边
            rg[y].push(x);
            deg[y] -= 1;
            if deg[y] == 0 {
                q.push_back(y);
            }
        }

        // 通过反图 rg 寻找树枝上最深的链
        fn rdfs(x: usize, rg: &Vec<Vec<usize>>) -> i32 {
            let mut max_depth = 1;
            for &son in &rg[x] {
                max_depth = max_depth.max(rdfs(son, rg) + 1);
            }
            max_depth
        }

        let mut max_ring_size = 0;
        let mut sum_chain_size = 0;
        for i in 0..n {
            if deg[i] == 0 {
                continue;
            }

            // 遍历基环上的点
            deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            let mut ring_size = 1; // 基环长度
            let mut x = favorite[i] as usize;
            while x != i {
                deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
                ring_size += 1;
                x = favorite[x] as usize;
            }

            if ring_size == 2 { // 基环长度为 2
                sum_chain_size += rdfs(i, &rg) + rdfs(favorite[i] as usize, &rg); // 累加两条最长链的长度
            } else {
                max_ring_size = max_ring_size.max(ring_size); // 取所有基环长度的最大值
            }
        }
        max_ring_size.max(sum_chain_size)
    }
}
```

对于本题来说，还有更加简洁的写法（特化的写法）：在拓扑排序的同时，计算出最长链的长度，这样就不需要建反图，也不需要在反图上找最长链了。（参考 [@Class](/u/class_/) 的评论）

```py [sol-Python3]
class Solution:
    def maximumInvitations(self, favorite: List[int]) -> int:
        n = len(favorite)
        deg = [0] * n
        for f in favorite:
            deg[f] += 1  # 统计基环树每个节点的入度

        max_depth = [1] * n
        q = deque(i for i, d in enumerate(deg) if d == 0)
        while q:  # 拓扑排序，剪掉图上所有树枝
            x = q.popleft()
            y = favorite[x]  # x 只有一条出边
            max_depth[y] = max_depth[x] + 1
            deg[y] -= 1
            if deg[y] == 0:
                q.append(y)

        max_ring_size = sum_chain_size = 0
        for i, d in enumerate(deg):
            if d == 0: continue

            # 遍历基环上的点
            deg[i] = 0  # 将基环上的点的入度标记为 0，避免重复访问
            ring_size = 1  # 基环长度
            x = favorite[i]
            while x != i:
                deg[x] = 0  # 将基环上的点的入度标记为 0，避免重复访问
                ring_size += 1
                x = favorite[x]

            if ring_size == 2:  # 基环长度为 2
                sum_chain_size += max_depth[i] + max_depth[favorite[i]]  # 累加两条最长链的长度
            else:
                max_ring_size = max(max_ring_size, ring_size)  # 取所有基环长度的最大值
        return max(max_ring_size, sum_chain_size)
```

```java [sol-Java]
class Solution {
    public int maximumInvitations(int[] favorite) {
        int n = favorite.length;
        int[] deg = new int[n];
        for (int f : favorite) {
            deg[f]++; // 统计基环树每个节点的入度
        }

        int[] maxDepth = new int[n];
        Deque<Integer> q = new ArrayDeque<>();
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) {
                q.add(i);
            }
        }
        while (!q.isEmpty()) { // 拓扑排序，剪掉图上所有树枝
            int x = q.poll();
            int y = favorite[x]; // x 只有一条出边
            maxDepth[y] = maxDepth[x] + 1;
            if (--deg[y] == 0) {
                q.add(y);
            }
        }

        int maxRingSize = 0, sumChainSize = 0;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) continue;

            // 遍历基环上的点
            deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            int ringSize = 1; // 基环长度
            for (int x = favorite[i]; x != i; x = favorite[x]) {
                deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
                ringSize++;
            }

            if (ringSize == 2) { // 基环长度为 2
                sumChainSize += maxDepth[i] + maxDepth[favorite[i]] + 2; // 累加两条最长链的长度
            } else {
                maxRingSize = Math.max(maxRingSize, ringSize); // 取所有基环长度的最大值
            }
        }
        return Math.max(maxRingSize, sumChainSize);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumInvitations(vector<int> &favorite) {
        int n = favorite.size();
        vector<int> deg(n);
        for (int f: favorite) {
            deg[f]++; // 统计基环树每个节点的入度
        }

        vector<int> max_depth(n, 1);
        queue<int> q;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) {
                q.push(i);
            }
        }
        while (!q.empty()) { // 拓扑排序，剪掉图上所有树枝
            int x = q.front();
            q.pop();
            int y = favorite[x]; // x 只有一条出边
            max_depth[y] = max_depth[x] + 1;
            if (--deg[y] == 0) {
                q.push(y);
            }
        }

        int max_ring_size = 0, sum_chain_size = 0;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) continue;

            // 遍历基环上的点
            deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            int ring_size = 1; // 基环长度
            for (int x = favorite[i]; x != i; x = favorite[x]) {
                deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
                ring_size++;
            }

            if (ring_size == 2) { // 基环长度为 2
                sum_chain_size += max_depth[i] + max_depth[favorite[i]]; // 累加两条最长链的长度
            } else {
                max_ring_size = max(max_ring_size, ring_size); // 取所有基环长度的最大值
            }
        }
        return max(max_ring_size, sum_chain_size);
    }
};
```

```go [sol-Go]
func maximumInvitations(favorite []int) int {
    n := len(favorite)
    deg := make([]int, n)
    for _, f := range favorite {
        deg[f]++ // 统计基环树每个节点的入度
    }

    maxDepth := make([]int, n)
    q := []int{}
    for i, d := range deg {
        if d == 0 {
            q = append(q, i)
        }
    }
    for len(q) > 0 { // 拓扑排序，剪掉图上所有树枝
        x := q[0]
        q = q[1:]
        y := favorite[x] // x 只有一条出边
        maxDepth[y] = maxDepth[x] + 1
        if deg[y]--; deg[y] == 0 {
            q = append(q, y)
        }
    }

    maxRingSize, sumChainSize := 0, 0
    for i, d := range deg {
        if d == 0 {
            continue
        }

        // 遍历基环上的点
        deg[i] = 0 // 将基环上的点的入度标记为 0，避免重复访问
        ringSize := 1 // 基环长度
        for x := favorite[i]; x != i; x = favorite[x] {
            deg[x] = 0 // 将基环上的点的入度标记为 0，避免重复访问
            ringSize++
        }

        if ringSize == 2 { // 基环长度为 2
            sumChainSize += maxDepth[i] + maxDepth[favorite[i]] + 2 // 累加两条最长链的长度
        } else {
            maxRingSize = max(maxRingSize, ringSize) // 取所有基环长度的最大值
        }
    }
    return max(maxRingSize, sumChainSize)
}
```

```js [sol-JavaScript]
var maximumInvitations = function (favorite) {
    const n = favorite.length;
    const deg = Array(n).fill(0);
    for (const f of favorite) {
        deg[f]++; // 统计基环树每个节点的入度
    }

    const maxDepth = Array(n).fill(1);
    const rg = Array(n).fill(null).map(() => []);
    const q = new Queue();
    for (let i = 0; i < n; i++) {
        if (deg[i] === 0) {
            q.enqueue(i);
        }
    }
    while (!q.isEmpty()) { // 拓扑排序，剪掉图上所有树枝
        const x = q.dequeue();
        const y = favorite[x];
        maxDepth[y] = maxDepth[x] + 1; // x 只有一条出边
        if (--deg[y] === 0) {
            q.enqueue(y);
        }
    }

    let maxRingSize = 0, sumChainSize = 0;
    for (let i = 0; i < n; i++) {
        if (deg[i] === 0) continue;

        // 遍历基环上的点
        deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
        let ringSize = 1; // 基环长度
        for (let x = favorite[i]; x !== i; x = favorite[x]) {
            deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            ringSize++;
        }

        if (ringSize === 2) { // 基环长度为 2
            sumChainSize += maxDepth[i] + maxDepth[favorite[i]]; // 累加两条最长链的长度
        } else {
            maxRingSize = Math.max(maxRingSize, ringSize); // 取所有基环长度的最大值
        }
    }
    return Math.max(maxRingSize, sumChainSize);
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn maximum_invitations(favorite: Vec<i32>) -> i32 {
        let n = favorite.len();
        let mut deg = vec![0; n];
        for &f in &favorite {
            deg[f as usize] += 1; // 统计基环树每个节点的入度
        }

        let mut max_depth = vec![1; n];
        let mut q = VecDeque::new();
        for (i, &d) in deg.iter().enumerate() {
            if d == 0 {
                q.push_back(i);
            }
        }
        while let Some(x) = q.pop_front() { // 拓扑排序，剪掉图上所有树枝
            let y = favorite[x] as usize; // x 只有一条出边
            max_depth[y] = max_depth[x] + 1;
            deg[y] -= 1;
            if deg[y] == 0 {
                q.push_back(y);
            }
        }

        let mut max_ring_size = 0;
        let mut sum_chain_size = 0;
        for i in 0..n {
            if deg[i] == 0 {
                continue;
            }

            // 遍历基环上的点
            deg[i] = 0; // 将基环上的点的入度标记为 0，避免重复访问
            let mut ring_size = 1; // 基环长度
            let mut x = favorite[i] as usize;
            while x != i {
                deg[x] = 0; // 将基环上的点的入度标记为 0，避免重复访问
                ring_size += 1;
                x = favorite[x] as usize;
            }

            if ring_size == 2 { // 基环长度为 2
                sum_chain_size += max_depth[i] + max_depth[favorite[i] as usize]; // 累加两条最长链的长度
            } else {
                max_ring_size = max_ring_size.max(ring_size); // 取所有基环长度的最大值
            }
        }
        max_ring_size.max(sum_chain_size)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 为 $\textit{favorite}$ 的长度。拓扑排序和遍历基环均为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、拓扑排序 / §2.3 基环树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、拓扑排序 / §2.3 基环树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
