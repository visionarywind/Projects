# 1970. 你能穿过矩阵的最后一天

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/last-day-where-you-can-still-cross/
- 题目 slug：`last-day-where-you-can-still-cross`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.2 进阶
- 难度分：2124
- 外部题解来源：https://leetcode.cn/problems/last-day-where-you-can-still-cross/solutions/936629/dao-xu-bing-cha-ji-by-endlesscheng-canj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：逆序 / 正序（Python/Java/C++/Go）](https://leetcode.cn/problems/last-day-where-you-can-still-cross/solutions/936629/dao-xu-bing-cha-ji-by-endlesscheng-canj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dao-xu-bing-cha-ji-by-endlesscheng-canj`
- topic id：`936629`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：逆序（时光倒流）

### 写法一：并查集

本质是判断最上边和最下边是否**连通**。

可以用**并查集**判断是否连通。并查集加边很好做，但删边不好做。

如果时光倒流，删边就成了加边（把水变成陆地）。

代码实现时，可以把格子 $(r,c)$ 映射为 $rn + c$，并创建两个虚拟节点 $mn$ 和 $mn+1$，分别表示最上边（第一行的上面）和最下边（最后一行的下面），方便我们判断最上边和最下边是否连通。

- 如果 $(r,c)$ 从水变成陆地，那么把 $(r,c)$ 和上下左右的陆地格子连起来（注意不要把陆地和水连起来）。
- 特别地，如果 $(r,c)$ 上面没有格子，那么和 $mn$ 相连。
- 特别地，如果 $(r,c)$ 下面没有格子，那么和 $mn+1$ 相连。

最上边和最下边是否连通，等价于判断节点 $mn$ 和 $mn+1$ 是否连通。

倒着遍历 $\textit{cells}$，一旦发现连通就立刻返回答案。

> 注意题目保证 $\textit{cells}$ 的长度等于 $mn$，第 $0$ 天一定连通，第 $mn-1$ 天一定不连通。

关于并查集的完整模板，见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/)。

```py [sol-Python3]
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己
        self._fa = list(range(n))  # 代表元

    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    def find(self, x: int) -> int:
        fa = self._fa
        # 如果 fa[x] == x，则表示 x 是代表元
        if fa[x] != x:
            fa[x] = self.find(fa[x])  # fa 改成代表元
        return fa[x]

    # 判断 x 和 y 是否在同一个集合
    def is_same(self, x: int, y: int) -> bool:
        # 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        # 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return self.find(x) == self.find(y)

    # 把 from 所在集合合并到 to 所在集合中
    def merge(self, from_: int, to: int) -> None:
        x, y = self.find(from_), self.find(to)
        self._fa[x] = y  # 合并集合。修改后就可以认为 from 和 to 在同一个集合了


class Solution:
    def latestDayToCross(self, m: int, n: int, cells: List[List[int]]) -> int:
        DIRS = (0, -1), (0, 1), (-1, 0), (1, 0)  # 左右上下

        top = m * n
        bottom = m * n + 1
        uf = UnionFind(m * n + 2)
        land = [[False] * n for _ in range(m)]

        for day in range(len(cells) - 1, -1, -1):
            r, c = cells[day]
            r -= 1  # 改成从 0 开始的下标
            c -= 1
            v = r * n + c
            land[r][c] = True  # 变成陆地

            if r == 0:
                uf.merge(v, top)  # 与最上边相连

            if r == m - 1:
                uf.merge(v, bottom)  # 与最下边相连

            for dx, dy in DIRS:
                x, y = r + dx, c + dy
                if 0 <= x < m and 0 <= y < n and land[x][y]:
                    uf.merge(v, x * n + y)  # 与四周的陆地相连

            if uf.is_same(top, bottom):  # 最上边和最下边连通
                return day
```

```java [sol-Java]
class UnionFind {
    private final int[] fa; // 代表元

    UnionFind(int n) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己
        fa = new int[n];
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    public int find(int x) {
        // 如果 fa[x] == x，则表示 x 是代表元
        if (fa[x] != x) {
            fa[x] = find(fa[x]); // fa 改成代表元
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    public boolean isSame(int x, int y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return find(x) == find(y);
    }

    // 把 from 所在集合合并到 to 所在集合中
    public void merge(int from, int to) {
        int x = find(from);
        int y = find(to);
        fa[x] = y; // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
    }
}

class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

    public int latestDayToCross(int m, int n, int[][] cells) {
        int top = m * n;
        int bottom = m * n + 1;
        UnionFind uf = new UnionFind(m * n + 2);
        boolean[][] land = new boolean[m][n];

        for (int day = cells.length - 1; ; day--) {
            int[] cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            int v = r * n + c;
            land[r][c] = true; // 变成陆地

            if (r == 0) {
                uf.merge(v, top); // 与最上边相连
            }

            if (r == m - 1) {
                uf.merge(v, bottom); // 与最下边相连
            }

            for (int[] d : DIRS) {
                int x = r + d[0];
                int y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && land[x][y]) {
                    uf.merge(v, x * n + y); // 与四周的陆地相连
                }
            }

            if (uf.isSame(top, bottom)) { // 最上边和最下边连通
                return day;
            }
        }
    }
}
```

```cpp [sol-C++]
class UnionFind {
    vector<int> fa; // 代表元

public:
    UnionFind(int n) : fa(n) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己
        ranges::iota(fa, 0); // iota(fa.begin(), fa.end(), 0);
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    int find(int x) {
        // 如果 fa[x] == x，则表示 x 是代表元
        if (fa[x] != x) {
            fa[x] = find(fa[x]); // fa 改成代表元
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    bool is_same(int x, int y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return find(x) == find(y);
    }

    // 把 from 所在集合合并到 to 所在集合中
    void merge(int from, int to) {
        int x = find(from), y = find(to);
        fa[x] = y; // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
    }
};

class Solution {
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

public:
    int latestDayToCross(int m, int n, vector<vector<int>>& cells) {
        int top = m * n;
        int bottom = m * n + 1;
        UnionFind uf(m * n + 2);
        vector land(m, vector<int8_t>(n));

        for (int day = cells.size() - 1; ; day--) {
            auto& cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            int v = r * n + c;
            land[r][c] = true; // 变成陆地

            if (r == 0) {
                uf.merge(v, top); // 与最上边相连
            }

            if (r == m - 1) {
                uf.merge(v, bottom); // 与最下边相连
            }

            for (auto& d : DIRS) {
                int x = r + d[0], y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && land[x][y]) {
                    uf.merge(v, x * n + y); // 与四周的陆地相连
                }
            }

            if (uf.is_same(top, bottom)) { // 最上边和最下边连通
                return day;
            }
        }
    }
};
```

```go [sol-Go]
type unionFind struct {
	fa []int // 代表元
}

func newUnionFind(n int) unionFind {
	fa := make([]int, n)
	// 一开始有 n 个集合 {0}, {1}, ..., {n-1}
	// 集合 i 的代表元是自己
	for i := range fa {
		fa[i] = i
	}
	return unionFind{fa}
}

// 返回 x 所在集合的代表元
// 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
func (u unionFind) find(x int) int {
	// 如果 fa[x] == x，则表示 x 是代表元
	if u.fa[x] != x {
		u.fa[x] = u.find(u.fa[x]) // fa 改成代表元
	}
	return u.fa[x]
}

// 判断 x 和 y 是否在同一个集合
func (u unionFind) same(x, y int) bool {
	// 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
	// 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
	return u.find(x) == u.find(y)
}

// 把 from 所在集合合并到 to 所在集合中
func (u *unionFind) merge(from, to int) {
	x, y := u.find(from), u.find(to)
	u.fa[x] = y // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
}

var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}} // 左右上下

func latestDayToCross(m, n int, cells [][]int) int {
	top := m * n
	bottom := m*n + 1
	uf := newUnionFind(m*n + 2)

	land := make([][]bool, m)
	for i := range land {
		land[i] = make([]bool, n)
	}

	for day := len(cells) - 1; ; day-- {
		cell := cells[day]
		r, c := cell[0]-1, cell[1]-1 // 改成从 0 开始的下标
		v := r*n + c
		land[r][c] = true // 变成陆地

		if r == 0 {
			uf.merge(v, top) // 与最上边相连
		}

		if r == m-1 {
			uf.merge(v, bottom) // 与最下边相连
		}

		for _, d := range dirs {
			x, y := r+d.x, c+d.y
			if 0 <= x && x < m && 0 <= y && y < n && land[x][y] {
				uf.merge(v, x*n+y) // 与四周的陆地相连
			}
		}

		if uf.same(top, bottom) { // 最上边和最下边连通
			return day
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log(mn))$。
- 空间复杂度：$\mathcal{O}(mn)$。

### 写法二：DFS

想象最上边有病毒，要感染到最下边。

在倒着遍历 $\textit{cells}$，把水变成陆地的过程中：

- 如果陆地在第一行，那么被感染。
- 如果陆地上下左右是水，或者未被感染的陆地，那么这个陆地未被感染。
- 如果陆地与被感染的陆地相邻，那么也被感染。⚠**注意**：如果 $(r,c)$ 从水变成陆地后，把被感染的陆地与未被感染的陆地**连起来**，那么我们还需要从 $(r,c)$ 出发，DFS 访问其余未被感染的陆地，把访问到的陆地也标记为被感染。

```py [sol-Python3]
class Solution:
    def latestDayToCross(self, m: int, n: int, cells: List[List[int]]) -> int:
        DIRS = (0, -1), (0, 1), (-1, 0), (1, 0)  # 左右上下

        # 0：水
        # 1：陆地（未被感染）
        # 2：陆地（已被感染）
        state = [[0] * n for _ in range(m)]

        # 能否从第一行到达 (r, c)
        def can_reach_from_top(r: int, c: int) -> bool:
            if r == 0:  # 已经是第一行
                return True
            for dx, dy in DIRS:
                x, y = r + dx, c + dy
                if 0 <= x < m and 0 <= y < n and state[x][y] == 2:
                    return True
            return False

        # 从 (r, c) 出发，能否到达最后一行
        def dfs(r: int, c: int) -> bool:
            if r == m - 1:
                return True
            state[r][c] = 2  # 感染
            for dx, dy in DIRS:
                x, y = r + dx, c + dy
                # 传播病毒到未被感染的陆地
                if 0 <= x < m and 0 <= y < n and state[x][y] == 1 and dfs(x, y):
                    return True
            return False

        for day in range(len(cells) - 1, -1, -1):
            r, c = cells[day]
            r -= 1  # 改成从 0 开始的下标
            c -= 1
            state[r][c] = 1  # 未被感染的陆地
            if can_reach_from_top(r, c) and dfs(r, c):
                return day
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

    public int latestDayToCross(int m, int n, int[][] cells) {
        // 0：水
        // 1：陆地（未被感染）
        // 2：陆地（已被感染）
        byte[][] state = new byte[m][n];

        for (int day = cells.length - 1; ; day--) {
            int[] cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            state[r][c] = 1; // 未被感染的陆地
            if (canReachFromTop(r, c, state) && dfs(r, c, state)) {
                return day;
            }
        }
    }

    // 能否从第一行到达 (r, c)
    private boolean canReachFromTop(int r, int c, byte[][] state) {
        if (r == 0) { // 已经是第一行
            return true;
        }
        for (int[] d : DIRS) {
            int x = r + d[0];
            int y = c + d[1];
            if (0 <= x && x < state.length && 0 <= y && y < state[x].length && state[x][y] == 2) {
                return true;
            }
        }
        return false;
    }

    // 从 (r, c) 出发，能否到达最后一行
    private boolean dfs(int r, int c, byte[][] state) {
        int m = state.length;
        if (r == m - 1) {
            return true;
        }
        state[r][c] = 2; // 感染
        for (int[] d : DIRS) {
            int x = r + d[0];
            int y = c + d[1];
            // 传播病毒到未被感染的陆地
            if (0 <= x && x < m && 0 <= y && y < state[x].length && state[x][y] == 1 && dfs(x, y, state)) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

public:
    int latestDayToCross(int m, int n, vector<vector<int>>& cells) {
        // 0：水
        // 1：陆地（未被感染）
        // 2：陆地（已被感染）
        vector state(m, vector<int8_t>(n));

        auto can_reach_from_top = [&](int r, int c) -> bool {
            if (r == 0) { // 已经是第一行
                return true;
            }
            for (auto& d : DIRS) {
                int x = r + d[0], y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && state[x][y] == 2) {
                    return true;
                }
            }
            return false;
        };

        auto dfs = [&](this auto&& dfs, int r, int c) -> bool {
            if (r == m - 1) {
                return true;
            }
            state[r][c] = 2; // 感染
            for (auto& d : DIRS) {
                int x = r + d[0], y = c + d[1];
                // 传播病毒到未被感染的陆地
                if (0 <= x && x < m && 0 <= y && y < n && state[x][y] == 1 && dfs(x, y)) {
                    return true;
                }
            }
            return false;
        };

        for (int day = cells.size() - 1; ; day--) {
            auto& cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            state[r][c] = 1; // 未被感染的陆地
            if (can_reach_from_top(r, c) && dfs(r, c)) {
                return day;
            }
        }
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}} // 左右上下

func latestDayToCross(m, n int, cells [][]int) int {
	// 0：水
	// 1：陆地（未被感染）
	// 2：陆地（已被感染）
	state := make([][]int8, m)
	for i := range state {
		state[i] = make([]int8, n)
	}

	// 能否从第一行到达 (r, c)
	canReachFromTop := func(r, c int) bool {
		if r == 0 { // 已经是第一行
			return true
		}
		for _, d := range dirs {
			x, y := r+d.x, c+d.y
			if 0 <= x && x < m && 0 <= y && y < n && state[x][y] == 2 {
				return true
			}
		}
		return false
	}

	// 从 (r, c) 出发，能否到达最后一行
	var dfs func(int, int) bool
	dfs = func(r, c int) bool {
		if r == m-1 {
			return true
		}
		state[r][c] = 2 // 感染
		for _, d := range dirs {
			x, y := r+d.x, c+d.y
			// 传播病毒到未被感染的陆地
			if 0 <= x && x < m && 0 <= y && y < n && state[x][y] == 1 && dfs(x, y) {
				return true
			}
		}
		return false
	}

	for day := len(cells) - 1; ; day-- {
		cell := cells[day]
		r, c := cell[0]-1, cell[1]-1 // 改成从 0 开始的下标
		state[r][c] = 1 // 未被感染的陆地
		if canReachFromTop(r, c) && dfs(r, c) {
			return day
		}
	}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$。在 DFS 中，同一个格子至多访问（被感染）一次。
- 空间复杂度：$\mathcal{O}(mn)$。

## 方法二：正序

也可以正序遍历 $\textit{cells}$。

![lc1970.png](https://pic.leetcode.cn/1766884834-AYoIVP-lc1970.png)

把网格图视作一块木板，把水视作被**腐蚀**掉的木板。

什么时候木板**断开**，即没有任何从上到下的完整段，那么前一天就是答案。

这等价于水可以**从最左边流到最右边**，注意水可以向八个方向移动（见上图最后一天）。

### 写法一：并查集

把格子 $(r,c)$ 映射为 $rn + c$，并创建两个虚拟节点 $mn$ 和 $mn+1$，分别表示最左边（第一列的左边）和最右边（最后一列的右边），方便我们判断最左边和最右边是否连通。

- 如果 $(r,c)$ 从陆地变成水，那么把 $(r,c)$ 和八个方向的水格子连起来。
- 特别地，如果 $(r,c)$ 左边没有格子，那么和 $mn$ 相连。
- 特别地，如果 $(r,c)$ 右边没有格子，那么和 $mn+1$ 相连。

最左边和最右边是否连通，等价于判断节点 $mn$ 和 $mn+1$ 是否连通。

```py [sol-Python3]
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己
        self._fa = list(range(n))  # 代表元

    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    def find(self, x: int) -> int:
        fa = self._fa
        # 如果 fa[x] == x，则表示 x 是代表元
        if fa[x] != x:
            fa[x] = self.find(fa[x])  # fa 改成代表元
        return fa[x]

    # 判断 x 和 y 是否在同一个集合
    def is_same(self, x: int, y: int) -> bool:
        # 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        # 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return self.find(x) == self.find(y)

    # 把 from 所在集合合并到 to 所在集合中
    def merge(self, from_: int, to: int) -> None:
        x, y = self.find(from_), self.find(to)
        self._fa[x] = y  # 合并集合。修改后就可以认为 from 和 to 在同一个集合了


class Solution:
    def latestDayToCross(self, m: int, n: int, cells: List[List[int]]) -> int:
        DIRS = (-1, -1), (-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1)

        left = m * n
        right = m * n + 1
        uf = UnionFind(m * n + 2)
        water = [[False] * n for _ in range(m)]

        for day, (r, c) in enumerate(cells):
            r -= 1  # 改成从 0 开始的下标
            c -= 1
            v = r * n + c
            water[r][c] = True  # 变成水

            if c == 0:
                uf.merge(v, left)  # 与最左边相连

            if c == n - 1:
                uf.merge(v, right)  # 与最右边相连

            for dx, dy in DIRS:
                x, y = r + dx, c + dy
                if 0 <= x < m and 0 <= y < n and water[x][y]:
                    uf.merge(v, x * n + y)  # 与八方向的水相连

            if uf.is_same(left, right):  # 最左边和最右边连通
                return day
```

```java [sol-Java]
class UnionFind {
    private final int[] fa; // 代表元

    UnionFind(int n) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己
        fa = new int[n];
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    public int find(int x) {
        // 如果 fa[x] == x，则表示 x 是代表元
        if (fa[x] != x) {
            fa[x] = find(fa[x]); // fa 改成代表元
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    public boolean isSame(int x, int y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return find(x) == find(y);
    }

    // 把 from 所在集合合并到 to 所在集合中
    public void merge(int from, int to) {
        int x = find(from);
        int y = find(to);
        fa[x] = y; // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
    }
}

class Solution {
    private static final int[][] DIRS = {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

    public int latestDayToCross(int m, int n, int[][] cells) {
        int left = m * n;
        int right = m * n + 1;
        UnionFind uf = new UnionFind(m * n + 2);
        boolean[][] water = new boolean[m][n];

        for (int day = 0; ; day++) {
            int[] cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            int v = r * n + c;
            water[r][c] = true; // 变成水

            if (c == 0) {
                uf.merge(v, left); // 与最左边相连
            }

            if (c == n - 1) {
                uf.merge(v, right); // 与最右边相连
            }

            for (int[] d : DIRS) {
                int x = r + d[0];
                int y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && water[x][y]) {
                    uf.merge(v, x * n + y); // 与八方向的水相连
                }
            }

            if (uf.isSame(left, right)) { // 最左边和最右边连通
                return day;
            }
        }
    }
}
```

```cpp [sol-C++]
class UnionFind {
    vector<int> fa; // 代表元

public:
    UnionFind(int n) : fa(n) {
        // 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        // 集合 i 的代表元是自己
        ranges::iota(fa, 0); // iota(fa.begin(), fa.end(), 0);
    }

    // 返回 x 所在集合的代表元
    // 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
    int find(int x) {
        // 如果 fa[x] == x，则表示 x 是代表元
        if (fa[x] != x) {
            fa[x] = find(fa[x]); // fa 改成代表元
        }
        return fa[x];
    }

    // 判断 x 和 y 是否在同一个集合
    bool is_same(int x, int y) {
        // 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
        // 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
        return find(x) == find(y);
    }

    // 把 from 所在集合合并到 to 所在集合中
    void merge(int from, int to) {
        int x = find(from), y = find(to);
        fa[x] = y; // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
    }
};

class Solution {
    static constexpr int DIRS[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

public:
    int latestDayToCross(int m, int n, vector<vector<int>>& cells) {
        int left = m * n;
        int right = m * n + 1;
        UnionFind uf(m * n + 2);
        vector water(m, vector<int8_t>(n));

        for (int day = 0; ; day++) {
            auto& cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            int v = r * n + c;
            water[r][c] = true; // 变成水

            if (c == 0) {
                uf.merge(v, left); // 与最左边相连
            }

            if (c == n - 1) {
                uf.merge(v, right); // 与最右边相连
            }

            for (auto& d : DIRS) {
                int x = r + d[0], y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && water[x][y]) {
                    uf.merge(v, x * n + y); // 与八方向的水相连
                }
            }

            if (uf.is_same(left, right)) { // 最左边和最右边连通
                return day;
            }
        }
    }
};
```

```go [sol-Go]
type unionFind struct {
	fa []int // 代表元
}

func newUnionFind(n int) unionFind {
	fa := make([]int, n)
	// 一开始有 n 个集合 {0}, {1}, ..., {n-1}
	// 集合 i 的代表元是自己
	for i := range fa {
		fa[i] = i
	}
	return unionFind{fa}
}

// 返回 x 所在集合的代表元
// 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元
func (u unionFind) find(x int) int {
	// 如果 fa[x] == x，则表示 x 是代表元
	if u.fa[x] != x {
		u.fa[x] = u.find(u.fa[x]) // fa 改成代表元
	}
	return u.fa[x]
}

// 判断 x 和 y 是否在同一个集合
func (u unionFind) same(x, y int) bool {
	// 如果 x 的代表元和 y 的代表元相同，那么 x 和 y 就在同一个集合
	// 这就是代表元的作用：用来快速判断两个元素是否在同一个集合
	return u.find(x) == u.find(y)
}

// 把 from 所在集合合并到 to 所在集合中
func (u *unionFind) merge(from, to int) {
	x, y := u.find(from), u.find(to)
	u.fa[x] = y // 合并集合。修改后就可以认为 from 和 to 在同一个集合了
}

var dirs = []struct{ x, y int }{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}}

func latestDayToCross(m, n int, cells [][]int) int {
	left := m * n
	right := m*n + 1
	uf := newUnionFind(m*n + 2)

	water := make([][]bool, m)
	for i := range water {
		water[i] = make([]bool, n)
	}

	for day, cell := range cells {
		r, c := cell[0]-1, cell[1]-1 // 改成从 0 开始的下标
		v := r*n + c
		water[r][c] = true // 变成水

		if c == 0 {
			uf.merge(v, left) // 与最左边相连
		}

		if c == n-1 {
			uf.merge(v, right) // 与最右边相连
		}

		for _, d := range dirs {
			x, y := r+d.x, c+d.y
			if 0 <= x && x < m && 0 <= y && y < n && water[x][y] {
				uf.merge(v, x*n+y) // 与八方向的水相连
			}
		}

		if uf.same(left, right) { // 最左边和最右边连通
			return day
		}
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\log(mn))$。
- 空间复杂度：$\mathcal{O}(mn)$。

### 写法二：DFS

思路同方法一的 DFS。

```py [sol-Python3]
class Solution:
    def latestDayToCross(self, m: int, n: int, cells: List[List[int]]) -> int:
        DIRS = (-1, -1), (-1, 0), (-1, 1), (0, 1), (1, 1), (1, 0), (1, -1), (0, -1)

        # 0：陆地
        # 1：水（未被感染）
        # 2：水（已被感染）
        state = [[0] * n for _ in range(m)]

        def can_reach_from_left(r: int, c: int) -> bool:
            if c == 0:  # 已经是第一列
                return True
            for dx, dy in DIRS:
                x, y = r + dx, c + dy
                if 0 <= x < m and 0 <= y < n and state[x][y] == 2:
                    return True
            return False

        def dfs(r: int, c: int) -> bool:
            if c == n - 1:
                return True
            state[r][c] = 2  # 感染
            for dx, dy in DIRS:
                x, y = r + dx, c + dy
                if 0 <= x < m and 0 <= y < n and state[x][y] == 1 and dfs(x, y):
                    return True
            return False

        for day, (r, c) in enumerate(cells):
            r -= 1  # 改成从 0 开始的下标
            c -= 1
            state[r][c] = 1  # 未被感染的水
            if can_reach_from_left(r, c) and dfs(r, c):
                return day
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

    // 能否从第一列到达 (r, c)
    private boolean canReachFromLeft(int r, int c, byte[][] state) {
        if (c == 0) { // 已经是第一列
            return true;
        }
        for (int[] d : DIRS) {
            int x = r + d[0];
            int y = c + d[1];
            if (0 <= x && x < state.length && 0 <= y && y < state[x].length && state[x][y] == 2) {
                return true;
            }
        }
        return false;
    }

    // 从 (r, c) 出发，能否到达最后一列
    private boolean dfs(int r, int c, byte[][] state) {
        int n = state[0].length;
        if (c == n - 1) {
            return true;
        }
        state[r][c] = 2; // 感染
        for (int[] d : DIRS) {
            int x = r + d[0];
            int y = c + d[1];
            if (0 <= x && x < state.length && 0 <= y && y < n && state[x][y] == 1 && dfs(x, y, state)) {
                return true;
            }
        }
        return false;
    }

    public int latestDayToCross(int m, int n, int[][] cells) {
        // 0：陆地
        // 1：水（未被感染）
        // 2：水（已被感染）
        byte[][] state = new byte[m][n];

        for (int day = 0; ; day++) {
            int[] cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            state[r][c] = 1; // 未被感染的水
            if (canReachFromLeft(r, c, state) && dfs(r, c, state)) {
                return day;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

public:
    int latestDayToCross(int m, int n, vector<vector<int>>& cells) {
        // 0：陆地
        // 1：水（未被感染）
        // 2：水（已被感染）
        vector state(m, vector<int8_t>(n));

        auto can_reach_from_left = [&](int r, int c) {
            if (c == 0) {
                return true;
            }
            for (auto& d : DIRS) {
                int x = r + d[0], y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && state[x][y] == 2) {
                    return true;
                }
            }
            return false;
        };

        auto dfs = [&](this auto&& dfs, int r, int c) -> bool {
            if (c == n - 1) {
                return true;
            }
            state[r][c] = 2; // 感染
            for (auto& d : DIRS) {
                int x = r + d[0], y = c + d[1];
                if (0 <= x && x < m && 0 <= y && y < n && state[x][y] == 1 && dfs(x, y)) {
                    return true;
                }
            }
            return false;
        };

        for (int day = 0; ; day++) {
            auto& cell = cells[day];
            int r = cell[0] - 1; // 改成从 0 开始的下标
            int c = cell[1] - 1;
            state[r][c] = 1; // 未被感染的水
            if (can_reach_from_left(r, c) && dfs(r, c)) {
                return day;
            }
        }
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}}

func latestDayToCross(m, n int, cells [][]int) int {
	// 0：陆地
	// 1：水（未被感染）
	// 2：水（已被感染）
	state := make([][]int8, m)
	for i := range state {
		state[i] = make([]int8, n)
	}

	// 能否从第一列到达 (r, c)
	canReachFromLeft := func(r, c int) bool {
		if c == 0 { // 已经是第一列
			return true
		}
		for _, d := range dirs {
			x, y := r+d.x, c+d.y
			if 0 <= x && x < m && 0 <= y && y < n && state[x][y] == 2 {
				return true
			}
		}
		return false
	}

	// 从 (r, c) 出发，能否到达最后一列
	var dfs func(int, int) bool
	dfs = func(r, c int) bool {
		if c == n-1 {
			return true
		}
		state[r][c] = 2 // 感染
		for _, d := range dirs {
			x, y := r+d.x, c+d.y
			// 传播病毒到未被感染的水
			if 0 <= x && x < m && 0 <= y && y < n && state[x][y] == 1 && dfs(x, y) {
				return true
			}
		}
		return false
	}

	for day, cell := range cells {
		r, c := cell[0]-1, cell[1]-1 // 改成从 0 开始的下标
		state[r][c] = 1 // 未被感染的水
		if canReachFromLeft(r, c) && dfs(r, c) {
			return day
		}
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。

## 相似题目

[803. 打砖块](https://leetcode.cn/problems/bricks-falling-when-hit/)

## 专题训练

见下面思维题单的「**§5.3 逆向思维**」。

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

本题来自 `七、并查集 / §7.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
