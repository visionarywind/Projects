# 996. 平方数组的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-squareful-arrays/
- 题目 slug：`number-of-squareful-arrays`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-squareful-arrays/solutions/3961714/xiang-lin-xiang-guan-zhuang-ya-dpji-yi-h-nf7z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[相邻相关状压 DP：记忆化搜索 -> 递推（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-squareful-arrays/solutions/3961714/xiang-lin-xiang-guan-zhuang-ya-dpji-yi-h-nf7z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xiang-lin-xiang-guan-zhuang-ya-dpji-yi-h-nf7z`
- topic id：`3961714`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

1. [教你一步步思考状压 DP：从记忆化搜索到递推](https://leetcode.cn/problems/beautiful-arrangement/solution/jiao-ni-yi-bu-bu-si-kao-zhuang-ya-dpcong-c6kd/)
2. [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)

## 一、寻找子问题

示例 1 的 $\textit{nums}=[1,17,8]$。首先枚举排列的第一个数：

- 如果第一个数是 $1$，问题变成：在前一个数是 $1$ 的情况下，剩余的 $\{17,8\}$ 可以组成多少个合法排列。如果排列的第二个数是 $8$，问题变成：在前一个数是 $8$ 的情况下，剩余的 $\{17\}$ 可以组成多少个合法排列。
- 如果第一个数是 $17$，问题变成：在前一个数是 $17$ 的情况下，剩余的 $\{1,8\}$ 可以组成多少个合法排列。
- 如果第一个数是 $8$，问题变成：在前一个数是 $8$ 的情况下，剩余的 $\{1,17\}$ 可以组成多少个合法排列。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- 当前还剩下哪些数可以选。由于 $\textit{nums}[i]$ 的值域很大，为方便用二进制表示，更好的方式是，跟踪当前还剩下哪些**下标**可以选。
- 上一个填入的数的下标。

因此，定义 $\textit{dfs}(S,i)$ 表示在剩余可选下标集合为 $S$，上一个数的下标为 $i$ 的情况下，剩余元素可以组成多少个合法排列。

枚举 $S$ 中的下标 $j$，如果 $\textit{nums}[i]+\textit{nums}[j]$ 是完全平方数，那么可以选 $\textit{nums}[j]$（填入排列），问题变成在剩余可选下标集合为 $S \setminus \{j\}$，上一个数的下标为 $j$ 的情况下，剩余元素可以组成多少个合法排列，即 $\textit{dfs}(S\setminus \{j\},j)$。

累加得

$$
\textit{dfs}(S,i) = \sum_{j} \textit{dfs}(S\setminus \{j\},j)
$$

其中 $j\in S$ 且 $\textit{nums}[i]+\textit{nums}[j]$ 是完全平方数。

**递归边界**：$\textit{dfs}(\varnothing)=1$。能递归到 $S=\varnothing$，说明所有数都选完了，且所有相邻元素都是完全平方数，我们找到了一个合法排列。

**递归入口**：枚举排列第一个数的下标为 $i=0,1,2,\ldots,n-1$，问题变成 $\textit{dfs}(U\setminus \{i\},i)$，其中全集 $U=\{0,1,2,\ldots,n-1\}$。

如果 $\textit{nums}$ 没有重复元素，那么答案就是

$$
A = \sum_{i=0}^{n-1} \textit{dfs}(U\setminus \{i\},i)
$$

然而本题 $\textit{nums}$ 可能有重复的元素，我们还需要**去重**。例如 $\textit{nums}=[1,8,1,8,1]$，对于任意合法排列 $P$，$3$ 个 $1$ 内部互换，可以得到 $3!=6$ 个完全一样的排列；$2$ 个 $8$ 内部互换，可以得到 $2!=2$ 个完全一样的排列。这两种互换是独立的，根据乘法原理，我们会算出 $3!2!=12$ 个完全一样的 $P$。所以要把答案除以 $3!2!$，从而去掉重复的排列。一般地，统计 $\textit{nums}$ 每个元素的出现次数，把 $A$ 除以出现次数的阶乘的乘积。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

⚠**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(S,i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

关于记忆化搜索的原理，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

代码实现时，用二进制表示集合，用位运算实现集合操作，具体请看 [从集合论到位运算，常见位运算技巧分类总结](https://leetcode.cn/circle/discuss/CaOJ45/)。

```py [sol-Python3]
class Solution:
    def numSquarefulPerms(self, nums: list[int]) -> int:
        # 判断 x 是否为完全平方数
        def is_square(x: int) -> bool:
            rt = isqrt(x)
            return rt * rt == x

        n = len(nums)

        # s 表示剩余可选元素的下标集合
        # i 表示上一个数（我们刚刚填入的数）的下标
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(s: int, i: int) -> int:
            if s == 0:  # 填完了
                return 1  # 找到一个合法排列

            res = 0
            # 枚举当前位置填 nums[j]
            for j in range(n):
                # (s>>j&1)==1 表示 j 在 s 中
                if s >> j & 1 and is_square(nums[i] + nums[j]):
                    res += dfs(s ^ (1 << j), j)  # 从 s 中去掉 j
            return res

        ans = 0
        # 枚举排列的第一个数的下标
        u = (1 << n) - 1  # 全集 u = {0,1,2,...,n-1}
        for i in range(n):
            ans += dfs(u ^ (1 << i), i)  # 从 u 中去掉 i

        # 去重
        cnt = defaultdict(int)
        for x in nums:
            cnt[x] += 1
            ans //= cnt[x]  # 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)

        return ans
```

```java [sol-Java]
class Solution {
    public int numSquarefulPerms(int[] nums) {
        int n = nums.length;
        int[][] memo = new int[1 << n][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        int ans = 0;
        // 枚举排列的第一个数的下标
        int u = (1 << n) - 1; // 全集 u = {0,1,2,...,n-1}
        for (int i = 0; i < n; i++) {
            ans += dfs(u ^ (1 << i), i, nums, memo); // 从 u 中去掉 i
        }

        // 去重
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            int c = cnt.merge(x, 1, Integer::sum); // c = ++cnt[x]
            ans /= c; // 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)
        }

        return ans;
    }

    // s 表示剩余可选元素的下标集合
    // i 表示上一个数（我们刚刚填入的数）的下标
    private int dfs(int s, int i, int[] nums, int[][] memo) {
        if (s == 0) { // 填完了
            return 1; // 找到一个合法排列
        }
        if (memo[s][i] != -1) { // 之前计算过
            return memo[s][i];
        }

        int res = 0;
        // 枚举当前位置填 nums[j]
        for (int j = 0; j < nums.length; j++) {
            // (s>>j&1)>0 表示 j 在 s 中
            if ((s >> j & 1) > 0 && isSquare(nums[i] + nums[j])) {
                res += dfs(s ^ (1 << j), j, nums, memo); // 从 s 中去掉 j
            }
        }
        memo[s][i] = res; // 记忆化
        return res;
    }

    // 判断 x 是否为完全平方数
    private boolean isSquare(int x) {
        int rt = (int) Math.sqrt(x);
        return rt * rt == x;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 判断 x 是否为完全平方数
    bool is_square(int x) {
        int rt = sqrt(x);
        return rt * rt == x;
    }

public:
    int numSquarefulPerms(vector<int>& nums) {
        int n = nums.size();
        vector memo(1 << n, vector<int>(n, -1)); // -1 表示没有计算过

        // s 表示剩余可选元素的下标集合
        // i 表示上一个数（我们刚刚填入的数）的下标
        auto dfs = [&](this auto&& dfs, int s, int i) -> int {
            if (s == 0) { // 填完了
                return 1; // 找到一个合法排列
            }

            int& res = memo[s][i]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }

            res = 0;
            // 枚举当前位置填 nums[j]
            for (int j = 0; j < n; j++) {
                // (s>>j&1)==1 表示 j 在 s 中
                if (s >> j & 1 && is_square(nums[i] + nums[j])) {
                    res += dfs(s ^ (1 << j), j); // 从 s 中去掉 j
                }
            }
            return res;
        };

        int ans = 0;
        // 枚举排列的第一个数的下标
        int u = (1 << n) - 1; // 全集 u = {0,1,2,...,n-1}
        for (int i = 0; i < n; i++) {
            ans += dfs(u ^ (1 << i), i); // 从 u 中去掉 i
        }

        // 去重
        unordered_map<int, int> cnt;
        for (int x : nums) {
            ans /= ++cnt[x]; // 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)
        }

        return ans;
    }
};
```

```go [sol-Go]
// 判断 x 是否为完全平方数
func isSquare(x int) bool {
	rt := int(math.Sqrt(float64(x)))
	return rt*rt == x
}

func numSquarefulPerms(nums []int) (ans int) {
	n := len(nums)
	memo := make([][]int, 1<<n)
	for i := range memo {
		memo[i] = make([]int, n)
		for j := range memo[i] {
			memo[i][j] = -1 // -1 表示没有计算过
		}
	}

	// s 表示剩余可选元素的下标集合
	// i 表示上一个数（我们刚刚填入的数）的下标
	var dfs func(int, int) int
	dfs = func(s, i int) (res int) {
		if s == 0 { // 填完了
			return 1 // 找到一个合法排列
		}

		p := &memo[s][i]
		if *p != -1 { // 之前计算过
			return *p
		}

		// 枚举当前位置填 nums[j]
		for j := range n {
			// s>>j&1 > 0 表示 j 在 s 中
			if s>>j&1 > 0 && isSquare(nums[i]+nums[j]) {
				res += dfs(s^1<<j, j) // 从 s 中去掉 j
			}
		}
		*p = res // 记忆化
		return
	}

	// 枚举排列的第一个数的下标
	u := 1<<n - 1 // 全集 u = {0,1,2,...,n-1}
	for i := range n {
		ans += dfs(u^1<<i, i) // 从 u 中去掉 i
	}

	// 去重
	cnt := map[int]int{}
	for _, x := range nums {
		cnt[x]++
		ans /= cnt[x] // 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)
	}

	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^22^n)$，其中 $n$ 是 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n2^n)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以总的时间复杂度为 $\mathcal{O}(n^22^n)$。
- 空间复杂度：$\mathcal{O}(n2^n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[S][i]$ 的定义和 $\textit{dfs}(S,i)$ 的定义是完全一样的，都表示在剩余可选下标集合为 $S$，上一个数的下标为 $i$ 的情况下，剩余元素可以组成多少个合法排列。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[S][i] = \sum_{j} f[S\setminus \{j\}][j]
$$

其中 $j\in S$ 且 $\textit{nums}[i]+\textit{nums}[j]$ 是完全平方数。

初始值 $f[\varnothing]=1$，翻译自递归边界 $\textit{dfs}(\varnothing)=1$。

递推完毕后，计算

$$
A = \sum_{i=0}^{n-1} f[U\setminus \{i\}][i]
$$

统计 $\textit{nums}$ 每个元素的出现次数，把 $A$ 除以出现次数的阶乘的乘积，即为最终答案。

```py [sol-Python3]
class Solution:
    def numSquarefulPerms(self, nums: list[int]) -> int:
        # 判断 x 是否为完全平方数
        def is_square(x: int) -> bool:
            rt = isqrt(x)
            return rt * rt == x

        n = len(nums)
        f = [[0] * n for _ in range(1 << n)]
        f[0] = [1] * n

        u = (1 << n) - 1
        for s in range(1, u):
            for i in range(n):
                if s >> i & 1:  # i 是填过的数的下标，不能在 s 中
                    continue
                for j in range(n):
                    if s >> j & 1 and is_square(nums[i] + nums[j]):
                        f[s][i] += f[s ^ (1 << j)][j]

        ans = 0
        # 枚举排列的第一个数的下标
        for i in range(n):
            ans += f[u ^ (1 << i)][i]

        # 去重
        cnt = defaultdict(int)
        for x in nums:
            cnt[x] += 1
            ans //= cnt[x]  # 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)

        return ans
```

```java [sol-Java]
class Solution {
    public int numSquarefulPerms(int[] nums) {
        int n = nums.length;
        int[][] f = new int[1 << n][n];
        Arrays.fill(f[0], 1);

        int u = (1 << n) - 1;
        for (int s = 1; s < u; s++) {
            for (int i = 0; i < n; i++) {
                if ((s >> i & 1) > 0) { // i 是填过的数的下标，不能在 s 中
                    continue;
                }
                for (int j = 0; j < n; j++) {
                    if ((s >> j & 1) > 0 && isSquare(nums[i] + nums[j])) {
                        f[s][i] += f[s ^ (1 << j)][j];
                    }
                }
            }
        }

        int ans = 0;
        // 枚举排列的第一个数的下一个标
        for (int i = 0; i < n; i++) {
            ans += f[u ^ (1 << i)][i];
        }

        // 去重
        Map<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums) {
            int c = cnt.merge(x, 1, Integer::sum); // c = ++cnt[x]
            ans /= c; // 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)
        }

        return ans;
    }

    // 判断 x 是否为完全平方数
    private boolean isSquare(int x) {
        int rt = (int) Math.sqrt(x);
        return rt * rt == x;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 判断 x 是否为完全平方数
    bool is_square(int x) {
        int rt = sqrt(x);
        return rt * rt == x;
    }

public:
    int numSquarefulPerms(vector<int>& nums) {
        int n = nums.size();
        vector f(1 << n, vector<int>(n));
        ranges::fill(f[0], 1);

        int u = (1 << n) - 1;
        for (int s = 1; s < u; s++) {
            for (int i = 0; i < n; i++) {
                if (s >> i & 1) { // i 是填过的数的下标，不能在 s 中
                    continue;
                }
                for (int j = 0; j < n; j++) {
                    if (s >> j & 1 && is_square(nums[i] + nums[j])) {
                        f[s][i] += f[s ^ (1 << j)][j];
                    }
                }
            }
        }

        int ans = 0;
        // 枚举排列的第一个数的下标
        for (int i = 0; i < n; i++) {
            ans += f[u ^ (1 << i)][i];
        }

        // 去重
        unordered_map<int, int> cnt;
        for (int x : nums) {
            ans /= ++cnt[x]; // 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)
        }

        return ans;
    }
};
```

```go [sol-Go]
// 判断 x 是否为完全平方数
func isSquare(x int) bool {
	rt := int(math.Sqrt(float64(x)))
	return rt*rt == x
}

func numSquarefulPerms(nums []int) (ans int) {
	n := len(nums)
	f := make([][]int, 1<<n)
	for i := range f {
		f[i] = make([]int, n)
	}
	for i := range f[0] {
		f[0][i] = 1
	}

	u := 1<<n - 1
	for s := 1; s < u; s++ {
		for i := range n {
			if s>>i&1 > 0 { // i 是填过的数的下标，不能在 s 中
				continue
			}
			for j := range n {
				if s>>j&1 > 0 && isSquare(nums[i]+nums[j]) {
					f[s][i] += f[s^1<<j][j]
				}
			}
		}
	}

	// 枚举排列的第一个数的下标
	for i := range n {
		ans += f[u^1<<i][i]
	}

	// 去重
	cnt := map[int]int{}
	for _, x := range nums {
		cnt[x]++
		ans /= cnt[x] // 比如 nums 有 3 个 x，这里会 /1 再 /2 再 /3，从而实现 /(3!)
	}

	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^22^n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n2^n)$。

## 专题训练

见下面动态规划题单的「**§9.2 排列型状压 DP ② 相邻相关**」和「**§9.3 旅行商问题（TSP）**」。

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

本题来自 `九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
