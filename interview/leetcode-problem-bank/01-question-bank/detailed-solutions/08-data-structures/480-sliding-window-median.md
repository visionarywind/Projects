# 480. 滑动窗口中位数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sliding-window-median/
- 题目 slug：`sliding-window-median`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sliding-window-median/solutions/3628827/295-ti-lan-shan-chu-dui-pythonjavacgojsr-66ch/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[295 题 + 懒删除堆（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/sliding-window-median/solutions/3628827/295-ti-lan-shan-chu-dui-pythonjavacgojsr-66ch/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`295-ti-lan-shan-chu-dui-pythonjavacgojsr-66ch`
- topic id：`3628827`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

1. 定长滑动窗口，原理见[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)
2. 不删除元素时的动态中位数，见 [295. 数据流的中位数](https://leetcode.cn/problems/find-median-from-data-stream/)，**请先完成这题**。
3. 懒删除堆。本文会介绍。

## 懒删除堆

本题当窗口左端点元素离开窗口时，需要从堆中删除该元素。

如果直接删除堆中元素，需要遍历整个堆，太慢了。怎么办？

删除的时候，我们只记录「要删除一个值为 $x$ 的数」，并不去「执行」删除操作。等到要出堆（或者查看堆顶）时才真正地执行删除操作。

具体来说：

- 用一个哈希表 $\textit{removeCnt}$ 记录每个元素剩余需要删除的次数。
- 删除 $\texttt{remove}(x)$：把 $\textit{removeCnt}[x]$ 加一。
- 出堆 $\texttt{pop}()$：如果 $\textit{removeCnt}[堆顶]>0$，则弹出堆顶，重复，直到 $\textit{removeCnt}[堆顶]=0$。最后弹出堆顶元素。
- 入堆的逻辑不变。

由于删除元素时只修改了 $\textit{removeCnt}$，并没有操作堆，所以堆的大小并不是其真实大小。但我们需要根据堆的大小来平衡两个堆，从而计算中位数。怎么办？

额外用一个变量 $\textit{size}$ 表示堆的大小：

- 删除和出堆：把 $\textit{size}$ 减一。
- 入堆：把 $\textit{size}$ 加一。

## 思路

整体是一个定长滑动窗口的框架。

1. 元素进入窗口：同 295 题，可以看 [我的题解](https://leetcode.cn/problems/find-median-from-data-stream/solutions/3015873/ru-he-zi-ran-yin-ru-da-xiao-dui-jian-ji-4v22k/)。
2. 计算中位数：同 295 题。
3. 元素离开窗口：设离开窗口的元素为 $x$，设 $\textit{left}$ 的堆顶元素值为 $t$。
    - 如果 $x \le t$，说明 $x$ 在 $\textit{left}$ 中，从 $\textit{left}$ 中删除 $x$。如果删除后 $\textit{left}.\textit{size} < \textit{right}.\textit{size}$，则弹出 $\textit{right}$ 的堆顶，添加到 $\textit{left}$ 中。
    - 如果 $x > t$，说明 $x$ 在 $\textit{right}$ 中，从 $\textit{right}$ 中删除 $x$。如果删除后 $\textit{left}.\textit{size} > \textit{right}.\textit{size} + 1$，则弹出 $\textit{left}$ 的堆顶，添加到 $\textit{right}$ 中。

```py [sol-Python3]
class LazyHeap:
    def __init__(self):
        self.heap = []
        self.remove_cnt = defaultdict(int)  # 每个元素剩余需要删除的次数
        self.size = 0  # 实际大小

    # 删除
    def remove(self, x: int) -> None:
        self.remove_cnt[x] += 1  # 懒删除
        self.size -= 1

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
        return heappop(self.heap)

    # 入堆
    def push(self, x: int) -> None:
        heappush(self.heap, x)
        self.size += 1

    # push(x) 然后 pop()
    def pushpop(self, x: int) -> int:
        self.apply_remove()
        return heappushpop(self.heap, x)

class Solution:
    def medianSlidingWindow(self, nums: list[int], k: int) -> list[float]:
        ans = [0] * (len(nums) - k + 1)
        left = LazyHeap()   # 最大堆（元素取反）
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
            if k % 2:
                ans[l] = -left.top()
            else:
                ans[l] = (right.top() - left.top()) / 2

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
```

```java [sol-Java]
class LazyHeap extends PriorityQueue<Integer> {
    private final Map<Integer, Integer> removeCnt = new HashMap<>(); // 每个元素剩余需要删除的次数
    private int size = 0; // 实际大小

    public LazyHeap(Comparator<Integer> comparator) {
        super(comparator);
    }

    public int size() {
        return size;
    }

    // 删除
    public void remove(int x) {
        removeCnt.merge(x, 1, Integer::sum); // 懒删除
        size--;
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
        return poll();
    }

    // 入堆
    public void push(int x) {
        offer(x);
        size++;
    }

    // push(x) 然后 pop()
    public int pushPop(int x) {
        if (size > 0 && comparator().compare(x, top()) > 0) { // 可以替换堆顶
            offer(x);
            return poll();
        }
        return x;
    }
}

class Solution {
    public double[] medianSlidingWindow(int[] nums, int k) {
        int n = nums.length;
        double[] ans = new double[n - k + 1];
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
            if (k % 2 > 0) {
                ans[l] = left.top();
            } else {
                ans[l] = ((long) left.top() + right.top()) / 2.0;
            }

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

    // 删除
    void remove(T x) {
        remove_cnt[x]++; // 懒删除
        sz--;
    }

    // 查看堆顶
    T top() {
        apply_remove();
        return pq.top();
    }

    // 出堆
    T pop() {
        apply_remove();
        sz--;
        T x = pq.top();
        pq.pop();
        return x;
    }

    // 入堆
    void push(T x) {
        pq.push(x);
        sz++;
    }

    // push(x) 然后 pop()
    T push_pop(T x) {
        if (sz > 0 && Compare()(x, top())) { // 可以替换堆顶
            pq.push(x);
            x = pq.top();
            pq.pop();
        }
        return x;
    }
};

class Solution {
public:
    vector<double> medianSlidingWindow(vector<int>& nums, int k) {
        int n = nums.size();
        vector<double> ans(n - k + 1);
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
            if (k % 2) {
                ans[l] = left.top();
            } else {
                ans[l] = ((long long) left.top() + right.top()) / 2.0;
            }

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
};
```

```go [sol-Go]
func medianSlidingWindow(nums []int, k int) []float64 {
	ans := make([]float64, len(nums)-k+1)
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
		if k%2 > 0 {
			ans[l] = float64(-left.top())
		} else {
			ans[l] = float64(right.top()-left.top()) / 2
		}

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
}

// 必须实现的两个接口
func (h *lazyHeap) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *lazyHeap) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }

// 删除
func (h *lazyHeap) remove(v int) {
	h.removeCnt[v]++ // 懒删除
	h.size--
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
	return heap.Pop(h).(int)
}

// 入堆
func (h *lazyHeap) push(v int) {
    heap.Push(h, v)
	h.size++
}

// push(v) 然后 pop()
func (h *lazyHeap) pushPop(v int) int {
	if h.size > 0 && v > h.top() { // 最小堆，v 比堆顶大就替换堆顶
		v, h.IntSlice[0] = h.IntSlice[0], v
		heap.Fix(h, 0)
	}
	return v
}
```

```js [sol-JavaScript]
class LazyHeap {
    constructor(isMin) {
        this.pq = isMin ? new MinPriorityQueue() : new MaxPriorityQueue();
        this.removeCnt = new Map(); // 每个元素剩余需要删除的次数
        this.size = 0; // 实际大小
    }

    // 正式执行删除操作
    #applyRemove() {
        while (!this.pq.isEmpty()) {
            const x = this.pq.front();
            const cnt = this.removeCnt.get(x) ?? 0;
            if (cnt === 0) {
                break;
            }
            this.removeCnt.set(x, cnt - 1);
            this.pq.dequeue();
        }
    }

    // 懒删除：标记元素需要删除
    remove(x) {
        this.removeCnt.set(x, (this.removeCnt.get(x) ?? 0) + 1);
        this.size--;
    }

    // 查看堆顶
    top() {
        this.#applyRemove();
        return this.pq.front();
    }

    // 出堆
    pop() {
        this.#applyRemove();
        this.size--;
        return this.pq.dequeue();
    }

    // 入堆
    push(x) {
        this.pq.enqueue(x);
        this.size++;
    }

    // push(x) 然后 pop()
    pushPop(x) {
        this.#applyRemove();
        this.pq.enqueue(x);
        return this.pq.dequeue();
    }
}

var medianSlidingWindow = function(nums, k) {
    const n = nums.length;
    const ans = Array(n - k + 1);
    const left = new LazyHeap(false); // 最大堆
    const right = new LazyHeap(true); // 最小堆

    for (let i = 0; i < n; i++) {
        const in_ = nums[i];
        // 1. 新元素进入窗口
        if (left.size === right.size) {
            left.push(right.pushPop(in_));
        } else {
            right.push(left.pushPop(in_));
        }

        const l = i + 1 - k;
        if (l < 0) { // 窗口大小不足 k
            continue;
        }

        // 2. 计算答案
        if (k % 2 > 0) {
            ans[l] = left.top();
        } else {
            ans[l] = (left.top() + right.top()) / 2;
        }

        // 3. 离开窗口
        const out = nums[l];
        if (out <= left.top()) {
            left.remove(out);
            if (left.size < right.size) {
                left.push(right.pop()); // 平衡两个堆的大小
            }
        } else {
            right.remove(out);
            if (left.size > right.size + 1) {
                right.push(left.pop()); // 平衡两个堆的大小
            }
        }
    }

    return ans;
};
```

```rust [sol-Rust]
use std::collections::{BinaryHeap, HashMap};

struct LazyHeap {
    heap: BinaryHeap<i64>,
    remove_cnt: HashMap<i64, i32>, // 记录需要删除的元素次数
    size: usize, // 实际大小
}

impl LazyHeap {
    fn new() -> Self {
        Self {
            heap: BinaryHeap::new(),
            remove_cnt: HashMap::new(),
            size: 0,
        }
    }

    fn len(&self) -> usize {
        self.size
    }

    // 删除
    fn remove(&mut self, x: i64) {
        *self.remove_cnt.entry(x).or_insert(0) += 1;
        self.size -= 1;
    }

    // 正式执行删除操作
    fn apply_remove(&mut self) {
        while let Some(&x) = self.heap.peek() {
            if let Some(cnt) = self.remove_cnt.get_mut(&x) {
                if *cnt == 0 {
                    break;
                }
                *cnt -= 1;
                self.heap.pop();
            } else {
                break;
            }
        }
    }

    // 查看堆顶
    fn top(&mut self) -> i64 {
        self.apply_remove();
        *self.heap.peek().unwrap()
    }

    // 出堆
    fn pop(&mut self) -> i64 {
        self.apply_remove();
        self.size -= 1;
        self.heap.pop().unwrap()
    }

    // 入堆
    fn push(&mut self, x: i64) {
        self.heap.push(x);
        self.size += 1;
    }

    // push(x) 然后 pop()
    fn push_pop(&mut self, x: i64) -> i64 {
        if self.size > 0 && x < self.top() { // 最大堆，x 比堆顶大就替换堆顶
            self.heap.push(x);
            return self.heap.pop().unwrap();
        }
        x
    }
}

impl Solution {
    pub fn median_sliding_window(nums: Vec<i32>, k: i32) -> Vec<f64> {
        let k = k as usize;
        let mut ans = vec![0.; nums.len() - k + 1];
        let mut left = LazyHeap::new();  // 最大堆
        let mut right = LazyHeap::new(); // 最小堆（元素取反）

        for (i, &x) in nums.iter().enumerate() {
            let x = x as i64;
            // 1. 进入窗口
            if left.len() == right.len() {
                left.push(-right.push_pop(-x));
            } else {
                right.push(-left.push_pop(x));
            }

            if i + 1 < k {
                continue;
            }
            let l = i + 1 - k;

            // 2. 计算答案
            ans[l] = if k % 2 > 0 {
                left.top() as f64
            } else {
                (left.top() - right.top()) as f64 / 2.
            };

            // 3. 离开窗口
            let x = nums[l] as i64;
            if x <= left.top() {
                left.remove(x);
                if left.len() < right.len() {
                    left.push(-right.pop());
                }
            } else {
                right.remove(-x);
                if left.len() > right.len() + 1 {
                    right.push(-left.pop());
                }
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。注意堆中有 $\mathcal{O}(n)$ 个元素（包含需要被删除，但没有执行删除的元素），而不是 $\mathcal{O}(k)$ 个元素。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面数据结构题单中的「**§5.7 对顶堆**」。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
