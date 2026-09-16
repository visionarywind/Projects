# 3670. 没有公共位的整数最大乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-of-two-integers-with-no-common-bits/
- 题目 slug：`maximum-product-of-two-integers-with-no-common-bits`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：2234
- 外部题解来源：https://leetcode.cn/problems/maximum-product-of-two-integers-with-no-common-bits/solutions/3768219/mo-ban-gao-wei-qian-zhui-he-sos-dppython-78fz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：普通状压 DP / 高维前缀和（SOS DP）Python/Java/C++/Go](https://leetcode.cn/problems/maximum-product-of-two-integers-with-no-common-bits/solutions/3768219/mo-ban-gao-wei-qian-zhui-he-sos-dppython-78fz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-gao-wei-qian-zhui-he-sos-dppython-78fz`
- topic id：`3768219`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 转化

首先我们有如下暴力思路：

- 外层循环枚举 $\textit{nums}[i]$，内层循环枚举 $\textit{nums}[j]$，如果 $\textit{nums}[i]\ &\ \textit{nums}[j] = 0$，那么用 $\textit{nums}[i]\cdot \textit{nums}[j]$ 更新答案的最大值。

这个做法的时间复杂度是 $\mathcal{O}(n^2)$ 的，会超时。

怎么样更快？能不能对于每个 $\textit{nums}[i]$，求出满足 $\textit{nums}[i]\ &\ \textit{nums}[j] = 0$ 的 $\textit{nums}[j]$ 的最大值？

如何理解 $\textit{nums}[i]\ &\ \textit{nums}[j] = 0$？

根据 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)，把每个 $\textit{nums}_i$ 视作一个集合 $S_i$。

用集合思考，两个数的按位与等于 $0$，等价于两个集合 $A$ 和 $B$ 的交集为空，即 

$$
A\cap B = \varnothing
$$

这又等价于 $B$ 是 $A$ 关于全集 $U$ 的补集的子集，即

$$
B \subseteq \complement_UA
$$

枚举 $\textit{nums}_i$，为了让乘积最大，我们需要知道 $\textit{nums}_j$ 的最大值，其中 $\textit{nums}_j$ 对应的集合为 $S_j$，需要满足 $S_j \subseteq \complement_US_i$。

## 方法一：普通状压 DP

注意到，集合 $S$ 等价于如下子集的**并集**。

- 枚举 $S$ 中的元素 $i$。
- 移除 $i$，得到 $S$ 的子集 $T_i = S \setminus \{i\}$。
- 所有 $T_i$ 的并集等于 $S$。

由于 $T_i$ 是一个更小的集合，是规模更小的子问题，所以可以用 DP 解决。

定义 $f[S]$ 表示 $S$ 的所有子集的对应二进制的最大值。

计算所有 $f[T_i] = f[S \setminus \{i\}]$ 的最大值

$$
\max_{i\in S} f[S \setminus \{i\}]
$$

去更新 $f[S]$ 的最大值。

> 注意上式只包含 $S$ 的真子集，没有包含 $S$ 自身，即 $f[S]$ 的初始值。所以不能直接说 $f[S] = \max\limits_{i\in S} f[S \setminus \{i\}]$。

初始值：$f[S_i] = \textit{nums}_i$。

答案为

$$
\max_{i=0}^{n-1} \textit{nums}_i\cdot f[\complement_US_i]
$$

代码实现时，用二进制数表示集合，用位运算实现上述有关集合的运算，具体请看 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

[本题视频讲解](https://www.bilibili.com/video/BV1SMaGz7EXe/)，欢迎点赞关注~

### 写法一

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x  # 初始值

        for s in range(u):  # 从小到大枚举集合 s
            for i in range(w):  # 枚举 s 中的元素 i
                if s >> i & 1:  # i 属于集合 s
                    v = f[s ^ (1 << i)]  # 从 s 的子集 s \ {i} 转移过来
                    if v > f[s]:
                        f[s] = v  # 手写 max 更快

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }
        
        int w = 32 - Integer.numberOfLeadingZeros(mx); // mx 的二进制长度
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x; // 初始值
        }

        for (int s = 0; s < u; s++) { // 从小到大枚举集合 s
            for (int i = 0; i < w; i++) { // 枚举 s 中的元素 i
                if ((s >> i & 1) > 0) { // i 属于集合 s
                    f[s] = Math.max(f[s], f[s ^ (1 << i)]); // 从 s 的子集 s \ {i} 转移过来
                }
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x; // 初始值
        }

        for (int s = 0; s < u; s++) { // 从小到大枚举集合 s
            for (int i = 0; i < w; i++) { // 枚举 s 中的元素 i
                if (s >> i & 1) { // i 属于集合 s
                    f[s] = max(f[s], f[s ^ (1 << i)]); // 从 s 的子集 s \ {i} 转移过来
                }
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x // 初始值
	}

	for s := range f { // 从小到大枚举集合 s
		for i := range w { // 枚举 s 中的元素 i
			if s>>i&1 > 0 { // i 属于集合 s
				f[s] = max(f[s], f[s^1<<i]) // 从 s 的子集 s \ {i} 转移过来
			}
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

### 写法二（优化）

利用 $\text{lowbit}$，可以直接枚举 $s$ 的每个比特 $1$。

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x

        for s in range(u):
            t = s
            while t:
                lb = t & -t
                v = f[s ^ lb]
                if v > f[s]:
                    f[s] = v
                t ^= lb

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }
        
        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int s = 0; s < u; s++) {
            for (int t = s, lb; t > 0; t ^= lb) {
                lb = t & -t;
                f[s] = Math.max(f[s], f[s ^ lb]);
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int s = 0; s < u; s++) {
            for (int t = s, lb; t > 0; t ^= lb) {
                lb = t & -t;
                f[s] = max(f[s], f[s ^ lb]);
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for s := range f {
		for t, lb := s, 0; t > 0; t ^= lb {
			lb = t & -t
			f[s] = max(f[s], f[s^lb])
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 方法二：高维前缀和（Sum Over Subsets DP，SOS DP）

> 注：Sum Over Subsets DP 直译过来是**子集和 DP**，国内算法竞赛圈一般叫高维前缀和。

方法一其实扩展性不太好，如果改成计算每个集合 $S$ 的所有子集的元素和，由于各个 $T_i = S \setminus \{i\}$ 之间存在交集，我们会重复累加，导致计算结果比正确答案大。

例如 $S=\{1,2,3\}$，去掉 $1$ 是 $\{2,3\}$，去掉 $2$ 是 $\{1,3\}$，这两个集合都有子集 $\{3\}$，我们会把 $3$ 累加两次。

### 如何正确地划分 S 的子集

例如 $S = \{1,3,4\}$，有 $8$ 个子集，按照是否包含元素 $4$，可以分为两类：

- 包含 $4$ 的子集：$\{4\},\{1,4\},\{3,4\},\{1,3,4\}$。
- 不含 $4$ 的子集：$\varnothing,\{1\},\{3\},\{1,3\}$。

这启发我们引入如下定义：

- 定义 $\mathcal{F}(S,i)$ 表示保留 $S$ 中的大于 $i$ 的数的所有子集。或者说只有小于等于 $i$ 的数可以选或不选。

注意 $\mathcal{F}(S,i)$ 是一个集合，其中的元素也是集合。

再来看 $S = \{1,3,4\}$ 这个例子：

- 包含 $4$ 的子集就是 $\mathcal{F}(S,3) = \{\{4\},\{1,4\},\{3,4\},\{1,3,4\}\}$。
- 不含 $4$ 的子集就是 $\mathcal{F}(S\setminus \{4\},3) = \{\varnothing,\{1\},\{3\},\{1,3\}\}$。

我们成功地把 $S$ 的所有子集分成了两类，这两类都是规模更小的子问题。

### 状态定义与状态转移方程

定义 $f[i][S]$ 表示保留 $S$ 中的大于 $i$ 的数的所有子集的对应二进制的最大值。或者说只有小于等于 $i$ 的数可以选或不选。

分类讨论：

- 如果 $i\notin S$，那么问题变成保留 $S$ 中的大于 $i-1$ 的数的所有子集的对应二进制的最大值，即 $f[i][S] = f[i-1][S]$。
- 如果 $i\in S$，用「选或不选」思考：
  - 选 $i$，问题变成保留 $S$ 中的大于 $i-1$ 的数的所有子集的对应二进制的最大值，即 $f[i-1][S]$。
  - 不选 $i$，问题变成保留 $S\setminus \{i\}$ 中的大于 $i-1$ 的数的所有子集的对应二进制的最大值，即 $f[i-1][S\setminus \{i\}]$。
  - 二者取最大值，得 $f[i][S] = \max(f[i-1][S], f[i-1][S\setminus \{i\}])$。

总结：

$$
f[i][S] =
\begin{cases}
f[i-1][S], & i\notin S     \\
\max(f[i-1][S], f[i-1][S\setminus \{i\}]), & i\in S     \\
\end{cases}
$$

初始值：$f[-1][S_i] = \textit{nums}_i$。

最后 $f[w-1][S]$ 就是 $S$ 的所有子集的对应二进制的最大值。其中 $w$ 是 $\textit{nums}$ 中的最大二进制长度。

代码实现时，$f$ 的第一个维度可以优化掉。优化后，只需计算 $S$ 包含 $i$ 的状态。换句话说，计算到 $f[i][S]$ 时，我们没有覆盖 $S$ 不包含 $i$ 的状态 $f[i-1][S\setminus \{i\}]$（值没变）。这意味着**从小到大枚举 $S$ 还是从大到小枚举 $S$ 都可以**。

⚠**注意**：方法一和方法二的代码很像，只有先枚举 $s$ 还是先枚举 $i$ 的区别，然而完全不一样！如果把求最大值改成求和，方法一的状态转移有重叠，会重复计算，方法二则不会。读者可以类比 [377. 组合总和 Ⅳ](https://leetcode.cn/problems/combination-sum-iv/)（排列）和 [518. 零钱兑换 II](https://leetcode.cn/problems/coin-change-ii/)（组合）的区别，加深体会。

### 写法一

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x

        for i in range(w):
            bit = 1 << i  # 避免在循环中反复计算 1 << i
            for s in range(u):
                if s & bit:
                    v = f[s ^ bit]
                    if v > f[s]:
                        f[s] = v

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                if ((s >> i & 1) > 0) {
                    f[s] = Math.max(f[s], f[s ^ (1 << i)]);
                }
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                if (s >> i & 1) {
                    f[s] = max(f[s], f[s ^ (1 << i)]);
                }
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for i := range w {
		for s := range f {
			if s>>i&1 > 0 {
				f[s] = max(f[s], f[s^1<<i])
			}
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

### 写法二（优化）

通过把 $s$ 或上 $2^i$，快速跳到第 $i$ 位是 $1$ 的 $s$，从而减少无效枚举。相当于跳过了所有 `(s >> i & 1) == 0` 的情况。

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x

        for i in range(w):
            bit = 1 << i  # 避免在循环中反复计算 1 << i
            s = 0
            while s < u:
                s |= bit  # 快速跳到第 i 位是 1 的 s
                v = f[s ^ bit]
                if v > f[s]:
                    f[s] = v
                s += 1

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                s |= 1 << i; // 快速跳到第 i 位是 1 的 s
                f[s] = Math.max(f[s], f[s ^ (1 << i)]);
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                s |= 1 << i; // 快速跳到第 i 位是 1 的 s
                f[s] = max(f[s], f[s ^ (1 << i)]);
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for i := range w {
		for s := 0; s < u; s++ {
			s |= 1 << i // 快速跳到第 i 位是 1 的 s
			f[s] = max(f[s], f[s^1<<i])
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 附：平衡暴力和 DP 的算法

如果 $n^2 \le U\log U$，直接用暴力算。 

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w

        n = len(nums)
        if n * n <= u * w:
            # 暴力
            ans = 0
            for i, x in enumerate(nums):
                for y in nums[:i]:
                    if x & y == 0 and x * y > ans:
                        ans = x * y
            return ans

        f = [0] * u
        for x in nums:
            f[x] = x

        for i in range(w):
            bit = 1 << i  # 避免在循环中反复计算 1 << i
            s = 0
            while s < u:
                s |= bit  # 快速跳到第 i 位是 1 的 s
                v = f[s ^ bit]
                if v > f[s]:
                    f[s] = v
                s += 1

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;

        int n = nums.length;
        if (n <= u * w / n) { // 避免 n*n 溢出
            // 暴力
            long ans = 0;
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                for (int j = 0; j < i; j++) {
                    int y = nums[j];
                    if ((x & y) == 0) {
                        ans = Math.max(ans, (long) x * y);
                    }
                }
            }
            return ans;
        }

        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                s |= 1 << i; // 快速跳到第 i 位是 1 的 s
                f[s] = Math.max(f[s], f[s ^ (1 << i)]);
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
// 基于方法一
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;

        int n = nums.size();
        if (n <= u * w / n) { // 避免 n*n 溢出
            // 暴力
            long long ans = 0;
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                for (int j = 0; j < i; j++) {
                    int y = nums[j];
                    if ((x & y) == 0) {
                        ans = max(ans, 1LL * x * y);
                    }
                }
            }
            return ans;
        }

        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int s = 0; s < u; s++) {
            for (int t = s, lb; t > 0; t ^= lb) {
                lb = t & -t;
                f[s] = max(f[s], f[s ^ lb]);
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
// 基于方法一
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w

	n := len(nums)
	if n*n <= u*w {
		// 暴力
		ans := 0
		for i, x := range nums {
			for _, y := range nums[:i] {
				if x&y == 0 {
					ans = max(ans, x*y)
				}
			}
		}
		return int64(ans)
	}

	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for s := range f {
		for t, lb := s, 0; t > 0; t ^= lb {
			lb = t & -t
			f[s] = max(f[s], f[s^lb])
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(n^2, n + U\log U))$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 专题训练

见下面动态规划题单的「**§9.5 SOS DP**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 转化

首先我们有如下暴力思路：

- 外层循环枚举 $\textit{nums}[i]$，内层循环枚举 $\textit{nums}[j]$，如果 $\textit{nums}[i]\ &\ \textit{nums}[j] = 0$，那么用 $\textit{nums}[i]\cdot \textit{nums}[j]$ 更新答案的最大值。

这个做法的时间复杂度是 $\mathcal{O}(n^2)$ 的，会超时。

怎么样更快？能不能对于每个 $\textit{nums}[i]$，求出满足 $\textit{nums}[i]\ &\ \textit{nums}[j] = 0$ 的 $\textit{nums}[j]$ 的最大值？

如何理解 $\textit{nums}[i]\ &\ \textit{nums}[j] = 0$？

根据 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)，把每个 $\textit{nums}_i$ 视作一个集合 $S_i$。

用集合思考，两个数的按位与等于 $0$，等价于两个集合 $A$ 和 $B$ 的交集为空，即 

$$
A\cap B = \varnothing
$$

这又等价于 $B$ 是 $A$ 关于全集 $U$ 的补集的子集，即

$$
B \subseteq \complement_UA
$$

枚举 $\textit{nums}_i$，为了让乘积最大，我们需要知道 $\textit{nums}_j$ 的最大值，其中 $\textit{nums}_j$ 对应的集合为 $S_j$，需要满足 $S_j \subseteq \complement_US_i$。

## 方法一：普通状压 DP

注意到，集合 $S$ 等价于如下子集的**并集**。

- 枚举 $S$ 中的元素 $i$。
- 移除 $i$，得到 $S$ 的子集 $T_i = S \setminus \{i\}$。
- 所有 $T_i$ 的并集等于 $S$。

由于 $T_i$ 是一个更小的集合，是规模更小的子问题，所以可以用 DP 解决。

定义 $f[S]$ 表示 $S$ 的所有子集的对应二进制的最大值。

计算所有 $f[T_i] = f[S \setminus \{i\}]$ 的最大值

$$
\max_{i\in S} f[S \setminus \{i\}]
$$

去更新 $f[S]$ 的最大值。

> 注意上式只包含 $S$ 的真子集，没有包含 $S$ 自身，即 $f[S]$ 的初始值。所以不能直接说 $f[S] = \max\limits_{i\in S} f[S \setminus \{i\}]$。

初始值：$f[S_i] = \textit{nums}_i$。

答案为

$$
\max_{i=0}^{n-1} \textit{nums}_i\cdot f[\complement_US_i]
$$

代码实现时，用二进制数表示集合，用位运算实现上述有关集合的运算，具体请看 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

[本题视频讲解](https://www.bilibili.com/video/BV1SMaGz7EXe/)，欢迎点赞关注~

### 写法一

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x  # 初始值

        for s in range(u):  # 从小到大枚举集合 s
            for i in range(w):  # 枚举 s 中的元素 i
                if s >> i & 1:  # i 属于集合 s
                    v = f[s ^ (1 << i)]  # 从 s 的子集 s \ {i} 转移过来
                    if v > f[s]:
                        f[s] = v  # 手写 max 更快

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }
        
        int w = 32 - Integer.numberOfLeadingZeros(mx); // mx 的二进制长度
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x; // 初始值
        }

        for (int s = 0; s < u; s++) { // 从小到大枚举集合 s
            for (int i = 0; i < w; i++) { // 枚举 s 中的元素 i
                if ((s >> i & 1) > 0) { // i 属于集合 s
                    f[s] = Math.max(f[s], f[s ^ (1 << i)]); // 从 s 的子集 s \ {i} 转移过来
                }
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x; // 初始值
        }

        for (int s = 0; s < u; s++) { // 从小到大枚举集合 s
            for (int i = 0; i < w; i++) { // 枚举 s 中的元素 i
                if (s >> i & 1) { // i 属于集合 s
                    f[s] = max(f[s], f[s ^ (1 << i)]); // 从 s 的子集 s \ {i} 转移过来
                }
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x // 初始值
	}

	for s := range f { // 从小到大枚举集合 s
		for i := range w { // 枚举 s 中的元素 i
			if s>>i&1 > 0 { // i 属于集合 s
				f[s] = max(f[s], f[s^1<<i]) // 从 s 的子集 s \ {i} 转移过来
			}
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

### 写法二（优化）

利用 $\text{lowbit}$，可以直接枚举 $s$ 的每个比特 $1$。

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x

        for s in range(u):
            t = s
            while t:
                lb = t & -t
                v = f[s ^ lb]
                if v > f[s]:
                    f[s] = v
                t ^= lb

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }
        
        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int s = 0; s < u; s++) {
            for (int t = s, lb; t > 0; t ^= lb) {
                lb = t & -t;
                f[s] = Math.max(f[s], f[s ^ lb]);
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int s = 0; s < u; s++) {
            for (int t = s, lb; t > 0; t ^= lb) {
                lb = t & -t;
                f[s] = max(f[s], f[s ^ lb]);
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for s := range f {
		for t, lb := s, 0; t > 0; t ^= lb {
			lb = t & -t
			f[s] = max(f[s], f[s^lb])
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 方法二：高维前缀和（Sum Over Subsets DP，SOS DP）

> 注：Sum Over Subsets DP 直译过来是**子集和 DP**，国内算法竞赛圈一般叫高维前缀和。

方法一其实扩展性不太好，如果改成计算每个集合 $S$ 的所有子集的元素和，由于各个 $T_i = S \setminus \{i\}$ 之间存在交集，我们会重复累加，导致计算结果比正确答案大。

例如 $S=\{1,2,3\}$，去掉 $1$ 是 $\{2,3\}$，去掉 $2$ 是 $\{1,3\}$，这两个集合都有子集 $\{3\}$，我们会把 $3$ 累加两次。

### 如何正确地划分 S 的子集

例如 $S = \{1,3,4\}$，有 $8$ 个子集，按照是否包含元素 $4$，可以分为两类：

- 包含 $4$ 的子集：$\{4\},\{1,4\},\{3,4\},\{1,3,4\}$。
- 不含 $4$ 的子集：$\varnothing,\{1\},\{3\},\{1,3\}$。

这启发我们引入如下定义：

- 定义 $\mathcal{F}(S,i)$ 表示保留 $S$ 中的大于 $i$ 的数的所有子集。或者说只有小于等于 $i$ 的数可以选或不选。

注意 $\mathcal{F}(S,i)$ 是一个集合，其中的元素也是集合。

再来看 $S = \{1,3,4\}$ 这个例子：

- 包含 $4$ 的子集就是 $\mathcal{F}(S,3) = \{\{4\},\{1,4\},\{3,4\},\{1,3,4\}\}$。
- 不含 $4$ 的子集就是 $\mathcal{F}(S\setminus \{4\},3) = \{\varnothing,\{1\},\{3\},\{1,3\}\}$。

我们成功地把 $S$ 的所有子集分成了两类，这两类都是规模更小的子问题。

### 状态定义与状态转移方程

定义 $f[i][S]$ 表示保留 $S$ 中的大于 $i$ 的数的所有子集的对应二进制的最大值。或者说只有小于等于 $i$ 的数可以选或不选。

分类讨论：

- 如果 $i\notin S$，那么问题变成保留 $S$ 中的大于 $i-1$ 的数的所有子集的对应二进制的最大值，即 $f[i][S] = f[i-1][S]$。
- 如果 $i\in S$，用「选或不选」思考：
  - 选 $i$，问题变成保留 $S$ 中的大于 $i-1$ 的数的所有子集的对应二进制的最大值，即 $f[i-1][S]$。
  - 不选 $i$，问题变成保留 $S\setminus \{i\}$ 中的大于 $i-1$ 的数的所有子集的对应二进制的最大值，即 $f[i-1][S\setminus \{i\}]$。
  - 二者取最大值，得 $f[i][S] = \max(f[i-1][S], f[i-1][S\setminus \{i\}])$。

总结：

$$
f[i][S] =
\begin{cases}
f[i-1][S], & i\notin S     \\
\max(f[i-1][S], f[i-1][S\setminus \{i\}]), & i\in S     \\
\end{cases}
$$

初始值：$f[-1][S_i] = \textit{nums}_i$。

最后 $f[w-1][S]$ 就是 $S$ 的所有子集的对应二进制的最大值。其中 $w$ 是 $\textit{nums}$ 中的最大二进制长度。

代码实现时，$f$ 的第一个维度可以优化掉。优化后，只需计算 $S$ 包含 $i$ 的状态。换句话说，计算到 $f[i][S]$ 时，我们没有覆盖 $S$ 不包含 $i$ 的状态 $f[i-1][S\setminus \{i\}]$（值没变）。这意味着**从小到大枚举 $S$ 还是从大到小枚举 $S$ 都可以**。

⚠**注意**：方法一和方法二的代码很像，只有先枚举 $s$ 还是先枚举 $i$ 的区别，然而完全不一样！如果把求最大值改成求和，方法一的状态转移有重叠，会重复计算，方法二则不会。读者可以类比 [377. 组合总和 Ⅳ](https://leetcode.cn/problems/combination-sum-iv/)（排列）和 [518. 零钱兑换 II](https://leetcode.cn/problems/coin-change-ii/)（组合）的区别，加深体会。

### 写法一

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x

        for i in range(w):
            bit = 1 << i  # 避免在循环中反复计算 1 << i
            for s in range(u):
                if s & bit:
                    v = f[s ^ bit]
                    if v > f[s]:
                        f[s] = v

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                if ((s >> i & 1) > 0) {
                    f[s] = Math.max(f[s], f[s ^ (1 << i)]);
                }
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                if (s >> i & 1) {
                    f[s] = max(f[s], f[s ^ (1 << i)]);
                }
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for i := range w {
		for s := range f {
			if s>>i&1 > 0 {
				f[s] = max(f[s], f[s^1<<i])
			}
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

### 写法二（优化）

通过把 $s$ 或上 $2^i$，快速跳到第 $i$ 位是 $1$ 的 $s$，从而减少无效枚举。相当于跳过了所有 `(s >> i & 1) == 0` 的情况。

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w
        f = [0] * u
        for x in nums:
            f[x] = x

        for i in range(w):
            bit = 1 << i  # 避免在循环中反复计算 1 << i
            s = 0
            while s < u:
                s |= bit  # 快速跳到第 i 位是 1 的 s
                v = f[s ^ bit]
                if v > f[s]:
                    f[s] = v
                s += 1

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;
        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                s |= 1 << i; // 快速跳到第 i 位是 1 的 s
                f[s] = Math.max(f[s], f[s ^ (1 << i)]);
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;
        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                s |= 1 << i; // 快速跳到第 i 位是 1 的 s
                f[s] = max(f[s], f[s ^ (1 << i)]);
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w
	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for i := range w {
		for s := 0; s < u; s++ {
			s |= 1 << i // 快速跳到第 i 位是 1 的 s
			f[s] = max(f[s], f[s^1<<i])
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 附：平衡暴力和 DP 的算法

如果 $n^2 \le U\log U$，直接用暴力算。 

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        w = max(nums).bit_length()
        u = 1 << w

        n = len(nums)
        if n * n <= u * w:
            # 暴力
            ans = 0
            for i, x in enumerate(nums):
                for y in nums[:i]:
                    if x & y == 0 and x * y > ans:
                        ans = x * y
            return ans

        f = [0] * u
        for x in nums:
            f[x] = x

        for i in range(w):
            bit = 1 << i  # 避免在循环中反复计算 1 << i
            s = 0
            while s < u:
                s |= bit  # 快速跳到第 i 位是 1 的 s
                v = f[s ^ bit]
                if v > f[s]:
                    f[s] = v
                s += 1

        return max(x * f[(u - 1) ^ x] for x in nums)
```

```java [sol-Java]
class Solution {
    public long maxProduct(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int w = 32 - Integer.numberOfLeadingZeros(mx);
        int u = 1 << w;

        int n = nums.length;
        if (n <= u * w / n) { // 避免 n*n 溢出
            // 暴力
            long ans = 0;
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                for (int j = 0; j < i; j++) {
                    int y = nums[j];
                    if ((x & y) == 0) {
                        ans = Math.max(ans, (long) x * y);
                    }
                }
            }
            return ans;
        }

        int[] f = new int[u];
        for (int x : nums) {
            f[x] = x;
        }

        for (int i = 0; i < w; i++) {
            for (int s = 0; s < u; s++) {
                s |= 1 << i; // 快速跳到第 i 位是 1 的 s
                f[s] = Math.max(f[s], f[s ^ (1 << i)]);
            }
        }

        long ans = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) x * f[(u - 1) ^ x]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
// 基于方法一
class Solution {
public:
    long long maxProduct(vector<int>& nums) {
        int w = bit_width((uint32_t) ranges::max(nums));
        int u = 1 << w;

        int n = nums.size();
        if (n <= u * w / n) { // 避免 n*n 溢出
            // 暴力
            long long ans = 0;
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                for (int j = 0; j < i; j++) {
                    int y = nums[j];
                    if ((x & y) == 0) {
                        ans = max(ans, 1LL * x * y);
                    }
                }
            }
            return ans;
        }

        vector<int> f(u);
        for (int x : nums) {
            f[x] = x;
        }

        for (int s = 0; s < u; s++) {
            for (int t = s, lb; t > 0; t ^= lb) {
                lb = t & -t;
                f[s] = max(f[s], f[s ^ lb]);
            }
        }

        long long ans = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * x * f[(u - 1) ^ x]);
        }
        return ans;
    }
};
```

```go [sol-Go]
// 基于方法一
func maxProduct(nums []int) int64 {
	w := bits.Len(uint(slices.Max(nums)))
	u := 1 << w

	n := len(nums)
	if n*n <= u*w {
		// 暴力
		ans := 0
		for i, x := range nums {
			for _, y := range nums[:i] {
				if x&y == 0 {
					ans = max(ans, x*y)
				}
			}
		}
		return int64(ans)
	}

	f := make([]int, u)
	for _, x := range nums {
		f[x] = x
	}

	for s := range f {
		for t, lb := s, 0; t > 0; t ^= lb {
			lb = t & -t
			f[s] = max(f[s], f[s^lb])
		}
	}

	ans := 0
	for _, x := range nums {
		ans = max(ans, x*f[u-1^x])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(n^2, n + U\log U))$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 专题训练

见下面动态规划题单的「**§9.5 SOS DP**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
