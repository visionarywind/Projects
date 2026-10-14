# K07 图、拓扑、最短路、最小生成树与并查集

## 核心问题

- **建图与遍历**：把题目中的"关系"抽象成点与边，用邻接表存储；DFS/BFS 是连通性、环检测、层数统计的通用工具。
- **拓扑排序**：处理有向无环图（DAG）的依赖与先后关系，同时兼作有向图环检测。
- **最短路**：单源非负权用 Dijkstra；边权为 1 用 BFS；多源或小规模全源用 Floyd；带负权（无负环）用 Bellman-Ford。
- **最小生成树**：无向连通图求最小总权连通子图，稀疏图用 Kruskal，稠密图用 Prim。
- **并查集**：动态维护连通分量，路径压缩 + 按秩合并后单次操作近似常数，是 Kruskal 与在线连通性问题的核心。

学习目标是能识别题目中的图结构、选择合适算法、证明复杂度并写出边界正确的 C++17 实现。判定口诀：先问"图是有向还是无向、边权是否非负、要单源还是全源"，答案基本就定了算法。

## 训练顺序

1. 先用邻接表 + DFS/BFS 打通建图与遍历，掌握"访问标记"和"原地标记"两个模板。
2. 再补拓扑排序（Kahn 队列版与 DFS 后序版），理解"入度为 0 入队"和"后序逆序"两种视角。
3. 接着做最短路三件套：Dijkstra（优先队列 + 松弛）、Floyd（三重循环 + 中转点），并区分各自适用条件。
4. 最后做最小生成树与并查集：Kruskal 依赖并查集，Prim 依赖优先队列；把并查集单独作为"动态连通性"模板反复默写。
5. 高级和专家题用于训练多约束组合、证明、在线约束和追问。

## 核心模式

### 1. 邻接表建图

```cpp
// n 个点，边表存 (邻点, 权)。无向图双向插入。
vector<vector<pair<int,int>>> adj(n);
auto addEdge = [&](int u, int v, int w) {
    adj[u].push_back({v, w});
    adj[v].push_back({u, w});   // 有向图删掉这一行
};
```

不变量：`adj[u]` 恰好列出所有从 `u` 出发的边；重复边（重边）与自环按题面保留或忽略。复杂度：建图 `O(V + E)`。

### 2. BFS 求无权最短路 / 分层

```cpp
vector<int> bfs(int n, const vector<vector<int>>& adj, int src) {
    vector<int> dist(n, -1);
    queue<int> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (dist[v] != -1) continue;   // 访问即标记，防止重复入队
            dist[v] = dist[u] + 1;
            q.push(v);
        }
    }
    return dist;                            // -1 表示不可达
}
```

不变量：出队时 `dist[u]` 已是 `src` 到 `u` 的最短距离（边权全为 1 时 BFS 天然分层）。复杂度：`O(V + E)`。

### 3. 拓扑排序（Kahn 队列版）

```cpp
vector<int> topoKahn(int n, const vector<vector<int>>& adj) {
    vector<int> indeg(n, 0);
    for (int u = 0; u < n; ++u)
        for (int v : adj[u]) ++indeg[v];
    queue<int> q;
    for (int u = 0; u < n; ++u) if (indeg[u] == 0) q.push(u);
    vector<int> order;
    order.reserve(n);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : adj[u])
            if (--indeg[v] == 0) q.push(v);
    }
    return order;   // order.size() < n 说明存在环（有向图才适用）
}
```

不变量：队列中只放"所有前驱都已输出"的点；出队顺序即一个合法拓扑序。复杂度：`O(V + E)`。判环：`order.size() != n` 当且仅当有环。

### 4. Dijkstra（优先队列，非负权单源最短路）

```cpp
vector<long long> dijkstra(int n, const vector<vector<pair<int,int>>>& adj, int src) {
    const long long INF = LLONG_MAX / 4;
    vector<long long> dist(n, INF);
    priority_queue<pair<long long,int>,
                   vector<pair<long long,int>>, greater<>> pq;
    dist[src] = 0;
    pq.emplace(0, src);
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;          // 懒惰删除过期条目
        for (auto [v, w] : adj[u]) {
            if (d + w < dist[v]) {          // 松弛
                dist[v] = d + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}
```

不变量：每次弹出的是当前剩余节点中距离最小者，其 `dist` 已确定（依赖非负权）。复杂度：`O((V + E) log V)`。**负权边会让该不变量失效**，不可用。

### 5. Floyd-Warshall（全源最短路）

```cpp
// d 初始为 n×n 邻接矩阵，无边为 INF，d[i][i] = 0
for (int k = 0; k < n; ++k)                 // 中转点必须是最外层
    for (int i = 0; i < n; ++i) {
        if (d[i][k] == INF) continue;
        for (int j = 0; j < n; ++j)
            if (d[k][j] != INF)
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
    }
```

不变量：第 `k` 轮结束时，`d[i][j]` 是允许中转点取自 `{0..k}` 时的最短距离。复杂度：`O(V^3)` 时间、`O(V^2)` 空间。

### 6. 并查集（路径压缩 + 按秩合并）

```cpp
struct DSU {
    vector<int> parent, rank_;
    explicit DSU(int n) : parent(n), rank_(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {                       // 路径减半，摊还近似 O(1)
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;           // 已连通
        if (rank_[a] < rank_[b]) swap(a, b);
        parent[b] = a;                      // 矮树挂到高树
        if (rank_[a] == rank_[b]) ++rank_[a];
        return true;                        // 成功合并
    }
};
```

不变量：`find(x)` 返回 `x` 所在集合的代表元；两元素代表元相同当且仅当它们连通。单次操作摊还 `O(α(n))`（反阿克曼函数，实际视为常数）。

### 7. Kruskal（稀疏图最小生成树）

```cpp
struct Edge { int u, v; long long w; };
long long kruskal(int n, vector<Edge>& edges) {
    sort(edges.begin(), edges.end(),
         [](const Edge& a, const Edge& b) { return a.w < b.w; });
    DSU dsu(n);
    long long total = 0;
    int used = 0;
    for (const auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {           // 不成环才选
            total += e.w;
            if (++used == n - 1) break;
        }
    }
    return used == n - 1 ? total : -1;       // -1 表示图不连通
}
```

不变量：按边权升序贪心，保留不成环的边，恰好得到一棵最小生成树（割性质 + 环性质证得）。复杂度：`O(E log E)`（排序主导），空间 `O(V + E)`。

### 8. Prim（稠密图最小生成树）

```cpp
long long prim(int n, const vector<vector<pair<int,int>>>& adj) {
    const int INF = INT_MAX / 4;
    vector<int> dist(n, INF);                // 到已有 MST 的最小边权
    vector<char> inMST(n, 0);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    dist[0] = 0;
    pq.emplace(0, 0);
    long long total = 0;
    int cnt = 0;
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (inMST[u]) continue;
        inMST[u] = 1;
        total += d;
        ++cnt;
        for (auto [v, w] : adj[u])
            if (!inMST[v] && w < dist[v]) {
                dist[v] = w;
                pq.emplace(w, v);
            }
    }
    return cnt == n ? total : -1;
}
```

不变量：每次把距离当前 MST 最近的未收录点加入集合。复杂度：`O((V + E) log V)`，稠密图可退化为朴素 `O(V^2)` 实现（不使用堆）。

## 常见陷阱

- **有向/无向混用**：拓扑排序只对有向图有意义；无向图判环要用 DFS 判"回边"或并查集，直接套拓扑会得到无意义结果。
- **Dijkstra 用于负权**：负权边破坏"弹出即确定"的不变量，结果错误；改用 Bellman-Ford，或在无负环时用 Floyd。
- **忽略不连通**：最短路要能返回 `INF`（不可达），生成树要检测"森林"（`used != n-1`），代码不能默认全图连通。
- **Floyd 中转点循环写在内层**：`k` 必须是最外层，否则会遗漏使用多个中转点的路径。
- **并查集退化**：只写 `find` 不写合并策略、或忘记路径压缩，链式结构使单次操作退化到 `O(n)`。
- **Kruskal 判环忘用并查集**：逐条加边时若不做连通判断会形成环，得到的不是树。
- **访问标记时机**：BFS/DFS 中应"入队/入栈即标记"，出队才标记会导致同一节点重复入队、队列膨胀。
- **网格方向与边界**：四方向/八方向混用会改变连通语义；边界判断写成 `<=` 而非 `<` 会越界读。
- **下标基准不一致**：题目点编号可能从 1 开始，建图时未统一到 0-based 导致数组越界。
- **重边与自环**：自环对最短路无害但影响度数/拓扑；重边在多权图（如第二权值）下不能简单去重。

## 复杂度速查

| 算法 | 适用 | 时间 | 空间 |
|---|---|---|---|
| 建图（邻接表） | 通用 | `O(V + E)` | `O(V + E)` |
| DFS / BFS | 遍历、连通分量、无权最短路 | `O(V + E)` | `O(V)` |
| 拓扑排序（Kahn） | 有向无环图依赖、判环 | `O(V + E)` | `O(V + E)` |
| Dijkstra（二叉堆） | 单源、非负权 | `O((V + E) log V)` | `O(V + E)` |
| Bellman-Ford | 单源、带负权、判负环 | `O(V·E)` | `O(V)` |
| Floyd-Warshall | 全源、小规模、可负权 | `O(V^3)` | `O(V^2)` |
| Kruskal | 稀疏图 MST | `O(E log E)` | `O(V + E)` |
| Prim（堆） | 稠密图 MST | `O((V + E) log V)` | `O(V + E)` |
| 并查集 | 动态连通性 | 单次摊还 `O(α(n))` | `O(V)` |

说明：`V` 为顶点数、`E` 为边数；`α(n)` 为反阿克曼函数，实际不超过常数 4~5。

## 题目索引

本域对应的真实题目文件：

- `../01-question-bank/advanced/A002-number-of-islands.md` —— 网格图连通分量计数，DFS/BFS/并查集三种解法对比，是 K07 的核心综合题。
- `../01-question-bank/beginner/B001-two-sum.md` —— 虽属 K01，但可作为"网格外一维问题"的对照，帮助区分线性扫描与图遍历。

延伸方向（题面变体，非独立文件）：拓扑排序（课程表）、Dijkstra（网络延迟时间）、并查集（等式方程可满足性）、MST（连接所有点的最小费用），可在覆盖矩阵 `../00-overview/coverage-matrix.md` 的 `K07` 条目下继续补题。
