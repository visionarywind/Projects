# 10. 正则表达式匹配

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/regular-expression-matching/
- 题目 slug：`regular-expression-matching`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/regular-expression-matching/solutions/902845/go-yi-xing-by-endlesscheng-1vf3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：记忆化搜索 -> 递推 -> 空间优化（Python/Java/C++/Go）](https://leetcode.cn/problems/regular-expression-matching/solutions/902845/go-yi-xing-by-endlesscheng-1vf3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-yi-xing-by-endlesscheng-1vf3`
- topic id：`902845`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 补充两个例子

除了题目给的三个示例，我再补充两个例子，加深你对匹配规则的理解。

例一。$s = \texttt{"abbb"}$，$p=\texttt{"a*b*c*d*"}$。

- $\texttt{a*}$ 匹配 $s$ 中的 $\texttt{a}$。
- $\texttt{b*}$ 匹配 $s$ 中的 $\texttt{bbb}$。
- $\texttt{c*}$ 和 $\texttt{d*}$ 都匹配零个字符。
- 匹配成功。

例二。$s = \texttt{"a"}$，$p=\texttt{".*.."}$。

- 虽然 $\texttt{.*}$ 可以匹配**任意多个任意字符**，但后面的 $\texttt{..}$ 意味着 $s$ 至少要有两个字符，所以无法匹配。

## 一、寻找子问题

从左往右思考。第一个字符 $s[0]$ 怎么匹配？

你肯定会去看 $p[0]$ 的值。但注意，对于正则表达式来说，如果 $p[1] = \texttt{*}$，那么 $p[0]$ 和 $p[1]$ 就是一个**整体**，不能单独把 $p[0]$ 拿出来去和 $s[0]$ 匹配。

所以我们首先得看 $p[1]$ 是不是 $\texttt{*}$：

- 如果 $p[1] \ne \texttt{*}$。
  - 如果 $p[0] = \texttt{.}$ 或者 $p[0] = s[0]$，那么匹配成功，下一步考虑 $s$ 的后缀 $[1,n-1]$ 能否与 $p$ 的后缀 $[1,m-1]$ 匹配。
  - 否则，匹配失败，返回 $\texttt{false}$。
- 如果 $p[1] = \texttt{*}$，那么 $p[0]$ 和 $p[1]$ 是一个整体。
  - 情况一，匹配零个字符，下一步考虑 $s$ 的 $[0,n-1]$ 能否与 $p$ 的 $[2,m-1]$ 匹配。
  - 情况二，我们可以先匹配一个字符（这要求 $p[0] = \texttt{.}$ 或者 $p[0] = s[0]$），下一步考虑 $s$ 的 $[1,n-1]$ 能否与 $p$ 的 $[0,m-1]$ 匹配。至于 $s[1]$ 是否要与 $p[0]$ 和 $p[1]$ 匹配，那是下一步要做的事情（这类似完全背包）。

由于下一步考虑的问题都是**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i,j)$ 表示 $s$ 的后缀 $[i,n-1]$ 能否与 $p$ 的后缀 $[j,m-1]$ 匹配。其中 $n$ 是 $s$ 的长度，$m$ 是 $p$ 的长度。

- 如果 $p[j+1] \ne \texttt{*}$（或者 $j=m-1$）。
    - 如果 $p[j] = \texttt{.}$ 或者 $p[j] = s[i]$，那么匹配成功，下一步考虑 $s$ 的后缀 $[i+1,n-1]$ 能否与 $p$ 的后缀 $[j+1,m-1]$ 匹配，即 $\textit{dfs}(i+1,j+1)$。
    - 否则，匹配失败，返回 $\texttt{false}$。
- 如果 $p[j+1] = \texttt{*}$，那么 $p[j]$ 和 $p[j+1]$ 是一个整体。
    - 情况一，匹配零个字符，下一步考虑 $s$ 的 $[i,n-1]$ 能否与 $p$ 的 $[j+2,m-1]$ 匹配，即 $\textit{dfs}(i,j+2)$。
    - 情况二，我们可以先匹配一个字符（这要求 $p[j] = \texttt{.}$ 或者 $p[j] = s[i]$），下一步考虑 $s$ 的 $[i+1,n-1]$ 能否与 $p$ 的 $[j,m-1]$ 匹配，即 $\textit{dfs}(i+1,j)$。至于 $s[i+1]$ 是否要与 $p[j]$ 和 $p[j+1]$ 匹配，那是下一步要做的事情（这类似完全背包）。

定义 $\textit{isMatch}(i,j)$ 表示 $p[j] = \texttt{.}$ 或者 $p[j] = s[i]$ 是否成立。特别地，$\textit{isMatch}(n,j)=\texttt{false}$。

状态转移方程为：

$$
\textit{dfs}(i,j) =
\begin{cases}
\textit{isMatch}(i,j) \land \textit{dfs}(i+1,j+1), & p[j+1] \ne \texttt{*}     \\
\textit{dfs}(i,j+2) \lor (\textit{isMatch}(i,j) \land \textit{dfs}(i+1,j)), & p[j+1] = \texttt{*}     \\
\end{cases}
$$

**递归边界**：$\textit{dfs}(n,m)=\texttt{true}$，其余 $\textit{dfs}(i,m)=\texttt{false}$。如果 $p$ 匹配完了，$s$ 还有剩余字符，则匹配失败。

**递归入口**：$\textit{dfs}(0,0)$，这是原问题，即 $s$ 能否与 $p$ 匹配。

> 注：题目保证 $p$ 是合法的正则表达式，不会包含 $\texttt{**}$，也不会出现 $p[0] = \texttt{*}$ 的情况。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

由于本题递归函数的返回值只有 $\texttt{false}$ 和 $\texttt{true}$，且一旦我们在递归边界返回了 $\texttt{true}$，那么这个 $\texttt{true}$ 会一路往上返回到递归入口。所以只需要用二维布尔数组标记每个状态是否访问过，如果访问了一个之前访问过的状态，这意味着之前计算这个状态的时候，并没有返回 $\texttt{true}$，那么再算一遍也不会返回 $\texttt{true}$，所以直接返回 $\texttt{false}$ 即可。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

关于记忆化搜索的原理，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

#### 答疑

**问**：为什么代码没有把 $i=n$ 作为递归边界？

**答**：如果 $i=n$，但 $j<m$，我们还需要继续判断 $p$ 的剩余部分能否匹配**空字符串**。例如 $p$ 的剩余部分是 $\texttt{c*d*}$，可以匹配空字符串。

```py [sol-Python3]
class Solution:
    def isMatch(self, s: str, p: str) -> bool:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> bool:
            if j == len(p):
                return i == len(s)

            is_match = i < len(s) and (p[j] == '.' or p[j] == s[i])

            # 匹配零个或多个
            if j + 1 < len(p) and p[j + 1] == '*':  # p[j] 和 p[j+1] 是一个整体
                return (dfs(i, j + 2) or  # 匹配零个
                        is_match and dfs(i + 1, j))  # 匹配一个，交给 dfs(i+1, j) 继续匹配

            # s[i] 与 p[j] 匹配
            return is_match and dfs(i + 1, j + 1)

        return dfs(0, 0)
```

```java [sol-Java]
class Solution {
    public boolean isMatch(String s, String p) {
        boolean[][] vis = new boolean[s.length() + 1][p.length() + 1];
        return dfs(0, 0, s.toCharArray(), p.toCharArray(), vis);
    }

    private boolean dfs(int i, int j, char[] s, char[] p, boolean[][] vis) {
        int n = s.length;
        int m = p.length;

        if (j == m) {
            return i == n;
        }

        if (vis[i][j]) {
            return false;
        }
        vis[i][j] = true;

        boolean isMatch = i < n && (p[j] == '.' || p[j] == s[i]);

        // 匹配零个或多个
        if (j + 1 < m && p[j + 1] == '*') { // p[j] 和 p[j+1] 是一个整体
            return dfs(i, j + 2, s, p, vis) || // 匹配零个
                   isMatch && dfs(i + 1, j, s, p, vis); // 匹配一个，交给 dfs(i+1, j) 继续匹配
        }

        // s[i] 与 p[j] 匹配
        return isMatch && dfs(i + 1, j + 1, s, p, vis);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isMatch(string s, string p) {
        int n = s.size(), m = p.size();
        vector vis(n + 1, vector<int8_t>(m + 1));

        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (j == m) {
                return i == n;
            }

            if (vis[i][j]) {
                return false;
            }
            vis[i][j] = true;

            bool is_match = i < n && (p[j] == '.' || p[j] == s[i]);

            // 匹配零个或多个
            if (j + 1 < m && p[j + 1] == '*') { // p[j] 和 p[j+1] 是一个整体
                return dfs(i, j + 2) || // 匹配零个
                       is_match && dfs(i + 1, j); // 匹配一个，交给 dfs(i+1, j) 继续匹配
            }

            // s[i] 与 p[j] 匹配
            return is_match && dfs(i + 1, j + 1);
        };

        return dfs(0, 0);
    }
};
```

```go [sol-Go]
func isMatch(s, p string) bool {
	n, m := len(s), len(p)
	vis := make([][]bool, n+1)
	for i := range vis {
		vis[i] = make([]bool, m+1)
	}

	var dfs func(int, int) bool
	dfs = func(i, j int) bool {
		if j == m {
			return i == n
		}

		if vis[i][j] {
			return false
		}
		vis[i][j] = true

		isMatch := i < n && (p[j] == '.' || p[j] == s[i])

		// 匹配零个或多个
		if j+1 < m && p[j+1] == '*' { // p[j] 和 p[j+1] 是一个整体
			return dfs(i, j+2) || // 匹配零个
				isMatch && dfs(i+1, j) // 匹配一个，交给 dfs(i+1, j) 继续匹配
		}

		// s[i] 与 p[j] 匹配
		return isMatch && dfs(i+1, j+1)
	}

	return dfs(0, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s$ 的长度，$m$ 是 $p$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nm)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。有多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示 $s$ 的后缀 $[i,n-1]$ 能否与 $p$ 的后缀 $[j,m-1]$ 匹配。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] =
\begin{cases}
\textit{isMatch}(i,j) \land f[i+1][j+1], & p[j+1] \ne \texttt{*}     \\
f[i][j+2] \lor (\textit{isMatch}(i,j) \land f[i+1][j]), & p[j+1] = \texttt{*}     \\
\end{cases}
$$

初始值 $f[n][m]=\texttt{true}$，翻译自递归边界 $\textit{dfs}(n,m)=\texttt{true}$。其余为 $\texttt{false}$。

答案为 $f[0][0]$，翻译自递归入口 $\textit{dfs}(0,0)$。

由于计算 $f[i][j]$ 依赖下标更大的状态 $f[i+1][j+1],f[i][j+2],f[i+1][j]$，所以 $i$ 和 $j$ 都需要**倒序循环**。

```py [sol-Python3]
class Solution:
    def isMatch(self, s: str, p: str) -> bool:
        n, m = len(s), len(p)
        f = [[False] * (m + 1) for _ in range(n + 1)]
        f[n][m] = True

        for i in range(n, -1, -1):
            for j in range(m - 1, -1, -1):
                if p[j] == '*':
                    continue
                is_match = i < n and (p[j] == '.' or p[j] == s[i])
                if j + 1 < m and p[j + 1] == '*':
                    f[i][j] = f[i][j + 2] or is_match and f[i + 1][j]
                else:
                    f[i][j] = is_match and f[i + 1][j + 1]

        return f[0][0]
```

```java [sol-Java]
class Solution {
    public boolean isMatch(String S, String P) {
        char[] s = S.toCharArray();
        char[] p = P.toCharArray();
        int n = s.length;
        int m = p.length;

        boolean[][] f = new boolean[n + 1][m + 1];
        f[n][m] = true;

        for (int i = n; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                if (p[j] == '*') {
                    continue;
                }
                boolean isMatch = i < n && (p[j] == '.' || p[j] == s[i]);
                if (j + 1 < m && p[j + 1] == '*') {
                    f[i][j] = f[i][j + 2] || isMatch && f[i + 1][j];
                } else {
                    f[i][j] = isMatch && f[i + 1][j + 1];
                }
            }
        }

        return f[0][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isMatch(string s, string p) {
        int n = s.size(), m = p.size();
        vector f(n + 1, vector<int8_t>(m + 1));
        f[n][m] = true;

        for (int i = n; i >= 0; i--) {
            for (int j = m - 1; j >= 0; j--) {
                if (p[j] == '*') {
                    continue;
                }
                bool is_match = i < n && (p[j] == '.' || p[j] == s[i]);
                if (j + 1 < m && p[j + 1] == '*') {
                    f[i][j] = f[i][j + 2] || is_match && f[i + 1][j];
                } else {
                    f[i][j] = is_match && f[i + 1][j + 1];
                }
            }
        }

        return f[0][0];
    }
};
```

```go [sol-Go]
func isMatch(s, p string) bool {
	n, m := len(s), len(p)
	f := make([][]bool, n+1)
	for i := range f {
		f[i] = make([]bool, m+1)
	}
	f[n][m] = true

	for i := n; i >= 0; i-- {
		for j := m - 1; j >= 0; j-- {
			if p[j] == '*' {
				continue
			}
			isMatch := i < n && (p[j] == '.' || p[j] == s[i])
			if j+1 < m && p[j+1] == '*' {
				f[i][j] = f[i][j+2] || isMatch && f[i+1][j]
			} else {
				f[i][j] = isMatch && f[i+1][j+1]
			}
		}
	}

	return f[0][0]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s$ 的长度，$m$ 是 $p$ 的长度。
- 空间复杂度：$\mathcal{O}(nm)$。

## 五、空间优化

请先熟练掌握 0-1 背包和完全背包的空间优化，再来学习这一节。

观察上面的状态转移方程，在计算 $f[i]$ 时，只会用到 $f[i+1]$，不会用到下标大于 $i+1$ 的状态。

所以可以去掉 $f$ 的第一个维度，把 $f[i+1]$ 和 $f[i]$ 保存到**同一个数组**中。

```py [sol-Python3]
class Solution:
    def isMatch(self, s: str, p: str) -> bool:
        n, m = len(s), len(p)
        f = [False] * m + [True]

        for i in range(n, -1, -1):
            pre = f[m]
            f[m] = i == n
            for j in range(m - 1, -1, -1):
                if p[j] == '*':
                    continue
                is_match = i < n and (p[j] == '.' or p[j] == s[i])
                tmp = f[j]
                if j + 1 < m and p[j + 1] == '*':
                    f[j] = f[j + 2] or is_match and f[j]
                else:
                    f[j] = is_match and pre
                pre = tmp

        return f[0]
```

```java [sol-Java]
class Solution {
    public boolean isMatch(String S, String P) {
        char[] s = S.toCharArray();
        char[] p = P.toCharArray();
        int n = s.length;
        int m = p.length;

        boolean[] f = new boolean[m + 1];
        f[m] = true;

        for (int i = n; i >= 0; i--) {
            boolean pre = f[m];
            f[m] = i == n;
            for (int j = m - 1; j >= 0; j--) {
                if (p[j] == '*') {
                    continue;
                }
                boolean isMatch = i < n && (p[j] == '.' || p[j] == s[i]);
                boolean tmp = f[j];
                if (j + 1 < m && p[j + 1] == '*') {
                    f[j] = f[j + 2] || isMatch && f[j];
                } else {
                    f[j] = isMatch && pre;
                }
                pre = tmp;
            }
        }

        return f[0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isMatch(string s, string p) {
        int n = s.size(), m = p.size();
        vector<int8_t> f(m + 1);
        f[m] = true;

        for (int i = n; i >= 0; i--) {
            bool pre = f[m];
            f[m] = i == n;
            for (int j = m - 1; j >= 0; j--) {
                if (p[j] == '*') {
                    continue;
                }
                bool is_match = i < n && (p[j] == '.' || p[j] == s[i]);
                bool tmp = f[j];
                if (j + 1 < m && p[j + 1] == '*') {
                    f[j] = f[j + 2] || is_match && f[j];
                } else {
                    f[j] = is_match && pre;
                }
                pre = tmp;
            }
        }

        return f[0];
    }
};
```

```go [sol-Go]
func isMatch(s, p string) bool {
	n, m := len(s), len(p)
	f := make([]bool, m+1)
	f[m] = true

	for i := n; i >= 0; i-- {
		pre := f[m]
		f[m] = i == n
		for j := m - 1; j >= 0; j-- {
			if p[j] == '*' {
				continue
			}
			isMatch := i < n && (p[j] == '.' || p[j] == s[i])
			tmp := f[j]
			if j+1 < m && p[j+1] == '*' {
				f[j] = f[j+2] || isMatch && f[j]
			} else {
				f[j] = isMatch && pre
			}
			pre = tmp
		}
	}

	return f[0]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s$ 的长度，$m$ 是 $p$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 附：库函数写法

```go [sol-Go]
func isMatch(s, p string) bool {
	return regexp.MustCompile("^" + p + "$").MatchString(s)
}
```

## 专题训练

见下面动态规划题单的「**§4.1 最长公共子序列**」。

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

本题来自 `四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
