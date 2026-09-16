# 2318. 不同骰子序列的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-distinct-roll-sequences/
- 题目 slug：`number-of-distinct-roll-sequences`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2090
- 外部题解来源：https://leetcode.cn/problems/number-of-distinct-roll-sequences/solutions/1625297/by-endlesscheng-tgkn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从三维到二维（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-distinct-roll-sequences/solutions/1625297/by-endlesscheng-tgkn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-tgkn`
- topic id：`1625297`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：三维 DP

#### 提示 1

序列计数问题，先往 DP 上想。

#### 提示 2

只定义 $f[i]$ 一个维度够吗（$i$ 表示序列长度）？

不够，没法准确统计满足题目第二个要求的序列个数。

注意到序列的元素只有 $1$ 到 $6$，我们可以把元素值也作为状态。

#### 提示 3

定义 $f[i][\textit{last}][\textit{last}_2]$ 表示序列长度为 $i$，最后一个元素是 $\textit{last}$，倒数第二个元素是 $\textit{last}_2$ 的序列数目。

通过枚举 $\textit{last}$ 和 $\textit{last}_2$，我们可以计算出 $f[i+1][j][\textit{last}]$，需满足

- $\text{GCD}(\textit{last},j)=1$（也可以写成 $\textit{last}\perp j$）
- $\textit{last}\ne j$
- $\textit{last}_2\ne j$

累加这些 $f[i][\textit{last}][\textit{last}_2]$，即得到 $f[i+1][j][\textit{last}]$。

答案为 $\sum\limits_{i}\sum\limits_{j} f[n][i][j]$。

代码实现时，可以将这些值在外部预计算出来，避免每次都重复计算一遍。

本题 [视频讲解](https://www.bilibili.com/video/BV143411w7Wx)，欢迎点赞三连~

#### 复杂度分析

- 时间复杂度：$O(nm^3)$，这里 $m=6$。求 $\text{GCD}$ 的时间忽略不计（也可以打表预处理出互质的数字）。
- 空间复杂度：$O(nm^2)$。

```Python [sol1-Python3]
MOD, MX = 10 ** 9 + 7, 10 ** 4
f = [[[0] * 6 for _ in range(6)] for _ in range(MX + 1)]
f[2] = [[int(j != i and gcd(j + 1, i + 1) == 1) for j in range(6)] for i in range(6)]
for i in range(2, MX):
    for j in range(6):
        for last in range(6):
            if last != j and gcd(last + 1, j + 1) == 1:
                f[i + 1][j][last] = sum(f[i][last][last2] for last2 in range(6) if last2 != j) % MOD

class Solution:
    def distinctSequences(self, n: int) -> int:
        return sum(sum(row) for row in f[n]) % MOD if n > 1 else 6
```

```java [sol1-Java]
class Solution {
    static final int MOD = (int) 1e9 + 7, MX = (int) 1e4;
    static final int[][][] f = new int[MX + 1][6][6];

    static {
        for (var i = 0; i < 6; ++i)
            for (var j = 0; j < 6; ++j)
                if (j != i && gcd(j + 1, i + 1) == 1)
                    f[2][i][j] = 1;
        for (var i = 2; i < MX; ++i)
            for (var j = 0; j < 6; ++j)
                for (var last = 0; last < 6; ++last)
                    if (last != j && gcd(last + 1, j + 1) == 1)
                        for (var last2 = 0; last2 < 6; ++last2)
                            if (last2 != j)
                                f[i + 1][j][last] = (f[i + 1][j][last] + f[i][last][last2]) % MOD;
    }

    public int distinctSequences(int n) {
        if (n == 1) return 6;
        var ans = 0;
        for (var i = 0; i < 6; ++i)
            for (var j = 0; j < 6; ++j)
                ans = (ans + f[n][i][j]) % MOD;
        return ans;
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```C++ [sol1-C++]
const int MOD = 1e9 + 7, MX = 1e4;
int f[MX + 1][6][6];
int init = []() {
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            f[2][i][j] = j != i && gcd(j + 1, i + 1) == 1;
    for (int i = 2; i < MX; ++i)
        for (int j = 0; j < 6; ++j)
            for (int last = 0; last < 6; ++last)
                if (last != j && gcd(last + 1, j + 1) == 1)
                    for (int last2 = 0; last2 < 6; ++last2)
                        if (last2 != j)
                            f[i + 1][j][last] = (f[i + 1][j][last] + f[i][last][last2]) % MOD;
    return 0;
}();

class Solution {
public:
    int distinctSequences(int n) {
        if (n == 1) return 6;
        int ans = 0;
        for (int i = 0; i < 6; ++i)
            for (int j = 0; j < 6; ++j)
                ans = (ans + f[n][i][j]) % MOD;
        return ans;
    }
};
```

```go [sol1-Go]
const mod int = 1e9 + 7
var f = [1e4 + 1][6][6]int{}

func init() {
	for last := 0; last < 6; last++ {
		for last2 := 0; last2 < 6; last2++ {
			if last2 != last && gcd(last2+1, last+1) == 1 {
				f[2][last][last2] = 1
			}
		}
	}
	for i := 2; i < 1e4; i++ {
		for j := 0; j < 6; j++ {
			for last := 0; last < 6; last++ {
				if last != j && gcd(last+1, j+1) == 1 {
					for last2 := 0; last2 < 6; last2++ {
						if last2 != j {
							f[i+1][j][last] = (f[i+1][j][last] + f[i][last][last2]) % mod
						}
					}
				}
			}
		}
	}
}

func distinctSequences(n int) (ans int) {
	if n == 1 {
		return 6
	}
	for _, row := range f[n] {
		for _, v := range row {
			ans = (ans + v) % mod
		}
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

附：Python 记忆化写法

```py
@cache
def f(n: int, last: int, last2: int) -> int:
    if n == 0: return 1
    res = 0
    for j in range(1, 7):
        if j != last and j != last2 and gcd(j, last) == 1:
            res += f(n - 1, j, last)
    return res % (10 ** 9 + 7)

class Solution:
    def distinctSequences(self, n: int) -> int:
        return f(n, 7, 7)  # 7 与 [1,6] 内的数字都不同且互质
```

## 方法二：二维 DP

定义 $f[i][j]$ 表示序列长度为 $i$，最后一个元素是 $j$ 时的序列个数。

类似方法一，我们可以枚举前一个元素 $k$，需满足

- $k\ne j$
- $k\perp j$

以 $f[i][4]$ 为例，$k$ 为 $1$、$3$ 和 $5$，如果直接把这些 $f[i-1][1/3/5]$ 转移到 $f[i][4]$ 上，我们会得到一个错误的转移方程

$$
f[i][4] = \sum
\begin{cases}
f[i-1][1]\\
f[i-1][3]\\
f[i-1][5]
\end{cases}
$$

原因在于**前前**一个元素不能为 $4$，而我们在计算 $f[i-1][1/3/5]$ 时，它的前一个元素是可以为 $4$ 的。

顺着这个思路，如果把 $f[i-2][4]$ 都减掉，我们仍然会得到一个错误的转移方程

$$
f[i][4] = \sum
\begin{cases}
f[i-1][1]-f[i-2][4]\\
f[i-1][3]-f[i-2][4]\\
f[i-1][5]-f[i-2][4]
\end{cases}
$$

这是因为：

- $f[i-1][1]$ 对应的序列，不包含第 $i-3$ 个数为 $1$ 的情况；
- $f[i-1][3]$ 对应的序列，不包含第 $i-3$ 个数为 $3$ 的情况；
- $f[i-1][5]$ 对应的序列，不包含第 $i-3$ 个数为 $5$ 的情况；
- $f[i-2][4]$ 对应的序列，**包含第 $i-3$ 个数为 $1/3/5$ 的情况**。

如果照这样计算，又要把这些多减去的部分重新加回来，那岂不是要一直加减加减下去？

仔细想想，上面加粗的文字所对应的序列，又恰好组成了一个 $f[i-2][4]$，所以只需要再多加一个 $f[i-2][4]$，就能得到正确的转移方程

$$
f[i][4] = \left(\sum
\begin{cases}
f[i-1][1]-f[i-2][4]\\
f[i-1][3]-f[i-2][4]\\
f[i-1][5]-f[i-2][4]
\end{cases}\right) + f[i-2][4]
$$

一般地，转移方程为

$$
f[i][j] = \left(\sum_{\substack{k\ne j\\ k\perp j}} (f[i-1][k]-f[i-2][j])\right) + f[i-2][j]
$$

一个特殊的边界是当 $i=3$ 时，$i-3$ 不存在，故不需要再多加一个 $f[i-2][j]$。

#### 复杂度分析

- 时间复杂度：$O(nm^2)$，这里 $m=6$。求 $\text{GCD}$ 的时间忽略不计（也可以打表预处理出互质的数字）。
- 空间复杂度：$O(nm)$。

```py [sol2-Python3]
MOD, MX = 10 ** 9 + 7, 10 ** 4 + 1
f = [[0] * 6 for _ in range(MX + 1)]
f[1] = [1] * 6
for i in range(2, MX):
    for j in range(6):
        for k in range(6):
            if k != j and gcd(k + 1, j + 1) == 1:
                f[i][j] += f[i - 1][k] - f[i - 2][j]
        if i > 3: f[i][j] += f[i - 2][j]
        f[i][j] %= MOD

class Solution:
    def distinctSequences(self, n: int) -> int:
        return sum(f[n]) % MOD
```

```java [sol2-Java]
class Solution {
    static final int MOD = (int) 1e9 + 7, MX = (int) 1e4;
    static final int[][] f = new int[MX + 1][6];

    static {
        for (var i = 0; i < 6; ++i)
            f[1][i] = 1;
        for (var i = 2; i <= MX; ++i)
            for (var j = 0; j < 6; ++j) {
                var s = 0L;
                for (var k = 0; k < 6; ++k)
                    if (k != j && gcd(k + 1, j + 1) == 1)
                        s += f[i - 1][k] - f[i - 2][j];
                if (i > 3) s += f[i - 2][j];
                f[i][j] = (int) (s % MOD);
            }
    }

    public int distinctSequences(int n) {
        var ans = 0L;
        for (var v : f[n])
            ans += v;
        return (int) (ans % MOD + MOD) % MOD; // 保证结果非负
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```cpp [sol2-C++]
const int MOD = 1e9 + 7, MX = 1e4;
int f[MX + 1][6];
int init = []() {
    for (int i = 0; i < 6; ++i)
        f[1][i] = 1;
    for (int i = 2; i <= MX; ++i)
        for (int j = 0; j < 6; ++j) {
            long s = 0L;
            for (int k = 0; k < 6; ++k)
                if (k != j && gcd(k + 1, j + 1) == 1)
                    s += f[i - 1][k] - f[i - 2][j];
            if (i > 3) s += f[i - 2][j];
            f[i][j] = s % MOD;
        }
    return 0;
}();

class Solution {
public:
    int distinctSequences(int n) {
        long ans = 0L;
        for (int v : f[n])
            ans += v;
        return (ans % MOD + MOD) % MOD; // 保证结果非负
    }
};
```

```go [sol2-Go]
const mod int = 1e9 + 7
var f = [1e4 + 1][6]int{}

func init() {
	for i := 0; i < 6; i++ {
		f[1][i] = 1
	}
	for i := 2; i <= 1e4; i++ {
		for j := 0; j < 6; j++ {
			for k := 0; k < 6; k++ {
				if k != j && gcd(k+1, j+1) == 1 {
					f[i][j] += f[i-1][k] - f[i-2][j]
				}
			}
			if i > 3 {
				f[i][j] += f[i-2][j]
			}
			f[i][j] %= mod
		}
	}
}

func distinctSequences(n int) (ans int) {
	for _, v := range f[n] {
		ans += v
	}
	return (ans%mod + mod) % mod // 保证结果非负
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

另一种写法是，注意到 $f[2][j]$ 实际上就是转移方程中求和的次数，因此我们可以将 $f[i-2][j]$ 提出来，得到

$$
f[i][j] = \left(\sum_{\substack{k\ne j\\ k\perp j}} f[i-1][k]\right) - (f[2][j]-1)\cdot f[i-2][j]
$$

```py [sol3-Python3]
MOD, MX = 10 ** 9 + 7, 10 ** 4 + 1
f = [[0] * 6 for _ in range(MX + 1)]
f[1] = [1] * 6
for i in range(2, MX):
    for j in range(6):
        f[i][j] = (sum(f[i - 1][k] for k in range(6) if k != j and gcd(k + 1, j + 1) == 1)
                   - (f[2][j] - (i > 3)) * f[i - 2][j]) % MOD

class Solution:
    def distinctSequences(self, n: int) -> int:
        return sum(f[n]) % MOD
```

```java [sol3-Java]
class Solution {
    static final int MOD = (int) 1e9 + 7, MX = (int) 1e4;
    static final int[][] f = new int[MX + 1][6];

    static {
        for (var i = 0; i < 6; ++i)
            f[1][i] = 1;
        for (var i = 2; i <= MX; ++i)
            for (var j = 0; j < 6; ++j) {
                var s = 0L;
                for (var k = 0; k < 6; ++k)
                    if (k != j && gcd(k + 1, j + 1) == 1)
                        s += f[i - 1][k];
                s -= (long) (i > 3 ? f[2][j] - 1 : f[2][j]) * f[i - 2][j];
                f[i][j] = (int) (s % MOD);
            }
    }

    public int distinctSequences(int n) {
        var ans = 0L;
        for (var v : f[n])
            ans += v;
        return (int) (ans % MOD + MOD) % MOD; // 保证结果非负
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```cpp [sol3-C++]
const int MOD = 1e9 + 7, MX = 1e4;
int f[MX + 1][6];
int init = []() {
    for (int i = 0; i < 6; ++i)
        f[1][i] = 1;
    for (int i = 2; i <= MX; ++i)
        for (int j = 0; j < 6; ++j) {
            long s = 0L;
            for (int k = 0; k < 6; ++k)
                if (k != j && gcd(k + 1, j + 1) == 1)
                    s += f[i - 1][k];
            s -= (long) (f[2][j] - (i > 3)) * f[i - 2][j];
            f[i][j] = s % MOD;
        }
    return 0;
}();

class Solution {
public:
    int distinctSequences(int n) {
        long ans = 0L;
        for (int v : f[n])
            ans += v;
        return (ans % MOD + MOD) % MOD; // 保证结果非负
    }
};
```

```go [sol3-Go]
const mod int = 1e9 + 7
var f = [1e4 + 1][6]int{}

func init() {
	for i := 0; i < 6; i++ {
		f[1][i] = 1
	}
	for i := 2; i <= 1e4; i++ {
		for j := 0; j < 6; j++ {
			for k := 0; k < 6; k++ {
				if k != j && gcd(k+1, j+1) == 1 {
					f[i][j] += f[i-1][k]
				}
			}
			c := f[2][j]
			if i > 3 {
				c--
			}
			f[i][j] = (f[i][j] - c*f[i-2][j]) % mod
		}
	}
}

func distinctSequences(n int) (ans int) {
	for _, v := range f[n] {
		ans += v
	}
	return (ans%mod + mod) % mod // 保证结果非负
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 方法一：三维 DP

#### 提示 1

序列计数问题，先往 DP 上想。

#### 提示 2

只定义 $f[i]$ 一个维度够吗（$i$ 表示序列长度）？

不够，没法准确统计满足题目第二个要求的序列个数。

注意到序列的元素只有 $1$ 到 $6$，我们可以把元素值也作为状态。

#### 提示 3

定义 $f[i][\textit{last}][\textit{last}_2]$ 表示序列长度为 $i$，最后一个元素是 $\textit{last}$，倒数第二个元素是 $\textit{last}_2$ 的序列数目。

通过枚举 $\textit{last}$ 和 $\textit{last}_2$，我们可以计算出 $f[i+1][j][\textit{last}]$，需满足

- $\text{GCD}(\textit{last},j)=1$（也可以写成 $\textit{last}\perp j$）
- $\textit{last}\ne j$
- $\textit{last}_2\ne j$

累加这些 $f[i][\textit{last}][\textit{last}_2]$，即得到 $f[i+1][j][\textit{last}]$。

答案为 $\sum\limits_{i}\sum\limits_{j} f[n][i][j]$。

代码实现时，可以将这些值在外部预计算出来，避免每次都重复计算一遍。

本题 [视频讲解](https://www.bilibili.com/video/BV143411w7Wx)，欢迎点赞三连~

#### 复杂度分析

- 时间复杂度：$O(nm^3)$，这里 $m=6$。求 $\text{GCD}$ 的时间忽略不计（也可以打表预处理出互质的数字）。
- 空间复杂度：$O(nm^2)$。

```Python [sol1-Python3]
MOD, MX = 10 ** 9 + 7, 10 ** 4
f = [[[0] * 6 for _ in range(6)] for _ in range(MX + 1)]
f[2] = [[int(j != i and gcd(j + 1, i + 1) == 1) for j in range(6)] for i in range(6)]
for i in range(2, MX):
    for j in range(6):
        for last in range(6):
            if last != j and gcd(last + 1, j + 1) == 1:
                f[i + 1][j][last] = sum(f[i][last][last2] for last2 in range(6) if last2 != j) % MOD

class Solution:
    def distinctSequences(self, n: int) -> int:
        return sum(sum(row) for row in f[n]) % MOD if n > 1 else 6
```

```java [sol1-Java]
class Solution {
    static final int MOD = (int) 1e9 + 7, MX = (int) 1e4;
    static final int[][][] f = new int[MX + 1][6][6];

    static {
        for (var i = 0; i < 6; ++i)
            for (var j = 0; j < 6; ++j)
                if (j != i && gcd(j + 1, i + 1) == 1)
                    f[2][i][j] = 1;
        for (var i = 2; i < MX; ++i)
            for (var j = 0; j < 6; ++j)
                for (var last = 0; last < 6; ++last)
                    if (last != j && gcd(last + 1, j + 1) == 1)
                        for (var last2 = 0; last2 < 6; ++last2)
                            if (last2 != j)
                                f[i + 1][j][last] = (f[i + 1][j][last] + f[i][last][last2]) % MOD;
    }

    public int distinctSequences(int n) {
        if (n == 1) return 6;
        var ans = 0;
        for (var i = 0; i < 6; ++i)
            for (var j = 0; j < 6; ++j)
                ans = (ans + f[n][i][j]) % MOD;
        return ans;
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```C++ [sol1-C++]
const int MOD = 1e9 + 7, MX = 1e4;
int f[MX + 1][6][6];
int init = []() {
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            f[2][i][j] = j != i && gcd(j + 1, i + 1) == 1;
    for (int i = 2; i < MX; ++i)
        for (int j = 0; j < 6; ++j)
            for (int last = 0; last < 6; ++last)
                if (last != j && gcd(last + 1, j + 1) == 1)
                    for (int last2 = 0; last2 < 6; ++last2)
                        if (last2 != j)
                            f[i + 1][j][last] = (f[i + 1][j][last] + f[i][last][last2]) % MOD;
    return 0;
}();

class Solution {
public:
    int distinctSequences(int n) {
        if (n == 1) return 6;
        int ans = 0;
        for (int i = 0; i < 6; ++i)
            for (int j = 0; j < 6; ++j)
                ans = (ans + f[n][i][j]) % MOD;
        return ans;
    }
};
```

```go [sol1-Go]
const mod int = 1e9 + 7
var f = [1e4 + 1][6][6]int{}

func init() {
	for last := 0; last < 6; last++ {
		for last2 := 0; last2 < 6; last2++ {
			if last2 != last && gcd(last2+1, last+1) == 1 {
				f[2][last][last2] = 1
			}
		}
	}
	for i := 2; i < 1e4; i++ {
		for j := 0; j < 6; j++ {
			for last := 0; last < 6; last++ {
				if last != j && gcd(last+1, j+1) == 1 {
					for last2 := 0; last2 < 6; last2++ {
						if last2 != j {
							f[i+1][j][last] = (f[i+1][j][last] + f[i][last][last2]) % mod
						}
					}
				}
			}
		}
	}
}

func distinctSequences(n int) (ans int) {
	if n == 1 {
		return 6
	}
	for _, row := range f[n] {
		for _, v := range row {
			ans = (ans + v) % mod
		}
	}
	return
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

附：Python 记忆化写法

```py
@cache
def f(n: int, last: int, last2: int) -> int:
    if n == 0: return 1
    res = 0
    for j in range(1, 7):
        if j != last and j != last2 and gcd(j, last) == 1:
            res += f(n - 1, j, last)
    return res % (10 ** 9 + 7)

class Solution:
    def distinctSequences(self, n: int) -> int:
        return f(n, 7, 7)  # 7 与 [1,6] 内的数字都不同且互质
```

## 方法二：二维 DP

定义 $f[i][j]$ 表示序列长度为 $i$，最后一个元素是 $j$ 时的序列个数。

类似方法一，我们可以枚举前一个元素 $k$，需满足

- $k\ne j$
- $k\perp j$

以 $f[i][4]$ 为例，$k$ 为 $1$、$3$ 和 $5$，如果直接把这些 $f[i-1][1/3/5]$ 转移到 $f[i][4]$ 上，我们会得到一个错误的转移方程

$$
f[i][4] = \sum
\begin{cases}
f[i-1][1]\\
f[i-1][3]\\
f[i-1][5]
\end{cases}
$$

原因在于**前前**一个元素不能为 $4$，而我们在计算 $f[i-1][1/3/5]$ 时，它的前一个元素是可以为 $4$ 的。

顺着这个思路，如果把 $f[i-2][4]$ 都减掉，我们仍然会得到一个错误的转移方程

$$
f[i][4] = \sum
\begin{cases}
f[i-1][1]-f[i-2][4]\\
f[i-1][3]-f[i-2][4]\\
f[i-1][5]-f[i-2][4]
\end{cases}
$$

这是因为：

- $f[i-1][1]$ 对应的序列，不包含第 $i-3$ 个数为 $1$ 的情况；
- $f[i-1][3]$ 对应的序列，不包含第 $i-3$ 个数为 $3$ 的情况；
- $f[i-1][5]$ 对应的序列，不包含第 $i-3$ 个数为 $5$ 的情况；
- $f[i-2][4]$ 对应的序列，**包含第 $i-3$ 个数为 $1/3/5$ 的情况**。

如果照这样计算，又要把这些多减去的部分重新加回来，那岂不是要一直加减加减下去？

仔细想想，上面加粗的文字所对应的序列，又恰好组成了一个 $f[i-2][4]$，所以只需要再多加一个 $f[i-2][4]$，就能得到正确的转移方程

$$
f[i][4] = \left(\sum
\begin{cases}
f[i-1][1]-f[i-2][4]\\
f[i-1][3]-f[i-2][4]\\
f[i-1][5]-f[i-2][4]
\end{cases}\right) + f[i-2][4]
$$

一般地，转移方程为

$$
f[i][j] = \left(\sum_{\substack{k\ne j\\ k\perp j}} (f[i-1][k]-f[i-2][j])\right) + f[i-2][j]
$$

一个特殊的边界是当 $i=3$ 时，$i-3$ 不存在，故不需要再多加一个 $f[i-2][j]$。

#### 复杂度分析

- 时间复杂度：$O(nm^2)$，这里 $m=6$。求 $\text{GCD}$ 的时间忽略不计（也可以打表预处理出互质的数字）。
- 空间复杂度：$O(nm)$。

```py [sol2-Python3]
MOD, MX = 10 ** 9 + 7, 10 ** 4 + 1
f = [[0] * 6 for _ in range(MX + 1)]
f[1] = [1] * 6
for i in range(2, MX):
    for j in range(6):
        for k in range(6):
            if k != j and gcd(k + 1, j + 1) == 1:
                f[i][j] += f[i - 1][k] - f[i - 2][j]
        if i > 3: f[i][j] += f[i - 2][j]
        f[i][j] %= MOD

class Solution:
    def distinctSequences(self, n: int) -> int:
        return sum(f[n]) % MOD
```

```java [sol2-Java]
class Solution {
    static final int MOD = (int) 1e9 + 7, MX = (int) 1e4;
    static final int[][] f = new int[MX + 1][6];

    static {
        for (var i = 0; i < 6; ++i)
            f[1][i] = 1;
        for (var i = 2; i <= MX; ++i)
            for (var j = 0; j < 6; ++j) {
                var s = 0L;
                for (var k = 0; k < 6; ++k)
                    if (k != j && gcd(k + 1, j + 1) == 1)
                        s += f[i - 1][k] - f[i - 2][j];
                if (i > 3) s += f[i - 2][j];
                f[i][j] = (int) (s % MOD);
            }
    }

    public int distinctSequences(int n) {
        var ans = 0L;
        for (var v : f[n])
            ans += v;
        return (int) (ans % MOD + MOD) % MOD; // 保证结果非负
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```cpp [sol2-C++]
const int MOD = 1e9 + 7, MX = 1e4;
int f[MX + 1][6];
int init = []() {
    for (int i = 0; i < 6; ++i)
        f[1][i] = 1;
    for (int i = 2; i <= MX; ++i)
        for (int j = 0; j < 6; ++j) {
            long s = 0L;
            for (int k = 0; k < 6; ++k)
                if (k != j && gcd(k + 1, j + 1) == 1)
                    s += f[i - 1][k] - f[i - 2][j];
            if (i > 3) s += f[i - 2][j];
            f[i][j] = s % MOD;
        }
    return 0;
}();

class Solution {
public:
    int distinctSequences(int n) {
        long ans = 0L;
        for (int v : f[n])
            ans += v;
        return (ans % MOD + MOD) % MOD; // 保证结果非负
    }
};
```

```go [sol2-Go]
const mod int = 1e9 + 7
var f = [1e4 + 1][6]int{}

func init() {
	for i := 0; i < 6; i++ {
		f[1][i] = 1
	}
	for i := 2; i <= 1e4; i++ {
		for j := 0; j < 6; j++ {
			for k := 0; k < 6; k++ {
				if k != j && gcd(k+1, j+1) == 1 {
					f[i][j] += f[i-1][k] - f[i-2][j]
				}
			}
			if i > 3 {
				f[i][j] += f[i-2][j]
			}
			f[i][j] %= mod
		}
	}
}

func distinctSequences(n int) (ans int) {
	for _, v := range f[n] {
		ans += v
	}
	return (ans%mod + mod) % mod // 保证结果非负
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

另一种写法是，注意到 $f[2][j]$ 实际上就是转移方程中求和的次数，因此我们可以将 $f[i-2][j]$ 提出来，得到

$$
f[i][j] = \left(\sum_{\substack{k\ne j\\ k\perp j}} f[i-1][k]\right) - (f[2][j]-1)\cdot f[i-2][j]
$$

```py [sol3-Python3]
MOD, MX = 10 ** 9 + 7, 10 ** 4 + 1
f = [[0] * 6 for _ in range(MX + 1)]
f[1] = [1] * 6
for i in range(2, MX):
    for j in range(6):
        f[i][j] = (sum(f[i - 1][k] for k in range(6) if k != j and gcd(k + 1, j + 1) == 1)
                   - (f[2][j] - (i > 3)) * f[i - 2][j]) % MOD

class Solution:
    def distinctSequences(self, n: int) -> int:
        return sum(f[n]) % MOD
```

```java [sol3-Java]
class Solution {
    static final int MOD = (int) 1e9 + 7, MX = (int) 1e4;
    static final int[][] f = new int[MX + 1][6];

    static {
        for (var i = 0; i < 6; ++i)
            f[1][i] = 1;
        for (var i = 2; i <= MX; ++i)
            for (var j = 0; j < 6; ++j) {
                var s = 0L;
                for (var k = 0; k < 6; ++k)
                    if (k != j && gcd(k + 1, j + 1) == 1)
                        s += f[i - 1][k];
                s -= (long) (i > 3 ? f[2][j] - 1 : f[2][j]) * f[i - 2][j];
                f[i][j] = (int) (s % MOD);
            }
    }

    public int distinctSequences(int n) {
        var ans = 0L;
        for (var v : f[n])
            ans += v;
        return (int) (ans % MOD + MOD) % MOD; // 保证结果非负
    }

    static int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }
}
```

```cpp [sol3-C++]
const int MOD = 1e9 + 7, MX = 1e4;
int f[MX + 1][6];
int init = []() {
    for (int i = 0; i < 6; ++i)
        f[1][i] = 1;
    for (int i = 2; i <= MX; ++i)
        for (int j = 0; j < 6; ++j) {
            long s = 0L;
            for (int k = 0; k < 6; ++k)
                if (k != j && gcd(k + 1, j + 1) == 1)
                    s += f[i - 1][k];
            s -= (long) (f[2][j] - (i > 3)) * f[i - 2][j];
            f[i][j] = s % MOD;
        }
    return 0;
}();

class Solution {
public:
    int distinctSequences(int n) {
        long ans = 0L;
        for (int v : f[n])
            ans += v;
        return (ans % MOD + MOD) % MOD; // 保证结果非负
    }
};
```

```go [sol3-Go]
const mod int = 1e9 + 7
var f = [1e4 + 1][6]int{}

func init() {
	for i := 0; i < 6; i++ {
		f[1][i] = 1
	}
	for i := 2; i <= 1e4; i++ {
		for j := 0; j < 6; j++ {
			for k := 0; k < 6; k++ {
				if k != j && gcd(k+1, j+1) == 1 {
					f[i][j] += f[i-1][k]
				}
			}
			c := f[2][j]
			if i > 3 {
				c--
			}
			f[i][j] = (f[i][j] - c*f[i-2][j]) % mod
		}
	}
}

func distinctSequences(n int) (ans int) {
	for _, v := range f[n] {
		ans += v
	}
	return (ans%mod + mod) % mod // 保证结果非负
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
