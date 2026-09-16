# 1320. 二指输入的的最小距离

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-distance-to-type-a-word-using-two-fingers/
- 题目 slug：`minimum-distance-to-type-a-word-using-two-fingers`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2028
- 外部题解来源：https://leetcode.cn/problems/minimum-distance-to-type-a-word-using-two-fingers/solutions/3946229/jiao-ni-yi-bu-bu-si-kao-dpji-yi-hua-sou-d9vls/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：记忆化搜索 -> 递推 -> 空间优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-distance-to-type-a-word-using-two-fingers/solutions/3946229/jiao-ni-yi-bu-bu-si-kao-dpji-yi-hua-sou-d9vls/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpji-yi-hua-sou-d9vls`
- topic id：`3946229`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、分析

示例 1 的 $\textit{word} = \texttt{CAKE}$，敲完 $\texttt{E}$ 就结束了，所以最后一定有一根手指停在 $\texttt{E}$。

另一根手指呢？最后一定停在 $\texttt{K}$ 吗？不一定，如果第一根手指输入 $\texttt{CA}$，第二根手指输入 $\texttt{KE}$，那么第一根手指最后会停在 $\texttt{A}$，第二根手指最后会停在 $\texttt{E}$。

只要我们能实时跟踪两根手指的位置（在哪个字母），就能暴力搜索所有输入的过程。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i, \textit{finger}_1, \textit{finger}_2)$ 表示在手指 1 位于字母 $\textit{finger}_1$，手指 2 位于字母 $\textit{finger}_2$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i]$ 的最小移动总距离。

> **注**：从右往左递归，主要是方便把递归翻译成从左往右的递推。从左往右递归也是可以的。

讨论用哪根手指输入 $\textit{word}[i]$：

- 用手指 1，那么接下来要解决的问题是，在手指 1 位于字母 $\textit{word}[i]$，手指 2 位于字母 $\textit{finger}_2$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i-1]$ 的最小移动总距离，即 $\textit{dfs}(i-1, \textit{word}[i],\textit{finger}_2)$，加上从 $\textit{finger}_1$ 到 $\textit{word}[i]$ 的距离。 
- 用手指 2，那么接下来要解决的问题是，在手指 1 位于字母 $\textit{finger}_1$，手指 2 位于字母 $\textit{word}[i]$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i-1]$ 的最小移动总距离，即 $\textit{dfs}(i-1, \textit{finger}_1,\textit{word}[i])$，加上从 $\textit{finger}_2$ 到 $\textit{word}[i]$ 的距离。

这两种情况取最小值，就得到了 $\textit{dfs}(i, \textit{finger}_1, \textit{finger}_2)$，即

$$
\textit{dfs}(i, \textit{finger}_1, \textit{finger}_2) = \min
\begin{cases}
\textit{dfs}(i-1, \textit{word}[i],\textit{finger}_2) + \textit{dis}[\textit{finger}_1][\textit{word}[i]]     \\
\textit{dfs}(i-1, \textit{finger}_1,\textit{word}[i]) + \textit{dis}[\textit{finger}_2][\textit{word}[i]]     \\
\end{cases}
$$

其中 $\textit{dis}[x][y]$ 表示从字母 $x$ 到字母 $y$ 的距离。这个二维数组可以在跑 $\textit{dfs}$ 之前预处理出来。

**递归边界**：$\textit{dfs}(-1, \textit{finger}_1, \textit{finger}_2)=0$。没有字母需要输入，无需移动。

**递归入口**：$\textit{dfs}(n-2, \textit{word}[n-1], \textit{finger}_2)$。最后一定有一根手指在 $\textit{word}[n-1]$，另一根手指的位置不确定，枚举 $\textit{finger}_2$。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

⚠**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i, \textit{finger}_1, \textit{finger}_2)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
# 预处理两个字母的距离
COLUMN = 6
get_dis = lambda a, b: abs(a // COLUMN - b // COLUMN) + abs(a % COLUMN - b % COLUMN)
dis = [[get_dis(i, j) for j in range(26)] for i in range(26)]

class Solution:
    def minimumDistance(self, word: str) -> int:
        word = [ord(ch) - ord('A') for ch in word]  # 避免在 dfs 中频繁调用 ord

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, finger1: int, finger2: int) -> int:
            if i < 0:
                return 0

            # 手指 1 移到 word[i]
            res1 = dfs(i - 1, word[i], finger2) + dis[finger1][word[i]]

            # 手指 2 移到 word[i]
            res2 = dfs(i - 1, finger1, word[i]) + dis[finger2][word[i]]

            return min(res1, res2)

        n = len(word)
        # 最后一定有一根手指在 word[-1]，另一根手指的位置不确定，枚举
        return min(dfs(n - 2, word[-1], finger2) for finger2 in range(26))
```

```java [sol-Java]
class Solution {
    private static final int[][] dis = new int[26][26];

    static {
        // 预处理两个字母的距离
        final int COLUMN = 6;
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                dis[i][j] = Math.abs(i / COLUMN - j / COLUMN) + Math.abs(i % COLUMN - j % COLUMN);
            }
        }
    }

    public int minimumDistance(String word) {
        char[] s = word.toCharArray();
        int n = s.length;

        int[][][] memo = new int[n][26][26];
        for (int[][] mat : memo) {
            for (int[] row : mat) {
                Arrays.fill(row, -1); // -1 表示没有计算过
            }
        }

        int ans = Integer.MAX_VALUE;
        // 最后一定有一根手指在 s[n-1]，另一根手指的位置不确定，枚举
        for (int finger2 = 0; finger2 < 26; finger2++) {
            ans = Math.min(ans, dfs(n - 2, s[n - 1] - 'A', finger2, s, memo));
        }
        return ans;
    }

    private int dfs(int i, int finger1, int finger2, char[] word, int[][][] memo) {
        if (i < 0) {
            return 0;
        }

        if (memo[i][finger1][finger2] != -1) { // 之前计算过
            return memo[i][finger1][finger2];
        }

        // 手指 1 移到 word[i]
        int w = word[i] - 'A';
        int res1 = dfs(i - 1, w, finger2, word, memo) + dis[finger1][w];

        // 手指 2 移到 word[i]
        int res2 = dfs(i - 1, finger1, w, word, memo) + dis[finger2][w];

        int res = Math.min(res1, res2);
        memo[i][finger1][finger2] = res; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
int dis[26][26];

auto init = [] {
    // 预处理两个字母的距离
    constexpr int COLUMN = 6;
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            dis[i][j] = abs(i / COLUMN - j / COLUMN) + abs(i % COLUMN - j % COLUMN);
        }
    }
    return 0;
}();

class Solution {
public:
    int minimumDistance(string word) {
        int n = word.size();
        vector memo(n, vector(26, vector<int>(26, -1))); // -1 表示没有计算过

        auto dfs = [&](this auto&& dfs, int i, int finger1, int finger2) -> int {
            if (i < 0) {
                return 0;
            }

            int& res = memo[i][finger1][finger2]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }

            // 手指 1 移到 word[i]
            int w = word[i] - 'A';
            int res1 = dfs(i - 1, w, finger2) + dis[finger1][w];

            // 手指 2 移到 word[i]
            int res2 = dfs(i - 1, finger1, w) + dis[finger2][w];

            res = min(res1, res2);
            return res;
        };

        int ans = INT_MAX;
        // 最后一定有一根手指在 word[n-1]，另一根手指的位置不确定，枚举
        for (int finger2 = 0; finger2 < 26; finger2++) {
            ans = min(ans, dfs(n - 2, word[n - 1] - 'A', finger2));
        }
        return ans;
    }
};
```

```go [sol-Go]
var dis [26][26]int

func init() {
	// 预处理两个字母的距离
	const column = 6
	for i := range 26 {
		for j := range 26 {
			dis[i][j] = abs(i/column-j/column) + abs(i%column-j%column)
		}
	}
}

func minimumDistance(word string) int {
	n := len(word)
	memo := make([][26][26]int, n)

	var dfs func(int, byte, byte) int
	dfs = func(i int, finger1, finger2 byte) (res int) {
		if i < 0 {
			return 0
		}

		p := &memo[i][finger1][finger2]
		if *p != 0 { // 之前计算过
			return *p - 1
		}
		defer func() { *p = res + 1 }() // 记忆化的时候加一，这样就无需初始化 memo 为 -1 了

		// 手指 1 移到 word[i]
		w := word[i] - 'A'
		res1 := dfs(i-1, w, finger2) + dis[finger1][w]

		// 手指 2 移到 word[i]
		res2 := dfs(i-1, finger1, w) + dis[finger2][w]

		return min(res1, res2)
	}

	ans := math.MaxInt
	// 最后一定有一根手指在 word[n-1]，另一根手指的位置不确定，枚举
	for finger2 := range byte(26) {
		ans = min(ans, dfs(n-2, word[n-1]-'A', finger2))
	}
	return ans
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$ 或 $\mathcal{O}(n|\Sigma|^2)$，其中 $n$ 是 $\textit{word}$ 的长度，$|\Sigma|=26$ 是字符集合的大小。如果用哈希表保存状态，则时间复杂度为 $\mathcal{O}(n|\Sigma|)$，否则瓶颈在创建 $\textit{memo}$ 数组上。理由见下一章。
- 空间复杂度：$\mathcal{O}(n|\Sigma|)$ 或 $\mathcal{O}(n|\Sigma|^2)$。理由见下一章。

## 四、状态优化

回顾上面的代码，在输入 $\textit{word}[i]$ 之前，一定有一根手指在 $\textit{word}[i+1]$。这意味着，知道了 $i$，就知道了其中一根手指的位置。所以 $\textit{finger}_1$ 和 $\textit{finger}_2$ 中的一个是**多余**的。

定义 $\textit{dfs}(i, \textit{anotherFinger})$ 表示在一根手指位于字母 $\textit{word}[i+1]$，另一根手指位于字母 $\textit{anotherFinger}$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i]$ 的最小移动总距离。

讨论用哪根手指输入 $\textit{word}[i]$：

- 用位于 $\textit{word}[i+1]$ 的手指输入 $\textit{word}[i]$，那么另一根手指仍然位于字母 $\textit{anotherFinger}$，所以接下来要解决的问题是，在一根手指位于字母 $\textit{word}[i]$，另一根手指位于字母 $\textit{anotherFinger}$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i-1]$ 的最小移动总距离，即 $\textit{dfs}(i-1, \textit{anotherFinger})$，加上从 $\textit{word}[i+1]$ 到 $\textit{word}[i]$ 的距离。
- 用位于 $\textit{anotherFinger}$ 的手指输入 $\textit{word}[i]$，那么位于 $\textit{word}[i+1]$ 的手指就变成了「另一根手指」，所以接下来要解决的问题是，在一根手指位于字母 $\textit{word}[i]$，另一根手指位于字母 $\textit{word}[i+1]$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i-1]$ 的最小移动总距离，即 $\textit{dfs}(i-1, \textit{word}[i+1])$，加上从 $\textit{anotherFinger}$ 到 $\textit{word}[i]$ 的距离。

这两种情况取最小值，就得到了 $\textit{dfs}(i, \textit{anotherFinger})$，即

$$
\textit{dfs}(i, \textit{anotherFinger}) = \min
\begin{cases}
\textit{dfs}(i-1, \textit{anotherFinger}) + \textit{dis}[\textit{word}[i+1]][\textit{word}[i]]     \\
\textit{dfs}(i-1, \textit{word}[i+1]) + \textit{dis}[\textit{anotherFinger}][\textit{word}[i]]     \\
\end{cases}
$$

**递归边界**：$\textit{dfs}(-1, \textit{anotherFinger})=0$。没有字母需要输入，无需移动。

**递归入口**：$\textit{dfs}(n-2, \textit{anotherFinger})$。最后一定有一根手指在 $\textit{word}[n-1]$，另一根手指的位置不确定，枚举 $\textit{anotherFinger}$。

```py [sol-Python3]
# 预处理两个字母的距离
COLUMN = 6
get_dis = lambda a, b: abs(a // COLUMN - b // COLUMN) + abs(a % COLUMN - b % COLUMN)
dis = [[get_dis(i, j) for j in range(26)] for i in range(26)]

class Solution:
    def minimumDistance(self, word: str) -> int:
        word = [ord(ch) - ord('A') for ch in word]  # 避免在 dfs 中频繁调用 ord

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, another_finger: int) -> int:
            if i < 0:
                return 0

            # 在 word[i+1] 的手指移到 word[i]
            res1 = dfs(i - 1, another_finger) + dis[word[i + 1]][word[i]]

            # 另一根手指移到 word[i]，原来在 word[i+1] 的手指变成 another_finger
            res2 = dfs(i - 1, word[i + 1]) + dis[another_finger][word[i]]

            return min(res1, res2)

        n = len(word)
        # 最后一定有一根手指在 word[-1]，另一根手指的位置不确定，枚举
        return min(dfs(n - 2, another_finger) for another_finger in range(26))
```

```java [sol-Java]
class Solution {
    private static final int[][] dis = new int[26][26];

    static {
        // 预处理两个字母的距离
        final int COLUMN = 6;
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                dis[i][j] = Math.abs(i / COLUMN - j / COLUMN) + Math.abs(i % COLUMN - j % COLUMN);
            }
        }
    }

    public int minimumDistance(String word) {
        char[] s = word.toCharArray();
        int n = s.length;

        int[][] memo = new int[n][26];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        int ans = Integer.MAX_VALUE;
        // 最后一定有一根手指在 s[n-1]，另一根手指的位置不确定，枚举
        for (int anotherFinger = 0; anotherFinger < 26; anotherFinger++) {
            ans = Math.min(ans, dfs(n - 2, anotherFinger, s, memo));
        }
        return ans;
    }

    private int dfs(int i, int anotherFinger, char[] word, int[][] memo) {
        if (i < 0) {
            return 0;
        }

        if (memo[i][anotherFinger] != -1) { // 之前计算过
            return memo[i][anotherFinger];
        }

        // 在 word[i+1] 的手指移到 word[i]
        int w = word[i] - 'A';
        int res1 = dfs(i - 1, anotherFinger, word, memo) + dis[word[i + 1] - 'A'][w];

        // 另一根手指移到 word[i]，原来在 word[i+1] 的手指变成 anotherFinger
        int res2 = dfs(i - 1, word[i + 1] - 'A', word, memo) + dis[anotherFinger][w];

        int res = Math.min(res1, res2);
        memo[i][anotherFinger] = res; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
int dis[26][26];

auto init = [] {
    // 预处理两个字母的距离
    constexpr int COLUMN = 6;
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            dis[i][j] = abs(i / COLUMN - j / COLUMN) + abs(i % COLUMN - j % COLUMN);
        }
    }
    return 0;
}();

class Solution {
public:
    int minimumDistance(string word) {
        int n = word.size();
        vector memo(n, vector<int>(26, -1)); // -1 表示没有计算过

        auto dfs = [&](this auto&& dfs, int i, int another_finger) -> int {
            if (i < 0) {
                return 0;
            }

            int& res = memo[i][another_finger]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }

            // 在 word[i+1] 的手指移到 word[i]
            int w = word[i] - 'A';
            int res1 = dfs(i - 1, another_finger) + dis[word[i + 1] - 'A'][w];

            // 另一根手指移到 word[i]，原来在 word[i+1] 的手指变成 another_finger
            int res2 = dfs(i - 1, word[i + 1] - 'A') + dis[another_finger][w];

            res = min(res1, res2);
            return res;
        };

        int ans = INT_MAX;
        // 最后一定有一根手指在 word[n-1]，另一根手指的位置不确定，枚举
        for (int another_finger = 0; another_finger < 26; another_finger++) {
            ans = min(ans, dfs(n - 2, another_finger));
        }
        return ans;
    }
};
```

```go [sol-Go]
var dis [26][26]int

func init() {
	// 预处理两个字母的距离
	const column = 6
	for i := range 26 {
		for j := range 26 {
			dis[i][j] = abs(i/column-j/column) + abs(i%column-j%column)
		}
	}
}

func minimumDistance(word string) int {
	n := len(word)
	memo := make([][26]int, n)

	var dfs func(int, byte) int
	dfs = func(i int, anotherFinger byte) (res int) {
		if i < 0 {
			return 0
		}

		p := &memo[i][anotherFinger]
		if *p != 0 { // 之前计算过
			return *p - 1
		}
		defer func() { *p = res + 1 }() // 记忆化的时候加一，这样就无需初始化 memo 为 -1 了

		// 手指 1 移到 word[i]
		w := word[i] - 'A'
		res1 := dfs(i-1, anotherFinger) + dis[word[i+1]-'A'][w]

		// 手指 2 移到 word[i]
		res2 := dfs(i-1, word[i+1]-'A') + dis[anotherFinger][w]

		return min(res1, res2)
	}

	ans := math.MaxInt
	// 最后一定有一根手指在 word[n-1]，另一根手指的位置不确定，枚举
	for anotherFinger := range byte(26) {
		ans = min(ans, dfs(n-2, anotherFinger))
	}
	return ans
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$，其中 $n$ 是 $\textit{word}$ 的长度，$|\Sigma|=26$ 是字符集合的大小。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n|\Sigma|)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n|\Sigma|)$。
- 空间复杂度：$\mathcal{O}(n|\Sigma|)$。保存多少状态，就需要多少空间。

## 五、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][\textit{anotherFinger}]$ 的定义和 $\textit{dfs}(i,\textit{anotherFinger})$ 的定义是一样的，都表示在一根手指位于字母 $\textit{word}[i+1]$，另一根手指位于字母 $\textit{anotherFinger}$ 的情况下，输入 $\textit{word}$ 的前缀 $[0,i]$ 的最小移动总距离。这里 $+1$ 是为了把 $\textit{dfs}(-1,\textit{anotherFinger})$ 这个状态也翻译过来，这样我们可以把 $f[0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][\textit{anotherFinger}] = \min
\begin{cases}
f[i][\textit{anotherFinger}] + \textit{dis}[\textit{word}[i+1]][\textit{word}[i]]     \\
f[i][\textit{word}[i+1]] + \textit{dis}[\textit{anotherFinger}][\textit{word}[i]]     \\
\end{cases}
$$

初始值 $f[0][\textit{anotherFinger}]=0$，翻译自递归边界 $\textit{dfs}(-1, \textit{anotherFinger})$。

答案为 $f[n-1][\textit{anotherFinger}]$，翻译自递归入口 $\textit{dfs}(n-2, \textit{anotherFinger})$。

```py [sol-Python3]
# 预处理两个字母的距离
COLUMN = 6
get_dis = lambda a, b: abs(a // COLUMN - b // COLUMN) + abs(a % COLUMN - b % COLUMN)
dis = [[get_dis(i, j) for j in range(26)] for i in range(26)]

class Solution:
    def minimumDistance(self, word: str) -> int:
        f = [[0] * 26 for _ in word]
        for i, (x, y) in enumerate(pairwise(word)):
            x = ord(x) - ord('A')
            y = ord(y) - ord('A')
            for another_finger in range(26):
                f[i + 1][another_finger] = min(f[i][another_finger] + dis[y][x], f[i][y] + dis[another_finger][x])
        return min(f[-1])
```

```java [sol-Java]
class Solution {
    private static final int[][] dis = new int[26][26];

    static {
        // 预处理两个字母的距离
        final int COLUMN = 6;
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                dis[i][j] = Math.abs(i / COLUMN - j / COLUMN) + Math.abs(i % COLUMN - j % COLUMN);
            }
        }
    }

    public int minimumDistance(String word) {
        char[] s = word.toCharArray();
        int n = s.length;

        int[][] f = new int[n][26];
        for (int i = 0; i < n - 1; i++) {
            int x = s[i] - 'A';
            int y = s[i + 1] - 'A';
            for (int anotherFinger = 0; anotherFinger < 26; anotherFinger++) {
                f[i + 1][anotherFinger] = Math.min(f[i][anotherFinger] + dis[y][x], f[i][y] + dis[anotherFinger][x]);
            }
        }

        int ans = Integer.MAX_VALUE;
        for (int res : f[n - 1]) {
            ans = Math.min(ans, res);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
int dis[26][26];

auto init = [] {
    // 预处理两个字母的距离
    constexpr int COLUMN = 6;
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            dis[i][j] = abs(i / COLUMN - j / COLUMN) + abs(i % COLUMN - j % COLUMN);
        }
    }
    return 0;
}();

class Solution {
public:
    int minimumDistance(string word) {
        int n = word.size();
        vector<array<int, 26>> f(n);
        for (int i = 0; i < n - 1; i++) {
            int x = word[i] - 'A', y = word[i + 1] - 'A';
            for (int another_finger = 0; another_finger < 26; another_finger++) {
                f[i + 1][another_finger] = min(f[i][another_finger] + dis[y][x], f[i][y] + dis[another_finger][x]);
            }
        }
        return ranges::min(f[n - 1]);
    }
};
```

```go [sol-Go]
var dis [26][26]int

func init() {
	// 预处理两个字母的距离
	const column = 6
	for i := range 26 {
		for j := range 26 {
			dis[i][j] = abs(i/column-j/column) + abs(i%column-j%column)
		}
	}
}

func minimumDistance(word string) int {
	n := len(word)
	f := make([][26]int, n)

	for i := range n - 1 {
		x, y := word[i]-'A', word[i+1]-'A'
		for anotherFinger := range 26 {
			f[i+1][anotherFinger] = min(f[i][anotherFinger]+dis[y][x], f[i][y]+dis[anotherFinger][x])
		}
	}

	return slices.Min(f[n-1][:])
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$，其中 $n$ 是 $\textit{word}$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(n|\Sigma|)$。

## 六、空间优化

去掉 $f$ 的第一个维度，把 $f[i+1]$ 和 $f[i]$ 保存到**同一个数组**中。

此外，由于 $\textit{dis}[x][y] = \textit{dis}[y][x]$，我们可以交换转移方程中的 $\textit{dis}$ 的两个维度，这样在同一个内层循环中只会访问 $\textit{dis}[x]$ 这一个数组。

```py [sol-Python3]
COLUMN = 6
get_dis = lambda a, b: abs(a // COLUMN - b // COLUMN) + abs(a % COLUMN - b % COLUMN)
dis = [[get_dis(i, j) for j in range(26)] for i in range(26)]

class Solution:
    def minimumDistance(self, word: str) -> int:
        f = [0] * 26
        for x, y in pairwise(word):
            x = ord(x) - ord('A')
            y = ord(y) - ord('A')
            dis_x = dis[x]
            fy = f[y]
            for another_finger in range(26):
                f[another_finger] = min(f[another_finger] + dis_x[y], fy + dis_x[another_finger])
        return min(f)
```

```java [sol-Java]
class Solution {
    private static final int[][] dis = new int[26][26];

    static {
        final int COLUMN = 6;
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                dis[i][j] = Math.abs(i / COLUMN - j / COLUMN) + Math.abs(i % COLUMN - j % COLUMN);
            }
        }
    }

    public int minimumDistance(String word) {
        char[] s = word.toCharArray();
        int[] f = new int[26];

        for (int i = 0; i < s.length - 1; i++) {
            int x = s[i] - 'A';
            int y = s[i + 1] - 'A';
            int fy = f[y];
            for (int anotherFinger = 0; anotherFinger < 26; anotherFinger++) {
                f[anotherFinger] = Math.min(f[anotherFinger] + dis[x][y], fy + dis[x][anotherFinger]);
            }
        }

        int ans = Integer.MAX_VALUE;
        for (int res : f) {
            ans = Math.min(ans, res);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
int dis[26][26];

auto init = [] {
    constexpr int COLUMN = 6;
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            dis[i][j] = abs(i / COLUMN - j / COLUMN) + abs(i % COLUMN - j % COLUMN);
        }
    }
    return 0;
}();

class Solution {
public:
    int minimumDistance(string word) {
        int n = word.size();
        int f[26]{};
        for (int i = 0; i < n - 1; i++) {
            int x = word[i] - 'A', y = word[i + 1] - 'A';
            int fy = f[y];
            for (int another_finger = 0; another_finger < 26; another_finger++) {
                f[another_finger] = min(f[another_finger] + dis[x][y], fy + dis[x][another_finger]);
            }
        }
        return ranges::min(f);
    }
};
```

```go [sol-Go]
var dis [26][26]int

func init() {
	const column = 6
	for i := range 26 {
		for j := range 26 {
			dis[i][j] = abs(i/column-j/column) + abs(i%column-j%column)
		}
	}
}

func minimumDistance(word string) int {
	f := [26]int{}
	for i := range len(word) - 1 {
		x, y := word[i]-'A', word[i+1]-'A'
		fy := f[y]
		for anotherFinger := range 26 {
			f[anotherFinger] = min(f[anotherFinger]+dis[x][y], fy+dis[x][anotherFinger])
		}
	}
	return slices.Min(f[:])
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$，其中 $n$ 是 $\textit{word}$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 专题训练

见下面动态规划题单的「**§7.6 多维 DP**」。

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

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
