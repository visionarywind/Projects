# 3826. 最小分割分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-partition-score/
- 题目 slug：`minimum-partition-score`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.7 斜率优化 DP
- 难度分：2345
- 外部题解来源：https://leetcode.cn/problems/minimum-partition-score/solutions/3893573/hua-fen-xing-dp-xie-lu-you-hua-tu-bao-yo-5cb0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[划分型 DP + 斜率优化（凸包优化）Python/Java/C++/Go](https://leetcode.cn/problems/minimum-partition-score/solutions/3893573/hua-fen-xing-dp-xie-lu-you-hua-tu-bao-yo-5cb0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-fen-xing-dp-xie-lu-you-hua-tu-bao-yo-5cb0`
- topic id：`3893573`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于子数组的值是个分数，不好处理，我们可以先计算分子之和（或者说，把子数组的值乘以 $2$），最后返回时再除以 $2$。

## 划分型 DP

本题是标准的划分型 DP。根据 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/)「§5.3 约束划分个数」，定义 $f_{K,i}$ 表示把长为 $i$ 的前缀 $[0,i-1]$ 划分成恰好 $K$ 个子数组的分数（乘以 $2$）。

枚举最后一段子数组的左端点 $j$，问题变成把前缀 $[0,j-1]$ 划分成恰好 $K-1$ 个子数组的分数（乘以 $2$），即 $f_{K-1,j}$。

设 $\textit{nums}$ 的**前缀和**数组为 $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

把子数组 $[j,i-1]$ 的元素和用前缀和表示，即 $s_i - s_j$。

子数组的值（乘以 $2$）为

$$
(s_i - s_j)(s_i - s_j+1)
$$

取最小值，有

$$
f_{K,i} = \min_{j=0}^{i-1} f_{K-1,j} + (s_i - s_j)(s_i - s_j+1)
$$

初始值 $f_{0,0} = 0$，$f_{i,i-1}=\infty$。注意不需要初始化 $K<i-1$ 的状态，因为我们不会访问这些状态。

答案为 $f_{k,n}$。

这样做的时间复杂度是 $\mathcal{O}(n^2k)$，太慢了，如何优化？

## 斜率优化（凸包优化）

**注**：以下内容基于点积，和斜率优化本质是一样的。我推荐从点积的角度理解，相比斜率的角度，不需要做麻烦的式子变形，更方便。

**前置知识**：二维计算几何，凸包，Andrew 算法。

把转移方程中的括号展开，得

$$
f_{K,i} = s_i^2+s_i + \min_{j=0}^{i-1} f_{K-1,j} -2s_is_j + s_j^2-s_j
$$

把其中的

$$
f_{K-1,j} -2s_is_j + s_j^2-s_j
$$

改成点积的形式，这样我们能得到来自几何意义上的观察。

设向量 $\mathbf{v}_j = (s_j, f_{K-1,j} + s_j^2-s_j)$。

设向量 $\mathbf{p} = (-2s_i, 1)$。

那么我们求的是

$$
\min_{j=0}^{i-1} \mathbf{p}\cdot \mathbf{v}_j
$$

根据点积的几何意义，我们求的是 $\mathbf{v}_j$ 在 $\mathbf{p}$ 方向上的投影长度，再乘以 $\mathbf{p}$ 的模长 $||\mathbf{p}||$。由于 $||\mathbf{p}||$ 是个定值，所以要最小化投影长度。

考虑 $\mathbf{v}_j$ 的**凸包**（用 Andrew 算法计算），在凸包内的点，比凸包顶点的投影长度长（注意 $\mathbf{p}$ 在凸包外面）。所以只需考虑凸包顶点。由于下凸包的投影长度比上凸包的短，所以只需考虑下凸包。

> 由于 $s_j$ 是单调递增的，求下凸包无需排序。

这样有一个很好的性质：从左到右遍历凸包顶点，$\mathbf{p}\cdot \mathbf{v}_j$ 会先变小再变大（单峰函数）。那么要计算最小值，就类似 [852. 山脉数组的峰顶索引](https://leetcode.cn/problems/peak-index-in-a-mountain-array/)，**二分**首个「上坡」的位置，见 [我的题解](https://leetcode.cn/problems/peak-index-in-a-mountain-array/solutions/2984800/er-fen-gen-ju-shang-po-huan-shi-xia-po-p-uoev/)。

实际上不需要二分。由于 $-2s_i$ 是单调递减的，可以用单调队列维护凸包。

![lc3826-2c.png](https://pic.leetcode.cn/1781133126-YPsSNw-lc3826-2c.png){:width=500px}

如上图，我们需要在凸包上找到一个点 $A$，使得 $\overrightarrow{OA}$ 到 $\mathbf{p}$ 的投影长度是最短的。这个点是向量 $\mathbf{p}$ 的垂线与凸包的交点。

当向量 $\mathbf{p}$ 向左旋转时，交点 $A$ 在凸包上会向右移动。如上图，绿色垂线与凸包的交点更靠右。

```py [sol-Python3]
# 注：由于把运算封装到了单独的方法，跑得比较慢，直接把计算逻辑写在 DP 中更快
class Vec:
    __slots__ = 'x', 'y'

    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __sub__(self, b: "Vec") -> "Vec":
        return Vec(self.x - b.x, self.y - b.y)

    def det(self, b: "Vec") -> int:
        return self.x * b.y - self.y * b.x

    def dot(self, b: "Vec") -> int:
        return self.x * b.x + self.y * b.y


class Solution:
    def minPartitionScore(self, nums: List[int], k: int) -> int:
        n = len(nums)
        pre = list(accumulate(nums, initial=0))
        f = [0] + [inf] * n

        for K in range(1, k + 1):
            s = pre[K - 1]
            q = deque([Vec(s, f[K - 1] + s * s - s)])
            for i in range(K, n - (k - K) + 1):  # 其他子数组的长度至少是 1
                s = pre[i]
                p = Vec(-2 * s, 1)
                while len(q) > 1 and p.dot(q[0]) >= p.dot(q[1]):
                    q.popleft()

                v = Vec(s, f[i] + s * s - s)
                f[i] = p.dot(q[0]) + s * s + s

                while len(q) > 1 and (q[-1] - q[-2]).det(v - q[-1]) <= 0:
                    q.pop()
                q.append(v)

        return f[n] // 2
```

```java [sol-Java]
import java.math.BigInteger;

class Solution {
    private static class Vec {
        long x;
        long y;

        Vec(long x, long y) {
            this.x = x;
            this.y = y;
        }
    }

    private Vec sub(Vec a, Vec b) {
        return new Vec(a.x - b.x, a.y - b.y);
    }

    private long dot(Vec a, Vec b) {
        return a.x * b.x + a.y * b.y;
    }

    // 如果乘法会溢出，用 detCmp
    private long det(Vec a, Vec b) {
        return a.x * b.y - a.y * b.x;
    }

    private int detCmp(Vec a, Vec b) {
        return BigInteger.valueOf(a.x).multiply(BigInteger.valueOf(b.y))
                .compareTo(BigInteger.valueOf(a.y).multiply(BigInteger.valueOf(b.x)));
    }

    public long minPartitionScore(int[] nums, int k) {
        int n = nums.length;
        int[] sum = new int[n + 1]; // nums 的前缀和
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + nums[i];
        }

        long[] f = new long[n + 1];
        Arrays.fill(f, Long.MAX_VALUE / 2);
        f[0] = 0;

        Vec[] q = new Vec[n + 1];

        for (int K = 1; K <= k; K++) {
            int head = 0;
            int tail = 0; // 模拟 deque

            long s = sum[K - 1];
            q[tail++] = new Vec(s, f[K - 1] + s * s - s);

            for (int i = K; i <= n - (k - K); i++) { // 其他子数组的长度至少是 1
                s = sum[i];
                Vec p = new Vec(-2 * s, 1);
                while (tail - head > 1 && dot(p, q[head]) >= dot(p, q[head + 1])) {
                    head++;
                }

                Vec v = new Vec(s, f[i] + s * s - s);
                f[i] = dot(p, q[head]) + s * s + s;

                // 读者可以把 detCmp 改成 det 感受下这个算法的效率
			    // 目前 det 也能过，可以试试 hack 一下
                while (tail - head > 1 && detCmp(sub(q[tail - 1], q[tail - 2]), sub(v, q[tail - 1])) <= 0) {
                    tail--;
                }
                q[tail++] = v;
            }
        }

        return f[n] / 2;
    }
}
```

```cpp [sol-C++]
struct vec {
    long long x, y;
};

vec sub(vec a, vec b) {
    return vec{a.x - b.x, a.y - b.y};
}

long long dot(vec a, vec b) {
    return a.x * b.x + a.y * b.y;
}

// 如果乘法会溢出，用 __int128
__int128 det(vec a, vec b) {
    return (__int128) a.x * b.y - (__int128) a.y * b.x;
}

class Solution {
public:
    long long minPartitionScore(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> sum(n + 1); // nums 的前缀和
        partial_sum(nums.begin(), nums.end(), sum.begin() + 1);

        vector<long long> f(n + 1, LLONG_MAX / 2);
        f[0] = 0;

        vector<vec> q(n - k + 2);

        for (int K = 1; K <= k; K++) {
            int head = 0, tail = 0; // 模拟 deque

            long long s = sum[K - 1];
            q[tail++] = vec{s, f[K - 1] + s * s - s};

            for (int i = K; i <= n - (k - K); i++) { // 其他子数组的长度至少是 1
                s = sum[i];
                vec p = {-2 * s, 1};
                while (tail - head > 1 && dot(p, q[head]) >= dot(p, q[head + 1])) {
                    head++;
                }

                vec v{s, f[i] + s * s - s};
                f[i] = dot(p, q[head]) + s * s + s;

                while (tail - head > 1 && det(sub(q[tail - 1], q[tail - 2]), sub(v, q[tail - 1])) <= 0) {
                    tail--;
                }
                q[tail++] = v;
            }
        }

        return f[n] / 2;
    }
};
```

```go [sol-Go]
type vec struct{ x, y int }

func (a vec) sub(b vec) vec { return vec{a.x - b.x, a.y - b.y} }
func (a vec) dot(b vec) int { return a.x*b.x + a.y*b.y }
func (a vec) det(b vec) int { return a.x*b.y - a.y*b.x } // 如果乘法会溢出，用 detCmp
func (a vec) detCmp(b vec) int {
	v := new(big.Int).Mul(big.NewInt(int64(a.x)), big.NewInt(int64(b.y)))
	w := new(big.Int).Mul(big.NewInt(int64(a.y)), big.NewInt(int64(b.x)))
	return v.Cmp(w)
}

func minPartitionScore(nums []int, k int) int64 {
	n := len(nums)
	sum := make([]int, n+1)
	for i, x := range nums {
		sum[i+1] = sum[i] + x
	}

	f := make([]int, n+1)
	for i := 1; i <= n; i++ {
		f[i] = math.MaxInt / 2
	}

	for K := 1; K <= k; K++ {
		s := sum[K-1]
		q := []vec{{s, f[K-1] + s*s - s}}
		for i := K; i <= n-(k-K); i++ { // 其他子数组的长度至少是 1
			s = sum[i]
			p := vec{-2 * s, 1}
			for len(q) > 1 && p.dot(q[0]) >= p.dot(q[1]) {
				q = q[1:]
			}

			v := vec{s, f[i] + s*s - s}
			f[i] = p.dot(q[0]) + s*s + s

			// 读者可以把 detCmp 改成 det 感受下这个算法的效率
			// 目前 det 也能过，可以试试 hack 一下
			for len(q) > 1 && q[len(q)-1].sub(q[len(q)-2]).detCmp(v.sub(q[len(q)-1])) <= 0 {
				q = q[:len(q)-1]
			}
			q = append(q, v)
		}
	}

	return int64(f[n] / 2)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n-k)k)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面动态规划题单的「**§5.3 约束划分个数**」和「**§11.7 斜率优化 DP**」。

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

## 划分型 DP

本题是标准的划分型 DP。根据 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/)「§5.3 约束划分个数」，定义 $f_{K,i}$ 表示把长为 $i$ 的前缀 $[0,i-1]$ 划分成恰好 $K$ 个子数组的分数（乘以 $2$）。

枚举最后一段子数组的左端点 $j$，问题变成把前缀 $[0,j-1]$ 划分成恰好 $K-1$ 个子数组的分数（乘以 $2$），即 $f_{K-1,j}$。

设 $\textit{nums}$ 的**前缀和**数组为 $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

把子数组 $[j,i-1]$ 的元素和用前缀和表示，即 $s_i - s_j$。

子数组的值（乘以 $2$）为

$$
(s_i - s_j)(s_i - s_j+1)
$$

取最小值，有

$$
f_{K,i} = \min_{j=0}^{i-1} f_{K-1,j} + (s_i - s_j)(s_i - s_j+1)
$$

初始值 $f_{0,0} = 0$，$f_{i,i-1}=\infty$。注意不需要初始化 $K<i-1$ 的状态，因为我们不会访问这些状态。

答案为 $f_{k,n}$。

这样做的时间复杂度是 $\mathcal{O}(n^2k)$，太慢了，如何优化？

## 斜率优化（凸包优化）

**注**：以下内容基于点积，和斜率优化本质是一样的。我推荐从点积的角度理解，相比斜率的角度，不需要做麻烦的式子变形，更方便。

**前置知识**：二维计算几何，凸包，Andrew 算法。

把转移方程中的括号展开，得

$$
f_{K,i} = s_i^2+s_i + \min_{j=0}^{i-1} f_{K-1,j} -2s_is_j + s_j^2-s_j
$$

把其中的

$$
f_{K-1,j} -2s_is_j + s_j^2-s_j
$$

改成点积的形式，这样我们能得到来自几何意义上的观察。

设向量 $\mathbf{v}_j = (s_j, f_{K-1,j} + s_j^2-s_j)$。

设向量 $\mathbf{p} = (-2s_i, 1)$。

那么我们求的是

$$
\min_{j=0}^{i-1} \mathbf{p}\cdot \mathbf{v}_j
$$

根据点积的几何意义，我们求的是 $\mathbf{v}_j$ 在 $\mathbf{p}$ 方向上的投影长度，再乘以 $\mathbf{p}$ 的模长 $||\mathbf{p}||$。由于 $||\mathbf{p}||$ 是个定值，所以要最小化投影长度。

考虑 $\mathbf{v}_j$ 的**凸包**（用 Andrew 算法计算），在凸包内的点，比凸包顶点的投影长度长（注意 $\mathbf{p}$ 在凸包外面）。所以只需考虑凸包顶点。由于下凸包的投影长度比上凸包的短，所以只需考虑下凸包。

> 由于 $s_j$ 是单调递增的，求下凸包无需排序。

这样有一个很好的性质：从左到右遍历凸包顶点，$\mathbf{p}\cdot \mathbf{v}_j$ 会先变小再变大（单峰函数）。那么要计算最小值，就类似 [852. 山脉数组的峰顶索引](https://leetcode.cn/problems/peak-index-in-a-mountain-array/)，**二分**首个「上坡」的位置，见 [我的题解](https://leetcode.cn/problems/peak-index-in-a-mountain-array/solutions/2984800/er-fen-gen-ju-shang-po-huan-shi-xia-po-p-uoev/)。

实际上不需要二分。由于 $-2s_i$ 是单调递减的，可以用单调队列维护凸包。

![lc3826-2c.png](https://pic.leetcode.cn/1781133126-YPsSNw-lc3826-2c.png){:width=500px}

如上图，我们需要在凸包上找到一个点 $A$，使得 $\overrightarrow{OA}$ 到 $\mathbf{p}$ 的投影长度是最短的。这个点是向量 $\mathbf{p}$ 的垂线与凸包的交点。

当向量 $\mathbf{p}$ 向左旋转时，交点 $A$ 在凸包上会向右移动。如上图，绿色垂线与凸包的交点更靠右。

```py [sol-Python3]
# 注：由于把运算封装到了单独的方法，跑得比较慢，直接把计算逻辑写在 DP 中更快
class Vec:
    __slots__ = 'x', 'y'

    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __sub__(self, b: "Vec") -> "Vec":
        return Vec(self.x - b.x, self.y - b.y)

    def det(self, b: "Vec") -> int:
        return self.x * b.y - self.y * b.x

    def dot(self, b: "Vec") -> int:
        return self.x * b.x + self.y * b.y


class Solution:
    def minPartitionScore(self, nums: List[int], k: int) -> int:
        n = len(nums)
        pre = list(accumulate(nums, initial=0))
        f = [0] + [inf] * n

        for K in range(1, k + 1):
            s = pre[K - 1]
            q = deque([Vec(s, f[K - 1] + s * s - s)])
            for i in range(K, n - (k - K) + 1):  # 其他子数组的长度至少是 1
                s = pre[i]
                p = Vec(-2 * s, 1)
                while len(q) > 1 and p.dot(q[0]) >= p.dot(q[1]):
                    q.popleft()

                v = Vec(s, f[i] + s * s - s)
                f[i] = p.dot(q[0]) + s * s + s

                while len(q) > 1 and (q[-1] - q[-2]).det(v - q[-1]) <= 0:
                    q.pop()
                q.append(v)

        return f[n] // 2
```

```java [sol-Java]
import java.math.BigInteger;

class Solution {
    private static class Vec {
        long x;
        long y;

        Vec(long x, long y) {
            this.x = x;
            this.y = y;
        }
    }

    private Vec sub(Vec a, Vec b) {
        return new Vec(a.x - b.x, a.y - b.y);
    }

    private long dot(Vec a, Vec b) {
        return a.x * b.x + a.y * b.y;
    }

    // 如果乘法会溢出，用 detCmp
    private long det(Vec a, Vec b) {
        return a.x * b.y - a.y * b.x;
    }

    private int detCmp(Vec a, Vec b) {
        return BigInteger.valueOf(a.x).multiply(BigInteger.valueOf(b.y))
                .compareTo(BigInteger.valueOf(a.y).multiply(BigInteger.valueOf(b.x)));
    }

    public long minPartitionScore(int[] nums, int k) {
        int n = nums.length;
        int[] sum = new int[n + 1]; // nums 的前缀和
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + nums[i];
        }

        long[] f = new long[n + 1];
        Arrays.fill(f, Long.MAX_VALUE / 2);
        f[0] = 0;

        Vec[] q = new Vec[n + 1];

        for (int K = 1; K <= k; K++) {
            int head = 0;
            int tail = 0; // 模拟 deque

            long s = sum[K - 1];
            q[tail++] = new Vec(s, f[K - 1] + s * s - s);

            for (int i = K; i <= n - (k - K); i++) { // 其他子数组的长度至少是 1
                s = sum[i];
                Vec p = new Vec(-2 * s, 1);
                while (tail - head > 1 && dot(p, q[head]) >= dot(p, q[head + 1])) {
                    head++;
                }

                Vec v = new Vec(s, f[i] + s * s - s);
                f[i] = dot(p, q[head]) + s * s + s;

                // 读者可以把 detCmp 改成 det 感受下这个算法的效率
			    // 目前 det 也能过，可以试试 hack 一下
                while (tail - head > 1 && detCmp(sub(q[tail - 1], q[tail - 2]), sub(v, q[tail - 1])) <= 0) {
                    tail--;
                }
                q[tail++] = v;
            }
        }

        return f[n] / 2;
    }
}
```

```cpp [sol-C++]
struct vec {
    long long x, y;
};

vec sub(vec a, vec b) {
    return vec{a.x - b.x, a.y - b.y};
}

long long dot(vec a, vec b) {
    return a.x * b.x + a.y * b.y;
}

// 如果乘法会溢出，用 __int128
__int128 det(vec a, vec b) {
    return (__int128) a.x * b.y - (__int128) a.y * b.x;
}

class Solution {
public:
    long long minPartitionScore(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> sum(n + 1); // nums 的前缀和
        partial_sum(nums.begin(), nums.end(), sum.begin() + 1);

        vector<long long> f(n + 1, LLONG_MAX / 2);
        f[0] = 0;

        vector<vec> q(n - k + 2);

        for (int K = 1; K <= k; K++) {
            int head = 0, tail = 0; // 模拟 deque

            long long s = sum[K - 1];
            q[tail++] = vec{s, f[K - 1] + s * s - s};

            for (int i = K; i <= n - (k - K); i++) { // 其他子数组的长度至少是 1
                s = sum[i];
                vec p = {-2 * s, 1};
                while (tail - head > 1 && dot(p, q[head]) >= dot(p, q[head + 1])) {
                    head++;
                }

                vec v{s, f[i] + s * s - s};
                f[i] = dot(p, q[head]) + s * s + s;

                while (tail - head > 1 && det(sub(q[tail - 1], q[tail - 2]), sub(v, q[tail - 1])) <= 0) {
                    tail--;
                }
                q[tail++] = v;
            }
        }

        return f[n] / 2;
    }
};
```

```go [sol-Go]
type vec struct{ x, y int }

func (a vec) sub(b vec) vec { return vec{a.x - b.x, a.y - b.y} }
func (a vec) dot(b vec) int { return a.x*b.x + a.y*b.y }
func (a vec) det(b vec) int { return a.x*b.y - a.y*b.x } // 如果乘法会溢出，用 detCmp
func (a vec) detCmp(b vec) int {
	v := new(big.Int).Mul(big.NewInt(int64(a.x)), big.NewInt(int64(b.y)))
	w := new(big.Int).Mul(big.NewInt(int64(a.y)), big.NewInt(int64(b.x)))
	return v.Cmp(w)
}

func minPartitionScore(nums []int, k int) int64 {
	n := len(nums)
	sum := make([]int, n+1)
	for i, x := range nums {
		sum[i+1] = sum[i] + x
	}

	f := make([]int, n+1)
	for i := 1; i <= n; i++ {
		f[i] = math.MaxInt / 2
	}

	for K := 1; K <= k; K++ {
		s := sum[K-1]
		q := []vec{{s, f[K-1] + s*s - s}}
		for i := K; i <= n-(k-K); i++ { // 其他子数组的长度至少是 1
			s = sum[i]
			p := vec{-2 * s, 1}
			for len(q) > 1 && p.dot(q[0]) >= p.dot(q[1]) {
				q = q[1:]
			}

			v := vec{s, f[i] + s*s - s}
			f[i] = p.dot(q[0]) + s*s + s

			// 读者可以把 detCmp 改成 det 感受下这个算法的效率
			// 目前 det 也能过，可以试试 hack 一下
			for len(q) > 1 && q[len(q)-1].sub(q[len(q)-2]).detCmp(v.sub(q[len(q)-1])) <= 0 {
				q = q[:len(q)-1]
			}
			q = append(q, v)
		}
	}

	return int64(f[n] / 2)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n-k)k)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面动态规划题单的「**§5.3 约束划分个数**」和「**§11.7 斜率优化 DP**」。

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

本题来自 `十一、优化 DP / §11.7 斜率优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.7 斜率优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
