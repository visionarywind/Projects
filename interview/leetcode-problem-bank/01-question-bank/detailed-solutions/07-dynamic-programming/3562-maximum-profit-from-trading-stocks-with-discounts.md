# 3562. 折扣价交易股票的最大利润

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-profit-from-trading-stocks-with-discounts/
- 题目 slug：`maximum-profit-from-trading-stocks-with-discounts`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.5 树上背包（选做）
- 难度分：2458
- 外部题解来源：https://leetcode.cn/problems/maximum-profit-from-trading-stocks-with-discounts/solutions/3685504/shu-shang-bei-bao-zhuang-tai-ji-dppython-2q7b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树上背包 + 状态机 DP，两种写法（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-profit-from-trading-stocks-with-discounts/solutions/3685504/shu-shang-bei-bao-zhuang-tai-ji-dppython-2q7b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-shang-bei-bao-zhuang-tai-ji-dppython-2q7b`
- topic id：`3685504`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**注**：数据范围说 `hierarchy.length == n - 1`，且 `员工 1 是所有员工的直接或间接上司`，所以输入的是一个 $n$ 点 $n-1$ 边的连通图，即树。

## 前置知识

请确认你已经掌握如下知识点：

1. **0-1 背包**，包括**空间优化**的原理。请看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。
2. **状态机 DP**。请看[【基础算法精讲 21】](https://www.bilibili.com/video/BV1ho4y1W7QK/)。

## 寻找子问题

站在节点 $x$ 上，讨论是否购买 $\textit{present}[x]$。设节点 $y$ 是节点 $x$ 的儿子。

- 如果不买 $\textit{present}[x]$，且预算至多为 $j$，那么问题变成：
   - 从 $x$ 的所有子树 $y$ 中能得到的最大利润之和。
   - 所有子树 $y$ 的花费总和必须 $\le j$。
   - $\textit{present}[y]$ 不能半价购买。
- 如果买 $\textit{present}[x]$，且预算至多为 $j$，那么问题变成：
   - 从 $x$ 的所有子树 $y$ 中能得到的最大利润之和，加上买 $\textit{present}[x]$ 得到的利润。
   - 买 $\textit{present}[x]$ 花了 $\textit{cost}$，其中 $\textit{cost}$ 等于 $\textit{present}[x]$（原价买股票）或者 $\left\lfloor\dfrac{\textit{present}[x]}{2}\right\rfloor$（半价买股票），取决于 $x$ 的父节点有没有买股票。
   - 所有子树 $y$ 的花费总和必须 $\le j-\textit{cost}$。
   - $\textit{present}[y]$ 可以半价购买。

## 状态设计和状态转移

$\textit{dfs}(x)$ 返回一个长为 $(\textit{budget}+1)\times 2$ 的二维数组 $f$，其中 $f[j][k]$ 表示：

- 从子树 $x$ 中能得到的最大利润之和。
- 预算为 $j$，即花费总和 $\le j$。
- $k=0$ 表示 $\textit{present}[x]$ 不能半价购买，$k=1$ 表示 $\textit{present}[x]$ 可以半价购买。

首先，计算 $x$ 的所有儿子子树 $y$ 的最大利润总和 $\textit{subF}[j][k]$。枚举 $x$ 的儿子 $y$：

- 枚举分配给当前儿子 $y$ 的预算 $j_y = 0,1,2,\ldots,j$，那么分配给前面遍历过的儿子的总预算为 $j-j_y$。
- 用前面遍历过的儿子的收益 $\textit{subF}[j-j_y][k]$ 加上当前儿子 $y$ 的收益 $\textit{dfs}(y)[j_y][k]$，更新 $\textit{subF}[j][k]$ 的最大值。注意这里用了 0-1 背包的空间优化。

然后，考虑 $\textit{present}[x]$ 是否购买，计算 $f[j][k]$：

- 不买 $\textit{present}[x]$，那么分配给儿子子树的预算不变，仍然为 $j$，即 $f[j][k] = \textit{subF}[j][0]$，这里的 $0$ 是因为对于子树 $y$ 来说，父节点 $x$ 一定不买。
- 买 $\textit{present}[x]$，那么分配给儿子子树的预算要扣掉 $\textit{cost}$，即 $f[j][k] = \textit{subF}[j-\textit{cost}][1]$，这里的 $1$ 是因为对于子树 $y$ 来说，父节点 $x$ 一定买。

两种情况取最大值，得

$$
f[j][k] = \max(\textit{subF}[j][0], \textit{subF}[j-\textit{cost}][1] + \textit{future}[x] - \textit{cost})
$$

最终答案为根节点的 $f[\textit{budget}][0]$，这里的 $0$ 是因为根节点没有父节点。

[本题视频讲解](https://www.bilibili.com/video/BV1o1jgzJE51/?t=7m52s)，从特殊到一般，带你一步步思考。

## 写法一：至多

```py [sol-Python3]
# 注意！这个写法很慢，更快的写法见写法二
max = lambda a, b: b if b > a else a

class Solution:
    def maxProfit(self, n: int, present: List[int], future: List[int], hierarchy: List[List[int]], budget: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in hierarchy:
            g[x - 1].append(y - 1)

        def dfs(x: int) -> List[List[int]]:
            # 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
            sub_f = [[0, 0] for _ in range(budget + 1)]
            for y in g[x]:
                fy = dfs(y)
                for j in range(budget, -1, -1):
                    # 枚举子树 y 的预算为 jy
                    # 当作一个体积为 jy，价值为 fy[jy][k] 的物品
                    for jy in range(j + 1):  
                        for k in range(2):  # k=0 表示 x 不买，k=1 表示 x 买
                            sub_f[j][k] = max(sub_f[j][k], sub_f[j - jy][k] + fy[jy][k])

            # 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
            f = [[0, 0] for _ in range(budget + 1)]
            for j in range(budget + 1):
                for k in range(2):  # k=0 表示 x 父节点不买，k=1 表示 x 父节点买
                    cost = present[x] // (k + 1)
                    if j >= cost:
                        # 不买 x，转移来源是 sub_f[j][0]
                        # 买 x，转移来源为 sub_f[j-cost][1]，因为对于子树来说，父节点一定买
                        f[j][k] = max(sub_f[j][0], sub_f[j - cost][1] + future[x] - cost)
                    else:  # 只能不买 x
                        f[j][k] = sub_f[j][0]
            return f

        return dfs(0)[budget][0]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[] present, int[] future, int[][] hierarchy, int budget) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : hierarchy) {
            g[e[0] - 1].add(e[1] - 1);
        }

        int[][] f0 = dfs(0, g, present, future, budget);
        return f0[budget][0];
    }

    private int[][] dfs(int x, List<Integer>[] g, int[] present, int[] future, int budget) {
        // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
        int[][] subF = new int[budget + 1][2];
        for (int y : g[x]) {
            int[][] fy = dfs(y, g, present, future, budget);
            for (int j = budget; j >= 0; j--) {
                // 枚举子树 y 的预算为 jy
                // 当作一个体积为 jy，价值为 fy[jy][k] 的物品
                for (int jy = 0; jy <= j; jy++) {
                    for (int k = 0; k < 2; k++) { // k=0 表示 x 不买，k=1 表示 x 买
                        subF[j][k] = Math.max(subF[j][k], subF[j - jy][k] + fy[jy][k]);
                    }
                }
            }
        }

        // 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
        int[][] f = new int[budget + 1][2];
        for (int j = 0; j <= budget; j++) {
            for (int k = 0; k < 2; k++) { // k=0 表示 x 父节点不买，k=1 表示 x 父节点买
                int cost = present[x] / (k + 1);
                if (j >= cost) {
                    // 不买 x，转移来源是 subF[j][0]
                    // 买 x，转移来源为 subF[j-cost][1]，因为对于子树来说，父节点一定买
                    f[j][k] = Math.max(subF[j][0], subF[j - cost][1] + future[x] - cost);
                } else { // 只能不买 x
                    f[j][k] = subF[j][0];
                }
            }
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(int n, vector<int>& present, vector<int>& future, vector<vector<int>>& hierarchy, int budget) {
        vector<vector<int>> g(n);
        for (auto& e : hierarchy) {
            g[e[0] - 1].push_back(e[1] - 1);
        }

        auto dfs = [&](this auto&& dfs, int x) -> vector<array<int, 2>> {
            // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
            vector<array<int, 2>> sub_f(budget + 1);
            for (int y : g[x]) {
                auto fy = dfs(y);
                for (int j = budget; j >= 0; j--) {
                    // 枚举子树 y 的预算为 jy
                    // 当作一个体积为 jy，价值为 fy[jy][k] 的物品
                    for (int jy = 0; jy <= j; jy++) {
                        for (int k = 0; k < 2; k++) { // k=0 表示 x 不买，k=1 表示 x 买
                            sub_f[j][k] = max(sub_f[j][k], sub_f[j - jy][k] + fy[jy][k]);
                        }
                    }
                }
            }

            // 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
            vector<array<int, 2>> f(budget + 1);
            for (int j = 0; j <= budget; j++) {
                for (int k = 0; k < 2; k++) { // k=0 表示 x 父节点不买，k=1 表示 x 父节点买
                    int cost = present[x] / (k + 1);
                    if (j >= cost) {
                        // 不买 x，转移来源是 sub_f[j][0]
                        // 买 x，转移来源为 sub_f[j-cost][1]，因为对于子树来说，父节点一定买
                        f[j][k] = max(sub_f[j][0], sub_f[j - cost][1] + future[x] - cost);
                    } else { // 只能不买 x
                        f[j][k] = sub_f[j][0];
                    }
                }
            }
            return f;
        };

        return dfs(0)[budget][0];
    }
};
```

```go [sol-Go]
func maxProfit(n int, present []int, future []int, hierarchy [][]int, budget int) int {
	g := make([][]int, n)
	for _, e := range hierarchy {
		x, y := e[0]-1, e[1]-1
		g[x] = append(g[x], y)
	}

	var dfs func(int) [][2]int
	dfs = func(x int) [][2]int {
		// 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
		subF := make([][2]int, budget+1)
		for _, y := range g[x] {
			fy := dfs(y)
			for j := budget; j >= 0; j-- {
				// 枚举子树 y 的预算为 jy
				// 当作一个体积为 jy，价值为 resY=fy[jy][k] 的物品
				for jy, p := range fy[:j+1] {
					for k, resY := range p { // k=0 表示 x 不买，k=1 表示 x 买
						subF[j][k] = max(subF[j][k], subF[j-jy][k]+resY)
					}
				}
			}
		}

		// 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
		f := make([][2]int, budget+1)
		for j, p := range subF {
			for k := range 2 { // k=0 表示 x 父节点不买，k=1 表示 x 父节点买
				cost := present[x] / (k + 1)
				if j >= cost {
					// 不买 x，转移来源是 subF[j][0]
					// 买 x，转移来源为 subF[j-cost][1]，因为对于子树来说，父节点一定买
					f[j][k] = max(p[0], subF[j-cost][1]+future[x]-cost)
				} else { // 只能不买 x
					f[j][k] = p[0]
				}
			}
		}
		return f
	}

	return dfs(0)[budget][0]
}
```

## 写法二：恰好

把状态值改成在总花费**恰好**为 $j$ 的情况下的最大利润。

优化：交换 $f$ 数组的维度，改成两个长为 $\textit{budget}+1$ 的数组。

```py [sol-Python3 列表]
# 更快的写法见【Python3 字典】
fmax = lambda a, b: b if b > a else a

class Solution:
    def maxProfit(self, n: int, present: List[int], future: List[int], hierarchy: List[List[int]], budget: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in hierarchy:
            g[x - 1].append(y - 1)

        def dfs(x: int) -> List[List[int]]:
            # 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
            sub_f = [[0] + [-inf] * budget for _ in range(2)]
            for y in g[x]:
                fy = dfs(y)
                for k, fyk in enumerate(fy):
                    nf = [0] + [-inf] * budget
                    for jy, res_y in enumerate(fyk):
                        if res_y < 0:  # 重要优化：物品价值为负数，一定不选
                            continue
                        for j in range(jy, budget + 1):
                            nf[j] = fmax(nf[j], sub_f[k][j - jy] + res_y)
                    sub_f[k] = nf

            f = [None] * 2
            for k in range(2):
                # 不买 x，转移来源为 sub_f[0]，因为对于子树来说，父节点一定不买
                f[k] = sub_f[0].copy()
                cost = present[x] // (k + 1)
                # 买 x，转移来源为 sub_f[1]，因为对于子树来说，父节点一定买
                for j in range(cost, budget + 1):
                    f[k][j] = fmax(f[k][j], sub_f[1][j - cost] + future[x] - cost)
            return f

        return max(dfs(0)[0])
```

```py [sol-Python3 字典]
fmax = lambda a, b: b if b > a else a

class Solution:
    def maxProfit(self, n: int, present: List[int], future: List[int], hierarchy: List[List[int]], budget: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in hierarchy:
            g[x - 1].append(y - 1)

        def dfs(x: int) -> List[Dict[int, int]]:
            sub_f = [defaultdict(int) for _ in range(2)]
            sub_f[0][0] = sub_f[1][0] = 0
            for y in g[x]:
                fy = dfs(y)
                for k, fyk in enumerate(fy):
                    nf = defaultdict(int)
                    for j, pre_res_y in sub_f[k].items():
                        for jy, res_y in fyk.items():
                            sj = j + jy
                            if sj <= budget:
                                nf[sj] = fmax(nf[sj], pre_res_y + res_y)
                    sub_f[k] = nf

            f = [None] * 2
            for k in range(2):
                res = sub_f[0].copy()
                cost = present[x] // (k + 1)
                if cost <= budget:
                    earn = future[x] - cost
                    for j, res_y in sub_f[1].items():
                        sj = j + cost
                        if sj <= budget:
                            res[sj] = fmax(res[sj], res_y + earn)
                f[k] = res
            return f

        return max(dfs(0)[0].values())
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[] present, int[] future, int[][] hierarchy, int budget) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : hierarchy) {
            g[e[0] - 1].add(e[1] - 1);
        }

        int[][] f0 = dfs(0, g, present, future, budget);
        return Arrays.stream(f0[0]).max().getAsInt();
    }

    private int[][] dfs(int x, List<Integer>[] g, int[] present, int[] future, int budget) {
        // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
        int[][] subF = new int[2][budget + 1];
        Arrays.fill(subF[0], Integer.MIN_VALUE / 2); // 表示不存在对应的花费总和
        Arrays.fill(subF[1], Integer.MIN_VALUE / 2);
        subF[0][0] = subF[1][0] = 0;
        for (int y : g[x]) {
            int[][] fy = dfs(y, g, present, future, budget);
            for (int k = 0; k < 2; k++) {
                int[] nf = new int[budget + 1];
                Arrays.fill(nf, Integer.MIN_VALUE / 2);
                nf[0] = 0;
                for (int jy = 0; jy <= budget; jy++) {
                    int resY = fy[k][jy];
                    if (resY < 0) { // 重要优化：物品价值为负数，一定不选
                        continue;
                    }
                    for (int j = jy; j <= budget; j++) {
                        nf[j] = Math.max(nf[j], subF[k][j - jy] + resY);
                    }
                }
                subF[k] = nf;
            }
        }

        int[][] f = new int[2][];
        for (int k = 0; k < 2; k++) {
            // 不买 x，转移来源为 subF[0]，因为对于子树来说，父节点一定不买
            f[k] = subF[0].clone();
            int cost = present[x] / (k + 1);
            // 买 x，转移来源为 subF[1]，因为对于子树来说，父节点一定买
            for (int j = cost; j <= budget; j++) {
                f[k][j] = Math.max(f[k][j], subF[1][j - cost] + future[x] - cost);
            }
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(int n, vector<int>& present, vector<int>& future, vector<vector<int>>& hierarchy, int budget) {
        vector<vector<int>> g(n);
        for (auto& e : hierarchy) {
            g[e[0] - 1].push_back(e[1] - 1);
        }

        auto dfs = [&](this auto&& dfs, int x) -> array<vector<int>, 2> {
            // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
            vector<int> sub_f[2]{vector<int>(budget + 1, INT_MIN / 2), vector<int>(budget + 1, INT_MIN / 2)};
            sub_f[0][0] = sub_f[1][0] = 0;
            for (int y : g[x]) {
                auto fy = dfs(y);
                for (int k = 0; k < 2; k++) {
                    vector<int> nf(budget + 1, INT_MIN / 2);
                    nf[0] = 0;
                    for (int jy = 0; jy <= budget; jy++) {
                        int res_y = fy[k][jy];
                        if (res_y < 0) { // 重要优化：物品价值为负数，一定不选
                            continue;
                        }
                        for (int j = jy; j <= budget; j++) {
                            nf[j] = max(nf[j], sub_f[k][j - jy] + res_y);
                        }
                    }
                    sub_f[k] = move(nf);
                }
            }

            array<vector<int>, 2> f;
            for (int k = 0; k < 2; k++) {
                // 不买 x，转移来源为 sub_f[0]，因为对于子树来说，父节点一定不买
                f[k] = sub_f[0];
                int cost = present[x] / (k + 1);
                // 买 x，转移来源为 sub_f[1]，因为对于子树来说，父节点一定买
                for (int j = cost; j <= budget; j++) {
                    f[k][j] = max(f[k][j], sub_f[1][j - cost] + future[x] - cost);
                }
            }
            return f;
        };

        return ranges::max(dfs(0)[0]);
    }
};
```

```go [sol-Go]
func maxProfit(n int, present []int, future []int, hierarchy [][]int, budget int) int {
	g := make([][]int, n)
	for _, e := range hierarchy {
		x, y := e[0]-1, e[1]-1
		g[x] = append(g[x], y)
	}

	var dfs func(int) [2][]int
	dfs = func(x int) [2][]int {
		// 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
		subF := [2][]int{make([]int, budget+1), make([]int, budget+1)}
		for i := 1; i <= budget; i++ {
			subF[0][i] = math.MinInt / 2 // 表示不存在对应的花费总和
			subF[1][i] = math.MinInt / 2
		}
		for _, y := range g[x] {
			fy := dfs(y)
			for k, fyk := range fy {
				nf := make([]int, budget+1)
				for i := 1; i <= budget; i++ {
					nf[i] = math.MinInt / 2
				}
				for jy, resY := range fyk {
					if resY < 0 { // 重要优化：物品价值为负数，一定不选
						continue
					}
					for j := jy; j <= budget; j++ {
						nf[j] = max(nf[j], subF[k][j-jy]+resY)
					}
				}
				subF[k] = nf
			}
		}

		f := [2][]int{}
		for k := range 2 {
			// 不买 x，转移来源为 subF[0]，因为对于子树来说，父节点一定不买
			f[k] = slices.Clone(subF[0])
			cost := present[x] / (k + 1)
			// 买 x，转移来源为 subF[1]，因为对于子树来说，父节点一定买
			for j := cost; j <= budget; j++ {
				f[k][j] = max(f[k][j], subF[1][j-cost]+future[x]-cost)
			}
		}
		return f
	}

	return slices.Max(dfs(0)[0])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot \textit{budget}^2)$。有 $n-1$ 条边，每条边计算一次 $\mathcal{O}(\textit{budget}^2)$ 的转移。
- 空间复杂度：$\mathcal{O}(h\cdot \textit{budget})$，其中 $h$ 是树的高度。在随机数据下，$h=\Theta(\sqrt n)$，这个做法比在 DFS 外面创建数组更好。

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

## 前置知识

请确认你已经掌握如下知识点：

1. **0-1 背包**，包括**空间优化**的原理。请看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。
2. **状态机 DP**。请看[【基础算法精讲 21】](https://www.bilibili.com/video/BV1ho4y1W7QK/)。

## 寻找子问题

站在节点 $x$ 上，讨论是否购买 $\textit{present}[x]$。设节点 $y$ 是节点 $x$ 的儿子。

- 如果不买 $\textit{present}[x]$，且预算至多为 $j$，那么问题变成：
   - 从 $x$ 的所有子树 $y$ 中能得到的最大利润之和。
   - 所有子树 $y$ 的花费总和必须 $\le j$。
   - $\textit{present}[y]$ 不能半价购买。
- 如果买 $\textit{present}[x]$，且预算至多为 $j$，那么问题变成：
   - 从 $x$ 的所有子树 $y$ 中能得到的最大利润之和，加上买 $\textit{present}[x]$ 得到的利润。
   - 买 $\textit{present}[x]$ 花了 $\textit{cost}$，其中 $\textit{cost}$ 等于 $\textit{present}[x]$（原价买股票）或者 $\left\lfloor\dfrac{\textit{present}[x]}{2}\right\rfloor$（半价买股票），取决于 $x$ 的父节点有没有买股票。
   - 所有子树 $y$ 的花费总和必须 $\le j-\textit{cost}$。
   - $\textit{present}[y]$ 可以半价购买。

## 状态设计和状态转移

$\textit{dfs}(x)$ 返回一个长为 $(\textit{budget}+1)\times 2$ 的二维数组 $f$，其中 $f[j][k]$ 表示：

- 从子树 $x$ 中能得到的最大利润之和。
- 预算为 $j$，即花费总和 $\le j$。
- $k=0$ 表示 $\textit{present}[x]$ 不能半价购买，$k=1$ 表示 $\textit{present}[x]$ 可以半价购买。

首先，计算 $x$ 的所有儿子子树 $y$ 的最大利润总和 $\textit{subF}[j][k]$。枚举 $x$ 的儿子 $y$：

- 枚举分配给当前儿子 $y$ 的预算 $j_y = 0,1,2,\ldots,j$，那么分配给前面遍历过的儿子的总预算为 $j-j_y$。
- 用前面遍历过的儿子的收益 $\textit{subF}[j-j_y][k]$ 加上当前儿子 $y$ 的收益 $\textit{dfs}(y)[j_y][k]$，更新 $\textit{subF}[j][k]$ 的最大值。注意这里用了 0-1 背包的空间优化。

然后，考虑 $\textit{present}[x]$ 是否购买，计算 $f[j][k]$：

- 不买 $\textit{present}[x]$，那么分配给儿子子树的预算不变，仍然为 $j$，即 $f[j][k] = \textit{subF}[j][0]$，这里的 $0$ 是因为对于子树 $y$ 来说，父节点 $x$ 一定不买。
- 买 $\textit{present}[x]$，那么分配给儿子子树的预算要扣掉 $\textit{cost}$，即 $f[j][k] = \textit{subF}[j-\textit{cost}][1]$，这里的 $1$ 是因为对于子树 $y$ 来说，父节点 $x$ 一定买。

两种情况取最大值，得

$$
f[j][k] = \max(\textit{subF}[j][0], \textit{subF}[j-\textit{cost}][1] + \textit{future}[x] - \textit{cost})
$$

最终答案为根节点的 $f[\textit{budget}][0]$，这里的 $0$ 是因为根节点没有父节点。

[本题视频讲解](https://www.bilibili.com/video/BV1o1jgzJE51/?t=7m52s)，从特殊到一般，带你一步步思考。

## 写法一：至多

```py [sol-Python3]
# 注意！这个写法很慢，更快的写法见写法二
max = lambda a, b: b if b > a else a

class Solution:
    def maxProfit(self, n: int, present: List[int], future: List[int], hierarchy: List[List[int]], budget: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in hierarchy:
            g[x - 1].append(y - 1)

        def dfs(x: int) -> List[List[int]]:
            # 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
            sub_f = [[0, 0] for _ in range(budget + 1)]
            for y in g[x]:
                fy = dfs(y)
                for j in range(budget, -1, -1):
                    # 枚举子树 y 的预算为 jy
                    # 当作一个体积为 jy，价值为 fy[jy][k] 的物品
                    for jy in range(j + 1):  
                        for k in range(2):  # k=0 表示 x 不买，k=1 表示 x 买
                            sub_f[j][k] = max(sub_f[j][k], sub_f[j - jy][k] + fy[jy][k])

            # 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
            f = [[0, 0] for _ in range(budget + 1)]
            for j in range(budget + 1):
                for k in range(2):  # k=0 表示 x 父节点不买，k=1 表示 x 父节点买
                    cost = present[x] // (k + 1)
                    if j >= cost:
                        # 不买 x，转移来源是 sub_f[j][0]
                        # 买 x，转移来源为 sub_f[j-cost][1]，因为对于子树来说，父节点一定买
                        f[j][k] = max(sub_f[j][0], sub_f[j - cost][1] + future[x] - cost)
                    else:  # 只能不买 x
                        f[j][k] = sub_f[j][0]
            return f

        return dfs(0)[budget][0]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[] present, int[] future, int[][] hierarchy, int budget) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : hierarchy) {
            g[e[0] - 1].add(e[1] - 1);
        }

        int[][] f0 = dfs(0, g, present, future, budget);
        return f0[budget][0];
    }

    private int[][] dfs(int x, List<Integer>[] g, int[] present, int[] future, int budget) {
        // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
        int[][] subF = new int[budget + 1][2];
        for (int y : g[x]) {
            int[][] fy = dfs(y, g, present, future, budget);
            for (int j = budget; j >= 0; j--) {
                // 枚举子树 y 的预算为 jy
                // 当作一个体积为 jy，价值为 fy[jy][k] 的物品
                for (int jy = 0; jy <= j; jy++) {
                    for (int k = 0; k < 2; k++) { // k=0 表示 x 不买，k=1 表示 x 买
                        subF[j][k] = Math.max(subF[j][k], subF[j - jy][k] + fy[jy][k]);
                    }
                }
            }
        }

        // 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
        int[][] f = new int[budget + 1][2];
        for (int j = 0; j <= budget; j++) {
            for (int k = 0; k < 2; k++) { // k=0 表示 x 父节点不买，k=1 表示 x 父节点买
                int cost = present[x] / (k + 1);
                if (j >= cost) {
                    // 不买 x，转移来源是 subF[j][0]
                    // 买 x，转移来源为 subF[j-cost][1]，因为对于子树来说，父节点一定买
                    f[j][k] = Math.max(subF[j][0], subF[j - cost][1] + future[x] - cost);
                } else { // 只能不买 x
                    f[j][k] = subF[j][0];
                }
            }
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(int n, vector<int>& present, vector<int>& future, vector<vector<int>>& hierarchy, int budget) {
        vector<vector<int>> g(n);
        for (auto& e : hierarchy) {
            g[e[0] - 1].push_back(e[1] - 1);
        }

        auto dfs = [&](this auto&& dfs, int x) -> vector<array<int, 2>> {
            // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
            vector<array<int, 2>> sub_f(budget + 1);
            for (int y : g[x]) {
                auto fy = dfs(y);
                for (int j = budget; j >= 0; j--) {
                    // 枚举子树 y 的预算为 jy
                    // 当作一个体积为 jy，价值为 fy[jy][k] 的物品
                    for (int jy = 0; jy <= j; jy++) {
                        for (int k = 0; k < 2; k++) { // k=0 表示 x 不买，k=1 表示 x 买
                            sub_f[j][k] = max(sub_f[j][k], sub_f[j - jy][k] + fy[jy][k]);
                        }
                    }
                }
            }

            // 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
            vector<array<int, 2>> f(budget + 1);
            for (int j = 0; j <= budget; j++) {
                for (int k = 0; k < 2; k++) { // k=0 表示 x 父节点不买，k=1 表示 x 父节点买
                    int cost = present[x] / (k + 1);
                    if (j >= cost) {
                        // 不买 x，转移来源是 sub_f[j][0]
                        // 买 x，转移来源为 sub_f[j-cost][1]，因为对于子树来说，父节点一定买
                        f[j][k] = max(sub_f[j][0], sub_f[j - cost][1] + future[x] - cost);
                    } else { // 只能不买 x
                        f[j][k] = sub_f[j][0];
                    }
                }
            }
            return f;
        };

        return dfs(0)[budget][0];
    }
};
```

```go [sol-Go]
func maxProfit(n int, present []int, future []int, hierarchy [][]int, budget int) int {
	g := make([][]int, n)
	for _, e := range hierarchy {
		x, y := e[0]-1, e[1]-1
		g[x] = append(g[x], y)
	}

	var dfs func(int) [][2]int
	dfs = func(x int) [][2]int {
		// 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和（x 不买，x 买）
		subF := make([][2]int, budget+1)
		for _, y := range g[x] {
			fy := dfs(y)
			for j := budget; j >= 0; j-- {
				// 枚举子树 y 的预算为 jy
				// 当作一个体积为 jy，价值为 resY=fy[jy][k] 的物品
				for jy, p := range fy[:j+1] {
					for k, resY := range p { // k=0 表示 x 不买，k=1 表示 x 买
						subF[j][k] = max(subF[j][k], subF[j-jy][k]+resY)
					}
				}
			}
		}

		// 计算从子树 x 中，能得到的最大利润之和（x 父节点不买，x 父节点买）
		f := make([][2]int, budget+1)
		for j, p := range subF {
			for k := range 2 { // k=0 表示 x 父节点不买，k=1 表示 x 父节点买
				cost := present[x] / (k + 1)
				if j >= cost {
					// 不买 x，转移来源是 subF[j][0]
					// 买 x，转移来源为 subF[j-cost][1]，因为对于子树来说，父节点一定买
					f[j][k] = max(p[0], subF[j-cost][1]+future[x]-cost)
				} else { // 只能不买 x
					f[j][k] = p[0]
				}
			}
		}
		return f
	}

	return dfs(0)[budget][0]
}
```

## 写法二：恰好

把状态值改成在总花费**恰好**为 $j$ 的情况下的最大利润。

优化：交换 $f$ 数组的维度，改成两个长为 $\textit{budget}+1$ 的数组。

```py [sol-Python3 列表]
# 更快的写法见【Python3 字典】
fmax = lambda a, b: b if b > a else a

class Solution:
    def maxProfit(self, n: int, present: List[int], future: List[int], hierarchy: List[List[int]], budget: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in hierarchy:
            g[x - 1].append(y - 1)

        def dfs(x: int) -> List[List[int]]:
            # 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
            sub_f = [[0] + [-inf] * budget for _ in range(2)]
            for y in g[x]:
                fy = dfs(y)
                for k, fyk in enumerate(fy):
                    nf = [0] + [-inf] * budget
                    for jy, res_y in enumerate(fyk):
                        if res_y < 0:  # 重要优化：物品价值为负数，一定不选
                            continue
                        for j in range(jy, budget + 1):
                            nf[j] = fmax(nf[j], sub_f[k][j - jy] + res_y)
                    sub_f[k] = nf

            f = [None] * 2
            for k in range(2):
                # 不买 x，转移来源为 sub_f[0]，因为对于子树来说，父节点一定不买
                f[k] = sub_f[0].copy()
                cost = present[x] // (k + 1)
                # 买 x，转移来源为 sub_f[1]，因为对于子树来说，父节点一定买
                for j in range(cost, budget + 1):
                    f[k][j] = fmax(f[k][j], sub_f[1][j - cost] + future[x] - cost)
            return f

        return max(dfs(0)[0])
```

```py [sol-Python3 字典]
fmax = lambda a, b: b if b > a else a

class Solution:
    def maxProfit(self, n: int, present: List[int], future: List[int], hierarchy: List[List[int]], budget: int) -> int:
        g = [[] for _ in range(n)]
        for x, y in hierarchy:
            g[x - 1].append(y - 1)

        def dfs(x: int) -> List[Dict[int, int]]:
            sub_f = [defaultdict(int) for _ in range(2)]
            sub_f[0][0] = sub_f[1][0] = 0
            for y in g[x]:
                fy = dfs(y)
                for k, fyk in enumerate(fy):
                    nf = defaultdict(int)
                    for j, pre_res_y in sub_f[k].items():
                        for jy, res_y in fyk.items():
                            sj = j + jy
                            if sj <= budget:
                                nf[sj] = fmax(nf[sj], pre_res_y + res_y)
                    sub_f[k] = nf

            f = [None] * 2
            for k in range(2):
                res = sub_f[0].copy()
                cost = present[x] // (k + 1)
                if cost <= budget:
                    earn = future[x] - cost
                    for j, res_y in sub_f[1].items():
                        sj = j + cost
                        if sj <= budget:
                            res[sj] = fmax(res[sj], res_y + earn)
                f[k] = res
            return f

        return max(dfs(0)[0].values())
```

```java [sol-Java]
class Solution {
    public int maxProfit(int n, int[] present, int[] future, int[][] hierarchy, int budget) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : hierarchy) {
            g[e[0] - 1].add(e[1] - 1);
        }

        int[][] f0 = dfs(0, g, present, future, budget);
        return Arrays.stream(f0[0]).max().getAsInt();
    }

    private int[][] dfs(int x, List<Integer>[] g, int[] present, int[] future, int budget) {
        // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
        int[][] subF = new int[2][budget + 1];
        Arrays.fill(subF[0], Integer.MIN_VALUE / 2); // 表示不存在对应的花费总和
        Arrays.fill(subF[1], Integer.MIN_VALUE / 2);
        subF[0][0] = subF[1][0] = 0;
        for (int y : g[x]) {
            int[][] fy = dfs(y, g, present, future, budget);
            for (int k = 0; k < 2; k++) {
                int[] nf = new int[budget + 1];
                Arrays.fill(nf, Integer.MIN_VALUE / 2);
                nf[0] = 0;
                for (int jy = 0; jy <= budget; jy++) {
                    int resY = fy[k][jy];
                    if (resY < 0) { // 重要优化：物品价值为负数，一定不选
                        continue;
                    }
                    for (int j = jy; j <= budget; j++) {
                        nf[j] = Math.max(nf[j], subF[k][j - jy] + resY);
                    }
                }
                subF[k] = nf;
            }
        }

        int[][] f = new int[2][];
        for (int k = 0; k < 2; k++) {
            // 不买 x，转移来源为 subF[0]，因为对于子树来说，父节点一定不买
            f[k] = subF[0].clone();
            int cost = present[x] / (k + 1);
            // 买 x，转移来源为 subF[1]，因为对于子树来说，父节点一定买
            for (int j = cost; j <= budget; j++) {
                f[k][j] = Math.max(f[k][j], subF[1][j - cost] + future[x] - cost);
            }
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(int n, vector<int>& present, vector<int>& future, vector<vector<int>>& hierarchy, int budget) {
        vector<vector<int>> g(n);
        for (auto& e : hierarchy) {
            g[e[0] - 1].push_back(e[1] - 1);
        }

        auto dfs = [&](this auto&& dfs, int x) -> array<vector<int>, 2> {
            // 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
            vector<int> sub_f[2]{vector<int>(budget + 1, INT_MIN / 2), vector<int>(budget + 1, INT_MIN / 2)};
            sub_f[0][0] = sub_f[1][0] = 0;
            for (int y : g[x]) {
                auto fy = dfs(y);
                for (int k = 0; k < 2; k++) {
                    vector<int> nf(budget + 1, INT_MIN / 2);
                    nf[0] = 0;
                    for (int jy = 0; jy <= budget; jy++) {
                        int res_y = fy[k][jy];
                        if (res_y < 0) { // 重要优化：物品价值为负数，一定不选
                            continue;
                        }
                        for (int j = jy; j <= budget; j++) {
                            nf[j] = max(nf[j], sub_f[k][j - jy] + res_y);
                        }
                    }
                    sub_f[k] = move(nf);
                }
            }

            array<vector<int>, 2> f;
            for (int k = 0; k < 2; k++) {
                // 不买 x，转移来源为 sub_f[0]，因为对于子树来说，父节点一定不买
                f[k] = sub_f[0];
                int cost = present[x] / (k + 1);
                // 买 x，转移来源为 sub_f[1]，因为对于子树来说，父节点一定买
                for (int j = cost; j <= budget; j++) {
                    f[k][j] = max(f[k][j], sub_f[1][j - cost] + future[x] - cost);
                }
            }
            return f;
        };

        return ranges::max(dfs(0)[0]);
    }
};
```

```go [sol-Go]
func maxProfit(n int, present []int, future []int, hierarchy [][]int, budget int) int {
	g := make([][]int, n)
	for _, e := range hierarchy {
		x, y := e[0]-1, e[1]-1
		g[x] = append(g[x], y)
	}

	var dfs func(int) [2][]int
	dfs = func(x int) [2][]int {
		// 计算从 x 的所有儿子子树 y 中，能得到的最大利润之和
		subF := [2][]int{make([]int, budget+1), make([]int, budget+1)}
		for i := 1; i <= budget; i++ {
			subF[0][i] = math.MinInt / 2 // 表示不存在对应的花费总和
			subF[1][i] = math.MinInt / 2
		}
		for _, y := range g[x] {
			fy := dfs(y)
			for k, fyk := range fy {
				nf := make([]int, budget+1)
				for i := 1; i <= budget; i++ {
					nf[i] = math.MinInt / 2
				}
				for jy, resY := range fyk {
					if resY < 0 { // 重要优化：物品价值为负数，一定不选
						continue
					}
					for j := jy; j <= budget; j++ {
						nf[j] = max(nf[j], subF[k][j-jy]+resY)
					}
				}
				subF[k] = nf
			}
		}

		f := [2][]int{}
		for k := range 2 {
			// 不买 x，转移来源为 subF[0]，因为对于子树来说，父节点一定不买
			f[k] = slices.Clone(subF[0])
			cost := present[x] / (k + 1)
			// 买 x，转移来源为 subF[1]，因为对于子树来说，父节点一定买
			for j := cost; j <= budget; j++ {
				f[k][j] = max(f[k][j], subF[1][j-cost]+future[x]-cost)
			}
		}
		return f
	}

	return slices.Max(dfs(0)[0])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot \textit{budget}^2)$。有 $n-1$ 条边，每条边计算一次 $\mathcal{O}(\textit{budget}^2)$ 的转移。
- 空间复杂度：$\mathcal{O}(h\cdot \textit{budget})$，其中 $h$ 是树的高度。在随机数据下，$h=\Theta(\sqrt n)$，这个做法比在 DFS 外面创建数组更好。

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

本题来自 `三、背包 / §3.5 树上背包（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.5 树上背包（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
