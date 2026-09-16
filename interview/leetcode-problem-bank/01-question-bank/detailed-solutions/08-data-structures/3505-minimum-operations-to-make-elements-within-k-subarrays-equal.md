# 3505. 使 K 个子数组内元素相等的最少操作数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-elements-within-k-subarrays-equal/
- 题目 slug：`minimum-operations-to-make-elements-within-k-subarrays-equal`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）
- 难度分：2539
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-elements-within-k-subarrays-equal/solutions/3633641/hua-dong-chuang-kou-zhong-wei-shu-hua-fe-e9cn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口中位数 + 距离和 + 划分型 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-make-elements-within-k-subarrays-equal/solutions/3633641/hua-dong-chuang-kou-zhong-wei-shu-hua-fe-e9cn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-dong-chuang-kou-zhong-wei-shu-hua-fe-e9cn`
- topic id：`3633641`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 把数组中的数都变成哪个数最优？

给定数组，每次操作可以把其中一个数加一/减一，把所有数都变成一样的，最少要操作多少次？

把所有数都变成数组的**中位数**是最优的。

[证明](https://zhuanlan.zhihu.com/p/1922938031687595039)。

## 滑动窗口中位数

见 [480. 滑动窗口中位数](https://leetcode.cn/problems/sliding-window-median/)，这可以用对顶堆做，具体见 [我的题解](https://leetcode.cn/problems/sliding-window-median/solutions/3628827/295-ti-lan-shan-chu-dui-pythonjavacgojsr-66ch/)。

## 如何计算操作次数？

需要额外维护**堆中元素之和**。

见[【一图秒懂】距离和](https://leetcode.cn/problems/minimum-operations-to-make-all-array-elements-equal/solution/yi-tu-miao-dong-pai-xu-qian-zhui-he-er-f-nf55/)。

本题 $j$ 为 $\textit{left}$（见 480 题解中的定义）的大小。

都变成 $\textit{left}$ 的堆顶 $v$，那么：

- 蓝色面积为 $v\cdot j - S_L$，其中 $S_L$ 为 $\textit{left}$ 的元素和。
- 绿色面积为 $S_R - v\cdot (x-j)$，其中 $S_R$ 为 $\textit{right}$ 的元素和。

操作次数即为面积之和

$$
v\cdot j - S_L + S_R - v\cdot (x-j)
$$

## 划分型 DP

根据 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/)「§5.3 约束划分个数」，定义 $f[i][j]$ 表示在下标区间 $[0,j-1]$ 中选 $i$ 个长为 $x$ 的子数组的最小操作次数。

分类讨论：

- 子数组不含 $\textit{nums}[j-1]$，问题变成下标 $[0,j-2]$ 中选 $i$ 个长为 $x$ 的子数组的最小操作次数，即 $f[i][j-1]$。
- 子数组含 $\textit{nums}[j-1]$，子数组的左端点为 $j-x$，问题变成下标 $[0,j-x-1]$ 中选 $i-1$ 个长为 $x$ 的子数组的最小操作次数，即 $f[i-1][j-x]$。

取最小值，有

$$
f[i][j] =\min(f[i][j-1], f[i-1][j-x] + \textit{dis}[j-x]) 
$$

其中 $dis[i]$ 是滑动窗口算出的距离和（结果保存在子数组左端点）。

初始值 $f[0][j]=0$，$f[i][i\cdot x-1]=\infty$。注意不需要初始化 $j<i\cdot x-1$ 的状态，因为我们不会访问这些状态。

答案为 $f[k][n]$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV17yZzYbEP8/?t=37m44s)，欢迎点赞关注~

```py [sol-Python3]
class LazyHeap:
    def __init__(self):
        self.heap = []
        self.remove_cnt = defaultdict(int)  # 每个元素剩余需要删除的次数
        self.size = 0  # 实际大小
        self.sum = 0  # 堆中元素总和

    # 删除
    def remove(self, x: int) -> None:
        self.remove_cnt[x] += 1  # 懒删除
        self.size -= 1
        self.sum -= x

    # 正式执行删除操作
    def apply_remove(self) -> None:
        while self.heap and self.remove_cnt[self.heap[0]] > 0:
            self.remove_cnt[self.heap[0]] -= 1
            heappop(self.heap)

    # 查看堆顶
    def top(self) -> int:
        self.apply_remove()
        return self.heap[0]

    # 出堆
    def pop(self) -> int:
        self.apply_remove()
        self.size -= 1
        self.sum -= self.heap[0]
        return heappop(self.heap)

    # 入堆
    def push(self, x: int) -> None:
        if self.remove_cnt[x] > 0:
            self.remove_cnt[x] -= 1  # 抵消之前的删除
        else:
            heappush(self.heap, x)
        self.size += 1
        self.sum += x

    # push(x) 然后 pop()
    def pushpop(self, x: int) -> int:
        self.apply_remove()
        if not self.heap or x <= self.heap[0]:
            return x
        self.sum += x - self.heap[0]
        return heappushpop(self.heap, x)


class Solution:
    # 480. 滑动窗口中位数（有改动）
    # 返回 nums 的所有长为 k 的子数组的（到子数组中位数的）距离和
    def medianSlidingWindow(self, nums: list[int], k: int) -> list[int]:
        ans = [0] * (len(nums) - k + 1)
        left = LazyHeap()  # 最大堆（元素取反）
        right = LazyHeap()  # 最小堆

        for i, x in enumerate(nums):
            # 1. 进入窗口
            if left.size == right.size:
                left.push(-right.pushpop(x))
            else:
                right.push(-left.pushpop(-x))

            l = i + 1 - k
            if l < 0:  # 窗口大小不足 k
                continue

            # 2. 计算答案
            v = -left.top()
            s1 = v * left.size + left.sum  # sum 取反
            s2 = right.sum - v * right.size
            ans[l] = s1 + s2

            # 3. 离开窗口
            x = nums[l]
            if x <= -left.top():
                left.remove(-x)
                if left.size < right.size:
                    left.push(-right.pop())  # 平衡两个堆的大小
            else:
                right.remove(x)
                if left.size > right.size + 1:
                    right.push(-left.pop())  # 平衡两个堆的大小

        return ans

    def minOperations(self, nums: List[int], x: int, k: int) -> int:
        n = len(nums)
        dis = self.medianSlidingWindow(nums, x)
        f = [[0] * (n + 1) for _ in range(k + 1)]
        for i in range(1, k + 1):
            f[i][i * x - 1] = inf
            for j in range(i * x, n - (k - i) * x + 1):  # 左右留出足够空间给其他子数组
                f[i][j] = min(f[i][j - 1], f[i - 1][j - x] + dis[j - x])  # j-x 为子数组左端点
        return f[k][n]
```

```java [sol-Java]
class LazyHeap extends PriorityQueue<Integer> {
    private final Map<Integer, Integer> removeCnt = new HashMap<>(); // 每个元素剩余需要删除的次数
    private int size = 0; // 实际大小
    private long sum = 0; // 堆中元素总和

    public LazyHeap(Comparator<Integer> comparator) {
        super(comparator);
    }

    public int size() {
        return size;
    }

    public long sum() {
        return sum;
    }

    // 删除
    public void remove(int x) {
        removeCnt.merge(x, 1, Integer::sum); // 懒删除
        size--;
        sum -= x;
    }

    // 正式执行删除操作
    private void applyRemove() {
        while (removeCnt.getOrDefault(peek(), 0) > 0) {
            removeCnt.merge(poll(), -1, Integer::sum);
        }
    }

    // 查看堆顶
    public int top() {
        applyRemove();
        return peek();
    }

    // 出堆
    public int pop() {
        applyRemove();
        size--;
        sum -= peek();
        return poll();
    }

    // 入堆
    public void push(int x) {
        int c = removeCnt.getOrDefault(x, 0);
        if (c > 0) {
            removeCnt.put(x, c - 1); // 抵消之前的删除
        } else {
            offer(x);
        }
        size++;
        sum += x;
    }

    // push(x) 然后 pop()
    public int pushPop(int x) {
        applyRemove();
        sum += x;
        offer(x);
        sum -= peek();
        return poll();
    }
}

class Solution {
    public long minOperations(int[] nums, int x, int k) {
        int n = nums.length;
        long[] dis = medianSlidingWindow(nums, x);
        long[][] f = new long[k + 1][n + 1];
        for (int i = 1; i <= k; i++) {
            f[i][i * x - 1] = Long.MAX_VALUE;
            for (int j = i * x; j <= n - (k - i) * x; j++) { // 左右留出足够空间给其他子数组
                f[i][j] = Math.min(f[i][j - 1], f[i - 1][j - x] + dis[j - x]); // j-x 为子数组左端点
            }
        }
        return f[k][n];
    }

    // 480. 滑动窗口中位数（有改动）
    // 返回 nums 的所有长为 k 的子数组的（到子数组中位数的）距离和
    private long[] medianSlidingWindow(int[] nums, int k) {
        int n = nums.length;
        long[] ans = new long[n - k + 1];
        LazyHeap left = new LazyHeap((a, b) -> Integer.compare(b, a)); // 最大堆
        LazyHeap right = new LazyHeap(Integer::compare); // 最小堆

        for (int i = 0; i < n; i++) {
            // 1. 进入窗口
            int in = nums[i];
            if (left.size() == right.size()) {
                left.push(right.pushPop(in));
            } else {
                right.push(left.pushPop(in));
            }

            int l = i + 1 - k;
            if (l < 0) { // 窗口大小不足 k
                continue;
            }

            // 2. 计算答案
            long v = left.top();
            long s1 = v * left.size() - left.sum();
            long s2 = right.sum() - v * right.size();
            ans[l] = s1 + s2;

            // 3. 离开窗口
            int out = nums[l];
            if (out <= left.top()) {
                left.remove(out);
                if (left.size() < right.size()) {
                    left.push(right.pop()); // 平衡两个堆的大小
                }
            } else {
                right.remove(out);
                if (left.size() > right.size() + 1) {
                    right.push(left.pop()); // 平衡两个堆的大小
                }
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
template<typename T, typename Compare = less<T>>
class LazyHeap {
    priority_queue<T, vector<T>, Compare> pq;
    unordered_map<T, int> remove_cnt; // 每个元素剩余需要删除的次数
    size_t sz = 0; // 实际大小
    long long s = 0; // 堆中元素总和

    // 正式执行删除操作
    void apply_remove() {
        while (!pq.empty() && remove_cnt[pq.top()] > 0) {
            remove_cnt[pq.top()]--;
            pq.pop();
        }
    }

public:
    size_t size() {
        return sz;
    }

    long long sum() {
        return s;
    }

    // 删除
    void remove(T x) {
        remove_cnt[x]++; // 懒删除
        sz--;
        s -= x;
    }

    // 查看堆顶
    T top() {
        apply_remove();
        return pq.top();
    }

    // 出堆
    T pop() {
        apply_remove();
        T x = pq.top();
        pq.pop();
        sz--;
        s -= x;
        return x;
    }

    // 入堆
    void push(T x) {
        if (remove_cnt[x] > 0) {
            remove_cnt[x]--; // 抵消之前的删除
        } else {
            pq.push(x);
        }
        sz++;
        s += x;
    }

    // push(x) 然后 pop()
    T push_pop(T x) {
        apply_remove();
        pq.push(x);
        s += x;
        x = pq.top();
        pq.pop();
        s -= x;
        return x;
    }
};

class Solution {
    // 480. 滑动窗口中位数（有改动）
    // 返回 nums 的所有长为 k 的子数组的（到子数组中位数的）距离和
    vector<long long> medianSlidingWindow(vector<int>& nums, int k) {
        int n = nums.size();
        vector<long long> ans(n - k + 1);
        LazyHeap<int> left; // 最大堆
        LazyHeap<int, greater<>> right; // 最小堆

        for (int i = 0; i < n; i++) {
            // 1. 进入窗口
            int in = nums[i];
            if (left.size() == right.size()) {
                left.push(right.push_pop(in));
            } else {
                right.push(left.push_pop(in));
            }

            int l = i + 1 - k;
            if (l < 0) { // 窗口大小不足 k
                continue;
            }

            // 2. 计算答案
            long long v = left.top();
            long long s1 = v * left.size() - left.sum();
            long long s2 = right.sum() - v * right.size();
            ans[l] = s1 + s2;

            // 3. 离开窗口
            int out = nums[l];
            if (out <= left.top()) {
                left.remove(out);
                if (left.size() < right.size()) {
                    left.push(right.pop()); // 平衡两个堆的大小
                }
            } else {
                right.remove(out);
                if (left.size() > right.size() + 1) {
                    right.push(left.pop()); // 平衡两个堆的大小
                }
            }
        }

        return ans;
    }

public:
    long long minOperations(vector<int>& nums, int x, int k) {
        int n = nums.size();
        vector<long long> dis = medianSlidingWindow(nums, x);
        vector f(k + 1, vector<long long>(n + 1));
        for (int i = 1; i <= k; i++) {
            f[i][i * x - 1] = LLONG_MAX;
            for (int j = i * x; j <= n - (k - i) * x; j++) { // 左右留出足够空间给其他子数组
                f[i][j] = min(f[i][j - 1], f[i - 1][j - x] + dis[j - x]); // j-x 为子数组左端点
            }
        }
        return f[k][n];
    }
};
```

```go [sol-Go]
func minOperations(nums []int, x, k int) int64 {
	n := len(nums)
	dis := medianSlidingWindow(nums, x)
	f := make([][]int, k+1)
	for i := range f {
		f[i] = make([]int, n+1)
	}
	for i := 1; i <= k; i++ {
		f[i][i*x-1] = math.MaxInt
		for j := i * x; j <= n-(k-i)*x; j++ { // 左右留出足够空间给其他子数组
			f[i][j] = min(f[i][j-1], f[i-1][j-x]+dis[j-x]) // j-x 为子数组左端点
		}
	}
	return int64(f[k][n])
}

// 480. 滑动窗口中位数（有改动）
// 返回 nums 的所有长为 k 的子数组的（到子数组中位数的）距离和
func medianSlidingWindow(nums []int, k int) []int {
	ans := make([]int, len(nums)-k+1)
	left := newLazyHeap()  // 最大堆（元素取反）
	right := newLazyHeap() // 最小堆

	for i, in := range nums {
		// 1. 进入窗口
		if left.size == right.size {
			left.push(-right.pushPop(in))
		} else {
			right.push(-left.pushPop(-in))
		}

		l := i + 1 - k
		if l < 0 { // 窗口大小不足 k
			continue
		}

		// 2. 计算答案
		v := -left.top()
		s1 := v*left.size + left.sum // sum 取反
		s2 := right.sum - v*right.size
		ans[l] = s1 + s2

		// 3. 离开窗口
		out := nums[l]
		if out <= -left.top() {
			left.remove(-out)
			if left.size < right.size {
				left.push(-right.pop()) // 平衡两个堆的大小
			}
		} else {
			right.remove(out)
			if left.size > right.size+1 {
				right.push(-left.pop()) // 平衡两个堆的大小
			}
		}
	}

	return ans
}

func newLazyHeap() *lazyHeap {
	return &lazyHeap{removeCnt: map[int]int{}}
}

// 懒删除堆
type lazyHeap struct {
	sort.IntSlice
	removeCnt map[int]int // 每个元素剩余需要删除的次数
	size      int         // 实际大小
	sum       int         // 堆中元素总和
}

// 必须实现的两个接口
func (h *lazyHeap) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *lazyHeap) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }

// 删除
func (h *lazyHeap) remove(v int) {
	h.removeCnt[v]++ // 懒删除
	h.size--
	h.sum -= v
}

// 正式执行删除操作
func (h *lazyHeap) applyRemove() {
	for h.removeCnt[h.IntSlice[0]] > 0 {
		h.removeCnt[h.IntSlice[0]]--
		heap.Pop(h)
	}
}

// 查看堆顶
func (h *lazyHeap) top() int {
	h.applyRemove()
	return h.IntSlice[0]
}

// 出堆
func (h *lazyHeap) pop() int {
	h.applyRemove()
	h.size--
	h.sum -= h.IntSlice[0]
	return heap.Pop(h).(int)
}

// 入堆
func (h *lazyHeap) push(v int) {
	if h.removeCnt[v] > 0 {
		h.removeCnt[v]-- // 抵消之前的删除
	} else {
		heap.Push(h, v)
	}
	h.size++
	h.sum += v
}

// push(v) 然后 pop()
func (h *lazyHeap) pushPop(v int) int {
	if h.size > 0 && v > h.top() { // 最小堆，v 比堆顶大就替换堆顶
		h.sum += v - h.IntSlice[0]
		v, h.IntSlice[0] = h.IntSlice[0], v
		heap.Fix(h, 0)
	}
	return v
}
```

## 空间优化

```py [sol-Python3]
class Solution:
    # medianSlidingWindow 同上，略

    def minOperations(self, nums: List[int], x: int, k: int) -> int:
        n = len(nums)
        dis = self.medianSlidingWindow(nums, x)
        f = [0] * (n + 1)
        g = [0] * (n + 1)  # 滚动数组
        for i in range(1, k + 1):
            g[i * x - 1] = inf
            for j in range(i * x, n - (k - i) * x + 1):
                g[j] = min(g[j - 1], f[j - x] + dis[j - x])
            f, g = g, f
        return f[n]
```

```java [sol-Java]
class Solution {
    public long minOperations(int[] nums, int x, int k) {
        int n = nums.length;
        long[] dis = medianSlidingWindow(nums, x);
        long[] f = new long[n + 1];
        long[] g = new long[n + 1]; // 滚动数组
        for (int i = 1; i <= k; i++) {
            g[i * x - 1] = Long.MAX_VALUE;
            for (int j = i * x; j <= n - (k - i) * x; j++) {
                g[j] = Math.min(g[j - 1], f[j - x] + dis[j - x]);
            }
            long[] tmp = f;
            f = g;
            g = tmp;
        }
        return f[n];
    }

    // medianSlidingWindow 同上，略
}
```

```cpp [sol-C++]
class Solution {
    // medianSlidingWindow 同上，略
public:
    long long minOperations(vector<int>& nums, int x, int k) {
        int n = nums.size();
        vector<long long> dis = medianSlidingWindow(nums, x);
        vector<long long> f(n + 1), g(n + 1); // 滚动数组
        for (int i = 1; i <= k; i++) {
            g[i * x - 1] = LLONG_MAX;
            for (int j = i * x; j <= n - (k - i) * x; j++) {
                g[j] = min(g[j - 1], f[j - x] + dis[j - x]);
            }
            swap(f, g);
        }
        return f[n];
    }
};
```

```go [sol-Go]
func minOperations(nums []int, x, k int) int64 {
	n := len(nums)
	dis := medianSlidingWindow(nums, x)
	f := make([]int, n+1)
	g := make([]int, n+1) // 滚动数组
	for i := 1; i <= k; i++ {
		g[i*x-1] = math.MaxInt
		for j := i * x; j <= n-(k-i)*x; j++ {
			g[j] = min(g[j-1], f[j-x]+dis[j-x])
		}
		f, g = g, f
	}
	return int64(f[n])
}

// medianSlidingWindow 同上，略
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + (n-kx)k)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
