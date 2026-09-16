# 3691. 最大子数组总值 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-total-subarray-value-ii/
- 题目 slug：`maximum-total-subarray-value-ii`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.6 第 K 小/大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-total-subarray-value-ii/solutions/3787892/st-biao-zui-da-dui-pythonjavacgo-by-endl-igja/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：ST 表 + 最大堆 / 二分 + Lazy 线段树（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-total-subarray-value-ii/solutions/3787892/st-biao-zui-da-dui-pythonjavacgo-by-endl-igja/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`st-biao-zui-da-dui-pythonjavacgo-by-endl-igja`
- topic id：`3787892`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：最大堆求前 k 大

示例 2 的 $\textit{nums}=[4,2,5,1]$，把所有连续子数组的值（极差）算出来，可以得到一个矩阵 $M$，其中 $M_{l,r}$ 表示子数组 $[l,r]$ 的极差。规定 $l>r$ 时极差为 $0$。

$$
M = \begin{bmatrix}
0 & 2 & 3 & 4    \\
0 & 0 & 3 & 4    \\
0 & 0 & 0 & 4    \\
0 & 0 & 0 & 0    \\
\end{bmatrix}
$$

当左端点固定时，右端点越大，子数组的最小值越小，最大值越大，所以子数组的极差也就越大。

所以矩阵 $M$ **每一行都是递增的**。问题相当于：

- 合并 $n$ 个递增列表，计算前 $k$ 大元素之和。

根据 [23. 合并 K 个升序链表](https://leetcode.cn/problems/merge-k-sorted-lists/) 的 [堆的做法](https://leetcode.cn/problems/merge-k-sorted-lists/solutions/2384305/liang-chong-fang-fa-zui-xiao-dui-fen-zhi-zbzx/)：

1. 把矩阵每一行的最后一个数 $M_{l,n-1}$ 加到一个**最大堆**中。
2. 循环 $k$ 次。
3. 每次循环，弹出堆顶，把堆顶 $M_{l,r}$ 加入答案，然后把左边元素 $M_{l,r-1}$ 入堆。**优化**：如果堆顶是 $0$，那么堆中剩余元素，以及后续未入堆的值都是 $0$，答案不会增大，直接跳出循环。

我们不能直接把整个矩阵 $M$ 算出来（太慢了），而是在入堆的时候计算矩阵值。这需要一个数据结构，支持查询**区间最小值**和**区间最大值**。这可以用线段树，或者 ST 表。由于只有查询没有更新，我选择用 ST 表。[ST 表原理讲解](https://www.bilibili.com/video/BV18gLE6VETZ/)。

[本题视频讲解](https://www.bilibili.com/video/BV19GWcziEYE/?t=20m56s)，欢迎点赞关注~

### 写法一

```py [sol-Python3]
# 手写 min max 更快
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

def op(a: Tuple[int, int], b: Tuple[int, int]) -> Tuple[int, int]:
    return min(a[0], b[0]), max(a[1], b[1])

class ST:
    def __init__(self, a: List[int]):
        n = len(a)
        w = n.bit_length()
        st = [[None] * n for _ in range(w)]
        st[0] = [(x, x) for x in a]
        for i in range(1, w):
            for j in range(n - (1 << i) + 1):
                st[i][j] = op(st[i - 1][j], st[i - 1][j + (1 << (i - 1))])
        self.st = st

    # [l, r) 左闭右开
    def query(self, l: int, r: int) -> int:
        k = (r - l).bit_length() - 1
        mn, mx = op(self.st[k][l], self.st[k][r - (1 << k)])
        return mx - mn

class Solution:
    def maxTotalValue(self, nums: List[int], k: int) -> int:
        n = len(nums)
        st = ST(nums)

        # 最大堆中保存 (子数组极差，左端点，右端点加一)
        h = [(st.query(i, n), i, n) for i in range(n)]
        # 由于 h 是递减的，无需堆化

        ans = 0
        for _ in range(k):
            d, l, r = h[0]
            if d == 0:  # 堆中剩余元素全是 0
                break
            ans += d
            heapreplace_max(h, (st.query(l, r - 1), l, r - 1))
        return ans
```

```java [sol-Java]
class ST {
    private final int[][] stMin;
    private final int[][] stMax;

    public ST(int[] a) {
        int n = a.length;
        int w = 32 - Integer.numberOfLeadingZeros(n);
        stMin = new int[w][n];
        stMax = new int[w][n];

        for (int j = 0; j < n; j++) {
            stMin[0][j] = a[j];
            stMax[0][j] = a[j];
        }

        for (int i = 1; i < w; i++) {
            for (int j = 0; j + (1 << i) <= n; j++) {
                stMin[i][j] = Math.min(stMin[i - 1][j], stMin[i - 1][j + (1 << (i - 1))]);
                stMax[i][j] = Math.max(stMax[i - 1][j], stMax[i - 1][j + (1 << (i - 1))]);
            }
        }
    }

    // [l, r) 左闭右开
    public int query(int l, int r) {
        int k = 31 - Integer.numberOfLeadingZeros(r - l);
        int mn = Math.min(stMin[k][l], stMin[k][r - (1 << k)]);
        int mx = Math.max(stMax[k][l], stMax[k][r - (1 << k)]);
        return mx - mn;
    }
}

class Solution{
    public long maxTotalValue(int[] nums, int k) {
        int n = nums.length;
        ST st = new ST(nums);

        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> b[0] - a[0]); // 最大堆
        for (int i = 0; i < n; i++) {
            pq.add(new int[]{st.query(i, n), i, n}); // 子数组极差，左端点，右端点加一
        }

        long ans = 0;
        while (k-- > 0 && pq.peek()[0] > 0) {
            int[] top = pq.poll();
            ans += top[0];
            top[2]--;
            top[0] = st.query(top[1], top[2]);
            pq.add(top);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class ST {
    vector<vector<int>> st_min;
    vector<vector<int>> st_max;

public:
    ST(const vector<int>& a) {
        size_t n = a.size();
        int w = bit_width(n);
        st_min.resize(w, vector<int>(n));
        st_max.resize(w, vector<int>(n));

        for (int j = 0; j < n; j++) {
            st_min[0][j] = a[j];
            st_max[0][j] = a[j];
        }

        for (int i = 1; i < w; i++) {
            for (int j = 0; j + (1 << i) <= n; j++) {
                st_min[i][j] = min(st_min[i - 1][j], st_min[i - 1][j + (1 << (i - 1))]);
                st_max[i][j] = max(st_max[i - 1][j], st_max[i - 1][j + (1 << (i - 1))]);
            }
        }
    }

    // [l, r) 左闭右开
    int query(int l, int r) const {
        int k = bit_width((uint32_t) r - l) - 1;
        int mn = min(st_min[k][l], st_min[k][r - (1 << k)]);
        int mx = max(st_max[k][l], st_max[k][r - (1 << k)]);
        return mx - mn;
    }
};

class Solution {
public:
    long long maxTotalValue(vector<int>& nums, int k) {
        int n = nums.size();
        ST st(nums);

        priority_queue<tuple<int, int, int>> pq;
        for (int i = 0; i < n; i++) {
            pq.emplace(st.query(i, n), i, n); // 子数组极差，左端点，右端点加一
        }

        long long ans = 0;
        while (k-- && get<0>(pq.top())) {
            auto [d, l, r] = pq.top();
            pq.pop();
            ans += d;
            pq.emplace(st.query(l, r - 1), l, r - 1);
        }
        return ans;
    }
};
```

```go [sol-Go]
type pair struct{ min, max int }

func op(a, b pair) pair {
	return pair{min(a.min, b.min), max(a.max, b.max)}
}

type ST [][16]pair // 16 = bits.Len(5e4)

func newST(a []int) ST {
	n := len(a)
	w := bits.Len(uint(n))
	st := make(ST, n)
	for i, x := range a {
		st[i][0] = pair{x, x}
	}
	for j := 1; j < w; j++ {
		for i := range n - 1<<j + 1 {
			st[i][j] = op(st[i][j-1], st[i+1<<(j-1)][j-1])
		}
	}
	return st
}

// [l,r) 左闭右开
func (st ST) query(l, r int) int {
	k := bits.Len(uint(r-l)) - 1
	p := op(st[l][k], st[r-1<<k][k])
	return p.max - p.min
}

func maxTotalValue(nums []int, k int) (ans int64) {
	n := len(nums)
	st := newST(nums)
	h := make(hp, n)
	for i := range h {
		h[i] = tuple{st.query(i, n), i, n} // 子数组极差，左端点，右端点加一
	}
	// 由于 h 是递减的，无需堆化

	for ; k > 0 && h[0].d > 0; k-- {
		ans += int64(h[0].d)
		h[0].r--
		h[0].d = st.query(h[0].l, h[0].r)
		heap.Fix(&h, 0)
	}
	return
}

type tuple struct{ d, l, r int }
type hp []tuple

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].d > h[j].d } // 最大堆
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (hp) Push(any)             {}
func (hp) Pop() (_ any)         { return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+k)\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\log n)$。用线段树可以做到 $\mathcal{O}(n)$ 空间。

### 写法二

由于矩阵每一列是递减的，我们可以先把 $M_{0,n-1}$ 入堆，在循环的过程中把其余 $M_{i,n-1}$ 入堆。

由于一开始堆的大小不大，出堆入堆更快，整体效率更高。

```py [sol-Python3]
# 手写 min max 更快
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

def op(a: Tuple[int, int], b: Tuple[int, int]) -> Tuple[int, int]:
    return min(a[0], b[0]), max(a[1], b[1])

class ST:
    def __init__(self, a: List[int]):
        n = len(a)
        w = n.bit_length()
        st = [[None] * n for _ in range(w)]
        st[0] = [(x, x) for x in a]
        for i in range(1, w):
            for j in range(n - (1 << i) + 1):
                st[i][j] = op(st[i - 1][j], st[i - 1][j + (1 << (i - 1))])
        self.st = st

    # [l, r) 左闭右开
    def query(self, l: int, r: int) -> int:
        k = (r - l).bit_length() - 1
        mn, mx = op(self.st[k][l], self.st[k][r - (1 << k)])
        return mx - mn

class Solution:
    def maxTotalValue(self, nums: List[int], k: int) -> int:
        n = len(nums)
        st = ST(nums)

        h = [(st.query(0, n), 0, n)]
        ans = 0
        for _ in range(k):
            d, l, r = h[0]
            if d == 0:  # 堆中剩余元素全是 0
                break
            ans += d
            heapreplace_max(h, (st.query(l, r - 1), l, r - 1))
            if r == n and l + 1 < n:
                heappush_max(h, (st.query(l + 1, n), l + 1, n))
        return ans
```

```java [sol-Java]
class ST {
    private final int[][] stMin;
    private final int[][] stMax;

    public ST(int[] a) {
        int n = a.length;
        int w = 32 - Integer.numberOfLeadingZeros(n);
        stMin = new int[w][n];
        stMax = new int[w][n];

        for (int j = 0; j < n; j++) {
            stMin[0][j] = a[j];
            stMax[0][j] = a[j];
        }

        for (int i = 1; i < w; i++) {
            for (int j = 0; j + (1 << i) <= n; j++) {
                stMin[i][j] = Math.min(stMin[i - 1][j], stMin[i - 1][j + (1 << (i - 1))]);
                stMax[i][j] = Math.max(stMax[i - 1][j], stMax[i - 1][j + (1 << (i - 1))]);
            }
        }
    }

    // [l, r) 左闭右开
    public int query(int l, int r) {
        int k = 31 - Integer.numberOfLeadingZeros(r - l);
        int mn = Math.min(stMin[k][l], stMin[k][r - (1 << k)]);
        int mx = Math.max(stMax[k][l], stMax[k][r - (1 << k)]);
        return mx - mn;
    }
}

class Solution{
    public long maxTotalValue(int[] nums, int k) {
        int n = nums.length;
        ST st = new ST(nums);

        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> b[0] - a[0]); // 最大堆
        pq.add(new int[]{st.query(0, n), 0, n}); // 子数组极差，左端点，右端点加一

        long ans = 0;
        while (k-- > 0 && pq.peek()[0] > 0) {
            int[] top = pq.poll();
            int d = top[0], l = top[1], r = top[2];
            ans += d;
            pq.add(new int[]{st.query(l, r - 1), l, r - 1});
            if (r == n && l + 1 < n) {
                pq.add(new int[]{st.query(l + 1, n), l + 1, n});
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class ST {
    vector<vector<int>> st_min;
    vector<vector<int>> st_max;

public:
    ST(const vector<int>& a) {
        size_t n = a.size();
        int w = bit_width(n);
        st_min.resize(w, vector<int>(n));
        st_max.resize(w, vector<int>(n));

        for (int j = 0; j < n; j++) {
            st_min[0][j] = a[j];
            st_max[0][j] = a[j];
        }

        for (int i = 1; i < w; i++) {
            for (int j = 0; j + (1 << i) <= n; j++) {
                st_min[i][j] = min(st_min[i - 1][j], st_min[i - 1][j + (1 << (i - 1))]);
                st_max[i][j] = max(st_max[i - 1][j], st_max[i - 1][j + (1 << (i - 1))]);
            }
        }
    }

    // [l, r) 左闭右开
    int query(int l, int r) const {
        int k = bit_width((uint32_t) r - l) - 1;
        int mn = min(st_min[k][l], st_min[k][r - (1 << k)]);
        int mx = max(st_max[k][l], st_max[k][r - (1 << k)]);
        return mx - mn;
    }
};

class Solution {
public:
    long long maxTotalValue(vector<int>& nums, int k) {
        int n = nums.size();
        ST st(nums);

        priority_queue<tuple<int, int, int>> pq;
        pq.emplace(st.query(0, n), 0, n); // 子数组极差，左端点，右端点加一

        long long ans = 0;
        while (k-- && get<0>(pq.top())) {
            auto [d, l, r] = pq.top();
            pq.pop();
            ans += d;
            pq.emplace(st.query(l, r - 1), l, r - 1);
            if (r == n && l + 1 < n) {
                pq.emplace(st.query(l + 1, n), l + 1, n);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type pair struct{ min, max int }

func op(a, b pair) pair {
	return pair{min(a.min, b.min), max(a.max, b.max)}
}

type ST [][16]pair // 16 = bits.Len(5e4)

func newST(a []int) ST {
	n := len(a)
	w := bits.Len(uint(n))
	st := make(ST, n)
	for i, x := range a {
		st[i][0] = pair{x, x}
	}
	for j := 1; j < w; j++ {
		for i := range n - 1<<j + 1 {
			st[i][j] = op(st[i][j-1], st[i+1<<(j-1)][j-1])
		}
	}
	return st
}

// [l,r) 左闭右开
func (st ST) query(l, r int) int {
	k := bits.Len(uint(r-l)) - 1
	p := op(st[l][k], st[r-1<<k][k])
	return p.max - p.min
}

func maxTotalValue(nums []int, k int) (ans int64) {
	n := len(nums)
	st := newST(nums)
	h := hp{{st.query(0, n), 0, n}} // 子数组极差，左端点，右端点加一

	for ; k > 0 && h[0].d > 0; k-- {
		ans += int64(h[0].d)
		l, r := h[0].l, h[0].r
		h[0].r--
		h[0].d = st.query(h[0].l, h[0].r)
		heap.Fix(&h, 0)
		if r == n && l+1 < n {
			heap.Push(&h, tuple{st.query(l+1, n), l + 1, n})
		}
	}
	return
}

type tuple struct{ d, l, r int }
type hp []tuple

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].d > h[j].d } // 最大堆
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(tuple)) }
func (hp) Pop() (_ any)         { return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + k\log \min(n,k))$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\log n)$。用线段树可以做到 $\mathcal{O}(n)$ 空间。

## 方法二：二分 + 滑动窗口 + 单调队列 + 单调栈 + Lazy 线段树 + 线段树二分

如果 $k$ 更大，方法一就超时了。下面介绍一种时间复杂度和 $k$ 无关的做法。

这个做法会多次用到一个简单又重要的**性质**：

- 当右端点固定时，左端点越小，子数组的最小值越小，最大值越大，所以子数组的值也就越大；反之，左端点越大，子数组的最小值越大，最大值越小，所以子数组的值也就越小。

### 核心思路

1. 二分找第 $k$ 大的子数组值 $\textit{lowD}$。
2. 用单调栈 + Lazy 线段树计算 $\ge \textit{lowD}$ 的子数组值的个数 $\textit{cnt}$，以及子数组值的和 $\textit{sum}$。
3. 得到最终答案 $\textit{sum} - (\textit{cnt} - k)\cdot \textit{lowD}$。

### 二分 + 滑动窗口 + 单调队列

类似 [378. 有序矩阵中第 K 小的元素](https://leetcode.cn/problems/kth-smallest-element-in-a-sorted-matrix/)，我们可以二分找第 $k$ 大的子数组值 $\textit{lowD}$，问题变成：

- 有多少个子数组值 $\ge \textit{lowD}$？

根据性质，右端点增大时，满足要求的最大左端点也会增大，所以可以用滑动窗口解决，原理见[【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

我们需要求出滑动窗口最大值和最小值，这可以用 [单调队列【基础算法精讲 27】](https://www.bilibili.com/video/BV1bM411X72E/)，相关题目是 [1438. 绝对差不超过限制的最长连续子数组](https://leetcode.cn/problems/longest-continuous-subarray-with-absolute-diff-less-than-or-equal-to-limit/)。

注意这是「越长越合法」型滑动窗口问题，根据 [滑动窗口题单](https://leetcode.cn/circle/discuss/0viNMK/) 中的总结，代码要写 `cnt += left`。

### 单调栈 + Lazy 线段树 + 线段树二分

比如 $\textit{nums}$ 遍历过的元素为 $2,1,2,3,4,5$，右端点为元素 $5$ 时，子数组的最小值从左到右依次为 $1,1,2,3,4,5$（子数组左端点分别为 $0,1,2,3,4,5$）。

继续向后遍历，比如遍历到了元素 $3$，那么以元素 $3$ 为右端点时，子数组的最小值从左到右依次为 $1,1,2,3,3,3,3$。

这相当于：

1. 先找到 $3$ 左边最近的小于等于 $3$ 的元素位置 $j$。这可以用单调栈实现，原理见[【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)。
2. 设 $3$ 的下标为 $i$。把 $[j+1,i]$ 中的元素都置为 $3$。这可以用 Lazy 线段树。

对于最大值的维护，做法同理。

然后，我们需要查询满足子数组值 $\ge \textit{lowD}$ 的最大左端点下标。根据性质，我们可以**在线段树上二分**。对于线段树二分，我们需要知道对于一棵子树，是否要递归这棵子树，还是完全没有必要递归。由于左端点越小，子数组的最小值越小，最大值越大，我们需要记录最靠左的最小值和最大值，如果发现最靠左的最大值减去最小值 $< \textit{lowD}$，那么就无需递归这棵子树。

设线段树二分得到的下标为 $l$，那么满足子数组值 $\ge \textit{lowD}$ 的子数组左端点可以是 $0,1,2,\dots,l$，这一共有 $l+1$ 个，加到 $\textit{cnt}$ 中。

为了查询右端点固定为 $i$，左端点在 $[0,l]$ 中的子数组值之和，线段树还需要维护最大值之和以及最小值之和。把查询到的子数组值之和加到 $\textit{sum}$ 中。

最终答案为

$$
\textit{sum} - (\textit{cnt} - k)\cdot \textit{lowD}
$$

其中 $(\textit{cnt} - k)\cdot \textit{lowD}$ 是把多算的 $\textit{cnt} - k$ 个恰好等于 $\textit{lowD}$ 的子数组值减去。

这个做法复杂度与 $k$ 无关，但常数比方法一大。

```py [sol-Python3]
class Node:
    # val = [sum_min, sum_max, l_min, l_max]
    # todo = [todo_min, todo_max]
    __slots__ = 'val', 'todo'


class LazySegmentTree:
    # 懒标记初始值
    _TODO_INIT = [-1, -1]

    def __init__(self, n: int):
        # 线段树维护一个长为 n 的数组（下标从 0 到 n-1）
        self._n = n
        self._tree = [Node() for _ in range(2 << (n - 1).bit_length())]
        self._build(1, 0, n - 1)

    # 合并两个 val
    def _merge_val(self, a: List[int], b: List[int]) -> List[int]:
        return [a[0] + b[0], a[1] + b[1], a[2], a[3]]

    # 把懒标记作用到 node 子树（本例为区间加）
    def _apply(self, node: int, l: int, r: int, todo) -> None:
        cur = self._tree[node]
        # 计算 tree[node] 区间的整体变化
        todo_min, todo_max = todo
        if todo_min >= 0:
            cur.val[0] = todo_min * (r - l + 1)
            cur.val[2] = todo_min
            cur.todo[0] = todo_min
        if todo_max >= 0:
            cur.val[1] = todo_max * (r - l + 1)
            cur.val[3] = todo_max
            cur.todo[1] = todo_max

    # 把当前节点的懒标记下传给左右儿子
    def _spread(self, node: int, l: int, r: int) -> None:
        todo = self._tree[node].todo
        if todo == self._TODO_INIT:  # 没有需要下传的信息
            return
        m = (l + r) // 2
        self._apply(node * 2, l, m, todo)
        self._apply(node * 2 + 1, m + 1, r, todo)
        todo[:] = self._TODO_INIT[:]  # 下传完毕

    # 合并左右儿子的 val 到当前节点的 val
    def _maintain(self, node: int) -> None:
        self._tree[node].val = self._merge_val(self._tree[node * 2].val, self._tree[node * 2 + 1].val)

    # 初始化线段树
    # 时间复杂度 O(n)
    def _build(self, node: int, l: int, r: int) -> None:
        self._tree[node].val = [0] * 4
        self._tree[node].todo = self._TODO_INIT[:]
        if l == r:  # 叶子
            return
        m = (l + r) // 2
        self._build(node * 2, l, m)  # 初始化左子树
        self._build(node * 2 + 1, m + 1, r)  # 初始化右子树
        self._maintain(node)

    def _update(self, node: int, l: int, r: int, ql: int, qr: int, f: Tuple[int, int]) -> None:
        if ql <= l and r <= qr:  # 当前子树完全在 [ql, qr] 内
            self._apply(node, l, r, f)
            return
        self._spread(node, l, r)
        m = (l + r) // 2
        if ql <= m:  # 更新左子树
            self._update(node * 2, l, m, ql, qr, f)
        if qr > m:  # 更新右子树
            self._update(node * 2 + 1, m + 1, r, ql, qr, f)
        self._maintain(node)

    def _query(self, node: int, l: int, r: int, ql: int, qr: int) -> List[int]:
        if ql <= l and r <= qr:  # 当前子树完全在 [ql, qr] 内
            return self._tree[node].val
        self._spread(node, l, r)
        m = (l + r) // 2
        if qr <= m:  # [ql, qr] 在左子树
            return self._query(node * 2, l, m, ql, qr)
        if ql > m:  # [ql, qr] 在右子树
            return self._query(node * 2 + 1, m + 1, r, ql, qr)
        l_res = self._query(node * 2, l, m, ql, qr)
        r_res = self._query(node * 2 + 1, m + 1, r, ql, qr)
        return self._merge_val(l_res, r_res)

    def _find_last(self, node: int, l: int, r: int, ql: int, qr: int, f: Callable[[List[int]], int]) -> int:
        if l > qr or r < ql or not f(self._tree[node].val):
            return -1
        if l == r:
            return l
        self._spread(node, l, r)
        m = (l + r) // 2
        idx = self._find_last(node * 2 + 1, m + 1, r, ql, qr, f)
        if idx < 0:
            idx = self._find_last(node * 2, l, m, ql, qr, f)
        return idx

    # 用 f 更新 [ql, qr] 中的每个 a[i]
    # 0 <= ql <= qr <= n-1
    # 时间复杂度 O(log n)
    def update(self, ql: int, qr: int, f: Tuple[int, int]) -> None:
        self._update(1, 0, self._n - 1, ql, qr, f)

    # 返回用 _merge_val 合并所有 a[i] 的计算结果，其中 i 在闭区间 [ql, qr] 中
    # 0 <= ql <= qr <= n-1
    # 时间复杂度 O(log n)
    def query(self, ql: int, qr: int) -> List[int]:
        return self._query(1, 0, self._n - 1, ql, qr)

    # 返回 [ql, qr] 内最后一个满足 f 的下标
    # 0 <= ql <= qr <= n-1
    # 时间复杂度 O(log n)
    def find_last(self, ql: int, qr: int, f: Callable[[List[int]], int]) -> int:
        return self._find_last(1, 0, self._n - 1, ql, qr, f)


class Solution:
    def maxTotalValue(self, nums: List[int], k: int) -> int:
        # 二分 + 滑动窗口 + 单调队列
        def check(low_d: int) -> bool:
            low_d += 1
            # 1438. 绝对差不超过限制的最长连续子数组（改成求子数组个数）
            min_q = deque()
            max_q = deque()
            cnt = left = 0

            for i, x in enumerate(nums):
                # 1. 右边入
                while min_q and x <= nums[min_q[-1]]:
                    min_q.pop()
                min_q.append(i)

                while max_q and x >= nums[max_q[-1]]:
                    max_q.pop()
                max_q.append(i)

                # 2. 左边出
                while nums[max_q[0]] - nums[min_q[0]] >= low_d:
                    left += 1
                    if min_q[0] < left:  # 队首不在窗口中
                        min_q.popleft()
                    if max_q[0] < left:  # 队首不在窗口中
                        max_q.popleft()

                cnt += left
                if cnt >= k:
                    return False
            return True

        low_d = bisect_left(range(max(nums) - min(nums)), True, key=check)

        # 单调栈
        n = len(nums)
        left_less_eq = [0] * n
        left_great_eq = [0] * n
        st1 = [-1]  # 哨兵
        st2 = [-1]
        for i, x in enumerate(nums):
            while len(st1) > 1 and nums[st1[-1]] > x:
                st1.pop()
            left_less_eq[i] = st1[-1]
            st1.append(i)

            while len(st2) > 1 and nums[st2[-1]] < x:
                st2.pop()
            left_great_eq[i] = st2[-1]
            st2.append(i)

        # Lazy 线段树
        t = LazySegmentTree(n)
        cnt = s = 0
        for i, x in enumerate(nums):
            t.update(left_less_eq[i] + 1, i, (x, -1))
            t.update(left_great_eq[i] + 1, i, (-1, x))
            l = t.find_last(0, i, lambda v: v[3] - v[2] >= low_d)
            if l >= 0:
                cnt += l + 1
                d = t.query(0, l)
                s += d[1] - d[0]

        return s - (cnt - k) * low_d  # 减掉多算的
```

```cpp [sol-C++]
class LazySegmentTree {
    using T = tuple<long long, long long, int, int>;
    using F = pair<int, int>;

    // 懒标记初始值
    const F TODO_INIT = {-1, -1};

    struct Node {
        T val;
        F todo;
    };

    int n;
    vector<Node> tree;

    // 合并两个 val
    T merge_val(T a, T b) const {
        auto [sum_min_a, sum_max_a, l_min, l_max] = a;
        auto [sum_min_b, sum_max_b, _, _] = b;
        return {sum_min_a + sum_min_b, sum_max_a + sum_max_b, l_min, l_max};
    }

    // 把懒标记作用到 node 子树（本例为区间加）
    void apply(int node, int l, int r, F todo) {
        Node& cur = tree[node];
        // 计算 tree[node] 区间的整体变化
        auto [todo_min, todo_max] = todo;
        auto& [sum_min, sum_max, l_min, l_max] = cur.val;
        auto& [cur_todo_min, cur_todo_max] = cur.todo;
        if (todo_min >= 0) {
            sum_min = 1LL * todo_min * (r - l + 1);
            l_min = todo_min;
            cur_todo_min = todo_min;
        }
        if (todo_max >= 0) {
            sum_max = 1LL * todo_max * (r - l + 1);
            l_max = todo_max;
            cur_todo_max = todo_max;
        }
    }

    // 把当前节点的懒标记下传给左右儿子
    void spread(int node, int l, int r) {
        Node& cur = tree[node];
        F todo = cur.todo;
        if (todo == TODO_INIT) { // 没有需要下传的信息
            return;
        }
        int m = (l + r) / 2;
        apply(node * 2, l, m, todo);
        apply(node * 2 + 1, m + 1, r, todo);
        cur.todo = TODO_INIT; // 下传完毕
    }

    // 合并左右儿子的 val 到当前节点的 val
    void maintain(int node) {
        tree[node].val = merge_val(tree[node * 2].val, tree[node * 2 + 1].val);
    }

    // 用 a 初始化线段树
    // 时间复杂度 O(n)
    void build(int node, int l, int r) {
        Node& cur = tree[node];
        cur.todo = TODO_INIT;
        if (l == r) { // 叶子
            return;
        }
        int m = (l + r) / 2;
        build(node * 2, l, m); // 初始化左子树
        build(node * 2 + 1, m + 1, r); // 初始化右子树
        maintain(node);
    }

    void update(int node, int l, int r, int ql, int qr, F f) {
        if (ql <= l && r <= qr) { // 当前子树完全在 [ql, qr] 内
            apply(node, l, r, f);
            return;
        }
        spread(node, l, r);
        int m = (l + r) / 2;
        if (ql <= m) { // 更新左子树
            update(node * 2, l, m, ql, qr, f);
        }
        if (qr > m) { // 更新右子树
            update(node * 2 + 1, m + 1, r, ql, qr, f);
        }
        maintain(node);
    }

    T query(int node, int l, int r, int ql, int qr) {
        if (ql <= l && r <= qr) { // 当前子树完全在 [ql, qr] 内
            return tree[node].val;
        }
        spread(node, l, r);
        int m = (l + r) / 2;
        if (qr <= m) { // [ql, qr] 在左子树
            return query(node * 2, l, m, ql, qr);
        }
        if (ql > m) { // [ql, qr] 在右子树
            return query(node * 2 + 1, m + 1, r, ql, qr);
        }
        T l_res = query(node * 2, l, m, ql, qr);
        T r_res = query(node * 2 + 1, m + 1, r, ql, qr);
        return merge_val(l_res, r_res);
    }

    int find_last(int node, int l, int r, int ql, int qr, int low_d) {
        if (l > qr || r < ql || get<3>(tree[node].val) - get<2>(tree[node].val) < low_d) {
            return -1;
        }
        if (l == r) {
            return l;
        }
        spread(node, l, r);
        int m = (l + r) / 2;
        int idx = find_last(node * 2 + 1, m + 1, r, ql, qr, low_d);
        if (idx < 0) {
            idx = find_last(node * 2, l, m, ql, qr, low_d);
        }
        return idx;
    }

public:
    // 线段树维护一个长为 n 的数组（下标从 0 到 n-1）
    LazySegmentTree(int n) : n(n), tree(2 << bit_width((uint32_t) n - 1)) {
        build(1, 0, n - 1);
    }

    // 用 f 更新 [ql, qr] 中的每个 a[i]
    // 0 <= ql <= qr <= n-1
    // 时间复杂度 O(log n)
    void update(int ql, int qr, F f) {
        update(1, 0, n - 1, ql, qr, f);
    }

    // 返回用 merge_val 合并所有 a[i] 的计算结果，其中 i 在闭区间 [ql, qr] 中
    // 0 <= ql <= qr <= n-1
    // 时间复杂度 O(log n)
    T query(int ql, int qr) {
        return query(1, 0, n - 1, ql, qr);
    }

    // 返回 [ql, qr] 内最后一个 sum_max - sum_min >= low_d 的下标
    // 0 <= ql <= qr <= n-1
    // 时间复杂度 O(log n)
    int find_last(int ql, int qr, int low_d) {
        return find_last(1, 0, n - 1, ql, qr, low_d);
    }
};

class Solution {
public:
    long long maxTotalValue(vector<int>& nums, int k) {
        // 二分 + 滑动窗口 + 单调队列
        // 1438. 绝对差不超过限制的最长连续子数组（改成求子数组个数）
        auto check = [&](int low_d) -> bool {
            deque<int> min_q, max_q;
            int cnt = 0, left = 0;

            for (int i = 0; i < nums.size(); i++) {
                int x = nums[i];

                // 1. 右边入
                while (!min_q.empty() && x <= nums[min_q.back()]) {
                    min_q.pop_back();
                }
                min_q.push_back(i);

                while (!max_q.empty() && x >= nums[max_q.back()]) {
                    max_q.pop_back();
                }
                max_q.push_back(i);

                // 2. 左边出
                while (nums[max_q.front()] - nums[min_q.front()] >= low_d) {
                    left++;
                    if (min_q.front() < left) { // 队首不在窗口中
                        min_q.pop_front();
                    }
                    if (max_q.front() < left) { // 队首不在窗口中
                        max_q.pop_front();
                    }
                }

                cnt += left;
                if (cnt >= k) {
                    return true;
                }
            }
            return false;
        };

        auto [mn, mx] = ranges::minmax(nums);
        int left = 0, right = mx - mn + 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? left : right) = mid;
        }
        int low_d = left;

        // 单调栈
        int n = nums.size();
        vector<int> left_less_eq(n), left_great_eq(n);
        stack<int> st1, st2;
        st1.push(-1); // 哨兵
        st2.push(-1);
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            while (st1.size() > 1 && nums[st1.top()] > x) {
                st1.pop();
            }
            left_less_eq[i] = st1.top();
            st1.push(i);

            while (st2.size() > 1 && nums[st2.top()] < x) {
                st2.pop();
            }
            left_great_eq[i] = st2.top();
            st2.push(i);
        }

        // Lazy 线段树
        LazySegmentTree t(n);
        long long cnt = 0, sum = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            t.update(left_less_eq[i] + 1, i, pair(x, -1));
            t.update(left_great_eq[i] + 1, i, pair(-1, x));
            int l = t.find_last(0, i, low_d);
            if (l >= 0) {
                cnt += l + 1;
                auto [sum_min, sum_max, _, _] = t.query(0, l);
                sum += sum_max - sum_min;
            }
        }

        return sum - (cnt - k) * low_d; // 减掉多算的
    }
};
```

```go [sol-Go]
type data struct{ sumMin, sumMax, lMin, lMax int }
type todo struct{ todoMin, todoMax int }
type lazySeg []struct {
	l, r int
	data
	todo
}

var todoInit = todo{-1, -1}

func merge(l, r data) data {
	return data{l.sumMin + r.sumMin, l.sumMax + r.sumMax, l.lMin, l.lMax}
}

func (t lazySeg) apply(o int, f todo) {
	cur := &t[o]
	sz := cur.r - cur.l + 1
	if f.todoMin >= 0 {
		cur.lMin = f.todoMin
		cur.sumMin = f.todoMin * sz
		cur.todoMin = f.todoMin
	}
	if f.todoMax >= 0 {
		cur.lMax = f.todoMax
		cur.sumMax = f.todoMax * sz
		cur.todoMax = f.todoMax
	}
}

func (t lazySeg) maintain(o int) {
	t[o].data = merge(t[o<<1].data, t[o<<1|1].data)
}

func (t lazySeg) spread(o int) {
	f := t[o].todo
	if f == todoInit {
		return
	}
	t.apply(o<<1, f)
	t.apply(o<<1|1, f)
	t[o].todo = todoInit
}

func (t lazySeg) build(o, l, r int) {
	t[o].l, t[o].r = l, r
	t[o].todo = todoInit
	if l == r {
		return
	}
	m := (l + r) >> 1
	t.build(o<<1, l, m)
	t.build(o<<1|1, m+1, r)
}

func (t lazySeg) update(o, l, r int, f todo) {
	if l <= t[o].l && t[o].r <= r {
		t.apply(o, f)
		return
	}
	t.spread(o)
	m := (t[o].l + t[o].r) >> 1
	if l <= m {
		t.update(o<<1, l, r, f)
	}
	if m < r {
		t.update(o<<1|1, l, r, f)
	}
	t.maintain(o)
}

func (t lazySeg) query(o, l, r int) data {
	if l <= t[o].l && t[o].r <= r {
		return t[o].data
	}
	t.spread(o)
	m := (t[o].l + t[o].r) >> 1
	if r <= m {
		return t.query(o<<1, l, r)
	}
	if l > m {
		return t.query(o<<1|1, l, r)
	}
	lRes := t.query(o<<1, l, r)
	rRes := t.query(o<<1|1, l, r)
	return merge(lRes, rRes)
}

// 返回 [l,r] 内最后一个满足 f 的下标
func (t lazySeg) findLast(o, l, r int, f func(data) bool) int {
	if t[o].l > r || t[o].r < l || !f(t[o].data) {
		return -1
	}
	if t[o].l == t[o].r {
		return t[o].l
	}
	t.spread(o)
	idx := t.findLast(o<<1|1, l, r, f)
	if idx < 0 {
		idx = t.findLast(o<<1, l, r, f)
	}
	return idx
}

func maxTotalValue(nums []int, k int) (ans int64) {
	// 二分 + 滑动窗口 + 单调队列
	lowD := sort.Search(slices.Max(nums)-slices.Min(nums), func(lowD int) bool {
		lowD++
		// 1438. 绝对差不超过限制的最长连续子数组（改成求子数组个数）
		var minQ, maxQ []int
		cnt, left := 0, 0
		for right, x := range nums {
			for len(minQ) > 0 && x <= nums[minQ[len(minQ)-1]] {
				minQ = minQ[:len(minQ)-1]
			}
			minQ = append(minQ, right)

			for len(maxQ) > 0 && x >= nums[maxQ[len(maxQ)-1]] {
				maxQ = maxQ[:len(maxQ)-1]
			}
			maxQ = append(maxQ, right)

			for nums[maxQ[0]]-nums[minQ[0]] >= lowD {
				left++
				if minQ[0] < left {
					minQ = minQ[1:]
				}
				if maxQ[0] < left {
					maxQ = maxQ[1:]
				}
			}

			cnt += left
			if cnt >= k {
				return false
			}
		}
		return true
	})

	// 单调栈
	n := len(nums)
	leftLessEq := make([]int, n)
	leftGreatEq := make([]int, n)
	st1 := []int{-1} // 哨兵
	st2 := []int{-1}
	for i, x := range nums {
		for len(st1) > 1 && nums[st1[len(st1)-1]] > x {
			st1 = st1[:len(st1)-1]
		}
		leftLessEq[i] = st1[len(st1)-1]
		st1 = append(st1, i)

		for len(st2) > 1 && nums[st2[len(st2)-1]] < x {
			st2 = st2[:len(st2)-1]
		}
		leftGreatEq[i] = st2[len(st2)-1]
		st2 = append(st2, i)
	}

	// Lazy 线段树
	t := make(lazySeg, 2<<bits.Len(uint(n-1)))
	t.build(1, 0, n-1)
	cnt, sum := 0, 0
	for i, x := range nums {
		t.update(1, leftLessEq[i]+1, i, todo{x, -1})
		t.update(1, leftGreatEq[i]+1, i, todo{-1, x})
		l := t.findLast(1, 0, i, func(d data) bool { return d.lMax-d.lMin >= lowD })
		if l >= 0 {
			cnt += l + 1
			d := t.query(1, 0, l)
			sum += d.sumMax - d.sumMin
		}
	}

	return int64(sum - (cnt-k)*lowD) // 减掉多算的
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U + n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§5.3 第 K 小/大**」。

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

本题来自 `二、二分答案 / §2.6 第 K 小/大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.6 第 K 小/大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
