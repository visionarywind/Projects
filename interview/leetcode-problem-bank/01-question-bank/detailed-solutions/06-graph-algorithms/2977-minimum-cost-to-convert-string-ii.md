# 2977. 转换字符串的最小成本 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-convert-string-ii/
- 题目 slug：`minimum-cost-to-convert-string-ii`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.2 全源最短路：Floyd 算法
- 难度分：2696
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-convert-string-ii/solutions/2577877/zi-dian-shu-floyddp-by-endlesscheng-oi2r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[字典树+Floyd+记忆化搜索/递推（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-convert-string-ii/solutions/2577877/zi-dian-shu-floyddp-by-endlesscheng-oi2r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zi-dian-shu-floyddp-by-endlesscheng-oi2r`
- topic id：`2577877`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 思路

1. 把每个字符串转换成一个整数编号，这一步可以用**字典树**完成。见 [208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/)，[原理讲解](https://leetcode.cn/problems/implement-trie-prefix-tree/solutions/2993894/cong-er-cha-shu-dao-er-shi-liu-cha-shu-p-xsj4/)。
2. 建图，从 $\textit{original}[i]$ 向 $\textit{changed}[i]$ 连边，边权为 $\textit{cost}[i]$。
3. 用 **Floyd 算法**求图中任意两点最短路，得到 $\textit{dis}$ 矩阵，原理请看[【图解】带你发明 Floyd 算法！](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/solution/dai-ni-fa-ming-floyd-suan-fa-cong-ji-yi-m8s51/)这里得到的 $\textit{dis}[i][j]$ 表示编号为 $i$ 的子串，通过若干次替换操作变成编号为 $j$ 的子串的最小成本。
4. 动态规划。定义 $\textit{dfs}(i)$ 表示从 $\textit{source}[i]$ 开始向后修改的最小成本。
5. 如果 $\textit{source}[i] = \textit{target}[i]$，可以不修改，$\textit{dfs}(i) = \textit{dfs}(i+1)$。
6. 也可以从 $\textit{source}[i]$ 开始向后修改，利用字典树**快速判断** $\textit{source}$ 和 $\textit{target}$ 的下标从 $i$ 到 $j$ 的子串是否在 $\textit{original}$ 和 $\textit{changed}$ 中，如果在就用 $\textit{dis}[x][y] + \textit{dfs}(j+1)$ 更新 $\textit{dfs}(i)$ 的最小值，其中 $x$ 和 $y$ 分别是 $\textit{source}$ 和 $\textit{target}$ 的这段子串对应的编号。
7. 递归边界 $\textit{dfs}(n) = 0$。
8. 递归入口 $\textit{dfs}(0)$，即为答案。如果答案是无穷大则返回 $-1$。

[本题视频讲解](https://www.bilibili.com/video/BV1rG411k72D/)

### 写法一：记忆化搜索

```py [sol-Python3 切片]
class Solution:
    def minimumCost(self, source: str, target: str, original: List[str], changed: List[str], cost: List[int]) -> int:
        len_to_strs = defaultdict(set)
        dis = defaultdict(lambda: defaultdict(lambda: inf))
        for x, y, c in zip(original, changed, cost):
            len_to_strs[len(x)].add(x)  # 按照长度分组
            len_to_strs[len(y)].add(y)
            dis[x][y] = min(dis[x][y], c)
            dis[x][x] = 0
            dis[y][y] = 0

        # 不同长度的字符串必然在不同的连通块中，分别计算 Floyd
        for strs in len_to_strs.values():
            for k in strs:
                for i in strs:
                    if dis[i][k] == inf:  # 加上这句话，巨大优化！
                        continue
                    for j in strs:
                        dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j])

        # 返回把 source[:i] 变成 target[:i] 的最小成本
        @cache
        def dfs(i: int) -> int:
            if i == 0:
                return 0
            res = inf
            if source[i - 1] == target[i - 1]:
                res = dfs(i - 1)  # 不修改 source[i]
            for size, strs in len_to_strs.items():  # 枚举子串长度
                if i < size:
                    continue
                s = source[i - size: i]
                t = target[i - size: i]
                if s in strs and t in strs:  # 可以替换
                    res = min(res, dis[s][t] + dfs(i - size))
            return res
        ans = dfs(len(source))
        return ans if ans < inf else -1
```

```py [sol-Python3 字典树]
class Node:
    __slots__ = 'son', 'sid'

    def __init__(self):
        self.son = [None] * 26
        self.sid = -1  # 字符串的编号

class Solution:
    def minimumCost(self, source: str, target: str, original: List[str], changed: List[str], cost: List[int]) -> int:
        ord_a = ord('a')
        root = Node()
        sid = 0

        def put(s: str) -> int:
            o = root
            for c in s:
                i = ord(c) - ord_a
                if o.son[i] is None:
                    o.son[i] = Node()
                o = o.son[i]
            if o.sid < 0:
                nonlocal sid
                o.sid = sid
                sid += 1
            return o.sid

        # 初始化距离矩阵
        m = len(cost)
        dis = [[inf] * (m * 2) for _ in range(m * 2)]
        for x, y, c in zip(original, changed, cost):
            x = put(x)
            y = put(y)
            dis[x][y] = min(dis[x][y], c)

        # Floyd 求任意两点最短路
        for k in range(sid):
            for i in range(sid):
                if dis[i][k] == inf:  # 加上这句话，巨大优化！
                    continue
                for j in range(sid):
                    dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j])

        n = len(source)
        @cache
        def dfs(i: int) -> int:
            if i >= n:
                return 0
            res = inf
            if source[i] == target[i]:
                res = dfs(i + 1)  # 不修改 source[i]
            p, q = root, root
            for j in range(i, n):
                p = p.son[ord(source[j]) - ord_a]
                q = q.son[ord(target[j]) - ord_a]
                if p is None or q is None:
                    break
                if p.sid < 0 or q.sid < 0:
                    continue
                # 修改从 i 到 j 的这一段
                res = min(res, dis[p.sid][q.sid] + dfs(j + 1))
            return res
        ans = dfs(0)
        return ans if ans < inf else -1
```

```java [sol-Java]
class Node {
    Node[] son = new Node[26];
    int sid = -1; // 字符串的编号
}

class Solution {
    private Node root = new Node();
    private int sid = 0;
    private char[] s, t;
    private int[][] dis;
    private long[] memo;

    public long minimumCost(String source, String target, String[] original, String[] changed, int[] cost) {
        // 初始化距离矩阵
        int m = cost.length;
        dis = new int[m * 2][m * 2];
        for (int i = 0; i < dis.length; i++) {
            Arrays.fill(dis[i], Integer.MAX_VALUE / 2);
            dis[i][i] = 0;
        }
        for (int i = 0; i < cost.length; i++) {
            int x = put(original[i]);
            int y = put(changed[i]);
            dis[x][y] = Math.min(dis[x][y], cost[i]);
        }

        // Floyd 求任意两点最短路
        for (int k = 0; k < sid; k++) {
            for (int i = 0; i < sid; i++) {
                if (dis[i][k] == Integer.MAX_VALUE / 2) {
                    continue;
                }
                for (int j = 0; j < sid; j++) {
                    dis[i][j] = Math.min(dis[i][j], dis[i][k] + dis[k][j]);
                }
            }
        }

        s = source.toCharArray();
        t = target.toCharArray();
        memo = new long[s.length];
        Arrays.fill(memo, -1);
        long ans = dfs(0);
        return ans < Long.MAX_VALUE / 2 ? ans : -1;
    }

    private int put(String s) {
        Node o = root;
        for (char b : s.toCharArray()) {
            int i = b - 'a';
            if (o.son[i] == null) {
                o.son[i] = new Node();
            }
            o = o.son[i];
        }
        if (o.sid < 0) {
            o.sid = sid++;
        }
        return o.sid;
    }

    private long dfs(int i) {
        if (i >= s.length) {
            return 0;
        }
        if (memo[i] != -1) { // 之前算过
            return memo[i];
        }
        long res = Long.MAX_VALUE / 2;
        if (s[i] == t[i]) {
            res = dfs(i + 1); // 不修改 source[i]
        }
        Node p = root, q = root;
        for (int j = i; j < s.length; j++) {
            p = p.son[s[j] - 'a'];
            q = q.son[t[j] - 'a'];
            if (p == null || q == null) {
                break;
            }
            if (p.sid < 0 || q.sid < 0) {
                continue;
            }
            // 修改从 i 到 j 的这一段
            int d = dis[p.sid][q.sid];
            if (d < Integer.MAX_VALUE / 2) {
                res = Math.min(res, d + dfs(j + 1));
            }
        }
        return memo[i] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
struct Node {
    Node* son[26]{};
    int sid = -1; // 字符串的编号
};

class Solution {
public:
    long long minimumCost(string source, string target, vector<string>& original, vector<string>& changed, vector<int>& cost) {
        Node* root = new Node();
        int sid = 0;
        auto put = [&](string& s) -> int {
            Node* o = root;
            for (char b : s) {
                int i = b - 'a';
                if (o->son[i] == nullptr) {
                    o->son[i] = new Node();
                }
                o = o->son[i];
            }
            if (o->sid < 0) {
                o->sid = sid++;
            }
            return o->sid;
        };

        // 初始化距离矩阵
        int m = cost.size();
        vector dis(m * 2, vector<int>(m * 2, INT_MAX / 2));
        for (int i = 0; i < m * 2; i++) {
            dis[i][i] = 0;
        }
        for (int i = 0; i < m; i++) {
            int x = put(original[i]);
            int y = put(changed[i]);
            dis[x][y] = min(dis[x][y], cost[i]);
        }

        // Floyd 求任意两点最短路
        for (int k = 0; k < sid; k++) {
            for (int i = 0; i < sid; i++) {
                if (dis[i][k] == INT_MAX / 2) { // 加上这句话，巨大优化！
                    continue;
                }
                for (int j = 0; j < sid; j++) {
                    dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j]);
                }
            }
        }

        int n = source.size();
        vector<long long> memo(n, -1);
        auto dfs = [&](this auto&& dfs, int i) -> long long {
            if (i >= n) {
                return 0;
            }
            auto& res = memo[i]; // 注意这里是引用
            if (res != -1) {
                return res;
            }
            res = LONG_LONG_MAX / 2;
            if (source[i] == target[i]) {
                res = dfs(i + 1); // 不修改 source[i]
            }
            Node* p = root;
            Node* q = root;
            for (int j = i; j < n; j++) {
                p = p->son[source[j] - 'a'];
                q = q->son[target[j] - 'a'];
                if (p == nullptr || q == nullptr) {
                    break;
                }
                if (p->sid < 0 || q->sid < 0) {
                    continue;
                }
                // 修改从 i 到 j 的这一段
                int d = dis[p->sid][q->sid];
                if (d < INT_MAX / 2) {
                    res = min(res, dis[p->sid][q->sid] + dfs(j + 1));
                }
            }
            return res;
        };
        long long ans = dfs(0);
        return ans < LONG_LONG_MAX / 2 ? ans : -1;
    }
};
```

```go [sol-Go]
func minimumCost(source, target string, original, changed []string, cost []int) int64 {
	const inf = math.MaxInt / 2
	type node struct {
		son [26]*node
		sid int // 字符串的编号
	}
	root := &node{}
	sid := 0
	put := func(s string) int {
		o := root
		for _, b := range s {
			b -= 'a'
			if o.son[b] == nil {
				o.son[b] = &node{sid: -1}
			}
			o = o.son[b]
		}
		if o.sid < 0 {
			o.sid = sid
			sid++
		}
		return o.sid
	}

	// 初始化距离矩阵
	m := len(cost)
	dis := make([][]int, m*2)
	for i := range dis {
		dis[i] = make([]int, m*2)
		for j := range dis[i] {
			if j != i {
				dis[i][j] = inf
			}
		}
	}
	for i, c := range cost {
		x := put(original[i])
		y := put(changed[i])
		dis[x][y] = min(dis[x][y], c)
	}

	// Floyd 求任意两点最短路
	for k := 0; k < sid; k++ {
		for i := 0; i < sid; i++ {
			if dis[i][k] == inf {
				continue
			}
			for j := 0; j < sid; j++ {
				dis[i][j] = min(dis[i][j], dis[i][k]+dis[k][j])
			}
		}
	}

	n := len(source)
	memo := make([]int, n)
	for i := range memo {
		memo[i] = -1
	}
	var dfs func(int) int
	dfs = func(i int) int {
		if i >= n {
			return 0
		}
		ptr := &memo[i]
		if *ptr != -1 {
			return *ptr
		}
		res := inf
		if source[i] == target[i] {
			res = dfs(i + 1) // 不修改 source[i]
		}
		p, q := root, root
		for j := i; j < n; j++ {
			p = p.son[source[j]-'a']
			q = q.son[target[j]-'a']
			if p == nil || q == nil {
				break
			}
			if p.sid >= 0 && q.sid >= 0 {
				// 修改从 i 到 j 的这一段
				res = min(res, dis[p.sid][q.sid]+dfs(j+1))
			}
		}
		*ptr = res
		return res
	}
	ans := dfs(0)
	if ans == inf {
		return -1
	}
	return int64(ans)
}
```

### 写法二：递推

也可以按照 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)中讲的方法，1:1 翻译成递推。$f[i]$ 的含义与 $\textit{dfs}(i)$ 的含义是一样的。

```py [sol-Python3 切片]
class Solution:
    def minimumCost(self, source: str, target: str, original: List[str], changed: List[str], cost: List[int]) -> int:
        len_to_strs = defaultdict(set)
        dis = defaultdict(lambda: defaultdict(lambda: inf))
        for x, y, c in zip(original, changed, cost):
            len_to_strs[len(x)].add(x)  # 按照长度分组
            len_to_strs[len(y)].add(y)
            dis[x][y] = min(dis[x][y], c)
            dis[x][x] = 0
            dis[y][y] = 0

        # 不同长度的字符串必然在不同的连通块中，分别计算 Floyd
        for strs in len_to_strs.values():
            for k in strs:
                for i in strs:
                    if dis[i][k] == inf:  # 加上这句话，巨大优化！
                        continue
                    for j in strs:
                        dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j])

        # f[i] 表示把 source[:i] 变成 target[:i] 的最小成本
        n = len(source)
        f = [0] + [inf] * n
        for i in range(1, n + 1):
            if source[i - 1] == target[i - 1]:
                f[i] = f[i - 1]  # 不修改 source[i]
            for size, strs in len_to_strs.items():  # 枚举子串长度
                if i < size:
                    continue
                s = source[i - size: i]
                t = target[i - size: i]
                if s in strs and t in strs:  # 可以替换
                    f[i] = min(f[i], dis[s][t] + f[i - size])
        return f[n] if f[n] < inf else -1
```

```py [sol-Python3 字典树]
class Node:
    __slots__ = 'son', 'sid'

    def __init__(self):
        self.son = [None] * 26
        self.sid = -1  # 字符串的编号

class Solution:
    def minimumCost(self, source: str, target: str, original: List[str], changed: List[str], cost: List[int]) -> int:
        ord_a = ord('a')
        root = Node()
        sid = 0

        def put(s: str) -> int:
            o = root
            for c in s:
                i = ord(c) - ord_a
                if o.son[i] is None:
                    o.son[i] = Node()
                o = o.son[i]
            if o.sid < 0:
                nonlocal sid
                o.sid = sid
                sid += 1
            return o.sid

        # 初始化距离矩阵
        m = len(cost)
        dis = [[inf] * (m * 2) for _ in range(m * 2)]
        for x, y, c in zip(original, changed, cost):
            x = put(x)
            y = put(y)
            dis[x][y] = min(dis[x][y], c)

        # Floyd 求任意两点最短路
        for k in range(sid):
            for i in range(sid):
                if dis[i][k] == inf:  # 加上这句话，巨大优化！
                    continue
                for j in range(sid):
                    dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j])

        n = len(source)
        f = [inf] * n + [0]
        for i in range(n - 1, -1, -1):
            if source[i] == target[i]:
                f[i] = f[i + 1]  # 不修改 source[i]
            p, q = root, root
            for j in range(i, n):
                p = p.son[ord(source[j]) - ord_a]
                q = q.son[ord(target[j]) - ord_a]
                if p is None or q is None:
                    break
                if p.sid < 0 or q.sid < 0:
                    continue
                # 修改从 i 到 j 的这一段
                f[i] = min(f[i], dis[p.sid][q.sid] + f[j + 1])
        return f[0] if f[0] < inf else -1
```

```java [sol-Java]
class Node {
    Node[] son = new Node[26];
    int sid = -1; // 字符串的编号
}

class Solution {
    private Node root = new Node();
    private int sid = 0;

    public long minimumCost(String source, String target, String[] original, String[] changed, int[] cost) {
        // 初始化距离矩阵
        int m = cost.length;
        int[][] dis = new int[m * 2][m * 2];
        for (int i = 0; i < dis.length; i++) {
            Arrays.fill(dis[i], Integer.MAX_VALUE / 2);
            dis[i][i] = 0;
        }
        for (int i = 0; i < cost.length; i++) {
            int x = put(original[i]);
            int y = put(changed[i]);
            dis[x][y] = Math.min(dis[x][y], cost[i]);
        }

        // Floyd 求任意两点最短路
        for (int k = 0; k < sid; k++) {
            for (int i = 0; i < sid; i++) {
                if (dis[i][k] == Integer.MAX_VALUE / 2) {
                    continue;
                }
                for (int j = 0; j < sid; j++) {
                    dis[i][j] = Math.min(dis[i][j], dis[i][k] + dis[k][j]);
                }
            }
        }

        char[] s = source.toCharArray();
        char[] t = target.toCharArray();
        int n = s.length;
        long[] f = new long[n + 1];
        for (int i = n - 1; i >= 0; i--) {
            // 不修改 source[i]
            f[i] = s[i] == t[i] ? f[i + 1] : Long.MAX_VALUE / 2;
            Node p = root, q = root;
            for (int j = i; j < n; j++) {
                p = p.son[s[j] - 'a'];
                q = q.son[t[j] - 'a'];
                if (p == null || q == null) {
                    break;
                }
                if (p.sid < 0 || q.sid < 0) {
                    continue;
                }
                // 修改从 i 到 j 的这一段
                int d = dis[p.sid][q.sid];
                if (d < Integer.MAX_VALUE / 2) {
                    f[i] = Math.min(f[i], d + f[j + 1]);
                }
            }
        }
        return f[0] < Long.MAX_VALUE / 2 ? f[0] : -1;
    }

    private int put(String s) {
        Node o = root;
        for (char b : s.toCharArray()) {
            int i = b - 'a';
            if (o.son[i] == null) {
                o.son[i] = new Node();
            }
            o = o.son[i];
        }
        if (o.sid < 0) {
            o.sid = sid++;
        }
        return o.sid;
    }
}
```

```cpp [sol-C++]
struct Node {
    Node* son[26]{};
    int sid = -1; // 字符串的编号
};

class Solution {
public:
    long long minimumCost(string source, string target, vector<string>& original, vector<string>& changed, vector<int>& cost) {
        Node* root = new Node();
        int sid = 0;
        auto put = [&](string& s) -> int {
            Node* o = root;
            for (char b: s) {
                int i = b - 'a';
                if (o->son[i] == nullptr) {
                    o->son[i] = new Node();
                }
                o = o->son[i];
            }
            if (o->sid < 0) {
                o->sid = sid++;
            }
            return o->sid;
        };

        // 初始化距离矩阵
        int m = cost.size();
        vector dis(m * 2, vector<int>(m * 2, INT_MAX / 2));
        for (int i = 0; i < m * 2; i++) {
            dis[i][i] = 0;
        }
        for (int i = 0; i < m; i++) {
            int x = put(original[i]);
            int y = put(changed[i]);
            dis[x][y] = min(dis[x][y], cost[i]);
        }

        // Floyd 求任意两点最短路
        for (int k = 0; k < sid; k++) {
            for (int i = 0; i < sid; i++) {
                if (dis[i][k] == INT_MAX / 2) { // 加上这句话，巨大优化！
                    continue;
                }
                for (int j = 0; j < sid; j++) {
                    dis[i][j] = min(dis[i][j], dis[i][k] + dis[k][j]);
                }
            }
        }

        int n = source.size();
        vector<long long> f(n + 1);
        for (int i = n - 1; i >= 0; i--) {
            // 不修改 source[i]
            f[i] = source[i] == target[i] ? f[i + 1] : LONG_LONG_MAX / 2;
            Node* p = root;
            Node* q = root;
            for (int j = i; j < n; j++) {
                p = p->son[source[j] - 'a'];
                q = q->son[target[j] - 'a'];
                if (p == nullptr || q == nullptr) {
                    break;
                }
                if (p->sid < 0 || q->sid < 0) {
                    continue;
                }
                // 修改从 i 到 j 的这一段
                int d = dis[p->sid][q->sid];
                if (d < INT_MAX / 2) {
                    f[i] = min(f[i], dis[p->sid][q->sid] + f[j + 1]);
                }
            }
        }
        return f[0] < LONG_LONG_MAX / 2 ? f[0] : -1;
    }
};
```

```go [sol-Go]
func minimumCost(source, target string, original, changed []string, cost []int) int64 {
	const inf = math.MaxInt / 2
	type node struct {
		son [26]*node
		sid int // 字符串的编号
	}
	root := &node{}
	sid := 0
	put := func(s string) int {
		o := root
		for _, b := range s {
			b -= 'a'
			if o.son[b] == nil {
				o.son[b] = &node{sid: -1}
			}
			o = o.son[b]
		}
		if o.sid < 0 {
			o.sid = sid
			sid++
		}
		return o.sid
	}

	// 初始化距离矩阵
	m := len(cost)
	dis := make([][]int, m*2)
	for i := range dis {
		dis[i] = make([]int, m*2)
		for j := range dis[i] {
			if j != i {
				dis[i][j] = inf
			}
		}
	}
	for i, c := range cost {
		x := put(original[i])
		y := put(changed[i])
		dis[x][y] = min(dis[x][y], c)
	}

	// Floyd 求任意两点最短路
	for k := 0; k < sid; k++ {
		for i := 0; i < sid; i++ {
			if dis[i][k] == inf {
				continue
			}
			for j := 0; j < sid; j++ {
				dis[i][j] = min(dis[i][j], dis[i][k]+dis[k][j])
			}
		}
	}

	n := len(source)
	f := make([]int, n+1)
	for i := n - 1; i >= 0; i-- {
		if source[i] == target[i] {
			f[i] = f[i+1] // 不修改 source[i]
		} else {
			f[i] = inf
		}
		p, q := root, root
		for j := i; j < n; j++ {
			p = p.son[source[j]-'a']
			q = q.son[target[j]-'a']
			if p == nil || q == nil {
				break
			}
			if p.sid >= 0 && q.sid >= 0 {
				// 修改从 i 到 j 的这一段
				f[i] = min(f[i], dis[p.sid][q.sid]+f[j+1])
			}
		}
	}
	if f[0] == inf {
		return -1
	}
	return int64(f[0])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2+mn+m^3)$，其中 $n$ 为 $\textit{source}$ 的长度，$m$ 为 $\textit{cost}$ 的长度。DP 需要 $\mathcal{O}(n^2)$ 的时间，把 $2m$ 个长度至多为 $n$ 的字符串插入字典树需要 $\mathcal{O}(mn)$ 的时间，Floyd 需要 $\mathcal{O}(m^3)$ 的时间。
- 空间复杂度：$\mathcal{O}(n+mn+m^2)$。DP 需要 $\mathcal{O}(n)$ 的空间，把 $2m$ 个长度至多为 $n$ 的字符串插入字典树需要 $\mathcal{O}(mn)$ 的空间，Floyd 需要 $\mathcal{O}(m^2)$ 的空间。

## 专题训练

1. 动态规划题单的「**§5.2 最优划分**」。
2. 图论题单的「**§3.2 全源最短路：Floyd 算法**」。
3. 数据结构题单的「**六、字典树（trie）**」。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、最短路 / §3.2 全源最短路：Floyd 算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、最短路 / §3.2 全源最短路：Floyd 算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
