# 756. 金字塔转换矩阵

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/pyramid-transition-matrix/
- 题目 slug：`pyramid-transition-matrix`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.5 轮廓线 DP
- 难度分：1990
- 外部题解来源：https://leetcode.cn/problems/pyramid-transition-matrix/solutions/3866474/tu-jie-hui-su-vis-you-hua-pythonjavacgo-9kcb9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】回溯 + vis 优化 + 位运算优化（Python/Java/C++/Go）](https://leetcode.cn/problems/pyramid-transition-matrix/solutions/3866474/tu-jie-hui-su-vis-you-hua-pythonjavacgo-9kcb9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-hui-su-vis-you-hua-pythonjavacgo-9kcb9`
- topic id：`3866474`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

考虑枚举金字塔的每个格子填什么字母。

写一个类似 [17. 电话号码的字母组合](https://leetcode.cn/problems/letter-combinations-of-a-phone-number/) 的回溯（搜索）。按照从下往上，从左到右的顺序填字母。

![lc756-1.png](https://pic.leetcode.cn/1766717955-HzbaJF-lc756-1.png){:width=600px}

比如 $\textit{allowed}$ 中有 $\texttt{AAB}$ 和 $\texttt{AAC}$，那么当 $(i+1,j)$ 和 $(i+1,j+1)$ 都填 $\texttt{A}$ 的时候，$(i,j)$ 可以填 $\texttt{B}$，也可以填 $\texttt{C}$。**枚举填哪个**。

为了快速知道 $\texttt{AA}\to [\texttt{B}, \texttt{C}]$ 的对应关系，可以把 $\textit{allowed}$ 用哈希表（或者二维数组）分组，把 $\textit{allowed}[i]$ 前两个字母对应的第三个字母，记录在一个列表中。

## 优化前

```py [sol-Python3]
class Solution:
    def pyramidTransition(self, bottom: str, allowed: List[str]) -> bool:
        groups = defaultdict(list)  # 三角形底部两个字母 -> [三角形顶部字母]
        for s in allowed:
            groups[s[:2]].append(s[2])

        n = len(bottom)
        pyramid = [[''] * (i + 1) for i in range(n)]
        pyramid[-1] = bottom

        # 现在准备填 (i, j) 这个格子
        # 返回继续填能否填完所有格子（从下往上填，每行从左到右填）
        def dfs(i: int, j: int) -> bool:
            if i < 0:  # 所有格子都已填完
                return True

            if j == i + 1:  # i 行已填完
                return dfs(i - 1, 0)  # 开始填 i-1 行

            # 枚举 (i, j) 填什么字母
            # 这取决于 (i+1, j) 和 (i+1, j+1) 填的字母
            for top in groups[pyramid[i + 1][j] + pyramid[i + 1][j + 1]]:
                pyramid[i][j] = top
                if dfs(i, j + 1):
                    return True
            return False

        # 从倒数第二行开始填
        return dfs(n - 2, 0)
```

```java [sol-Java]
class Solution {
    public boolean pyramidTransition(String bottom, List<String> allowed) {
        // 三角形底部两个字母 -> [三角形顶部字母]
        List<Character>[][] groups = new ArrayList[6][6];
        for (List<Character>[] row : groups) {
            Arrays.setAll(row, _ -> new ArrayList<>());
        }
        for (String S : allowed) {
            char[] s = S.toCharArray();
            groups[s[0] - 'A'][s[1] - 'A'].add(s[2]);
        }

        int n = bottom.length();
        char[][] pyramid = new char[n][];
        for (int i = 0; i < n - 1; i++) {
            pyramid[i] = new char[i + 1];
        }
        pyramid[n - 1] = bottom.toCharArray();

        // 从倒数第二行开始填
        return dfs(n - 2, 0, pyramid, groups);
    }

    // 现在准备填 (i, j) 这个格子
    // 返回继续填能否填完所有格子（从下往上填，每行从左到右填）
    private boolean dfs(int i, int j, char[][] pyramid, List<Character>[][] groups) {
        if (i < 0) { // 所有格子都已填完
            return true;
        }

        if (j == i + 1) { // i 行已填完
            return dfs(i - 1, 0, pyramid, groups); // 开始填 i-1 行
        }

        // 枚举 (i, j) 填什么字母
        // 这取决于 (i+1, j) 和 (i+1, j+1) 填的字母
        for (char top : groups[pyramid[i + 1][j] - 'A'][pyramid[i + 1][j + 1] - 'A']) {
            pyramid[i][j] = top;
            if (dfs(i, j + 1, pyramid, groups)) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool pyramidTransition(string bottom, vector<string>& allowed) {
        string groups[6][6]{}; // 三角形底部两个字母 -> [三角形顶部字母]
        for (auto& s : allowed) {
            groups[s[0] - 'A'][s[1] - 'A'] += s[2];
        }

        int n = bottom.size();
        vector<string> pyramid(n);
        for (int i = 0; i < n - 1; i++) {
            pyramid[i].resize(i + 1);
        }
        pyramid[n - 1] = move(bottom);

        // 现在准备填 (i, j) 这个格子
        // 返回继续填能否填完所有格子（从下往上填，每行从左到右填）
        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (i < 0) { // 所有格子都已填完
                return true;
            }

            if (j == i + 1) { // i 行已填完
                return dfs(i - 1, 0); // 开始填 i-1 行
            }

            // 枚举 (i, j) 填什么字母
            // 这取决于 (i+1, j) 和 (i+1, j+1) 填的字母
            for (char top : groups[pyramid[i + 1][j] - 'A'][pyramid[i + 1][j + 1] - 'A']) {
                pyramid[i][j] = top;
                if (dfs(i, j + 1)) {
                    return true;
                }
            }
            return false;
        };

        // 从倒数第二行开始填
        return dfs(n - 2, 0);
    }
};
```

```go [sol-Go]
func pyramidTransition(bottom string, allowed []string) bool {
	groups := [6][6][]byte{} // 三角形底部两个字母 -> [三角形顶部字母]
	for _, s := range allowed {
		a, b := s[0]-'A', s[1]-'A'
		groups[a][b] = append(groups[a][b], s[2])
	}

	n := len(bottom)
	pyramid := make([][]byte, n)
	for i := range n - 1 {
		pyramid[i] = make([]byte, i+1)
	}
	pyramid[n-1] = []byte(bottom)

	// 现在准备填 (i, j) 这个格子
	// 返回继续填能否填完所有格子（从下往上填，每行从左到右填）
	var dfs func(int, int) bool
	dfs = func(i, j int) bool {
		if i < 0 { // 所有格子都已填完
			return true
		}

		if j == i+1 { // i 行已填完
			return dfs(i-1, 0) // 开始填 i-1 行
		}

		// 枚举 (i, j) 填什么字母
		// 这取决于 (i+1, j) 和 (i+1, j+1) 填的字母
		for _, top := range groups[pyramid[i+1][j]-'A'][pyramid[i+1][j+1]-'A'] {
			pyramid[i][j] = top
			if dfs(i, j+1) {
				return true
			}
		}
		return false
	}

	// 从倒数第二行开始填
	return dfs(n-2, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|\Sigma|^h)$，其中 $n$ 是 $\textit{bottom}$ 的长度，$h=1+2+\cdots+(n-1)=\dfrac{n(n-1)}{2}$，$|\Sigma|=6$ 是字符集合的大小。搜索树的高度为 $h$，每个节点至多有 $|\Sigma|$ 个儿子，所以搜索树有 $\mathcal{O}(|\Sigma|^h)$ 个节点，遍历这棵搜索树需要 $\mathcal{O}(|\Sigma|^h)$ 的时间。
- 空间复杂度：$\mathcal{O}(m + n^2)$，其中 $m$ 是 $\textit{allowed}$ 的长度。

## 优化一：减少重复搜索

![lc756-2c.png](https://pic.leetcode.cn/1766718143-nacsoi-lc756-2c.png){:width=600px}

```py [sol-Python3]
class Solution:
    def pyramidTransition(self, bottom: str, allowed: List[str]) -> bool:
        groups = defaultdict(list)
        for s in allowed:
            groups[s[:2]].append(s[2])

        n = len(bottom)
        pyramid = [[''] * (i + 1) for i in range(n)]
        pyramid[-1] = bottom

        vis = set()  # 访问标记

        def dfs(i: int, j: int) -> bool:
            if i < 0:
                return True

            if j == i + 1:
                row = ''.join(pyramid[i])
                if row in vis:  # 这一行之前填过一模一样的，继续填，没能填到塔顶
                    return False  # 直接返回
                vis.add(row)
                return dfs(i - 1, 0)

            for top in groups[pyramid[i + 1][j] + pyramid[i + 1][j + 1]]:
                pyramid[i][j] = top
                if dfs(i, j + 1):
                    return True
            return False

        return dfs(n - 2, 0)
```

```java [sol-Java]
class Solution {
    public boolean pyramidTransition(String bottom, List<String> allowed) {
        List<Character>[][] groups = new ArrayList[6][6];
        for (List<Character>[] row : groups) {
            Arrays.setAll(row, _ -> new ArrayList<>());
        }
        for (String S : allowed) {
            char[] s = S.toCharArray();
            groups[s[0] - 'A'][s[1] - 'A'].add(s[2]);
        }

        int n = bottom.length();
        char[][] pyramid = new char[n][];
        for (int i = 0; i < n - 1; i++) {
            pyramid[i] = new char[i + 1];
        }
        pyramid[n - 1] = bottom.toCharArray();

        Set<String> vis = new HashSet<>(); // 访问标记

        return dfs(n - 2, 0, pyramid, vis, groups);
    }

    private boolean dfs(int i, int j, char[][] pyramid, Set<String> vis, List<Character>[][] groups) {
        if (i < 0) {
            return true;
        }

        if (j == i + 1) {
            String row = new String(pyramid[i]);
            if (!vis.add(row)) { // 这一行之前填过一模一样的，继续填，没能填到塔顶
                return false; // 直接返回
            }
            return dfs(i - 1, 0, pyramid, vis, groups);
        }

        for (char top : groups[pyramid[i + 1][j] - 'A'][pyramid[i + 1][j + 1] - 'A']) {
            pyramid[i][j] = top;
            if (dfs(i, j + 1, pyramid, vis, groups)) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool pyramidTransition(string bottom, vector<string>& allowed) {
        string groups[6][6];
        for (auto& s : allowed) {
            groups[s[0] - 'A'][s[1] - 'A'] += s[2];
        }

        int n = bottom.size();
        vector<string> pyramid(n);
        for (int i = 0; i < n - 1; i++) {
            pyramid[i].resize(i + 1);
        }
        pyramid[n - 1] = move(bottom);

        unordered_set<string> vis; // 访问标记

        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (i < 0) {
                return true;
            }

            if (j == i + 1) {
                if (!vis.insert(pyramid[i]).second) { // 这一行之前填过一模一样的，继续填，没能填到塔顶
                    return false; // 直接返回
                }
                return dfs(i - 1, 0);
            }

            for (char top : groups[pyramid[i + 1][j] - 'A'][pyramid[i + 1][j + 1] - 'A']) {
                pyramid[i][j] = top;
                if (dfs(i, j + 1)) {
                    return true;
                }
            }
            return false;
        };

        return dfs(n - 2, 0);
    }
};
```

```go [sol-Go]
func pyramidTransition(bottom string, allowed []string) bool {
	groups := [6][6][]byte{}
	for _, s := range allowed {
		a, b := s[0]-'A', s[1]-'A'
		groups[a][b] = append(groups[a][b], s[2])
	}

	n := len(bottom)
	pyramid := make([][]byte, n)
	for i := range n - 1 {
		pyramid[i] = make([]byte, i+1)
	}
	pyramid[n-1] = []byte(bottom)

	vis := map[string]struct{}{} // 访问标记

	var dfs func(int, int) bool
	dfs = func(i, j int) bool {
		if i < 0 {
			return true
		}

		if j == i+1 {
			row := string(pyramid[i])
			if _, ok := vis[row]; ok { // 这一行之前填过一模一样的，继续填，没能填到塔顶
				return false // 直接返回
			}
			vis[row] = struct{}{}
			return dfs(i-1, 0)
		}

		for _, top := range groups[pyramid[i+1][j]-'A'][pyramid[i+1][j+1]-'A'] {
			pyramid[i][j] = top
			if dfs(i, j+1) {
				return true
			}
		}
		return false
	}

	return dfs(n-2, 0)
}
```

## 优化二：减少更多重复搜索

![lc756-3c.png](https://pic.leetcode.cn/1766718366-ZQWxfN-lc756-3c.png){:width=600px}

```py [sol-Python3]
class Solution:
    def pyramidTransition(self, bottom: str, allowed: List[str]) -> bool:
        groups = defaultdict(list)
        for s in allowed:
            groups[s[:2]].append(s[2])

        n = len(bottom)
        pyramid = [[] for _ in range(n)]
        pyramid[-1] = bottom

        vis = set()

        def dfs(i: int, j: int) -> bool:
            if i < 0:
                return True

            row = ''.join(pyramid[i])
            if row in vis:  # 之前填过一模一样的，这个局部的金字塔无法填完
                return False  # 继续递归也无法填完，直接返回

            if j == i + 1:
                vis.add(row)
                return dfs(i - 1, 0)

            for top in groups[pyramid[i + 1][j] + pyramid[i + 1][j + 1]]:
                pyramid[i].append(top)
                if dfs(i, j + 1):
                    return True
                pyramid[i].pop()
            return False

        return dfs(n - 2, 0)
```

```java [sol-Java]
class Solution {
    public boolean pyramidTransition(String bottom, List<String> allowed) {
        List<Character>[][] groups = new ArrayList[6][6];
        for (List<Character>[] row : groups) {
            Arrays.setAll(row, _ -> new ArrayList<>());
        }
        for (String S : allowed) {
            char[] s = S.toCharArray();
            groups[s[0] - 'A'][s[1] - 'A'].add(s[2]);
        }

        int n = bottom.length();
        char[][] pyramid = new char[n][];
        for (int i = 0; i < n - 1; i++) {
            pyramid[i] = new char[i + 1];
        }
        pyramid[n - 1] = bottom.toCharArray();

        Set<String> vis = new HashSet<>();

        return dfs(n - 2, 0, pyramid, vis, groups);
    }

    private boolean dfs(int i, int j, char[][] pyramid, Set<String> vis, List<Character>[][] groups) {
        if (i < 0) {
            return true;
        }

        String row = new String(pyramid[i], 0, j);
        if (vis.contains(row)) { // 之前填过一模一样的，这个局部的金字塔无法填完
            return false; // 继续递归也无法填完，直接返回
        }

        if (j == i + 1) {
            vis.add(row);
            return dfs(i - 1, 0, pyramid, vis, groups);
        }

        for (char top : groups[pyramid[i + 1][j] - 'A'][pyramid[i + 1][j + 1] - 'A']) {
            pyramid[i][j] = top;
            if (dfs(i, j + 1, pyramid, vis, groups)) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool pyramidTransition(string bottom, vector<string>& allowed) {
        string groups[6][6];
        for (auto& s : allowed) {
            groups[s[0] - 'A'][s[1] - 'A'] += s[2];
        }

        int n = bottom.size();
        vector<string> pyramid(n);
        pyramid[n - 1] = move(bottom);

        unordered_set<string> vis;

        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (i < 0) {
                return true;
            }

            if (vis.contains(pyramid[i])) { // 之前填过一模一样的，这个局部的金字塔无法填完
                return false; // 继续递归也无法填完，直接返回
            }

            if (j == i + 1) {
                vis.insert(pyramid[i]);
                return dfs(i - 1, 0);
            }

            for (char top : groups[pyramid[i + 1][j] - 'A'][pyramid[i + 1][j + 1] - 'A']) {
                pyramid[i] += top;
                if (dfs(i, j + 1)) {
                    return true;
                }
                pyramid[i].pop_back();
            }
            return false;
        };

        return dfs(n - 2, 0);
    }
};
```

```go [sol-Go]
func pyramidTransition(bottom string, allowed []string) bool {
	groups := [6][6][]byte{}
	for _, s := range allowed {
		a, b := s[0]-'A', s[1]-'A'
		groups[a][b] = append(groups[a][b], s[2])
	}

	n := len(bottom)
	pyramid := make([][]byte, n)
	for i := range n - 1 {
		pyramid[i] = make([]byte, i+1)
	}
	pyramid[n-1] = []byte(bottom)

	vis := map[string]struct{}{}

	var dfs func(int, int) bool
	dfs = func(i, j int) bool {
		if i < 0 {
			return true
		}

		row := string(pyramid[i][:j])
		if _, ok := vis[row]; ok { // 之前填过一模一样的，这个局部的金字塔无法填完
			return false // 继续递归也无法填完，直接返回
		}

		if j == i+1 {
			vis[row] = struct{}{}
			return dfs(i-1, 0)
		}

		for _, top := range groups[pyramid[i+1][j]-'A'][pyramid[i+1][j+1]-'A'] {
			pyramid[i][j] = top
			if dfs(i, j+1) {
				return true
			}
		}
		return false
	}

	return dfs(n-2, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n|\Sigma|^{2n-5})$，其中 $n$ 是 $\textit{bottom}$ 的长度，$|\Sigma|=6$ 是字符集合的大小。最坏情况举例：$\textit{bottom}=\texttt{AAAAAE}$，其中 $\texttt{AE}$ 上面只能填 $\texttt{F}$，$\texttt{*F}$ 上面不能填字母，其余随便填。这会导致倒数第二排的 $\mathcal{O}(|\Sigma|^{n-2})$ 种填法，在递归倒数第三排时，会枚举倒数第三排的 $\mathcal{O}(|\Sigma|^{n-3})$ 种字符串，一共有 $\mathcal{O}(|\Sigma|^{2n-5})$ 种组合。每个字符串花费 $\mathcal{O}(n)$ 的时间与 $\textit{vis}$ 交互（查询或插入）。
- 空间复杂度：$\mathcal{O}(n|\Sigma|^n)$。$\textit{vis}$ 至多保存 $\mathcal{O}(|\Sigma|^n)$ 种不同的字符串，每个字符串需要 $\mathcal{O}(n)$ 的空间。其中 $\mathcal{O}(|\Sigma|^n)$ 来自等比数列求和 $|\Sigma| + |\Sigma|^2 + \cdots + |\Sigma|^{n-1} = \mathcal{O}(|\Sigma|^n)$。

## 优化三：位运算

把 $\texttt{A}$ 到 $\texttt{F}$ 映射为 $1$ 到 $6$。由于每个字母只占用 $3$ 个比特位，可以用二进制数表示字符串。

> 由于倒数第二排最多 $5$ 个字母，所以记录到 $\textit{vis}$ 中的二进制数的长度最多为 $15$。

**问**：为什么不映射为 $0$ 到 $5$？

**答**：比如，我们无法区分 $\texttt{AA}$ 和 $\texttt{AAA}$，二者都是 $0$。

```py [sol-Python3]
class Solution:
    def pyramidTransition(self, bottom: str, allowed: List[str]) -> bool:
        groups = [[[] for _ in range(7)] for _ in range(7)]
        for a, b, c in allowed:
            # A~F -> 1~6
            groups[ord(a) & 31][ord(b) & 31].append(ord(c) & 31)

        n = len(bottom)
        pyramid = [0] * n
        for i, ch in enumerate(bottom):
            pyramid[-1] |= (ord(ch) & 31) << (i * 3)  # 等价于 pyramid[-1][i] = ord(ch)&31

        vis = set()

        def dfs(i: int, j: int) -> bool:
            if i < 0:
                return True

            if pyramid[i] in vis:
                return False

            if j == i + 1:
                vis.add(pyramid[i])
                return dfs(i - 1, 0)

            for top in groups[pyramid[i + 1] >> (j * 3) & 7][pyramid[i + 1] >> ((j + 1) * 3) & 7]:
                pyramid[i] &= ~(7 << (j * 3))  # 清除之前填的字母，等价于 pyramid[i][j] = 0
                pyramid[i] |= top << (j * 3)  # 等价于 pyramid[i][j] = top
                if dfs(i, j + 1):
                    return True
            return False

        return dfs(n - 2, 0)
```

```java [sol-Java]
class Solution {
    public boolean pyramidTransition(String bottom, List<String> allowed) {
        List<Integer>[][] groups = new ArrayList[7][7];
        for (List<Integer>[] row : groups) {
            Arrays.setAll(row, _ -> new ArrayList<>());
        }
        for (String S : allowed) {
            char[] s = S.toCharArray();
            // A~F -> 1~6
            groups[s[0] & 31][s[1] & 31].add(s[2] & 31);
        }

        char[] s = bottom.toCharArray();
        int n = s.length;
        int[] pyramid = new int[n];
        for (int i = 0; i < n; i++) {
            pyramid[n - 1] |= (s[i] & 31) << (i * 3); // 等价于 pyramid[n-1][i] = s[i]&31
        }

        boolean[] vis = new boolean[1 << ((n - 1) * 3)];

        return dfs(n - 2, 0, pyramid, vis, groups);
    }

    private boolean dfs(int i, int j, int[] pyramid, boolean[] vis, List<Integer>[][] groups) {
        if (i < 0) {
            return true;
        }

        if (vis[pyramid[i]]) {
            return false;
        }

        if (j == i + 1) {
            vis[pyramid[i]] = true;
            return dfs(i - 1, 0, pyramid, vis, groups);
        }

        for (int top : groups[pyramid[i + 1] >> (j * 3) & 7][pyramid[i + 1] >> ((j + 1) * 3) & 7]) {
            pyramid[i] &= ~(7 << (j * 3)); // 清除之前填的字母，等价于 pyramid[i][j] = 0
            pyramid[i] |= top << (j * 3); // 等价于 pyramid[i][j] = top
            if (dfs(i, j + 1, pyramid, vis, groups)) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool pyramidTransition(string bottom, vector<string>& allowed) {
        vector<int> groups[7][7];
        for (auto& s : allowed) {
            // A~F -> 1~6
            groups[s[0] & 31][s[1] & 31].push_back(s[2] & 31);
        }

        int n = bottom.size();
        vector<int> pyramid(n);
        for (int i = 0; i < n; i++) {
            pyramid[n - 1] |= (bottom[i] & 31) << (i * 3); // 等价于 pyramid[n-1][i] = bottom[i]&31
        }

        vector<uint8_t> vis(1 << ((n - 1) * 3));

        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (i < 0) {
                return true;
            }

            if (vis[pyramid[i]]) {
                return false;
            }

            if (j == i + 1) {
                vis[pyramid[i]] = true;
                return dfs(i - 1, 0);
            }

            for (int top : groups[pyramid[i + 1] >> (j * 3) & 7][pyramid[i + 1] >> ((j + 1) * 3) & 7]) {
                pyramid[i] &= ~(7 << (j * 3)); // 清除之前填的字母，等价于 pyramid[i][j] = 0
                pyramid[i] |= top << (j * 3); // 等价于 pyramid[i][j] = top
                if (dfs(i, j + 1)) {
                    return true;
                }
            }
            return false;
        };

        return dfs(n - 2, 0);
    }
};
```

```go [sol-Go]
func pyramidTransition(bottom string, allowed []string) bool {
	groups := [7][7][]byte{}
	for _, s := range allowed {
		a, b := s[0]&31, s[1]&31 // A~F -> 1~6
		groups[a][b] = append(groups[a][b], s[2]&31)
	}

	n := len(bottom)
	pyramid := make([]int, n)
	for i, ch := range bottom {
		pyramid[n-1] |= int(ch&31) << (i * 3) // 等价于 pyramid[n-1][i] = ch&31
	}

	vis := make([]bool, 1<<((n-1)*3))

	var dfs func(int, int) bool
	dfs = func(i, j int) bool {
		if i < 0 {
			return true
		}

		if vis[pyramid[i]] {
			return false
		}

		if j == i+1 {
			vis[pyramid[i]] = true
			return dfs(i-1, 0)
		}

		for _, top := range groups[pyramid[i+1]>>(j*3)&7][pyramid[i+1]>>((j+1)*3)&7] {
			pyramid[i] &^= 7 << (j * 3) // 清除之前填的字母，等价于 pyramid[i][j] = 0
			pyramid[i] |= int(top) << (j * 3) // 等价于 pyramid[i][j] = top
			if dfs(i, j+1) {
				return true
			}
		}
		return false
	}

	return dfs(n-2, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|\Sigma|^{2n-5})$，其中 $n$ 是 $\textit{bottom}$ 的长度，$|\Sigma|=6$ 是字符集合的大小。见优化二的复杂度分析。
- 空间复杂度：$\mathcal{O}(|\Sigma|^n)$。

## 优化四：剪枝

如果发现当前填的字母与左侧相邻字母，无法得到上面的字母，则不填。这可以避免陷入优化二复杂度分析中的最坏情况。

```py [sol-Python3]
class Solution:
    def pyramidTransition(self, bottom: str, allowed: List[str]) -> bool:
        groups = [[[] for _ in range(7)] for _ in range(7)]
        for a, b, c in allowed:
            # A~F -> 1~6
            groups[ord(a) & 31][ord(b) & 31].append(ord(c) & 31)

        n = len(bottom)
        pyramid = [0] * n
        for i, ch in enumerate(bottom):
            pyramid[-1] |= (ord(ch) & 31) << (i * 3)  # 等价于 pyramid[-1][i] = ord(ch)&31

        vis = set()

        def dfs(i: int, j: int) -> bool:
            if i < 0:
                return True

            if pyramid[i] in vis:
                return False

            if j == i + 1:
                vis.add(pyramid[i])
                return dfs(i - 1, 0)

            for top in groups[pyramid[i + 1] >> (j * 3) & 7][pyramid[i + 1] >> ((j + 1) * 3) & 7]:
                if j and not groups[pyramid[i] >> ((j - 1) * 3) & 7][top]:
                    continue
                pyramid[i] &= ~(7 << (j * 3))  # 清除之前填的字母，等价于 pyramid[i][j] = 0
                pyramid[i] |= top << (j * 3)  # 等价于 pyramid[i][j] = top
                if dfs(i, j + 1):
                    return True
            return False

        return dfs(n - 2, 0)
```

```java [sol-Java]
class Solution {
    public boolean pyramidTransition(String bottom, List<String> allowed) {
        List<Integer>[][] groups = new ArrayList[7][7];
        for (List<Integer>[] row : groups) {
            Arrays.setAll(row, _ -> new ArrayList<>());
        }
        for (String S : allowed) {
            char[] s = S.toCharArray();
            // A~F -> 1~6
            groups[s[0] & 31][s[1] & 31].add(s[2] & 31);
        }

        char[] s = bottom.toCharArray();
        int n = s.length;
        int[] pyramid = new int[n];
        for (int i = 0; i < n; i++) {
            pyramid[n - 1] |= (s[i] & 31) << (i * 3); // 等价于 pyramid[n-1][i] = s[i]&31
        }

        boolean[] vis = new boolean[1 << ((n - 1) * 3)];

        return dfs(n - 2, 0, pyramid, vis, groups);
    }

    private boolean dfs(int i, int j, int[] pyramid, boolean[] vis, List<Integer>[][] groups) {
        if (i < 0) {
            return true;
        }

        if (vis[pyramid[i]]) {
            return false;
        }

        if (j == i + 1) {
            vis[pyramid[i]] = true;
            return dfs(i - 1, 0, pyramid, vis, groups);
        }

        for (int top : groups[pyramid[i + 1] >> (j * 3) & 7][pyramid[i + 1] >> ((j + 1) * 3) & 7]) {
            if (j > 0 && groups[pyramid[i] >> ((j - 1) * 3) & 7][top].isEmpty()) {
                continue;
            }
            pyramid[i] &= ~(7 << (j * 3)); // 清除之前填的字母，等价于 pyramid[i][j] = 0
            pyramid[i] |= top << (j * 3); // 等价于 pyramid[i][j] = top
            if (dfs(i, j + 1, pyramid, vis, groups)) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool pyramidTransition(string bottom, vector<string>& allowed) {
        vector<int> groups[7][7];
        for (auto& s : allowed) {
            // A~F -> 1~6
            groups[s[0] & 31][s[1] & 31].push_back(s[2] & 31);
        }

        int n = bottom.size();
        vector<int> pyramid(n);
        for (int i = 0; i < n; i++) {
            pyramid[n - 1] |= (bottom[i] & 31) << (i * 3); // 等价于 pyramid[n-1][i] = bottom[i]&31
        }

        vector<uint8_t> vis(1 << ((n - 1) * 3));

        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (i < 0) {
                return true;
            }

            if (vis[pyramid[i]]) {
                return false;
            }

            if (j == i + 1) {
                vis[pyramid[i]] = true;
                return dfs(i - 1, 0);
            }

            for (int top : groups[pyramid[i + 1] >> (j * 3) & 7][pyramid[i + 1] >> ((j + 1) * 3) & 7]) {
                if (j > 0 && groups[pyramid[i] >> ((j - 1) * 3) & 7][top].empty()) {
                    continue;
                }
                pyramid[i] &= ~(7 << (j * 3)); // 清除之前填的字母，等价于 pyramid[i][j] = 0
                pyramid[i] |= top << (j * 3); // 等价于 pyramid[i][j] = top
                if (dfs(i, j + 1)) {
                    return true;
                }
            }
            return false;
        };

        return dfs(n - 2, 0);
    }
};
```

```go [sol-Go]
func pyramidTransition(bottom string, allowed []string) bool {
	groups := [7][7][]byte{}
	for _, s := range allowed {
		a, b := s[0]&31, s[1]&31 // A~F -> 1~6
		groups[a][b] = append(groups[a][b], s[2]&31)
	}

	n := len(bottom)
	pyramid := make([]int, n)
	for i, ch := range bottom {
		pyramid[n-1] |= int(ch&31) << (i * 3) // 等价于 pyramid[n-1][i] = ch&31
	}

	vis := make([]bool, 1<<((n-1)*3))

	var dfs func(int, int) bool
	dfs = func(i, j int) bool {
		if i < 0 {
			return true
		}

		if vis[pyramid[i]] {
			return false
		}

		if j == i+1 {
			vis[pyramid[i]] = true
			return dfs(i-1, 0)
		}

		for _, top := range groups[pyramid[i+1]>>(j*3)&7][pyramid[i+1]>>((j+1)*3)&7] {
			if j > 0 && groups[pyramid[i]>>((j-1)*3)&7][top] == nil {
				continue
			}
			pyramid[i] &^= 7 << (j * 3) // 清除之前填的字母，等价于 pyramid[i][j] = 0
			pyramid[i] |= int(top) << (j * 3) // 等价于 pyramid[i][j] = top
			if dfs(i, j+1) {
				return true
			}
		}
		return false
	}

	return dfs(n-2, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(|\Sigma|^n)$，其中 $n$ 是 $\textit{bottom}$ 的长度，$|\Sigma|=6$ 是字符集合的大小。至多有 $|\Sigma| + |\Sigma|^2 + \cdots + |\Sigma|^{n-1} = \mathcal{O}(|\Sigma|^n)$ 个不同的字符串。
- 空间复杂度：$\mathcal{O}(|\Sigma|^n)$。

## 专题训练

见下面回溯题单的「**§4.7 搜索**」。

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

本题来自 `九、状态压缩 DP（状压 DP） / §9.5 轮廓线 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.5 轮廓线 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
