# 3841. 查询树上回文路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindromic-path-queries-in-a-tree/
- 题目 slug：`palindromic-path-queries-in-a-tree`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.1 树状数组
- 难度分：2384
- 外部题解来源：https://leetcode.cn/problems/palindromic-path-queries-in-a-tree/solutions/3903005/lca-chai-fen-shu-zhuang-shu-zu-pythonjav-cuf3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[LCA + 差分树状数组（Python/Java/C++/Go）](https://leetcode.cn/problems/palindromic-path-queries-in-a-tree/solutions/3903005/lca-chai-fen-shu-zhuang-shu-zu-pythonjav-cuf3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`lca-chai-fen-shu-zhuang-shu-zu-pythonjav-cuf3`
- topic id：`3903005`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置题目/知识点

1. [2791. 树中可以形成回文的路径数](https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/)，[我的题解](https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/solutions/2355288/yong-wei-yun-suan-chu-li-by-endlesscheng-n9ws/)。
2. 最近公共祖先（LCA），[模板讲解](https://leetcode.cn/problems/kth-ancestor-of-a-tree-node/solution/mo-ban-jiang-jie-shu-shang-bei-zeng-suan-v3rw/)。
3. [3515. 带权树中的最短路径](https://leetcode.cn/problems/shortest-path-in-a-weighted-tree/)，[我的题解](https://leetcode.cn/problems/shortest-path-in-a-weighted-tree/solutions/3649372/dfs-shi-jian-chuo-chai-fen-shu-zhuang-sh-h8q3/)。
4. [231. 2 的幂](https://leetcode.cn/problems/power-of-two/)，[我的题解](https://leetcode.cn/problems/power-of-two/solutions/2973442/yan-ge-zheng-ming-yi-xing-xie-fa-pythonj-h04o/)。

## 处理 query

根据 2791 题，问题等价于：

- 从 $x$ 到 $y$ 的路径上，至多有一个字母的出现次数是奇数。

用二进制数表示路径中的字母出现次数的奇偶性。如果二进制数是 $0$ 或者 $2^k\ (k\ge 0)$，那么至多有一个字母的出现次数是奇数。

定义 $\textit{XOR}[i]$ 表示从 $0$ 到 $i$ 的路径的字母出现次数的奇偶性（对应的二进制数）。

根据异或的性质，从 $x$ 到 $y$ 的路径的字母出现次数的奇偶性，等于如下三者的异或和：

- $0$ 到 $x$ 的路径的字母出现次数的奇偶性 $\textit{XOR}[x]$。
- $0$ 到 $y$ 的路径的字母出现次数的奇偶性 $\textit{XOR}[y]$。
- 上面两条路径异或后，$x$ 和 $y$ 的 $\textit{lca}$ **被抵消了**，所以要添加回来，即异或 $s[\textit{lca}]$ 对应的二进制数 `1 << (s[lca] - 'a')`。

$\textit{XOR}[i]$ 可以通过一次自顶向下的 DFS 求出。

## 处理 update

想一想，当我们修改 $s[x]$ 后，哪些 $\textit{XOR}[i]$ 会变？变成什么了？

由于 $\textit{XOR}[i]$ 对应从 $0$ 到 $i$ 的路径，如果路径经过 $x$，那么 $\textit{XOR}[i]$ 就会变。所以修改 $s[x]$ 后，在**子树** $x$ 中的节点 $i$ 的 $\textit{XOR}[i]$ 会变。

去掉原来的 $s[x]$，改成新的字母 $c$，用位运算解决，把这些 $\textit{XOR}[i]$ 都异或 `val = (1 << (s[x] - 'a')) ^ (1 << (c - 'a'))`。

根据 3515 题，求出节点 $x$ 的进出时间戳 $\textit{tin}[x]$ 和 $\textit{tout}[x]$，那么对于子树 $x$ 中的所有节点，其进出时间戳都在闭区间 $[\textit{tin}[x],\textit{tout}[x]]$ 中。

即把子树中的所有 $\textit{XOR}[i]$ 都异或同一个数，变成区间异或同一个数，即「区间更新」操作。

所以本题是「区间更新，单点查询」，最适合的数据结构是**差分树状数组**，转换成「单点更新，前缀查询」，做法同 3515 题。

代码实现时，差分树状数组保存的是「区间更新」操作的异或结果，没有保存初始值 $\textit{XOR}[i]$。获取 $\textit{XOR}[i]$ 修改后的值，可以用其初始值 $\textit{XOR}[i]$ 异或差分树状数组的前缀查询结果。

[本题视频讲解](https://www.bilibili.com/video/BV15TZ4B1Eev/?t=12m23s)，欢迎点赞关注~

```py [sol-Python3]
# 模板来自我的题单 https://leetcode.cn/circle/discuss/mOr1u6/
class FenwickTree:
    def __init__(self, n: int):
        self.tree = [0] * (n + 1)  # 使用下标 1 到 n

    # a[i] ^= val
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def update(self, i: int, val: int) -> None:
        t = self.tree
        while i < len(t):
            t[i] ^= val
            i += i & -i

    # 计算前缀异或和 a[1] ^ ... ^ a[i]
    # 1 <= i <= n
    # 时间复杂度 O(log n)
    def pre(self, i: int) -> int:
        t = self.tree
        res = 0
        while i > 0:
            res ^= t[i]
            i &= i - 1
        return res


# 模板来自我的题单 https://leetcode.cn/circle/discuss/K0n2gO/
class LcaBinaryLifting:
    def __init__(self, edges: List[List[int]], s: List[int]):
        n = len(edges) + 1
        m = n.bit_length()
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        depth = [0] * n
        pa = [[-1] * n for _ in range(m)]
        tin = [0] * n  # DFS 时间戳
        tout = [0] * n
        clock = 0
        path_xor_from_root = [0] * n  # 从根开始的路径的字母出现次数的奇偶性
        path_xor_from_root[0] = 1 << s[0]

        def dfs(x: int, fa: int) -> None:
            pa[0][x] = fa
            nonlocal clock
            clock += 1
            tin[x] = clock
            for y in g[x]:
                if y != fa:
                    depth[y] = depth[x] + 1
                    path_xor_from_root[y] = path_xor_from_root[x] ^ (1 << s[y])
                    dfs(y, x)
            tout[x] = clock

        dfs(0, -1)

        for i in range(m - 1):
            for x in range(n):
                if (p := pa[i][x]) != -1:
                    pa[i + 1][x] = pa[i][p]

        self.depth = depth
        self.pa = pa
        self.tin = tin
        self.tout = tout
        self.path_xor_from_root = path_xor_from_root

    # 返回 node 的第 k 个祖先节点
    # 如果不存在，返回 -1
    def get_kth_ancestor(self, node: int, k: int) -> int:
        pa = self.pa
        for i in range(k.bit_length()):
            if k >> i & 1:
                node = pa[i][node]
                if node < 0:
                    return -1
        return node

    # 返回 x 和 y 的最近公共祖先
    def get_lca(self, x: int, y: int) -> int:
        if self.depth[x] > self.depth[y]:
            x, y = y, x
        # 使 y 和 x 在同一深度
        y = self.get_kth_ancestor(y, self.depth[y] - self.depth[x])
        if y == x:
            return x
        pa = self.pa
        for i in range(len(pa) - 1, -1, -1):
            px, py = pa[i][x], pa[i][y]
            if px != py:
                x, y = px, py  # 同时往上跳 2**i 步
        return pa[0][x]


class Solution:
    def palindromePath(self, n: int, edges: list[list[int]], s: str, queries: list[str]) -> list[bool]:
        ord_a = ord('a')
        t = [ord(ch) - ord_a for ch in s]  # 映射成 [0, 25] 中的整数

        g = LcaBinaryLifting(edges, t)
        tin = g.tin
        tout = g.tout
        path_xor_from_root = g.path_xor_from_root

        f = FenwickTree(n)  # 注意树状数组是异或运算
        ans = []

        for q in queries:
            op, x, y = q.split()
            x = int(x)
            if op[0] == 'u':
                c = ord(y) - ord_a
                val = (1 << t[x]) ^ (1 << c)  # 擦除旧的，换上新的
                t[x] = c
                # 子树 x 全部异或 val，转换成对区间 [tin[x], tout[x]] 的差分更新
                f.update(tin[x], val)
                f.update(tout[x] + 1, val)
            else:
                y = int(y)
                lca = g.get_lca(x, y)
                res = path_xor_from_root[x] ^ path_xor_from_root[y] ^ f.pre(tin[x]) ^ f.pre(tin[y]) ^ (1 << t[lca])
                ans.append(res & (res - 1) == 0)  # 至多一个字母的出现次数是奇数

        return ans
```

```java [sol-Java]
// 模板来自我的题单 https://leetcode.cn/circle/discuss/mOr1u6/
class FenwickTree {
    private final int[] tree;

    public FenwickTree(int n) {
        tree = new int[n + 1]; // 使用下标 1 到 n
    }

    // a[i] ^= val
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    public void update(int i, int val) {
        for (; i < tree.length; i += i & -i) {
            tree[i] ^= val;
        }
    }

    // 求前缀异或和 a[1] ^ ... ^ a[i]
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    public int pre(int i) {
        int res = 0;
        for (; i > 0; i &= i - 1) {
            res ^= tree[i];
        }
        return res;
    }
}

// 模板来自我的题单 https://leetcode.cn/circle/discuss/K0n2gO/
class LcaBinaryLifting {
    private final int[] depth;
    private final int[][] pa;
    private int clock = 0;

    public final int[] tin;
    public final int[] tout;
    public final int[] pathXorFromRoot;

    LcaBinaryLifting(int[][] edges, char[] s) {
        int n = edges.length + 1;
        int m = 32 - Integer.numberOfLeadingZeros(n); // n 的二进制长度
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        depth = new int[n];
        pa = new int[m][n];
        tin = new int[n]; // DFS 时间戳
        tout = new int[n];
        pathXorFromRoot = new int[n]; // 从根开始的路径的字母出现次数的奇偶性
        pathXorFromRoot[0] = 1 << (s[0] - 'a');

        dfs(0, -1, g, s);

        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                int p = pa[i][x];
                pa[i + 1][x] = p < 0 ? -1 : pa[i][p];
            }
        }
    }

    private void dfs(int x, int fa, List<Integer>[] g, char[] s) {
        pa[0][x] = fa;
        tin[x] = ++clock;
        for (int y : g[x]) {
            if (y != fa) {
                depth[y] = depth[x] + 1;
                pathXorFromRoot[y] = pathXorFromRoot[x] ^ (1 << (s[y] - 'a'));
                dfs(y, x, g, s);
            }
        }
        tout[x] = clock;
    }

    // 返回 node 的第 k 个祖先节点
    // 如果不存在，返回 -1
    private int getKthAncestor(int node, int k) {
        for (; k > 0 && node >= 0; k &= k - 1) {
            node = pa[Integer.numberOfTrailingZeros(k)][node];
        }
        return node;
    }

    // 返回 x 和 y 的最近公共祖先（节点编号从 0 开始）
    public int getLCA(int x, int y) {
        if (depth[x] > depth[y]) {
            int tmp = y;
            y = x;
            x = tmp;
        }
        // 使 y 和 x 在同一深度
        y = getKthAncestor(y, depth[y] - depth[x]);
        if (y == x) {
            return x;
        }
        for (int i = pa.length - 1; i >= 0; i--) {
            int px = pa[i][x], py = pa[i][y];
            if (px != py) {
                x = px;
                y = py; // 同时往上跳 2^i 步
            }
        }
        return pa[0][x];
    }
}

class Solution {
    public List<Boolean> palindromePath(int n, int[][] edges, String s, String[] queries) {
        char[] t = s.toCharArray();
        LcaBinaryLifting g = new LcaBinaryLifting(edges, t);
        FenwickTree f = new FenwickTree(n); // 注意树状数组是异或运算
        List<Boolean> ans = new ArrayList<>();

        for (String q : queries) {
            String[] parts = q.split(" ");
            int x = Integer.parseInt(parts[1]);
            if (parts[0].charAt(0) == 'u') {
                char c = parts[2].charAt(0);
                int val = (1 << (t[x] - 'a')) ^ (1 << (c - 'a')); // 擦除旧的，换上新的
                t[x] = c;
                // 子树 x 全部异或 val，转换成对区间 [tin[x], tout[x]] 的差分更新
                f.update(g.tin[x], val);
                f.update(g.tout[x] + 1, val);
            } else {
                int y = Integer.parseInt(parts[2]);
                int lca = g.getLCA(x, y);
                int res = g.pathXorFromRoot[x] ^ g.pathXorFromRoot[y] ^ f.pre(g.tin[x]) ^ f.pre(g.tin[y]) ^ (1 << (t[lca] - 'a'));
                ans.add((res & (res - 1)) == 0); // 至多一个字母的出现次数是奇数
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
// 模板来自我的题单 https://leetcode.cn/circle/discuss/mOr1u6/
// 根据题目用 FenwickTree<int> t(n) 或者 FenwickTree<long long> t(n) 初始化
template<typename T>
class FenwickTree {
    vector<T> tree;

public:
    // 使用下标 1 到 n
    FenwickTree(int n) : tree(n + 1) {}

    // a[i] ^= val
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    void update(int i, T val) {
        for (; i < tree.size(); i += i & -i) {
            tree[i] ^= val;
        }
    }

    // 求前缀异或和 a[1] ^ ... ^ a[i]
    // 1 <= i <= n
    // 时间复杂度 O(log n)
    T pre(int i) const {
        T res = 0;
        for (; i > 0; i &= i - 1) {
            res ^= tree[i];
        }
        return res;
    }
};

// 模板来自我的题单 https://leetcode.cn/circle/discuss/K0n2gO/
class LcaBinaryLifting {
    vector<int> depth;
    vector<vector<int>> pa;

public:
    vector<int> tin; // DFS 时间戳
    vector<int> tout;
    vector<int> path_xor_from_root; // 从根开始的路径的字母出现次数的奇偶性

    LcaBinaryLifting(vector<vector<int>>& edges, string& s) {
        int n = edges.size() + 1;
        int m = bit_width((uint32_t) n);
        vector<vector<int>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        depth.resize(n);
        pa.resize(m, vector<int>(n, -1));
        tin.resize(n);
        tout.resize(n);
        path_xor_from_root.resize(n);
        path_xor_from_root[0] = 1 << (s[0] - 'a');
        int clock = 0;

        auto dfs = [&](this auto&& dfs, int x, int fa) -> void {
            pa[0][x] = fa;
            tin[x] = ++clock;
            for (int y : g[x]) {
                if (y != fa) {
                    depth[y] = depth[x] + 1;
                    path_xor_from_root[y] = path_xor_from_root[x] ^ (1 << (s[y] - 'a'));
                    dfs(y, x);
                }
            }
            tout[x] = clock;
        };
        dfs(0, -1);

        for (int i = 0; i < m - 1; i++) {
            for (int x = 0; x < n; x++) {
                if (int p = pa[i][x]; p != -1) {
                    pa[i + 1][x] = pa[i][p];
                }
            }
        }
    }

    // 返回 node 的第 k 个祖先节点
    // 如果不存在，返回 -1
    int get_kth_ancestor(int node, int k) {
        for (; k > 0 && node >= 0; k &= k - 1) {
            node = pa[countr_zero((uint32_t) k)][node];
        }
        return node;
    }

    // 返回 x 和 y 的最近公共祖先（节点编号从 0 开始）
    int get_lca(int x, int y) {
        if (depth[x] > depth[y]) {
            swap(x, y);
        }
        y = get_kth_ancestor(y, depth[y] - depth[x]); // 使 y 和 x 在同一深度
        if (y == x) {
            return x;
        }
        for (int i = pa.size() - 1; i >= 0; i--) {
            int px = pa[i][x], py = pa[i][y];
            if (px != py) {
                x = px;
                y = py; // 同时往上跳 2^i 步
            }
        }
        return pa[0][x];
    }
};

class Solution {
public:
    vector<bool> palindromePath(int n, vector<vector<int>>& edges, string s, vector<string>& queries) {
        LcaBinaryLifting g(edges, s);
        FenwickTree<int> f(n); // 注意树状数组是异或运算
        vector<bool> ans;

        string op;
        int x, y;
        char c;

        for (auto& q : queries) {
            stringstream ss(q);
            ss >> op >> x;
            if (op[0] == 'u') {
                ss >> c;
                int val = (1 << (s[x] - 'a')) ^ (1 << (c - 'a')); // 擦除旧的，换上新的
                s[x] = c;
                // 子树 x 全部异或 val，转换成对区间 [tin[x], tout[x]] 的差分更新
                f.update(g.tin[x], val);
                f.update(g.tout[x] + 1, val);
            } else {
                ss >> y;
                int lca = g.get_lca(x, y);
                int res = g.path_xor_from_root[x] ^ g.path_xor_from_root[y] ^ f.pre(g.tin[x]) ^ f.pre(g.tin[y]) ^ (1 << (s[lca] - 'a'));
                ans.push_back((res & (res - 1)) == 0); // 至多一个字母的出现次数是奇数
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
// 模板来自我的题单 https://leetcode.cn/circle/discuss/mOr1u6/
type fenwick []int

func newFenwickTree(n int) fenwick {
	return make(fenwick, n+1) // 使用下标 1 到 n
}

// a[i] ^= val
// 1 <= i <= n
// 时间复杂度 O(log n)
func (f fenwick) update(i int, val int) {
	for ; i < len(f); i += i & -i {
		f[i] ^= val
	}
}

// 计算前缀异或和 a[1] ^ ... ^ a[i]
// 1 <= i <= n
// 时间复杂度 O(log n)
func (f fenwick) pre(i int) (res int) {
	for ; i > 0; i &= i - 1 {
		res ^= f[i]
	}
	return
}

func palindromePath(n int, edges [][]int, s string, queries []string) (ans []bool) {
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	mx := bits.Len(uint(n))
	pa := make([][16]int, n)
	dep := make([]int, n)
	timeIn := make([]int, n) // DFS 时间戳
	timeOut := make([]int, n)
	clock := 0
	pathXorFromRoot := make([]int, n) // 从根开始的路径中的字母奇偶性的集合
	pathXorFromRoot[0] = 1 << (s[0] - 'a')

	var dfs func(int, int)
	dfs = func(x, p int) {
		pa[x][0] = p
		clock++
		timeIn[x] = clock
		for _, y := range g[x] {
			if y != p {
				dep[y] = dep[x] + 1
				pathXorFromRoot[y] = pathXorFromRoot[x] ^ 1<<(s[y]-'a')
				dfs(y, x)
			}
		}
		timeOut[x] = clock
	}
	dfs(0, -1)

	for i := range mx - 1 {
		for x := range pa {
			p := pa[x][i]
			if p != -1 {
				pa[x][i+1] = pa[p][i]
			} else {
				pa[x][i+1] = -1
			}
		}
	}

	uptoDep := func(x, d int) int {
		for k := uint32(dep[x] - d); k > 0; k &= k - 1 {
			x = pa[x][bits.TrailingZeros32(k)]
		}
		return x
	}

	// 返回 x 和 y 的最近公共祖先
	getLCA := func(x, y int) int {
		if dep[x] > dep[y] {
			x, y = y, x
		}
		y = uptoDep(y, dep[x]) // 使 y 和 x 在同一深度
		if y == x {
			return x
		}
		for i := mx - 1; i >= 0; i-- {
			px, py := pa[x][i], pa[y][i]
			if px != py {
				x, y = px, py // 同时往上跳 2^i 步
			}
		}
		return pa[x][0]
	}

	// 上面全是模板，下面开始本题逻辑

	t := []byte(s)
	f := newFenwickTree(n) // 注意树状数组是异或运算
	for _, q := range queries {
		if q[0] == 'u' {
			x, _ := strconv.Atoi(q[7 : len(q)-2])
			c := q[len(q)-1]
			val := 1<<(t[x]-'a') ^ 1<<(c-'a') // 擦除旧的，换上新的
			t[x] = c
			// 子树 x 全部异或 val，转换成对区间 [timeIn[x], timeOut[x]] 的差分更新
			f.update(timeIn[x], val)
			f.update(timeOut[x]+1, val)
		} else {
			q = q[6:]
			i := strings.IndexByte(q, ' ')
			x, _ := strconv.Atoi(q[:i])
			y, _ := strconv.Atoi(q[i+1:])
			lca := getLCA(x, y)
			res := pathXorFromRoot[x] ^ pathXorFromRoot[y] ^ f.pre(timeIn[x]) ^ f.pre(timeIn[y]) ^ 1<<(t[lca]-'a')
			ans = append(ans, res&(res-1) == 0) // 至多一个字母的出现次数是奇数
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+q)\log n)$，其中 $q$ 是 $\textit{queries}$ 的长度。注意，把字符串转成整数的时间复杂度也是 $\mathcal{O}(\log n)$，因为字符串中的数字长度不超过 $n-1$ 的十进制长度。
- 空间复杂度：$\mathcal{O}(n\log n)$。返回值不计入。

## 专题训练

1. 树题单的「**§3.7 DFS 时间戳**」和「**§3.8 最近公共祖先（LCA）**」。
2. 数据结构题单的「**§1.4 状态压缩前缀和**」和「**§8.1 树状数组**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.1 树状数组`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.1 树状数组`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
