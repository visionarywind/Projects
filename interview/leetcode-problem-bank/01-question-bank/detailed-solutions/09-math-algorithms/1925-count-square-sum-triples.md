# 1925. 统计平方和三元组的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-square-sum-triples/
- 题目 slug：`count-square-sum-triples`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.10 数论分块
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-square-sum-triples/solutions/869409/go-yu-chu-li-you-hua-by-endlesscheng-61mj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[非暴力做法：本原勾股数组+数论分块+容斥（Python/Java/C++/Go）](https://leetcode.cn/problems/count-square-sum-triples/solutions/869409/go-yu-chu-li-you-hua-by-endlesscheng-61mj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-yu-chu-li-you-hua-by-endlesscheng-61mj`
- topic id：`869409`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力枚举

先说暴力怎么做。

我们可以枚举所有 $a>b$ 的平方和三元组 $(a,b,c)$。由于 $a^2+b^2=b^2+a^2$，所以 $(b,a,c)$ 也是平方和三元组。所以只需统计 $a>b$ 的情况，最后把统计结果乘以 $2$，即为答案。

$a=b$ 的情况呢？如果 $2a^2 = c^2$，那么 $c = \sqrt 2 a$，所以 $c$ 一定不是整数（反证法：如果 $c$ 是整数，式子变形得 $\dfrac{c}{a} = \sqrt 2$，左边是有理数，右边是无理数，有理数不可能等于无理数）。所以无需考虑 $a=b$ 的情况。

枚举 $a=2,3,\ldots,n-1$，枚举 $b=1,2,\ldots,a-1$，如果 $a^2+b^2\le n$ 且 $c = \sqrt{a^2+b^2}$ 是整数，那么我们找到了一个平方和三元组 $(a,b,c)$，计数器加一。

```py [sol-Python3]
class Solution:
    def countTriples(self, n: int) -> int:
        ans = 0
        for a in range(1, n):
            for b in range(1, a):
                if a * a + b * b > n * n:
                    break
                c2 = a * a + b * b
                if isqrt(c2) ** 2 == c2:
                    ans += 1
        return ans * 2  # (a,b,c) 和 (b,a,c) 各算一次
```

```java [sol-Java]
class Solution {
    public int countTriples(int n) {
        int ans = 0;
        for (int a = 1; a < n; a++) {
            for (int b = 1; b < a && a * a + b * b <= n * n; b++) {
                int c2 = a * a + b * b;
                int c = (int) Math.sqrt(c2);
                if (c * c == c2) {
                    ans++;
                }
            }
        }
        return ans * 2; // (a,b,c) 和 (b,a,c) 各算一次
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countTriples(int n) {
        int ans = 0;
        for (int a = 1; a < n; a++) {
            for (int b = 1; b < a && a * a + b * b <= n * n; b++) {
                int c2 = a * a + b * b;
                int c = sqrt(c2);
                if (c * c == c2) {
                    ans++;
                }
            }
        }
        return ans * 2; // (a,b,c) 和 (b,a,c) 各算一次
    }
};
```

```go [sol-Go]
func countTriples(n int) (ans int) {
	for a := 1; a < n; a++ {
		for b := 1; b < a && a*a+b*b <= n*n; b++ {
			c2 := a*a + b*b
			c := int(math.Sqrt(float64(c2)))
			if c*c == c2 {
				ans++
			}
		}
	}
	return ans * 2 // (a,b,c) 和 (b,a,c) 各算一次
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：本原勾股数组

请看 [勾三股四弦五：生成勾股数的公式](https://zhuanlan.zhihu.com/p/1978057341963368012)。

```py [sol-Python3]
class Solution:
    def countTriples(self, n: int) -> int:
        ans = 0
        u = 3
        while u * u < n * 2:
            v = 1
            while v < u and u * u + v * v <= n * 2:
                if gcd(u, v) == 1:
                    ans += n * 2 // (u * u + v * v)
                v += 2
            u += 2
        return ans * 2  # (a,b,c) 和 (b,a,c) 各算一次
```

```py [sol-Python3 预处理]
MX = isqrt(500) + 1
gcds = [[0] * MX for _ in range(MX)]
for i in range(1, MX):
    gcds[i][0] = i
    for j in range(1, MX):
        # 更相减损术
        gcds[i][j] = gcds[i][j - i] if j >= i else gcds[j][i]

class Solution:
    def countTriples(self, n: int) -> int:
        ans = 0
        u = 3
        while u * u < n * 2:
            v = 1
            while v < u and u * u + v * v <= n * 2:
                if gcds[u][v] == 1:
                    ans += n * 2 // (u * u + v * v)
                v += 2
            u += 2
        return ans * 2  # (a,b,c) 和 (b,a,c) 各算一次
```

```java [sol-Java]
class Solution {
    public int countTriples(int n) {
        int ans = 0;
        for (int u = 3; u * u < n * 2; u += 2) {
            for (int v = 1; v < u && u * u + v * v <= n * 2; v += 2) {
                if (gcd(u, v) == 1) {
                    ans += n * 2 / (u * u + v * v);
                }
            }
        }
        return ans * 2; // (a,b,c) 和 (b,a,c) 各算一次
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countTriples(int n) {
        int ans = 0;
        for (int u = 3; u * u < n * 2; u += 2) {
            for (int v = 1; v < u && u * u + v * v <= n * 2; v += 2) {
                if (gcd(u, v) == 1) {
                    ans += n * 2 / (u * u + v * v);
                }
            }
        }
        return ans * 2; // (a,b,c) 和 (b,a,c) 各算一次
    }
};
```

```go [sol-Go]
func countTriples(n int) (ans int) {
	for u := 3; u*u < n*2; u += 2 {
		for v := 1; v < u && u*u+v*v <= n*2; v += 2 {
			if gcd(u, v) == 1 {
				ans += n * 2 / (u*u + v*v)
			}
		}
	}
	return ans * 2 // (a,b,c) 和 (b,a,c) 各算一次
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$。枚举 $\mathcal{O}(\sqrt n\cdot \sqrt n) = \mathcal{O}(n)$ 次，每次花费 $\mathcal{O}(\log n)$ 的时间计算 GCD。如果预处理 $\sqrt {2n}$ 以内的数对 GCD，则可以做到 $\mathcal{O}(n)$ 时间，参考 Python3 写法二。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法三：数论分块 + 容斥原理

### 数论分块

更仔细地考察枚举 $(u,v)$ 计算 $\left\lfloor\dfrac{2n}{u^2+v^2}\right\rfloor$ 的过程：

- 当 $u\le \sqrt[3] {n}$ 时，枚举 $(u,v)$ 的循环次数为 $\mathcal{O}(n^{2/3})$。
- 当 $u > \sqrt[3] {n}$ 时，由于 $\dfrac{2n}{u^2+v^2} < \dfrac{2n}{u^2}$，所以对于固定的 $u$，$\left\lfloor\dfrac{2n}{u^2+v^2}\right\rfloor$ 只有 $\mathcal{O}\left(\dfrac{n}{u^2}\right)$ 个不同的整数值。横看成岭侧成峰，考虑对这 $\mathcal{O}\left(\dfrac{n}{u^2}\right)$ 个不同的整数值，分别计算有多少个 $v$。

推荐读者先看 [数论分块](https://oi-wiki.org/math/number-theory/sqrt-decomposition/) 中的图，先理解怎么算 $\displaystyle\sum_{i=1}^{n} \left\lfloor\dfrac{n}{i}\right\rfloor$，再理解更复杂的 $\left\lfloor\dfrac{2n}{u^2+v^2}\right\rfloor$。

外层循环枚举 $u=3,5,7,\ldots$ 内层循环用数论分块枚举 $v$。

设闭区间 $[L,R]$ 内的 $v$，对应的 $\left\lfloor\dfrac{2n}{u^2+v^2}\right\rfloor$ 都相等。

如果已知 $L$，如何计算 $R$？一开始 $L=1$，只要我们算出了 $R$，那么下一个区间的左端点 $L'$ 就是 $R+1$。再用同样的方法根据 $L'$ 算出 $R'$，得到下下个区间的左端点 $L'' = R'+1$，依此类推，用这个方法可以算出后续所有区间。

设 $\textit{num} = \left\lfloor\dfrac{2n}{u^2+L^2}\right\rfloor$。

由于 $\dfrac{2n}{u^2+v^2}$ 的整数部分是 $\textit{num}$，所以 

$$
\dfrac{2n}{u^2+v^2}\ge \textit{num}
$$

即

$$
(u^2+v^2)\cdot \textit{num}\le 2n
$$

在正整数情况下，上式等价于

$$
u^2+v^2\le \left\lfloor\dfrac{2n}{\textit{num}}\right\rfloor
$$

解得

$$
v\le \sqrt{\left\lfloor\dfrac{2n}{\textit{num}}\right\rfloor - u^2}
$$

由于 $v < u$，所以还要与 $u-1$ 取最小值，最终得到

$$
R = \min\left(\left\lfloor \sqrt{\left\lfloor\dfrac{2n}{\textit{num}}\right\rfloor - u^2} \right\rfloor,u-1\right)
$$

### 互质个数

对于 $[L,R]$ 内的 $v$，还要满足 $v$ 是奇数，且 $v$ 与 $u$ 互质。

现在问题变成：

- 给定正奇数 $u$ 和整数 $L,R$，计算 $[L,R]$ 内的与 $u$ 互质的奇数个数。

用 $[1,R]$ 的答案，减去 $[1,L-1]$ 的答案，就是 $[L,R]$ 的答案。

$[1,R]$ 内的与 $u$ 互质的奇数个数，等于 $[1,R]$ 内的与 $u$ 互质的整数个数，减去 $[1,R]$ 内的与 $u$ 互质的偶数个数。对于后者，设偶数为 $2k$，由于 $u$ 是奇数（不含质因子 $2$），所以 $\gcd(2k,u) = \gcd(k,u)$，我们只需考虑 $k$ 与 $u$ 是否互质。由于 $2k\le R$，所以 $k\le \lfloor\frac{R}{2}\rfloor$。所以 $[1,R]$ 内的与 $u$ 互质的偶数个数，等于 $[1,\lfloor\frac{R}{2}\rfloor]$ 内的与 $u$ 互质的整数个数。

设 $n$ 以内的与 $u$ 互质的正整数个数为 $f(n,u)$。那么 $[1,R]$ 的答案就是 $f(R,u) - f(\lfloor\frac{R}{2}\rfloor, u)$。

### 容斥原理

计算 $n$ 以内的与 $u$ 互质的正整数个数 $f(n,u)$。

例如 $u=15$。与 $15$ 互质的数，不能包含质因子 $3$ 和 $5$。

- $[1,n]$ 中有 $n$ 个数。
- 其中有 $\left\lfloor\dfrac{n}{3}\right\rfloor$ 个数是 $3$ 的倍数，这些数不与 $15$ 互质，减掉。
- 其中有 $\left\lfloor\dfrac{n}{5}\right\rfloor$ 个数是 $5$ 的倍数，这些数不与 $15$ 互质，减掉。
- 其中有 $\left\lfloor\dfrac{n}{15}\right\rfloor$ 个数既是 $3$ 的倍数，又是 $5$ 的倍数（即 $15$ 的倍数），我们多减了一次，加回来。

所以

$$
f(n,15) = n - \left\lfloor\dfrac{n}{3}\right\rfloor - \left\lfloor\dfrac{n}{5}\right\rfloor + \left\lfloor\dfrac{n}{15}\right\rfloor
$$

一般地，设 $u$ 的质因子集合为 $P$，由容斥原理可得

$$
f(n,u) = \sum_{S\subseteq P} (-1)^{|S|} \left\lfloor\dfrac{n}{\prod S}\right\rfloor
$$

用莫比乌斯函数表示就是

$$
f(n,u) = \sum_{d|u} \mu(d) \left\lfloor\dfrac{n}{d}\right\rfloor
$$

其中莫比乌斯函数为

$$
\mu(n)=
\begin{cases}
1, & n=1\\
(-1)^k, & n = p_1p_2\dots p_k\\
0, & n\ 有大于\ 1\ 的平方因子
\end{cases}
$$

> 注：也可以用莫比乌斯反演推导 $f(n,u)$ 的式子。

```py [sol-Python3]
MX = isqrt(500) + 1

# 预处理莫比乌斯函数
# 当 n > 1 时，sum_{d|n} mu[d] = 0
# 所以 mu[n] = -sum_{d|n ∧ d<n} mu[d]
mu = [0] * MX
mu[1] = 1
for i in range(1, MX):
    for j in range(i * 2, MX, i):
        mu[j] -= mu[i]  # i 是 j 的真因子

# 预处理不含平方因子的因子列表，用于 count_coprime
divisors = [[] for _ in range(MX)]
for i in range(1, MX):
    if mu[i]:
        for j in range(i, MX, i):
            divisors[j].append(i)  # i 是 j 的因子，且 mu[i] != 0

# 返回 [1,n] 中与 x 互质的整数个数
def count_coprime(n: int, x: int) -> int:
    return sum(mu[d] * (n // d) for d in divisors[x])

# 返回 [1,n] 中与奇数 x 互质的奇数个数
# 与 x 互质的整数个数 - 与 x 互质的偶数个数
def count_coprime_odd(n: int, x: int) -> int:
    return count_coprime(n, x) - count_coprime(n // 2, x)

class Solution:
    def countTriples(self, n: int) -> int:
        ans = 0
        u = 3
        while u * u < n * 2:
            l = 1
            while l < u and u * u + l * l <= n * 2:
                num = n * 2 // (u * u + l * l)
                # 对于 [l,r] 中的整数 v，2n // (u^2 + v^2) 都等于 num
                r = min(isqrt(n * 2 // num - u * u), u - 1)  # 推导过程见题解
                # 只有与 u 互质的奇数 v 才能得到本原勾股数组
                num_coprime_odd_v = count_coprime_odd(r, u) - count_coprime_odd(l - 1, u)
                ans += num * num_coprime_odd_v
                l = r + 1
            u += 2
        return ans * 2  # (a,b,c) 和 (b,a,c) 各算一次
```

```java [sol-Java]
class Solution {
    private static final int MX = 23; // floor(sqrt(250 * 2)) + 1
    private static final int[] mu = new int[MX];
    private static final List<Integer>[] divisors = new ArrayList[MX];

    static {
        // 预处理莫比乌斯函数
        // 当 n > 1 时，sum_{d|n} mu[d] = 0
        // 所以 mu[n] = -sum_{d|n ∧ d<n} mu[d]
        mu[1] = 1;
        for (int i = 1; i < MX; i++) {
            for (int j = i * 2; j < MX; j += i) {
                mu[j] -= mu[i]; // i 是 j 的真因子
            }
        }

        // 预处理不含平方因子的因子列表，用于 countCoprime
        Arrays.setAll(divisors, _ -> new ArrayList<>());
        for (int i = 1; i < MX; i++) {
            if (mu[i] == 0) {
                continue;
            }
            for (int j = i; j < MX; j += i) {
                divisors[j].add(i); // i 是 j 的因子，且 mu[i] != 0
            }
        }
    }

    public int countTriples(int n) {
        int ans = 0;
        for (int u = 3; u * u < n * 2; u += 2) {
            for (int l = 1, r; l < u && u * u + l * l <= n * 2; l = r + 1) {
                int num = n * 2 / (u * u + l * l);
                // 对于 [l,r] 中的整数 v，floor(2n / (u^2 + v^2)) 都等于 num
                r = Math.min((int) Math.sqrt(n * 2 / num - u * u), u - 1); // 推导过程见题解
                // 只有与 u 互质的奇数 v 才能得到本原勾股数组
                int numCoprimeOddV = countCoprimeOdd(r, u) - countCoprimeOdd(l - 1, u);
                ans += num * numCoprimeOddV;
            }
        }
        return ans * 2; // (a,b,c) 和 (b,a,c) 各算一次
    }

    // 返回 [1,n] 中与奇数 x 互质的奇数个数
    // 与 x 互质的整数个数 - 与 x 互质的偶数个数
    private int countCoprimeOdd(int n, int x) {
        return countCoprime(n, x) - countCoprime(n / 2, x);
    }

    // 返回 [1,n] 中与 x 互质的整数个数
    private int countCoprime(int n, int x) {
        int res = 0;
        for (int d : divisors[x]) {
            res += mu[d] * (n / d);
        }
        return res;
    }
}
```

```cpp [sol-C++]
constexpr int MX = 23; // floor(sqrt(250 * 2)) + 1
int mu[MX];
vector<int> divisors[MX];

int init = [] {
    // 预处理莫比乌斯函数
    // 当 n > 1 时，sum_{d|n} mu[d] = 0
    // 所以 mu[n] = -sum_{d|n ∧ d<n} mu[d]
    mu[1] = 1;
    for (int i = 1; i < MX; i++) {
        for (int j = i * 2; j < MX; j += i) {
            mu[j] -= mu[i]; // i 是 j 的真因子
        }
    }

    // 预处理不含平方因子的因子列表，用于 count_coprime
    for (int i = 1; i < MX; i++) {
        if (mu[i]) {
            for (int j = i; j < MX; j += i) {
                divisors[j].push_back(i); // i 是 j 的因子，且 mu[i] != 0
            }
        }
    }
    return 0;
}();

class Solution {
    // 返回 [1,n] 中与 x 互质的整数个数
    int count_coprime(int n, int x) {
        int res = 0;
        for (int d : divisors[x]) {
            res += mu[d] * (n / d);
        }
        return res;
    }

    // 返回 [1,n] 中与奇数 x 互质的奇数个数
    // 与 x 互质的整数个数 - 与 x 互质的偶数个数
    int count_coprime_odd(int n, int x) {
        return count_coprime(n, x) - count_coprime(n / 2, x);
    }

public:
    int countTriples(int n) {
        int ans = 0;
        for (int u = 3; u * u < n * 2; u += 2) {
            for (int l = 1, r; l < u && u * u + l * l <= n * 2; l = r + 1) {
                int num = n * 2 / (u * u + l * l);
                // 对于 [l,r] 中的整数 v，floor(2n / (u^2 + v^2)) 都等于 num
                r = min((int) sqrt(n * 2 / num - u * u), u - 1); // 推导过程见题解
                // 只有与 u 互质的奇数 v 才能得到本原勾股数组
                int num_coprime_odd_v = count_coprime_odd(r, u) - count_coprime_odd(l - 1, u);
                ans += num * num_coprime_odd_v;
            }
        }
        return ans * 2; // (a,b,c) 和 (b,a,c) 各算一次
    }
};
```

```go [sol-Go]
const mx = 23 // floor(sqrt(250 * 2)) + 1
var mu = [mx]int{1: 1}
var divisors [mx][]int

func init() {
	// 预处理莫比乌斯函数
	// 当 n > 1 时，sum_{d|n} mu[d] = 0
	// 所以 mu[n] = -sum_{d|n ∧ d<n} mu[d]
	for i := 1; i < mx; i++ {
		for j := i * 2; j < mx; j += i {
			mu[j] -= mu[i] // i 是 j 的真因子
		}
	}

	// 预处理不含平方因子的因子列表，用于 countCoprime
	for i := 1; i < mx; i++ {
		if mu[i] == 0 {
			continue
		}
		for j := i; j < mx; j += i {
			divisors[j] = append(divisors[j], i) // i 是 j 的因子，且 mu[i] != 0
		}
	}
}

// 返回 [1,n] 中与 x 互质的整数个数
func countCoprime(n, x int) (res int) {
	for _, d := range divisors[x] {
		res += mu[d] * (n / d)
	}
	return
}

// 返回 [1,n] 中与奇数 x 互质的奇数个数
// 与 x 互质的整数个数 - 与 x 互质的偶数个数
func countCoprimeOdd(n, x int) (res int) {
	return countCoprime(n, x) - countCoprime(n/2, x)
}

func countTriples(n int) (ans int) {
	for u := 3; u*u < n*2; u += 2 {
		for l, r := 1, 0; l < u && u*u+l*l <= n*2; l = r + 1 {
			num := n * 2 / (u*u + l*l)
			// 对于 [l,r] 中的整数 v，floor(2n / (u^2 + v^2)) 都等于 num
			r = min(int(math.Sqrt(float64(n*2/num-u*u))), u-1) // 推导过程见题解
			// 只有与 u 互质的奇数 v 才能得到本原勾股数组
			numCoprimeOddV := countCoprimeOdd(r, u) - countCoprimeOdd(l-1, u)
			ans += num * numCoprimeOddV
		}
	}
	return ans * 2 // (a,b,c) 和 (b,a,c) 各算一次
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 复杂度分析

> 预处理的时空复杂度为 $\mathcal{O}(\sqrt N\log N)$，$N=500$，不计入。

- 时间复杂度：$\mathcal{O}(n^{2/3} \log n)$。为方便分析，这里省略 $2n$ 中的常系数 $2$（不影响时间复杂度）。当 $u\le \sqrt[3] n$ 时，枚举 $u$ 和 $v$ 是 $\mathcal{O}(n^{2/3})$ 的。当 $u > \sqrt[3] n$ 时，对于固定的 $u$，$\left\lfloor\dfrac{n}{u^2+v^2}\right\rfloor$ 有 $\mathcal{O}\left(\dfrac{n}{u^2}\right)$ 种不同的整数值。用积分估计，$\displaystyle\sum_{u=n^{1/3}}^{n^{1/2}} \dfrac{n}{u^2}\approx \displaystyle \int_{n^{1/3}}^{n^{1/2}} \dfrac{n}{u^2} \, \mathrm{d}u = n^{2/3} - n^{1/2}$，所以当 $u > \sqrt[3] n$ 时的循环次数为 $\mathcal{O}(n^{2/3})$。由于 $\sqrt n$ 以内正整数的平均因子个数是 $\mathcal{O}(\log n)$ 的（即使只考虑 $\mu(d)\ne 0$ 的因子，平均因子个数仍然是 $\mathcal{O}(\log n)$ 的），所以总的时间复杂度为 $\mathcal{O}(n^{2/3} \log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

**注**：进一步地，考虑根号分解，取阈值 $B =  \sqrt[3] {n\ln n} $，当 $u\le B$ 时使用方法二，当 $u>B$ 时使用方法三，可以做到 $\mathcal{O}\left((n\log n)^{2/3}\right)$ 的时间复杂度。

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

本题来自 `一、数论 / §1.10 数论分块`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.10 数论分块`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
