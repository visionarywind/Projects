# 399. 除法求值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/evaluate-division/
- 题目 slug：`evaluate-division`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.6 带权并查集（边权并查集）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/evaluate-division/solutions/3805496/dai-quan-bing-cha-ji-pythonjavacgojsrust-a4a1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[带权并查集（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/evaluate-division/solutions/3805496/dai-quan-bing-cha-ji-pythonjavacgojsrust-a4a1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dai-quan-bing-cha-ji-pythonjavacgojsrust-a4a1`
- topic id：`3805496`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

已知条件是一些形如 $\dfrac{a}{b} = \textit{value}$ 的方程。

例如，已知 

$$
\dfrac{a}{b} = 3,\ \dfrac{b}{c} = 1,\ \dfrac{c}{d} = 4
$$

这三个比值建立了 $a,b,c,d$ 的联系，我们可以算出其中任意两个量的比值，比如

$$
\dfrac{a}{d} = \dfrac{a}{b}\cdot \dfrac{b}{c}\cdot \dfrac{c}{d} = 3\times 1 \times 4 = 12
$$

下面介绍并查集的方法，你需要先学会 [普通并查集](https://oi-wiki.org/ds/dsu/)。

定义 $\textit{mul}[x]$ 表示

$$
x \cdot \textit{mul}[x] = x\ 的代表元的值
$$

如果 $c$ 和 $d$ 在同一个集合中，那么

$$
c\cdot \textit{mul}[c] = d\cdot \textit{mul}[d] = 代表元的值
$$

得

$$
\dfrac{c}{d} = \dfrac{\textit{mul}[d]}{\textit{mul}[c]}
$$

我们不需要知道代表元的值，也不需要知道每个元素的值，只需要知道每个元素与其代表元的**比值**，就能算出同一集合中任意两个元素的**比值**。比如昨天下了一个西湖的雨，今天下了两个西湖的雨，计算今天降水量与昨天降水量的比值，并不需要知道西湖有多少水。

![lc399-c2.png](https://pic.leetcode.cn/1760420428-pqRaZE-lc399-c2.png){:width=700px}

如上图，在已知 $\dfrac{x}{b} = 3$ 和 $\dfrac{y}{a} = 6$ 的情况下，新增信息 $\dfrac{a}{b} = 2$，我们需要计算 $x$ 到其代表元 $y$ 的 $\textit{mul}$ 值，即 $\dfrac{y}{x}$。

由于

$$
\dfrac{y}{b} = \dfrac{y}{x}\cdot \dfrac{x}{b} = \dfrac{y}{a}\cdot \dfrac{a}{b}
$$

所以

$$
\textit{mul}[x] = \dfrac{y}{x} = \dfrac{\dfrac{y}{a}\cdot \dfrac{a}{b}}{\dfrac{x}{b}} = \dfrac{\textit{mul}[a]\cdot \textit{value}}{\textit{mul}[b]} = 4
$$

计算 $x$ 到其代表元 $y$ 的 $\textit{mul}$ 值有什么用？

比如询问 $\dfrac{x}{a}$ 的值，就可以用前文得到的公式

$$
\dfrac{x}{a} = \dfrac{\textit{mul}[a]}{\textit{mul}[x]} = \dfrac{6}{4}
$$

代码实现时，可以把字符串映射为从 $0$ 开始的编号，方便使用并查集。

```py [sol-Python3]
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己，自己 * 1 = 自己
        self.fa = list(range(n))  # 代表元
        self.mul = [1.0] * n  # x 的值 * mul[x] = x 的代表元的值

    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    def find(self, x: int) -> int:
        fa = self.fa
        if fa[x] != x:
            root = self.find(fa[x])
            self.mul[x] *= self.mul[fa[x]]  # 更新 x 到其代表元的 mul 值
            fa[x] = root
        return fa[x]

    # 判断 x 和 y 是否在同一个集合
    def same(self, x: int, y: int) -> bool:
        # 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        # 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return self.find(x) == self.find(y)

    # 合并 from 和 to，新增信息 to / from = value
    # 其中 to 和 from 表示未知量，下文的 x 和 y 也表示未知量
    def merge(self, from_: int, to: int, value: float) -> None:
        x, y = self.find(from_), self.find(to)
        if x == y:  # from 和 to 在同一个集合，不做合并
            return
        #    x --------- y
        #   /           /
        # from ------- to
        # 已知 x/from = mul[from] 和 y/to = mul[to]，现在合并 from 和 to，新增信息 to/from = value
        # 由于 y/from = (y/x) * (x/from) = (y/to) * (to/from)
        # 所以 y/x = (y/to) * (to/from) / (x/from) = mul[to] * value / mul[from]
        self.mul[x] = self.mul[to] * value / self.mul[from_]
        self.fa[x] = y


class Solution:
    def calcEquation(self, equations: List[List[str]], values: List[float], queries: List[List[str]]) -> List[float]:
        # 把不同字符串映射为不同的数字，方便使用并查集
        variable_to_id = {}
        for equation in equations:
            for s in equation:
                if s not in variable_to_id:
                    variable_to_id[s] = len(variable_to_id)

        # 初始化并查集
        uf = UnionFind(len(variable_to_id))
        for (a, b), value in zip(equations, values):
            uf.merge(variable_to_id[b], variable_to_id[a], value)

        # 回答询问
        ans = []
        for c, d in queries:
            c = variable_to_id.get(c, -1)
            d = variable_to_id.get(d, -1)
            if c != -1 and d != -1 and uf.same(c, d):
                #    c * mul[c] = d * mul[d] = 代表元的值
                # => c / d = mul[d] / mul[c]
                ans.append(uf.mul[d] / uf.mul[c])
            else:
                ans.append(-1.0)
        return ans
```

```java [sol-Java]
class UnionFind {
    private final int[] fa; // 代表元
    public final double[] mul; // x 的值 * mul[x] = x 的代表元的值

    public UnionFind(int n) {
        fa = new int[n];
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己，自己 * 1 = 自己
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }

        mul = new double[n];
        Arrays.fill(mul, 1);
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    private int find(int x) {
        if (fa[x] != x) {
            int root = find(fa[x]);
            mul[x] *= mul[fa[x]]; // 更新 x 到其代表元的 mul 值
            fa[x] = root;
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    public boolean same(int x, int y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return find(x) == find(y);
    }

    // 合并 from 和 to，新增信息 to / from = value
    // 其中 to 和 from 表示未知量，下文的 x 和 y 也表示未知量
    public void merge(int from, int to, double value) {
        int x = find(from);
        int y = find(to);
        if (x == y) { // from 和 to 在同一个集合，不做合并
            return;
        }
        //    x --------- y
        //   /           /
        // from ------- to
        // 已知 x/from = mul[from] 和 y/to = mul[to]，现在合并 from 和 to，新增信息 to/from = value
        // 由于 y/from = (y/x) * (x/from) = (y/to) * (to/from)
        // 所以 y/x = (y/to) * (to/from) / (x/from) = mul[to] * value / mul[from]
        mul[x] = mul[to] * value / mul[from];
        fa[x] = y;
    }
}

class Solution {
    public double[] calcEquation(List<List<String>> equations, double[] values, List<List<String>> queries) {
        // 把不同字符串映射为不同的数字，方便使用并查集
        Map<String, Integer> variableToId = new HashMap<>();
        for (List<String> equation : equations) {
            for (String s : equation) {
                variableToId.putIfAbsent(s, variableToId.size());
            }
        }

        // 初始化并查集
        UnionFind uf = new UnionFind(variableToId.size());
        for (int i = 0; i < equations.size(); i++) {
            List<String> equation = equations.get(i);
            uf.merge(variableToId.get(equation.get(1)), variableToId.get(equation.get(0)), values[i]);
        }

        // 回答询问
        double[] ans = new double[queries.size()];
        for (int i = 0; i < queries.size(); i++) {
            List<String> query = queries.get(i);
            Integer c = variableToId.get(query.get(0));
            Integer d = variableToId.get(query.get(1));
            if (c != null && d != null && uf.same(c, d)) {
                //    c * mul[c] = d * mul[d] = 代表元的值
                // => c / d = mul[d] / mul[c]
                ans[i] = uf.mul[d] / uf.mul[c];
            } else {
                ans[i] = -1;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class UnionFind {
public:
    vector<int> fa; // 代表元
    vector<double> mul; // x 的值 * mul[x] = x 的代表元的值

    UnionFind(int n) : fa(n), mul(n, 1) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己，自己 * 1 = 自己
        ranges::iota(fa, 0); // iota(fa.begin(), fa.end(), 0);
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    int find(int x) {
        if (fa[x] != x) {
            int root = find(fa[x]);
            mul[x] *= mul[fa[x]]; // 更新 x 到其代表元的 mul 值
            fa[x] = root;
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    bool same(int x, int y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return find(x) == find(y);
    }

    // 合并 from 和 to，新增信息 to / from = value
    // 其中 to 和 from 表示未知量，下文的 x 和 y 也表示未知量
    void merge(int from, int to, double value) {
        int x = find(from), y = find(to);
        if (x == y) { // from 和 to 在同一个集合，不做合并
            return;
        }
        //    x --------- y
        //   /           /
        // from ------- to
        // 已知 x/from = mul[from] 和 y/to = mul[to]，现在合并 from 和 to，新增信息 to/from = value
        // 由于 y/from = (y/x) * (x/from) = (y/to) * (to/from)
        // 所以 y/x = (y/to) * (to/from) / (x/from) = mul[to] * value / mul[from]
        mul[x] = mul[to] * value / mul[from];
        fa[x] = y;
    }
};

class Solution {
public:
    vector<double> calcEquation(vector<vector<string>>& equations, vector<double>& values, vector<vector<string>>& queries) {
        // 把不同字符串映射为不同的数字，方便使用并查集
        unordered_map<string, int> variable_to_id;
        for (auto& equation : equations) {
            for (auto& s : equation) {
                if (!variable_to_id.contains(s)) {
                    variable_to_id[s] = variable_to_id.size();
                }
            }
        }

        // 初始化并查集
        UnionFind uf(variable_to_id.size());
        for (int i = 0; i < equations.size(); i++) {
            uf.merge(variable_to_id[equations[i][1]], variable_to_id[equations[i][0]], values[i]);
        }

        // 回答询问
        vector<double> ans;
        for (auto& query : queries) {
            auto it_c = variable_to_id.find(query[0]);
            if (it_c == variable_to_id.end()) { // 变量 c 不在已知条件中
                ans.push_back(-1);
                continue;
            }

            auto it_d = variable_to_id.find(query[1]);
            if (it_d == variable_to_id.end() || !uf.same(it_c->second, it_d->second)) { // 变量 d 不在已知条件中，或者无法确定答案
                ans.push_back(-1);
                continue;
            }

            //    c * mul[c] = d * mul[d] = 代表元的值
            // => c / d = mul[d] / mul[c]
            ans.push_back(uf.mul[it_d->second] / uf.mul[it_c->second]);
        }
        return ans;
    }
};
```

```go [sol-Go]
type unionFind struct {
    fa  []int     // 代表元
    mul []float64 // x 的值 * mul[x] = x 的代表元的值
}

func newUnionFind(n int) unionFind {
    fa := make([]int, n)
    mul := make([]float64, n)
    // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
    // 集合 i 的代表元是自己，自己 * 1 = 自己
    for i := range fa {
        fa[i] = i
        mul[i] = 1
    }
    return unionFind{fa, mul}
}

// 返回 x 所在集合的代表元
// 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
func (u unionFind) find(x int) int {
    if u.fa[x] != x {
        root := u.find(u.fa[x])
        u.mul[x] *= u.mul[u.fa[x]] // 更新 x 到其代表元的 mul 值
        u.fa[x] = root
    }
    return u.fa[x]
}

// 判断 x 和 y 是否在同一个集合
func (u unionFind) same(x, y int) bool {
    // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
    // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
    return u.find(x) == u.find(y)
}

// 合并 from 和 to，新增信息 to / from = value
// 其中 to 和 from 表示未知量，下文的 x 和 y 也表示未知量
func (u *unionFind) merge(from, to int, value float64) {
    x, y := u.find(from), u.find(to)
    if x == y { // from 和 to 在同一个集合，不做合并
        return
    }
    //    x --------- y
    //   /           /
    // from ------- to
    // 已知 x/from = mul[from] 和 y/to = mul[to]，现在合并 from 和 to，新增信息 to/from = value
    // 由于 y/from = (y/x) * (x/from) = (y/to) * (to/from)
    // 所以 y/x = (y/to) * (to/from) / (x/from) = mul[to] * value / mul[from]
    u.mul[x] = u.mul[to] * value / u.mul[from]
    u.fa[x] = y
}

func calcEquation(equations [][]string, values []float64, queries [][]string) []float64 {
    // 把不同字符串映射为不同的数字，方便使用并查集
    variableToId := map[string]int{}
    for _, equation := range equations {
        for _, s := range equation {
            if _, ok := variableToId[s]; !ok {
                variableToId[s] = len(variableToId)
            }
        }
    }

    // 初始化并查集
    uf := newUnionFind(len(variableToId))
    for i, equation := range equations {
        uf.merge(variableToId[equation[1]], variableToId[equation[0]], values[i])
    }

    // 回答询问
    ans := make([]float64, len(queries))
    for i, query := range queries {
        c, ok := variableToId[query[0]]
        if !ok { // 变量 c 不在已知条件中
            ans[i] = -1
            continue
        }

        d, ok := variableToId[query[1]]
        if !ok || !uf.same(c, d) { // 变量 d 不在已知条件中，或者无法确定答案
            ans[i] = -1
            continue
        }

        //    c * mul[c] = d * mul[d] = 代表元的值
        // => c / d = mul[d] / mul[c]
        ans[i] = uf.mul[d] / uf.mul[c]
    }
    return ans
}
```

```js [sol-JavaScript]
class UnionFind {
    constructor(n) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己，自己 * 1 = 自己
        this.fa = Array.from({length: n}, (_, i) => i); // 代表元
        this.mul = Array(n).fill(1); // x 的值 * mul[x] = x 的代表元的值
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    find(x) {
        const fa = this.fa;
        if (fa[x] !== x) {
            const root = this.find(fa[x]);
            this.mul[x] *= this.mul[fa[x]]; // 更新 x 到其代表元的 mul 值
            fa[x] = root;
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    same(x, y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return this.find(x) === this.find(y);
    }

    // 合并 from 和 to，新增信息 to / from = value
    // 其中 to 和 from 表示未知量，下文的 x 和 y 也表示未知量
    merge(from, to, value) {
        const x = this.find(from), y = this.find(to);
        if (x === y) { // from 和 to 在同一个集合，不做合并
            return;
        }
        //    x --------- y
        //   /           /
        // from ------- to
        // 已知 x/from = mul[from] 和 y/to = mul[to]，现在合并 from 和 to，新增信息 to/from = value
        // 由于 y/from = (y/x) * (x/from) = (y/to) * (to/from)
        // 所以 y/x = (y/to) * (to/from) / (x/from) = mul[to] * value / mul[from]
        this.mul[x] = this.mul[to] * value / this.mul[from];
        this.fa[x] = y;
    }
}

var calcEquation = function(equations, values, queries) {
    // 把不同字符串映射为不同的数字，方便使用并查集
    const variableToId = new Map();
    for (const equation of equations) {
        for (const s of equation) {
            if (!variableToId.has(s)) {
                variableToId.set(s, variableToId.size);
            }
        }
    }

    // 初始化并查集
    const uf = new UnionFind(variableToId.size);
    for (let i = 0; i < equations.length; i++) {
        uf.merge(variableToId.get(equations[i][1]), variableToId.get(equations[i][0]), values[i]);
    }

    // 回答询问
    const ans = [];
    for (let [c, d] of queries) {
        c = variableToId.get(c);
        d = variableToId.get(d);
        if (c !== undefined && d !== undefined && uf.same(c, d)) {
            //    c * mul[c] = d * mul[d] = 代表元的值
            // => c / d = mul[d] / mul[c]
            ans.push(uf.mul[d] / uf.mul[c]);
        } else {
            ans.push(-1);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

struct UnionFind {
    fa: Vec<usize>, // 代表元
    mul: Vec<f64>, // x 的值 * mul[x] = x 的代表元的值
}

impl UnionFind {
    fn new(n: usize) -> Self {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己，自己 * 1 = 自己
        let fa = (0..n).collect::<Vec<_>>();
        let mul = vec![1.0; n];
        Self { fa, mul }
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    fn find(&mut self, x: usize) -> usize {
        if self.fa[x] != x {
            let root = self.find(self.fa[x]);
            self.mul[x] *= self.mul[self.fa[x]]; // 更新 x 到其代表元的 mul 值
            self.fa[x] = root;
        }
        self.fa[x]
    }

    // 判断 x 和 y 是否在同一个集合
    fn same(&mut self, x: usize, y: usize) -> bool {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        self.find(x) == self.find(y)
    }

    // 合并 from 和 to，新增信息 to / from = value
    // 其中 to 和 from 表示未知量，下文的 x 和 y 也表示未知量
    fn merge(&mut self, from: usize, to: usize, value: f64) {
        let x = self.find(from);
        let y = self.find(to);
        if x == y { // from 和 to 在同一个集合，不做合并
            return;
        }
        //    x --------- y
        //   /           /
        // from ------- to
        // 已知 x/from = mul[from] 和 y/to = mul[to]，现在合并 from 和 to，新增信息 to/from = value
        // 由于 y/from = (y/x) * (x/from) = (y/to) * (to/from)
        // 所以 y/x = (y/to) * (to/from) / (x/from) = mul[to] * value / mul[from]
        self.mul[x] = self.mul[to] * value / self.mul[from];
        self.fa[x] = y;
    }
}

impl Solution {
    pub fn calc_equation(equations: Vec<Vec<String>>, values: Vec<f64>, queries: Vec<Vec<String>>) -> Vec<f64> {
        // 把不同字符串映射为不同的数字，方便使用并查集
        let mut variable_to_id = HashMap::new();
        for equation in &equations {
            for s in equation {
                let size = variable_to_id.len();
                variable_to_id.entry(s.clone()).or_insert(size);
            }
        }

        // 初始化并查集
        let mut uf = UnionFind::new(variable_to_id.len());
        for (equation, value) in equations.into_iter().zip(values) {
            let a = *variable_to_id.get(&equation[0]).unwrap();
            let b = *variable_to_id.get(&equation[1]).unwrap();
            uf.merge(b, a, value);
        }

        // 回答询问
        queries.into_iter().map(|query| {
            let c_opt = variable_to_id.get(&query[0]);
            let d_opt = variable_to_id.get(&query[1]);
            if !c_opt.is_none() && !d_opt.is_none() && uf.same(*c_opt.unwrap(), *d_opt.unwrap()) {
                //    c * mul[c] = d * mul[d] = 代表元的值
                // => c / d = mul[d] / mul[c]
                uf.mul[*d_opt.unwrap()] / uf.mul[*c_opt.unwrap()]
            } else {
                -1.0
            }
        }).collect()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nL+qL + (n+q)\log n)$，其中 $n$ 是 $\textit{equations}$ 的长度，$q$ 是 $\textit{queries}$ 的长度，$L\le 5$ 是单个变量（字符串）的最大长度。这里为了实现方便，并查集只写了路径压缩，每次操作需要 $\mathcal{O}(\log n)$ 的时间。
- 空间复杂度：$\mathcal{O}(nL)$。

## 专题训练

见下面数据结构题单的「**§7.6 带权并查集**」。

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

本题来自 `七、并查集 / §7.6 带权并查集（边权并查集）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.6 带权并查集（边权并查集）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
