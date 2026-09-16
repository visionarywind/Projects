# 3850. 统计结果等于 K 的序列数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-sequences-to-k/
- 题目 slug：`count-sequences-to-k`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.4 生成函数（母函数）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-sequences-to-k/solutions/3906202/zhi-yin-zi-fen-jie-ji-yi-hua-sou-suo-pyt-mb7w/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[记忆化搜索：三种写法（Python/Java/C++/Go）](https://leetcode.cn/problems/count-sequences-to-k/solutions/3906202/zhi-yin-zi-fen-jie-ji-yi-hua-sou-suo-pyt-mb7w/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-yin-zi-fen-jie-ji-yi-hua-sou-suo-pyt-mb7w`
- topic id：`3906202`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

在每个下标 $i$ 处，我们有 $3$ 种操作，所以一共有 $3^n\le 3^{19} = 1162261467$ 个不同的操作组合。但这里面有很多重复的搜索。比如 $\textit{nums} = [2,2,\ldots]$，乘以第一个 $2$，除以第二个 $2$，或者除以第一个 $2$，乘以第二个 $2$，或者两个 $2$ 都不选，都会得到 $\textit{val} = 1$。

所以可以用**记忆化搜索**优化。原理请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

用分数 $\dfrac{p}{q}$ 表示 $\textit{val}$。从初始值 $p = q = 1$ 开始，目标是 $\dfrac{p}{q} = k$。

```py [sol-Python3]
class Solution:
    def countSequences(self, nums: List[int], k: int) -> int:
        @cache
        def dfs(i: int, p: int, q: int) -> int:
            if i < 0:
                # p / q == k 等价于 p == q * k
                return 1 if p == q * k else 0

            res1 = dfs(i - 1, p * nums[i], q)  # 乘以 nums[i]
            res2 = dfs(i - 1, p, q * nums[i])  # 除以 nums[i]
            res3 = dfs(i - 1, p, q)  # 不变
            return res1 + res2 + res3

        return dfs(len(nums) - 1, 1, 1)  # 从 1/1 开始，目标是变成 k/1
```

```java [sol-Java]
class Solution {
    private record Args(int i, long p, long q) {
    }

    public int countSequences(int[] nums, long k) {
        Map<Args, Integer> memo = new HashMap<>();
        return dfs(nums.length - 1, 1, 1, nums, k, memo); // 从 1/1 开始，目标是变成 k/1
    }

    private int dfs(int i, long p, long q, int[] nums, long k, Map<Args, Integer> memo) {
        if (i < 0) {
            // 不能直接写 p == q * k，乘法会溢出
            return p % q == 0 && p / q == k ? 1 : 0;
        }

        Args t = new Args(i, p, q);
        Integer cachedRes = memo.get(t);
        if (cachedRes != null) {
            return cachedRes;
        }

        int res1 = dfs(i - 1, p * nums[i], q, nums, k, memo); // 乘以 nums[i]
        int res2 = dfs(i - 1, p, q * nums[i], nums, k, memo); // 除以 nums[i]
        int res3 = dfs(i - 1, p, q, nums, k, memo); // 不变
        int res = res1 + res2 + res3;

        memo.put(t, res);
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countSequences(vector<int>& nums, long long k) {
        // 用 unordered_map 的写法见另一份代码【C++ 自定义哈希】
        map<tuple<int, long long, long long>, int> memo;

        auto dfs = [&](this auto&& dfs, int i, long long p, long long q) -> int {
            if (i < 0) {
                // 不能直接写 p == q * k，乘法会溢出
                return p % q == 0 && p / q == k;
            }

            auto t = tuple(i, p, q);
            auto it = memo.find(t);
            if (it != memo.end()) {
                return it->second;
            }

            int res1 = dfs(i - 1, p * nums[i], q); // 乘以 nums[i]
            int res2 = dfs(i - 1, p, q * nums[i]); // 除以 nums[i]
            int res3 = dfs(i - 1, p, q); // 不变
            int res = res1 + res2 + res3;

            memo[t] = res;
            return res;
        };

        return dfs(nums.size() - 1, 1, 1); // 从 1/1 开始，目标是变成 k/1
    }
};
```

```cpp [sol-C++ 自定义哈希]
struct TupleHash {
    template<typename T>
    static void hash_combine(size_t& seed, const T& v) {
        // 参考 boost::hash_combine
        seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename Tuple, size_t Index = 0>
    static void hash_tuple(size_t& seed, const Tuple& t) {
        if constexpr (Index < tuple_size_v<Tuple>) {
            hash_combine(seed, get<Index>(t));
            hash_tuple<Tuple, Index + 1>(seed, t);
        }
    }

    template<typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const {
        size_t seed = 0;
        hash_tuple(seed, t);
        return seed;
    }
};

class Solution {
public:
    int countSequences(vector<int>& nums, long long k) {
        unordered_map<tuple<int, long long, long long>, int, TupleHash> memo;

        auto dfs = [&](this auto&& dfs, int i, long long p, long long q) -> int {
            if (i < 0) {
                // 不能直接写 p == q * k，乘法会溢出
                return p % q == 0 && p / q == k;
            }

            auto t = tuple(i, p, q);
            auto it = memo.find(t);
            if (it != memo.end()) {
                return it->second;
            }

            int res1 = dfs(i - 1, p * nums[i], q); // 乘以 nums[i]
            int res2 = dfs(i - 1, p, q * nums[i]); // 除以 nums[i]
            int res3 = dfs(i - 1, p, q); // 不变
            int res = res1 + res2 + res3;

            memo[t] = res;
            return res;
        };

        return dfs(nums.size() - 1, 1, 1); // 从 1/1 开始，目标是变成 k/1
    }
};
```

```go [sol-Go]
func countSequences(nums []int, k int64) int {
	type args struct{ i, p, q int }
	memo := map[args]int{}

	var dfs func(int, int, int) int
	dfs = func(i, p, q int) int {
		if i < 0 {
			// 不能直接写 p == q * k，乘法会溢出
			if p%q == 0 && p/q == int(k) {
				return 1
			}
			return 0
		}

		t := args{i, p, q}
		if res, ok := memo[t]; ok {
			return res
		}

		res1 := dfs(i-1, p*nums[i], q) // 乘以 nums[i]
		res2 := dfs(i-1, p, q*nums[i]) // 除以 nums[i]
		res3 := dfs(i-1, p, q)         // 不变
		res := res1 + res2 + res3

		memo[t] = res
		return res
	}

	return dfs(len(nums)-1, 1, 1) // 从 1/1 开始，目标是变成 k/1
}
```

#### 复杂度分析

状态个数：注意本题 $\textit{nums}[i]\le 6$，只包含质因子 $2,3,5$。考虑这样的极端情况：$\textit{nums}$ 包含 $\dfrac{n}{3}$ 个 $2,3,5$。对于 $2$ 来说，假设有 $x$ 个 $2$ 放在分子上，有 $y$ 个 $2$ 放在分母上，有 $z$ 个 $2$ 没有选，那么有 $x+y+z = \dfrac{n}{3}$。根据 [图解：多重集组合数](https://leetcode.cn/problems/count-ways-to-make-array-with-product/solutions/2713481/tu-jie-zhi-yin-zi-fen-jie-fang-qiu-wen-t-fboo/)，这个不定方程的非负整数解 $(x,y,z)$ 的个数，等价于把 $\dfrac{n}{3}$ 个相同小球放入 $3$ 个不同的盒子，允许空盒的方案数，即 $C(n/3 + 2,2)$，可以粗略地认为是 $\dfrac{n^2}{18}$。那么 $\dfrac{n}{3}$ 个 $2,3,5$ 约可以得到 $\left(\dfrac{n^2}{18}\right)^3 = \dfrac{n^6}{5832}$ 个不同的**未约分**的分数。再算上 $\mathcal{O}(n)$ 个参数 $i$，可得状态个数为 $\mathcal{O}(n^7)$。由于常数非常小，足以让 $\mathcal{O}(n^7)$ 的算法通过。

- 时间复杂度：$\mathcal{O}(n^7)$，其中 $n$ 是 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^7)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n^7)$。
- 空间复杂度：$\mathcal{O}(n^7)$。保存多少状态，就需要多少空间。

## 约分

用**最简分数** $\dfrac{p}{q}$ 表示 $\textit{val}$。在算完乘除后，如果 $p$ 和 $q$ 不互质，要约分，把 $p$ 和 $q$ 都除以 $\gcd(p,q)$，从而让 $p$ 和 $q$ 互质。

```py [sol-Python3]
class Solution:
    def countSequences(self, nums: List[int], k: int) -> int:
        @cache
        def dfs(i: int, p: int, q: int) -> int:
            if i < 0:
                return 1 if p == k and q == 1 else 0

            x = nums[i]
            g = gcd(p * x, q)
            res1 = dfs(i - 1, p * x // g, q // g)  # 乘以 nums[i]
            g = gcd(p, q * x)
            res2 = dfs(i - 1, p // g, q * x // g)  # 除以 nums[i]
            res3 = dfs(i - 1, p, q)  # 不变
            return res1 + res2 + res3

        return dfs(len(nums) - 1, 1, 1)  # 从 1/1 开始，目标是变成 k/1
```

```java [sol-Java]
class Solution {
    private record Args(int i, long p, long q) {
    }

    public int countSequences(int[] nums, long k) {
        Map<Args, Integer> memo = new HashMap<>();
        return dfs(nums.length - 1, 1, 1, nums, k, memo); // 从 1/1 开始，目标是变成 k/1
    }

    private int dfs(int i, long p, long q, int[] nums, long k, Map<Args, Integer> memo) {
        if (i < 0) {
            return p == k && q == 1 ? 1 : 0;
        }

        Args t = new Args(i, p, q);
        Integer cachedRes = memo.get(t);
        if (cachedRes != null) {
            return cachedRes;
        }

        int x = nums[i];
        long g = gcd(p * x, q);
        int res1 = dfs(i - 1, p * x / g, q / g, nums, k, memo); // 乘以 nums[i]
        g = gcd(p, q * x);
        int res2 = dfs(i - 1, p / g, q * x / g, nums, k, memo); // 除以 nums[i]
        int res3 = dfs(i - 1, p, q, nums, k, memo); // 不变
        int res = res1 + res2 + res3;

        memo.put(t, res);
        return res;
    }

    private long gcd(long a, long b) {
        while (a != 0) {
            long tmp = a;
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
    int countSequences(vector<int>& nums, long long k) {
        // 用 unordered_map 的写法见另一份代码【C++ 自定义哈希】
        map<tuple<int, long long, long long>, int> memo;

        auto dfs = [&](this auto&& dfs, int i, long long p, long long q) -> int {
            if (i < 0) {
                return p == k && q == 1;
            }

            auto t = tuple(i, p, q);
            auto it = memo.find(t);
            if (it != memo.end()) {
                return it->second;
            }

            int x = nums[i];
            long long g = gcd(p * x, q);
            int res1 = dfs(i - 1, p * x / g, q / g); // 乘以 nums[i]
            g = gcd(p, q * x);
            int res2 = dfs(i - 1, p / g, q * x / g); // 除以 nums[i]
            int res3 = dfs(i - 1, p, q); // 不变
            int res = res1 + res2 + res3;

            memo[t] = res;
            return res;
        };

        return dfs(nums.size() - 1, 1, 1); // 从 1/1 开始，目标是变成 k/1
    }
};
```

```cpp [sol-C++ 自定义哈希]
struct TupleHash {
    template<typename T>
    static void hash_combine(size_t& seed, const T& v) {
        // 参考 boost::hash_combine
        seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename Tuple, size_t Index = 0>
    static void hash_tuple(size_t& seed, const Tuple& t) {
        if constexpr (Index < tuple_size_v<Tuple>) {
            hash_combine(seed, get<Index>(t));
            hash_tuple<Tuple, Index + 1>(seed, t);
        }
    }

    template<typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const {
        size_t seed = 0;
        hash_tuple(seed, t);
        return seed;
    }
};

class Solution {
public:
    int countSequences(vector<int>& nums, long long k) {
        unordered_map<tuple<int, long long, long long>, int, TupleHash> memo;

        auto dfs = [&](this auto&& dfs, int i, long long p, long long q) -> int {
            if (i < 0) {
                return p == k && q == 1;
            }

            auto t = tuple(i, p, q);
            auto it = memo.find(t);
            if (it != memo.end()) {
                return it->second;
            }

            int x = nums[i];
            long long g = gcd(p * x, q);
            int res1 = dfs(i - 1, p * x / g, q / g); // 乘以 nums[i]
            g = gcd(p, q * x);
            int res2 = dfs(i - 1, p / g, q * x / g); // 除以 nums[i]
            int res3 = dfs(i - 1, p, q); // 不变
            int res = res1 + res2 + res3;

            memo[t] = res;
            return res;
        };

        return dfs(nums.size() - 1, 1, 1); // 从 1/1 开始，目标是变成 k/1
    }
};
```

```go [sol-Go]
func countSequences(nums []int, k int64) int {
	type args struct{ i, p, q int }
	memo := map[args]int{}

	var dfs func(int, int, int) int
	dfs = func(i, p, q int) int {
		if i < 0 {
			if p == int(k) && q == 1 {
				return 1
			}
			return 0
		}

		t := args{i, p, q}
		if res, ok := memo[t]; ok {
			return res
		}

		x := nums[i]
		g := gcd(p*x, q)
		res2 := dfs(i-1, p*x/g, q/g) // 乘以 nums[i]
		g = gcd(p, q*x)
		res1 := dfs(i-1, p/g, q*x/g) // 除以 nums[i]
		res3 := dfs(i-1, p, q)       // k 不变
		res := res1 + res2 + res3

		memo[t] = res
		return res
	}

	return dfs(len(nums)-1, 1, 1) // 从 1/1 开始，目标是变成 k/1
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}
```

#### 复杂度分析

状态个数：见后文「质因数分解优化」。

- 时间复杂度：$\mathcal{O}(n^5\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})\le 6$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^4)$，单个状态的计算时间瓶颈在计算 $\gcd$ 上，复杂度为 $\mathcal{O}(\log(U^n)) = \mathcal{O}(n\log U)$，所以总的时间复杂度为 $\mathcal{O}(n^5\log U)$。
- 空间复杂度：$\mathcal{O}(n^4)$。保存多少状态，就需要多少空间。

## 质因数分解优化

如果对 $\textit{nums}[i]$ 做质因数分解，分解出的 $2,3,5$ 的个数均为 $\mathcal{O}(1)$，所以计算乘除后，最终结果的质因数分解中的 $2,3,5$ 的幂次是 $\mathcal{O}(n)$ 的。所以这 $3^n$ 个不同的操作，只会得到 $\mathcal{O}(n^3)$ 个不同的 $\textit{val}$。本题涉及到分数，这里把分母的质因子的幂次视作负数，例如 $\dfrac{1}{8} = 2^{-3}$，$\dfrac{25}{6} = 2^{-1}3^{-1}5^2$。

考虑记忆化搜索，从 $k$ 和 $n-1$ 开始，倒着乘除，目标是得到 $1$，即质因数分解中的 $2,3,5$ 的幂次均为 $0$。

$\textit{dfs}$ 需要 $4$ 个参数 $i,e_2,e_3,e_5$，分别表示当前要考虑下标 $i$ 怎么操作，以及当前 $k$ 的质因数分解中的 $2,3,5$ 的幂次。

设 $\textit{nums}[i]$ 的质因数分解中有 $x,y,z$ 个 $2,3,5$，那么有如下三种情况：

- 把 $k$ 除以 $\textit{nums}[i]$，$e_2,e_3,e_5$ 分别减少了 $x,y,z$。
- 把 $k$ 乘以 $\textit{nums}[i]$，$e_2,e_3,e_5$ 分别增加了 $x,y,z$。
- 不变。

三种情况继续递归，方案数累加，即为 $\textit{dfs}$ 的返回值。

**递归边界**：当 $i<0$ 时，如果 $e_2 = e_3 = e_5 = 0$，说明此时 $k=1$，我们找到了一个合法的操作组合，返回 $1$；否则返回 $0$。

**递归入口**：$\textit{dfs}(n-1,E_2,E_3,E_5)$，其中 $E_2,E_3,E_5$ 分别是初始 $k$ 的质因数分解中的 $2,3,5$ 的幂次。

**特殊情况**：如果 $k$ 包含大于 $5$ 的质因子，无解，返回 $0$。

[本题视频讲解](https://www.bilibili.com/video/BV1HeZfB7EBt/?t=22m54s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    # 返回 k 中的质因子 2,3,5 的个数，以及 k 是否只包含 <= 5 的质因子
    def primeFactorization(self, k: int) -> Tuple[Tuple[int, int, int], bool]:
        e2 = (k & -k).bit_length() - 1
        k >>= e2

        e3 = 0
        while k % 3 == 0:
            e3 += 1
            k //= 3

        e5 = 0
        while k % 5 == 0:
            e5 += 1
            k //= 5

        return (e2, e3, e5), k == 1

    def countSequences(self, nums: List[int], k: int) -> int:
        (e2, e3, e5), ok = self.primeFactorization(int(k))
        if not ok:  # k 有大于 5 的质因子
            return 0

        es = [self.primeFactorization(x)[0] for x in nums]

        @cache
        def dfs(i: int, e2: int, e3: int, e5: int) -> int:
            if i < 0:
                return 1 if e2 == e3 == e5 == 0 else 0

            x, y, z = es[i]
            res1 = dfs(i - 1, e2 - x, e3 - y, e5 - z)  # k 除以 nums[i]
            res2 = dfs(i - 1, e2 + x, e3 + y, e5 + z)  # k 乘以 nums[i]
            res3 = dfs(i - 1, e2, e3, e5)  # k 不变
            return res1 + res2 + res3

        return dfs(len(nums) - 1, e2, e3, e5)  # 从 k 开始，目标是变成 1
```

```java [sol-Java]
class Solution {
    private record Exp(int e2, int e3, int e5) {
    }

    // 返回 k 中的质因子 2,3,5 的个数，以及 k 是否只包含 <= 5 的质因子
    private Pair<Exp, Boolean> primeFactorization(long k) {
        int e2 = Long.numberOfTrailingZeros(k);
        k >>= e2;

        int e3 = 0;
        while (k % 3 == 0) {
            e3++;
            k /= 3;
        }

        int e5 = 0;
        while (k % 5 == 0) {
            e5++;
            k /= 5;
        }

        return new Pair<>(new Exp(e2, e3, e5), k == 1);
    }

    public int countSequences(int[] nums, long k) {
        Pair<Exp, Boolean> res = primeFactorization(k);
        Exp e = res.getKey();
        boolean ok = res.getValue();
        if (!ok) { // k 有大于 5 的质因子
            return 0;
        }

        int n = nums.length;
        Exp[] es = new Exp[n];
        for (int i = 0; i < n; i++) {
            es[i] = primeFactorization(nums[i]).getKey();
        }

        Map<Integer, Integer> memo = new HashMap<>();
        return dfs(n - 1, e.e2, e.e3, e.e5, es, memo);
    }

    private int dfs(int i, int e2, int e3, int e5, Exp[] es, Map<Integer, Integer> memo) {
        if (i < 0) {
            return e2 == 0 && e3 == 0 && e5 == 0 ? 1 : 0; // k 变成 1
        }

        // 把 i,e2,e3,e5 拼成一个 int（每个数至多 7 位）
        int n = es.length;
        int key = i << 21 | (e2 + n * 2) << 14 | (e3 + n) << 7 | (e5 + n);
        if (memo.containsKey(key)) {
            return memo.get(key);
        }

        Exp e = es[i];
        int x = e.e2, y = e.e3, z = e.e5;
        int res1 = dfs(i - 1, e2 - x, e3 - y, e5 - z, es, memo); // k 除以 nums[i]
        int res2 = dfs(i - 1, e2 + x, e3 + y, e5 + z, es, memo); // k 乘以 nums[i]
        int res3 = dfs(i - 1, e2, e3, e5, es, memo); // k 不变
        int res = res1 + res2 + res3;

        memo.put(key, res);
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 返回 k 中的质因子 2,3,5 的个数，以及 k 是否只包含 <= 5 的质因子
    pair<tuple<int, int, int>, bool> primeFactorization(long long k) {
        int e2 = countr_zero((uint64_t) k);
        k >>= e2;

        int e3 = 0;
        while (k % 3 == 0) {
            e3++;
            k /= 3;
        }

        int e5 = 0;
        while (k % 5 == 0) {
            e5++;
            k /= 5;
        }

        return {tuple(e2, e3, e5), k == 1};
    }

public:
    int countSequences(vector<int>& nums, long long k) {
        auto [e, ok] = primeFactorization(k);
        if (!ok) { // k 有大于 5 的质因子
            return 0;
        }

        int n = nums.size();
        vector<tuple<int, int, int>> es(n);
        for (int i = 0; i < n; i++) {
            es[i] = primeFactorization(nums[i]).first;
        }

        unordered_map<int, int> memo;

        auto dfs = [&](this auto&& dfs, int i, int e2, int e3, int e5) -> int {
            if (i < 0) {
                return e2 == 0 && e3 == 0 && e5 == 0; // k 变成 1
            }

            // 把 i,e2,e3,e5 拼成一个 int（每个数至多 7 位）
            int key = i << 21 | (e2 + n * 2) << 14 | (e3 + n) << 7 | (e5 + n);
            auto it = memo.find(key);
            if (it != memo.end()) {
                return it->second;
            }

            auto [x, y, z] = es[i];
            int res1 = dfs(i - 1, e2 - x, e3 - y, e5 - z); // k 除以 nums[i]
            int res2 = dfs(i - 1, e2 + x, e3 + y, e5 + z); // k 乘以 nums[i]
            int res3 = dfs(i - 1, e2, e3, e5); // k 不变
            int res = res1 + res2 + res3;

            memo[key] = res;
            return res;
        };

        auto [e2, e3, e5] = e;
        return dfs(n - 1, e2, e3, e5); // 从 k 开始，目标是变成 1
    }
};
```

```go [sol-Go]
// 返回 k 中的质因子 2,3,5 的个数，以及 k 是否只包含 <= 5 的质因子
func primeFactorization(k int) ([3]int, bool) {
	e2 := bits.TrailingZeros(uint(k))
	k >>= e2

	e3 := 0
	for k%3 == 0 {
		e3++
		k /= 3
	}

	e5 := 0
	for k%5 == 0 {
		e5++
		k /= 5
	}

	return [3]int{e2, e3, e5}, k == 1
}

func countSequences(nums []int, k int64) int {
	e, ok := primeFactorization(int(k))
	if !ok { // k 有大于 5 的质因子
		return 0
	}

	n := len(nums)
	es := make([][3]int, n)
	for i, x := range nums {
		es[i], _ = primeFactorization(x)
	}

	type args struct{ i, e2, e3, e5 int }
	memo := map[args]int{}

	var dfs func(int, int, int, int) int
	dfs = func(i, e2, e3, e5 int) int {
		if i < 0 {
			if e2 == 0 && e3 == 0 && e5 == 0 { // k 变成 1
				return 1
			}
			return 0
		}
		p := args{i, e2, e3, e5}
		if res, ok := memo[p]; ok {
			return res
		}

		e := es[i]
		res1 := dfs(i-1, e2-e[0], e3-e[1], e5-e[2]) // k 除以 nums[i]
		res2 := dfs(i-1, e2+e[0], e3+e[1], e5+e[2]) // k 乘以 nums[i]
		res3 := dfs(i-1, e2, e3, e5)                // k 不变
		res := res1 + res2 + res3

		memo[p] = res
		return res
	}

	return dfs(n-1, e[0], e[1], e[2]) // 从 k 开始，目标是变成 1
}
```

#### 复杂度分析

状态个数：由于 $\textit{nums}[i]\le 6$，$\textit{nums}[i]$ 分解出的质因子 $2,3,5$ 的个数均为 $\mathcal{O}(1)$，所以参数 $i,e_2,e_3,e_5$ 都分别有 $\mathcal{O}(n)$ 个不同的值，一共有 $\mathcal{O}(n^4)$ 个状态。

- 时间复杂度：$\mathcal{O}(n^4 + \log k)$，其中 $n$ 是 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^4)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n^4)$。分解 $k$ 需要 $\mathcal{O}(\log k)$ 的时间。
- 空间复杂度：$\mathcal{O}(n^4)$。保存多少状态，就需要多少空间。

**注**：也可以用折半搜索解决，时间复杂度是 $\mathcal{O}(3^{n/2})$。

见下面回溯题单的「**§4.8 折半搜索**」。

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

本题来自 `二、组合数学 / §2.4 生成函数（母函数）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.4 生成函数（母函数）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
