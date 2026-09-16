# 3568. 清理教室的最少移动

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-clean-the-classroom/
- 题目 slug：`minimum-moves-to-clean-the-classroom`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.3 旅行商问题（TSP）
- 难度分：2143
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-clean-the-classroom/solutions/3690747/bfs-by-endlesscheng-rpk6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS + 优化，附相似题目（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-moves-to-clean-the-classroom/solutions/3690747/bfs-by-endlesscheng-rpk6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfs-by-endlesscheng-rpk6`
- topic id：`3690747`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本质是计算最短路，但需要一些额外的信息。

- 基本信息是当前位置，即行列下标 $(x,y)$。
- 需要额外知道当前能量值 $e$。
- 需要额外知道当前已收集的垃圾有哪些，即垃圾的编号集合 $\textit{mask}$。

从当前状态 $(x,y,e,\textit{mask})$ 移动到四方向相邻格子，新的状态为：

- 新的位置 $(x',y')$。
- 新的能量值：如果新的位置是 $\texttt{R}$，那么新的能量值为 $\textit{energy}$，否则为 $e-1$。
- 新的已收集垃圾编号集合：如果新的位置是 $\texttt{L}$，往 $\textit{mask}$ 中添加这个垃圾的编号。**注**：BFS 之前，给每个垃圾分配一个从 $0$ 开始的编号。

起点：$(\textit{sx},\textit{sy},\textit{energy},\varnothing)$，其中 $(\textit{sx},\textit{sy})$ 是学生的起始位置。

终点：$(x,y,e,U)$，所有垃圾清理完毕。其中 $U$ 是所有垃圾编号的集合。

关于 BFS 的原理和双列表写法，见[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

代码实现时，集合可以用二进制表示，集合相关运算可以用位运算代替，原理请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

[本题视频讲解](https://www.bilibili.com/video/BV1Dz76zfEdi/?t=15m01s)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
class Solution:
    def minMoves(self, classroom: List[str], energy: int) -> int:
        m, n = len(classroom), len(classroom[0])
        idx = [[0] * n for _ in range(m)]
        cnt_l = sx = sy = 0
        for i, row in enumerate(classroom):
            for j, b in enumerate(row):
                if b == 'L':
                    idx[i][j] = 1 << cnt_l  # 给垃圾分配编号（提前计算左移）
                    cnt_l += 1
                elif b == 'S':
                    sx, sy = i, j
        if cnt_l == 0:
            return 0

        DIRS = (-1, 0), (1, 0), (0, -1), (0, 1)
        vis = [[[[False] * (1 << cnt_l) for _ in range(energy + 1)] for _ in range(n)] for _ in range(m)]
        vis[sx][sy][energy][0] = True
        q = [(sx, sy, energy, 0)]

        full_mask = (1 << cnt_l) - 1
        ans = 0
        while q:
            tmp = q
            q = []
            for x, y, e, mask in tmp:
                if mask == full_mask:  # 所有垃圾收集完毕
                    return ans
                if e == 0:  # 没能量了
                    continue
                for dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < m and 0 <= ny < n and classroom[nx][ny] != 'X':
                        new_e = energy if classroom[nx][ny] == 'R' else e - 1
                        new_mask = mask | idx[nx][ny]  # 添加垃圾（没有垃圾时 mask 不变）
                        if not vis[nx][ny][new_e][new_mask]:
                            vis[nx][ny][new_e][new_mask] = True
                            q.append((nx, ny, new_e, new_mask))
            ans += 1
        return -1
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    private record Node(int x, int y, int e, int mask) {
    }

    public int minMoves(String[] classroom, int energy) {
        int m = classroom.length;
        int n = classroom[0].length();
        int[][] idx = new int[m][n];
        int cntL = 0, sx = 0, sy = 0;
        for (int i = 0; i < m; i++) {
            String row = classroom[i];
            for (int j = 0; j < n; j++) {
                char b = row.charAt(j);
                if (b == 'L') {
                    idx[i][j] = 1 << cntL++; // 给垃圾分配编号（提前计算左移）
                } else if (b == 'S') {
                    sx = i;
                    sy = j;
                }
            }
        }

        int u = 1 << cntL;
        boolean[][][][] vis = new boolean[m][n][energy + 1][u];
        vis[sx][sy][energy][0] = true;

        List<Node> q = new ArrayList<>();
        q.add(new Node(sx, sy, energy, 0));
        for (int ans = 0; !q.isEmpty(); ans++) {
            List<Node> tmp = q;
            q = new ArrayList<>();
            for (Node p : tmp) {
                if (p.mask == u - 1) { // 所有垃圾收集完毕
                    return ans;
                }
                if (p.e == 0) { // 走不动了
                    continue;
                }
                for (int[] d : DIRS) {
                    int x = p.x + d[0], y = p.y + d[1];
                    if (x >= 0 && x < m && y >= 0 && y < n && classroom[x].charAt(y) != 'X') {
                        int newE = classroom[x].charAt(y) == 'R' ? energy : p.e - 1;
                        int newMask = p.mask | idx[x][y]; // 添加垃圾（没有垃圾时 mask 不变）
                        if (!vis[x][y][newE][newMask]) {
                            vis[x][y][newE][newMask] = true;
                            q.add(new Node(x, y, newE, newMask));
                        }
                    }
                }
            }
        }
        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

public:
    int minMoves(vector<string>& classroom, int energy) {
        int m = classroom.size(), n = classroom[0].size();
        vector idx(m, vector<int>(n));
        int cnt_l = 0, sx = 0, sy = 0;
        for (int i = 0; i < m; i++) {
            auto& row = classroom[i];
            for (int j = 0; j < n; j++) {
                char b = row[j];
                if (b == 'L') {
                    idx[i][j] = 1 << cnt_l++; // 给垃圾分配编号（提前计算左移）
                } else if (b == 'S') {
                    sx = i;
                    sy = j;
                }
            }
        }

        int u = 1 << cnt_l;
        vector vis(m, vector(n, vector(energy + 1, vector<int8_t>(u))));
        vis[sx][sy][energy][0] = true;
        struct Node { int x, y, e, mask; };
        vector<Node> q = {{sx, sy, energy, 0}};

        for (int ans = 0; !q.empty(); ans++) {
            auto tmp = move(q); // move 后 q 为空
            for (auto& [x, y, e, mask] : tmp) {
                if (mask == u - 1) { // 所有垃圾收集完毕
                    return ans;
                }
                if (e == 0) { // 走不动了
                    continue;
                }
                for (auto& [dx, dy] : dirs) {
                    int nx = x + dx, ny = y + dy;
                    if (0 <= nx && nx < m && 0 <= ny && ny < n && classroom[nx][ny] != 'X') {
                        int new_e = classroom[nx][ny] == 'R' ? energy : e - 1;
                        int new_mask = mask | idx[nx][ny]; // 添加垃圾（没有垃圾时 mask 不变）
                        if (!vis[nx][ny][new_e][new_mask]) {
                            vis[nx][ny][new_e][new_mask] = true;
                            q.emplace_back(nx, ny, new_e, new_mask);
                        }
                    }
                }
            }
        }
        return -1;
    }
};
```

```go [sol-Go]
func minMoves(classroom []string, energy int) (ans int) {
	m, n := len(classroom), len(classroom[0])
	idx := make([][]int, m)
	for i := range idx {
		idx[i] = make([]int, n)
	}
	var cntL, sx, sy int
	for i, row := range classroom {
		for j, b := range row {
			if b == 'L' {
				idx[i][j] = 1 << cntL // 给垃圾分配编号（提前计算左移）
				cntL++
			} else if b == 'S' {
				sx, sy = i, j
			}
		}
	}

	dirs := []struct{ x, y int }{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}
	u := 1 << cntL
	vis := make([][][][]bool, m)
	for i := range vis {
		vis[i] = make([][][]bool, n)
		for j := range vis[i] {
			vis[i][j] = make([][]bool, energy+1)
			for k := range vis[i][j] {
				vis[i][j][k] = make([]bool, u)
			}
		}
	}

	vis[sx][sy][energy][0] = true
	type tuple struct{ x, y, e, mask int }
	q := []tuple{{sx, sy, energy, 0}}

	for ; len(q) > 0; ans++ {
		tmp := q
		q = nil
		for _, p := range tmp {
			if p.mask == u-1 { // 所有垃圾收集完毕
				return
			}
			if p.e == 0 { // 走不动了
				continue
			}
			for _, d := range dirs {
				x, y := p.x+d.x, p.y+d.y
				if 0 <= x && x < m && 0 <= y && y < n && classroom[x][y] != 'X' {
					newE := p.e - 1
					if classroom[x][y] == 'R' {
						newE = energy // 充满能量
					}
					newMask := p.mask | idx[x][y] // 添加垃圾（没有垃圾时 mask 不变）
					if !vis[x][y][newE][newMask] {
						vis[x][y][newE][newMask] = true
						q = append(q, tuple{x, y, newE, newMask})
					}
				}
			}
		}
	}
	return -1
}
```

## 优化

把 $\textit{vis}$ 改名为 $\textit{maxEnergy}$，其中的能量 $e$ 这一维度去掉，改为数组保存的值。

只有当状态 $(x,y,e,\textit{mask})$ 中的 $e > \textit{maxEnergy}[x][y][\textit{mask}]$，才入队，并更新 $\textit{maxEnergy}[x][y][\textit{mask}]=e$。

这样做的好处是，不会让相同 $(x,y,\textit{mask})$ 下的更小的能量入队，从而避免在两个相邻位置之间反复横跳，**避免无意义地消耗能量**。

```py [sol-Python3]
class Solution:
    def minMoves(self, classroom: List[str], energy: int) -> int:
        m, n = len(classroom), len(classroom[0])
        idx = [[0] * n for _ in range(m)]
        cnt_l = sx = sy = 0
        for i, row in enumerate(classroom):
            for j, b in enumerate(row):
                if b == 'L':
                    idx[i][j] = 1 << cnt_l
                    cnt_l += 1
                elif b == 'S':
                    sx, sy = i, j
        if cnt_l == 0:
            return 0

        DIRS = (-1, 0), (1, 0), (0, -1), (0, 1)
        max_energy = [[[-1] * (1 << cnt_l) for _ in range(n)] for _ in range(m)]
        max_energy[sx][sy][0] = energy
        q = [(sx, sy, energy, 0)]

        full_mask = (1 << cnt_l) - 1
        ans = 0
        while q:
            tmp = q
            q = []
            for x, y, e, mask in tmp:
                if mask == full_mask:
                    return ans
                if e == 0:
                    continue
                for dx, dy in DIRS:
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < m and 0 <= ny < n and classroom[nx][ny] != 'X':
                        new_e = energy if classroom[nx][ny] == 'R' else e - 1
                        new_mask = mask | idx[nx][ny]
                        if new_e > max_energy[nx][ny][new_mask]:
                            max_energy[nx][ny][new_mask] = new_e
                            q.append((nx, ny, new_e, new_mask))
            ans += 1
        return -1
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    private record Node(int x, int y, byte e, int mask) {
    }

    public int minMoves(String[] classroom, int energy) {
        int m = classroom.length;
        int n = classroom[0].length();
        char[][] grid = new char[m][]; // 把 String[] 转成 char[][]，读取效率更高
        int[][] idx = new int[m][n];
        int cntL = 0, sx = 0, sy = 0;
        for (int i = 0; i < m; i++) {
            grid[i] = classroom[i].toCharArray();
            for (int j = 0; j < n; j++) {
                char b = grid[i][j];
                if (b == 'L') {
                    idx[i][j] = 1 << cntL++;
                } else if (b == 'S') {
                    sx = i;
                    sy = j;
                }
            }
        }

        int u = 1 << cntL;
        byte[][][] maxEnergy = new byte[m][n][u]; // byte 空间小
        for (byte[][] mat : maxEnergy) {
            for (byte[] row : mat) {
                Arrays.fill(row, (byte) -1);
            }
        }
        maxEnergy[sx][sy][0] = (byte) energy;
        List<Node> q = new ArrayList<>();
        q.add(new Node(sx, sy, (byte) energy, 0));

        for (int ans = 0; !q.isEmpty(); ans++) {
            List<Node> tmp = q;
            q = new ArrayList<>();
            for (Node p : tmp) {
                if (p.mask == u - 1) {
                    return ans;
                }
                if (p.e == 0) {
                    continue;
                }
                for (int[] d : DIRS) {
                    int x = p.x + d[0], y = p.y + d[1];
                    if (x >= 0 && x < m && y >= 0 && y < n && grid[x][y] != 'X') {
                        byte newE = (byte) (grid[x][y] == 'R' ? energy : p.e - 1);
                        int newMask = p.mask | idx[x][y];
                        if (newE > maxEnergy[x][y][newMask]) {
                            maxEnergy[x][y][newMask] = newE;
                            q.add(new Node(x, y, newE, newMask));
                        }
                    }
                }
            }
        }
        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

public:
    int minMoves(vector<string>& classroom, int energy) {
        int m = classroom.size(), n = classroom[0].size();
        vector idx(m, vector<int>(n));
        int cnt_l = 0, sx = 0, sy = 0;
        for (int i = 0; i < m; i++) {
            auto& row = classroom[i];
            for (int j = 0; j < n; j++) {
                char b = row[j];
                if (b == 'L') {
                    idx[i][j] = 1 << cnt_l++;
                } else if (b == 'S') {
                    sx = i;
                    sy = j;
                }
            }
        }

        int u = 1 << cnt_l;
        vector max_energy(m, vector(n, vector<int8_t>(u, -1)));
        max_energy[sx][sy][0] = energy;
        struct Node { int x, y, e, mask; };
        vector<Node> q = {{sx, sy, energy, 0}};

        for (int ans = 0; !q.empty(); ans++) {
            auto tmp = move(q); // move 后 q 为空
            for (auto& [x, y, e, mask] : tmp) {
                if (mask == u - 1) {
                    return ans;
                }
                if (e == 0) {
                    continue;
                }
                for (auto& [dx, dy] : dirs) {
                    int nx = x + dx, ny = y + dy;
                    if (0 <= nx && nx < m && 0 <= ny && ny < n && classroom[nx][ny] != 'X') {
                        int new_e = classroom[nx][ny] == 'R' ? energy : e - 1;
                        int new_mask = mask | idx[nx][ny];
                        if (new_e > max_energy[nx][ny][new_mask]) {
                            max_energy[nx][ny][new_mask] = new_e;
                            q.emplace_back(nx, ny, new_e, new_mask);
                        }
                    }
                }
            }
        }
        return -1;
    }
};
```

```go [sol-Go]
func minMoves(classroom []string, energy int) (ans int) {
	m, n := len(classroom), len(classroom[0])
	idx := make([][]int, m)
	for i := range idx {
		idx[i] = make([]int, n)
	}
	var cntL, sx, sy int
	for i, row := range classroom {
		for j, b := range row {
			if b == 'L' {
				idx[i][j] = 1 << cntL
				cntL++
			} else if b == 'S' {
				sx, sy = i, j
			}
		}
	}

	dirs := []struct{ x, y int }{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}
	u := 1 << cntL
	maxEnergy := make([][][]int8, m)
	for i := range maxEnergy {
		maxEnergy[i] = make([][]int8, n)
		for j := range maxEnergy[i] {
			maxEnergy[i][j] = make([]int8, u)
			for k := range maxEnergy[i][j] {
				maxEnergy[i][j][k] = -1
			}
		}
	}

	maxEnergy[sx][sy][0] = int8(energy)
	type tuple struct{ x, y, e, mask int }
	q := []tuple{{sx, sy, energy, 0}}

	for ; len(q) > 0; ans++ {
		tmp := q
		q = nil
		for _, p := range tmp {
			if p.mask == u-1 {
				return
			}
			if p.e == 0 {
				continue
			}
			for _, d := range dirs {
				x, y := p.x+d.x, p.y+d.y
				if 0 <= x && x < m && 0 <= y && y < n && classroom[x][y] != 'X' {
					newE := p.e - 1
					if classroom[x][y] == 'R' {
						newE = energy
					}
					newMask := p.mask | idx[x][y]
					if int8(newE) > maxEnergy[x][y][newMask] {
						maxEnergy[x][y][newMask] = int8(newE)
						q = append(q, tuple{x, y, newE, newMask})
					}
				}
			}
		}
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\cdot n\cdot \textit{energy}\cdot 2^L)$，其中 $m$ 和 $n$ 分别为 $\textit{classroom}$ 的行数和列数，$L\le 10$ 为垃圾个数。每个状态至多访问一次。
- 空间复杂度：$\mathcal{O}(m\cdot n\cdot \textit{energy}\cdot 2^L)$。这是双列表（队列）需要的空间。

## 相似题目

- [864. 获取所有钥匙的最短路径](https://leetcode.cn/problems/shortest-path-to-get-all-keys/)
- [LCP 13. 寻宝](https://leetcode.cn/problems/xun-bao/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.3 旅行商问题（TSP）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.3 旅行商问题（TSP）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
