# 1696. 跳跃游戏 VI

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/jump-game-vi/
- 题目 slug：`jump-game-vi`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.3 单调队列优化 DP
- 难度分：1954
- 外部题解来源：https://leetcode.cn/problems/jump-game-vi/solutions/2631981/yi-bu-bu-you-hua-cong-di-gui-dao-di-tui-84qn3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步优化：从递归到递推到单调队列（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/jump-game-vi/solutions/2631981/yi-bu-bu-you-hua-cong-di-gui-dao-di-tui-84qn3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-bu-bu-you-hua-cong-di-gui-dao-di-tui-84qn3`
- topic id：`2631981`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、启发思考：寻找子问题

看示例 2，$\textit{nums}=[10,-5,-2,4,0,3]$，$k=3$。

我们要解决的问题是：从下标 $0$ 跳到下标 $n-1=5$，经过的所有数字之和最大是多少？

思考「最后一步」发生了什么，有 $3$ 种选择：

- 从 $4$ 跳到 $5$，我们需要知道：从 $0$ 跳到 $4$，经过的所有数字之和最大是多少？
- 从 $3$ 跳到 $5$，我们需要知道：从 $0$ 跳到 $3$，经过的所有数字之和最大是多少？
- 从 $2$ 跳到 $5$，我们需要知道：从 $0$ 跳到 $2$，经过的所有数字之和最大是多少？

由于这 $3$ 种选择，都把原问题变为**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注 1：从右往左倒着思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

因为要解决的问题都形如「从 $0$ 跳到 $i$，经过的所有数字之和最大是多少」，所以定义 $\textit{dfs}(i)$ 表示从 $0$ 跳到 $i$，经过的所有数字之和的最大值。

如果从 $j$ 跳过来，那么有 $\textit{dfs}(i) = \textit{dfs}(j) + \textit{nums}[i]$，其中 $\max(i-k, 0) \le j \le i-1$。

枚举 $j$，取转移来源的最大值，得

$$
\textit{dfs}(i) = \max_{j = \max(i-k, 0)}^{i-1} \textit{dfs}(j) + \textit{nums}[i]
$$

**递归边界**：$\textit{dfs}(0)=\textit{nums}[0]$。

**递归入口**：$\textit{dfs}(n-1)$，也就是答案。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def maxResult(self, nums: List[int], k: int) -> int:
        def dfs(i: int) -> int:
            if i == 0:
                return nums[0]
            return max(dfs(j) for j in range(max(i - k, 0), i)) + nums[i]

        return dfs(len(nums) - 1)
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    public int maxResult(int[] nums, int k) {
        return dfs(nums.length - 1, nums, k);
    }

    private int dfs(int i, int[] nums, int k) {
        if (i == 0) {
            return nums[0];
        }
        int mx = Integer.MIN_VALUE;
        for (int j = Math.max(i - k, 0); j < i; j++) {
            mx = Math.max(mx, dfs(j, nums, k));
        }
        return mx + nums[i];
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int maxResult(vector<int> &nums, int k) {
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i == 0) {
                return nums[0];
            }
            int mx = INT_MIN;
            for (int j = max(i - k, 0); j < i; j++) {
                mx = max(mx, dfs(j));
            }
            return mx + nums[i];
        };

        return dfs(nums.size() - 1);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func maxResult(nums []int, k int) int {
    var dfs func(int) int
    dfs = func(i int) int {
        if i == 0 {
            return nums[0]
        }
        mx := math.MinInt
        for j := max(i-k, 0); j < i; j++ {
            mx = max(mx, dfs(j))
        }
        return mx + nums[i]
    }

    return dfs(len(nums) - 1)
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var maxResult = function(nums, k) {
    function dfs(i) {
        if (i === 0) {
            return nums[0];
        }
        let mx = -Infinity;
        for (let j = Math.max(i - k, 0); j < i; j++) {
            mx = Math.max(mx, dfs(j));
        }
        return mx + nums[i];
    }

    return dfs(nums.length - 1);
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn max_result(nums: Vec<i32>, k: i32) -> i32 {
        fn dfs(i: usize, nums: &[i32], k: usize) -> i32 {
            if i == 0 {
                return nums[0];
            }
            let mut mx = i32::MIN;
            for j in i.saturating_sub(k)..i {
                mx = mx.max(dfs(j, nums, k));
            }
            mx + nums[i]
        }

        dfs(nums.len() - 1, &nums, k as usize)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k^n)$，其中 $n$ 分别为 $\textit{nums}$ 的长度。搜索树可以近似为一棵 $k$ 叉树，树高为 $\mathcal{O}(n)$，所以节点个数为 $\mathcal{O}(k^n)$，遍历搜索树需要 $\mathcal{O}(k^n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是一样的，都表示从 $0$ 跳到 $i$，经过的所有数字之和的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \max_{j = \max(i-k, 0)}^{i-1} f[j] + \textit{nums}[i]
$$

> 相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

初始值 $f[0]=\textit{nums}[0]$，翻译自递归边界 $\textit{dfs}(0)=\textit{nums}[0]$。

答案为 $f[n-1]$，翻译自递归入口 $\textit{dfs}(n-1)$。

```py [sol-Python3]
# 超时，需要进一步优化
class Solution:
    def maxResult(self, nums: List[int], k: int) -> int:
        n = len(nums)
        f = [0] * n
        f[0] = nums[0]
        for i in range(1, n):
            f[i] = max(f[max(i - k, 0): i]) + nums[i]
        return f[-1]
```

```java [sol-Java]
// 超时，需要进一步优化
class Solution {
    public int maxResult(int[] nums, int k) {
        int n = nums.length;
        int[] f = new int[n];
        f[0] = nums[0];
        for (int i = 1; i < n; i++) {
            int mx = Integer.MIN_VALUE;
            for (int j = Math.max(i - k, 0); j < i; j++) {
                mx = Math.max(mx, f[j]);
            }
            f[i] = mx + nums[i];
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
// 超时，需要进一步优化
class Solution {
public:
    int maxResult(vector<int> &nums, int k) {
        int n = nums.size();
        vector<int> f(n);
        f[0] = nums[0];
        for (int i = 1; i < n; i++) {
            f[i] = *max_element(f.begin() + max(i - k, 0), f.begin() + i) + nums[i];
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
// 超时，需要进一步优化
func maxResult(nums []int, k int) int {
    n := len(nums)
    f := make([]int, n)
    f[0] = nums[0]
    for i := 1; i < n; i++ {
        f[i] = slices.Max(f[max(i-k, 0):i]) + nums[i]
    }
    return f[n-1]
}
```

```js [sol-JavaScript]
// 超时，需要进一步优化
var maxResult = function(nums, k) {
    const n = nums.length;
    const f = Array(n);
    f[0] = nums[0];
    for (let i = 1; i < n; i++) {
        f[i] = Math.max(...f.slice(Math.max(i - k, 0), i)) + nums[i];
    }
    return f[n - 1];
};
```

```rust [sol-Rust]
// 超时，需要进一步优化
impl Solution {
    pub fn max_result(nums: Vec<i32>, k: i32) -> i32 {
        let n = nums.len();
        let k = k as usize;
        let mut f = vec![0; n];
        f[0] = nums[0];
        for i in 1..n {
            f[i] = f[i.saturating_sub(k)..i].iter().max().unwrap() + nums[i];
        }
        f[n - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 分别为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 四、单调队列优化

当 $i<k$ 时，我们计算的是 $f[0],f[1],\cdots,f[i-1]$ 的最大值。

当 $i\ge k$ 时，我们计算的是 $f[i-k],f[i-k+1],\cdots,f[i-1]$ 的最大值。这是一个长度固定为 $k$ 的 [239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/) 问题，我在 [单调队列【基础算法精讲 27】](https://www.bilibili.com/video/BV1bM411X72E/)中讲了如何解决。

对于本题，我们维护一个 $f$ 值从左到右严格递减的单调队列（双端队列）。在计算 $f[i]$ 时，需要**保证队首就是转移来源最大值的下标**，方法如下：

1. **出**：如果队首小于 $i-k$，则弹出队首。注意单调队列只需保存下标。
2. **转移**：$f[i] = f[q[0]] + \textit{nums}[i]$。其中 $q[0]$ 表示单调队列 $q$ 的队首，此时队首就是转移来源最大值的下标。
3. **入**：不断弹出队尾，直到队列为空，或者 $f[i]$ 小于队尾对应的 $f$ 值为止。然后把 $i$ 加到队尾。

#### 答疑

**问**：我在做题时，上面这三步有时候是先 2 后 3，有时候是先 3 后 2，如何判断我该用哪种写法？

**答**：这取决于要计算的最值是否包含当前元素。如果当前元素在要计算的最值中，那么先入队，再获取最值，例如 239 题。本题 $f[i]$ 不在要计算的最值中，所以是先获取最值，再入队。

```py [sol-Python3]
class Solution:
    def maxResult(self, nums: List[int], k: int) -> int:
        n = len(nums)
        f = [0] * n
        f[0] = nums[0]
        q = deque([0])
        for i in range(1, n):
            # 1. 出
            if q[0] < i - k:
                q.popleft()
            # 2. 转移
            f[i] = f[q[0]] + nums[i]
            # 3. 入
            while q and f[i] >= f[q[-1]]:
                q.pop()
            q.append(i)
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int maxResult(int[] nums, int k) {
        int n = nums.length;
        int[] f = new int[n];
        f[0] = nums[0];
        Deque<Integer> q = new ArrayDeque<>();
        q.add(0);
        for (int i = 1; i < n; i++) {
            // 1. 出
            if (q.peekFirst() < i - k) {
                q.pollFirst();
            }
            // 2. 转移
            f[i] = f[q.peekFirst()] + nums[i];
            // 3. 入
            while (!q.isEmpty() && f[i] >= f[q.peekLast()]) {
                q.pollLast();
            }
            q.add(i);
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxResult(vector<int> &nums, int k) {
        int n = nums.size();
        vector<int> f(n);
        f[0] = nums[0];
        deque<int> q = {0};
        for (int i = 1; i < n; i++) {
            // 1. 出
            if (q.front() < i - k) {
                q.pop_front();
            }
            // 2. 转移
            f[i] = f[q.front()] + nums[i];
            // 3. 入
            while (!q.empty() && f[i] >= f[q.back()]) {
                q.pop_back();
            }
            q.push_back(i);
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func maxResult(nums []int, k int) int {
    n := len(nums)
    f := make([]int, n)
    f[0] = nums[0]
    q := []int{0}
    for i := 1; i < n; i++ {
        // 1. 出
        if q[0] < i-k {
            q = q[1:]
        }
        // 2. 转移
        f[i] = f[q[0]] + nums[i]
        // 3. 入
        for len(q) > 0 && f[i] >= f[q[len(q)-1]] {
            q = q[:len(q)-1]
        }
        q = append(q, i)
    }
    return f[n-1]
}
```

```js [sol-JavaScript]
var maxResult = function(nums, k) {
    const n = nums.length;
    const f = Array(n);
    f[0] = nums[0];
    const q = Array(n);
    let head = 0, tail = 0;
    q[tail++] = 0;
    for (let i = 1; i < n; i++) {
        // 1. 出
        if (q[head] < i - k) {
            head++;
        }
        // 2. 转移
        f[i] = f[q[head]] + nums[i];
        // 3. 入
        while (head < tail && f[i] >= f[q[tail - 1]]) {
            tail--;
        }
        q[tail++] = i;
    }
    return f[n - 1];
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn max_result(nums: Vec<i32>, k: i32) -> i32 {
        let n = nums.len();
        let k = k as usize;
        let mut f = vec![0; n];
        f[0] = nums[0];
        let mut q = VecDeque::new();
        q.push_back(0);
        for i in 1..n {
            // 1. 出
            if *q.front().unwrap() + k < i {
                q.pop_front();
            }
            // 2. 转移
            f[i] = f[*q.front().unwrap()] + nums[i];
            // 3. 入
            while !q.is_empty() && f[i] >= f[*q.back().unwrap()] {
                q.pop_back();
            }
            q.push_back(i);
        }
        f[n - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 分别为 $\textit{nums}$ 的长度。虽然我们写了个二重循环，但由于每个下标入队出队各至多一次，所以二重循环的循环次数是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

把 $f[i]$ 直接记在 $\textit{nums}$ 中，这样就只需要 $\mathcal{O}(k)$ 的额外空间。

> 注：如果不想修改 $\textit{nums}[i]$，也可以把数对 $(i,f[i])$ 保存到单调队列中。

```py [sol-Python3]
class Solution:
    def maxResult(self, nums: List[int], k: int) -> int:
        q = deque([0])
        for i in range(1, len(nums)):
            # 1. 出
            if q[0] < i - k:
                q.popleft()
            # 2. 转移
            nums[i] += nums[q[0]]
            # 3. 入
            while q and nums[i] >= nums[q[-1]]:
                q.pop()
            q.append(i)
        return nums[-1]
```

```java [sol-Java]
class Solution {
    public int maxResult(int[] nums, int k) {
        int n = nums.length;
        Deque<Integer> q = new ArrayDeque<>(); // 更快的写法见【Java 数组】
        q.add(0);
        for (int i = 1; i < n; i++) {
            // 1. 出
            if (q.peekFirst() < i - k) {
                q.pollFirst();
            }
            // 2. 转移
            nums[i] += nums[q.peekFirst()];
            // 3. 入
            while (!q.isEmpty() && nums[i] >= nums[q.peekLast()]) {
                q.pollLast();
            }
            q.add(i);
        }
        return nums[n - 1];
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int maxResult(int[] nums, int k) {
        int n = nums.length;
        int[] q = new int[n];
        int head = 0;
        int tail = 0;
        q[tail++] = 0;
        for (int i = 1; i < n; i++) {
            // 1. 出
            if (q[head] < i - k) {
                head++;
            }
            // 2. 转移
            nums[i] += nums[q[head]];
            // 3. 入
            while (head < tail && nums[i] >= nums[q[tail - 1]]) {
                tail--;
            }
            q[tail++] = i;
        }
        return nums[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxResult(vector<int> &nums, int k) {
        deque<int> q = {0};
        for (int i = 1; i < nums.size(); i++) {
            // 1. 出
            if (q.front() < i - k) {
                q.pop_front();
            }
            // 2. 转移
            nums[i] += nums[q.front()];
            // 3. 入
            while (!q.empty() && nums[i] >= nums[q.back()]) {
                q.pop_back();
            }
            q.push_back(i);
        }
        return nums.back();
    }
};
```

```go [sol-Go]
func maxResult(nums []int, k int) int {
    n := len(nums)
    q := []int{0}
    for i := 1; i < n; i++ {
        // 1. 出
        if q[0] < i-k {
            q = q[1:]
        }
        // 2. 转移
        nums[i] += nums[q[0]]
        // 3. 入
        for len(q) > 0 && nums[i] >= nums[q[len(q)-1]] {
            q = q[:len(q)-1]
        }
        q = append(q, i)
    }
    return nums[n-1]
}
```

```js [sol-JavaScript]
var maxResult = function(nums, k) {
    const n = nums.length;
    const q = Array(n); // 为方便起见，仍然用一个大小为 n 的数组（空间 O(k) 需要实现环形队列）
    let head = 0, tail = 0;
    q[tail++] = 0;
    for (let i = 1; i < n; i++) {
        // 1. 出
        if (q[head] < i - k) {
            head++;
        }
        // 2. 转移
        nums[i] += nums[q[head]];
        // 3. 入
        while (head < tail && nums[i] >= nums[q[tail - 1]]) {
            tail--;
        }
        q[tail++] = i;
    }
    return nums[n - 1];
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn max_result(mut nums: Vec<i32>, k: i32) -> i32 {
        let n = nums.len();
        let k = k as usize;
        let mut q = VecDeque::new();
        q.push_back(0);
        for i in 1..n {
            // 1. 出
            if *q.front().unwrap() + k < i {
                q.pop_front();
            }
            // 2. 转移
            nums[i] += nums[*q.front().unwrap()];
            // 3. 入
            while !q.is_empty() && nums[i] >= nums[*q.back().unwrap()] {
                q.pop_back();
            }
            q.push_back(i);
        }
        nums[n - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 分别为 $\textit{nums}$ 的长度。虽然我们写了个二重循环，但由于每个下标入队出队各至多一次，所以二重循环的循环次数是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(k)$。单调队列需要 $\mathcal{O}(k)$ 的空间。

## 思考题

把 $k$ 改成一个数组 $\textit{jumpLimits}$，其中 $\textit{jumpLimits}[i]$ 表示从下标 $i$ 处向右跳，至多可以跳 $\textit{jumpLimits}[i]$ 步。

如此修改后，应该怎么做呢？

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面动态规划题单的「**§11.3 单调队列优化 DP**」。

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

本题来自 `十一、优化 DP / §11.3 单调队列优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.3 单调队列优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
