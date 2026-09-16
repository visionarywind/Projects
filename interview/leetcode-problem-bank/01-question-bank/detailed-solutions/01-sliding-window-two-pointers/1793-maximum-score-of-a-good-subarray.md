# 1793. 好子数组的最大分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-of-a-good-subarray/
- 题目 slug：`maximum-score-of-a-good-subarray`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.4 背向双指针
- 难度分：1946
- 外部题解来源：https://leetcode.cn/problems/maximum-score-of-a-good-subarray/solutions/2695415/liang-chong-fang-fa-dan-diao-zhan-shuang-24zl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：单调栈/双指针（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-score-of-a-good-subarray/solutions/2695415/liang-chong-fang-fa-dan-diao-zhan-shuang-24zl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-dan-diao-zhan-shuang-24zl`
- topic id：`2695415`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：单调栈

本题要计算的分数，和 [84. 柱状图中最大的矩形](https://leetcode.cn/problems/largest-rectangle-in-histogram/) 是一样的，计算的是最大矩形**面积**。只不过多了一个约束：矩形必须包含下标 $k$。

![lc84.jpg](https://pic.leetcode.cn/1710804602-sheXGS-lc84.jpg)

假设 $h=\textit{nums}[p]$ 是矩形的高度，那么矩形的宽度最大是多少？我们需要知道：

- 在 $p$ 左侧的**小于** $h$ 的最近元素的下标 $\textit{left}$。
- 在 $p$ 右侧的**小于** $h$ 的最近元素的下标 $\textit{right}$。

这可以用单调栈求出。原理请看视频：[单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)，欢迎点赞关注~

那么矩形的宽度就是 $\textit{right}-\textit{left}-1$，矩形面积为 $h\cdot (\textit{right}-\textit{left}-1)$。

对于本题，矩形必须包含下标 $k$，即 $\textit{left} < k < \textit{right}$ 成立时，才能用矩形面积更新答案的最大值。

```py [sol-Python3]
class Solution:
    def maximumScore(self, nums: List[int], k: int) -> int:
        n = len(nums)
        left = [-1] * n
        st = []
        for i, x in enumerate(nums):
            while st and x <= nums[st[-1]]:
                st.pop()
            if st:
                left[i] = st[-1]
            st.append(i)

        right = [n] * n
        st.clear()
        for i in range(n - 1, -1, -1):
            x = nums[i]
            while st and x <= nums[st[-1]]:
                st.pop()
            if st:
                right[i] = st[-1]
            st.append(i)

        ans = 0
        for h, l, r in zip(nums, left, right):
            if l < k < r:  # 相比 84 题多了这一行
                ans = max(ans, h * (r - l - 1))
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumScore(int[] nums, int k) {
        int n = nums.length;
        int[] left = new int[n];
        Deque<Integer> st = new ArrayDeque<>();
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            while (!st.isEmpty() && x <= nums[st.peek()]) {
                st.pop();
            }
            left[i] = st.isEmpty() ? -1 : st.peek();
            st.push(i);
        }

        int[] right = new int[n];
        st.clear();
        for (int i = n - 1; i >= 0; i--) {
            int x = nums[i];
            while (!st.isEmpty() && x <= nums[st.peek()]) {
                st.pop();
            }
            right[i] = st.isEmpty() ? n : st.peek();
            st.push(i);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            int h = nums[i];
            int l = left[i];
            int r = right[i];
            if (l < k && k < r) { // 相比 84 题多了个 if 判断
                ans = Math.max(ans, h * (r - l - 1));
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumScore(vector<int> &nums, int k) {
        int n = nums.size();
        vector<int> left(n, -1);
        stack<int> st;
        for (int i = 0; i < n; i++) {
            while (!st.empty() && nums[i] <= nums[st.top()]) {
                st.pop();
            }
            if (!st.empty()) {
                left[i] = st.top();
            }
            st.push(i);
        }

        vector<int> right(n, n);
        st = stack<int>();
        for (int i = n - 1; i >= 0; i--) {
            while (!st.empty() && nums[i] <= nums[st.top()]) {
                st.pop();
            }
            if (!st.empty()) {
                right[i] = st.top();
            }
            st.push(i);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            int h = nums[i], l = left[i], r = right[i];
            if (l < k && k < r) { // 相比 84 题多了个 if 判断
                ans = max(ans, h * (r - l - 1));
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumScore(nums []int, k int) (ans int) {
    n := len(nums)
    left := make([]int, n)
    st := []int{}
    for i, x := range nums {
        for len(st) > 0 && x <= nums[st[len(st)-1]] {
            st = st[:len(st)-1]
        }
        if len(st) > 0 {
            left[i] = st[len(st)-1]
        } else {
            left[i] = -1
        }
        st = append(st, i)
    }

    right := make([]int, n)
    st = st[:0]
    for i := n - 1; i >= 0; i-- {
        for len(st) > 0 && nums[i] <= nums[st[len(st)-1]] {
            st = st[:len(st)-1]
        }
        if len(st) > 0 {
            right[i] = st[len(st)-1]
        } else {
            right[i] = n
        }
        st = append(st, i)
    }

    for i, h := range nums {
        l, r := left[i], right[i]
        if l < k && k < r { // 相比 84 题多了个 if 判断
            ans = max(ans, h*(r-l-1))
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var maximumScore = function (nums, k) {
    const n = nums.length;
    const left = Array(n).fill(-1);
    const st = [];
    for (let i = 0; i < n; i++) {
        const x = nums[i];
        while (st.length && x <= nums[st[st.length - 1]]) {
            st.pop();
        }
        if (st.length) {
            left[i] = st[st.length - 1];
        }
        st.push(i);
    }

    const right = Array(n).fill(n);
    st.length = 0;
    for (let i = n - 1; i >= 0; i--) {
        const x = nums[i];
        while (st.length && x <= nums[st[st.length - 1]]) {
            st.pop();
        }
        if (st.length) {
            right[i] = st[st.length - 1];
        }
        st.push(i);
    }

    let ans = 0;
    for (let i = 0; i < n; i++) {
        const h = nums[i], l = left[i], r = right[i];
        if (l < k && k < r) { // 相比 84 题多了个 if 判断
            ans = Math.max(ans, h * (r - l - 1));
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_score(nums: Vec<i32>, k: i32) -> i32 {
        let n = nums.len();
        let mut left = vec![-1; n];
        let mut st = Vec::new();
        for (i, &x) in nums.iter().enumerate() {
            while !st.is_empty() && x <= nums[*st.last().unwrap()] {
                st.pop();
            }
            if let Some(&j) = st.last() {
                left[i] = j as i32;
            }
            st.push(i);
        }

        let mut right = vec![n as i32; n];
        st.clear();
        for (i, &x) in nums.iter().enumerate().rev() {
            while !st.is_empty() && x <= nums[*st.last().unwrap()] {
                st.pop();
            }
            if let Some(&j) = st.last() {
                right[i] = j as i32;
            }
            st.push(i);
        }

        let mut ans = 0;
        for ((&h, l), r) in nums.iter().zip(left).zip(right) {
            if l < k && k < r { // 相比 84 题多了个 if 判断
                ans = ans.max(h * (r - l - 1));
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：双指针

例如 $\textit{nums}=[1,9,7,8,8,1],\ k=3$。

其中面积最大的矩形，左边界下标 $L=1$，右边界下标 $R=4$。

我们尝试从 $i=k,\ j=k$ 出发，通过不断移动指针来找到最大矩形。

类似 [11. 盛最多水的容器](https://leetcode.cn/problems/container-with-most-water/)，比较 $x=\textit{nums}[i-1]$ 和 $y=\textit{nums}[j+1]$ 的大小，谁大就移动谁（一样大移动哪个都可以），让最小值减少得更慢。

**定理**：按照这种移动方式，一定会在某个时刻恰好满足 $i=L$ 且 $j=R$。

**证明**：如果 $i$ 先到达 $L$，那么此时 $j<R$。设 $L$ 到 $R$ 之间的最小元素为 $m$，在方法一中我们知道 $\textit{nums}[L-1]<m$，由于 $\textit{nums}[i-1]=\textit{nums}[L-1]<m\le\textit{nums}[j+1]$，那么后续一定是 $j$ 一直向右移动到 $R$。对于 $j$ 先到达 $R$ 的情况也同理。所以一定会在某个时刻恰好满足 $i=L$ 且 $j=R$。

在移动过程中，不断用 $\textit{nums}[i]$ 和 $\textit{nums}[j]$ 更新矩形高度的最小值 $\textit{minH}$，同时用 $\textit{minH}\cdot(j-i+1)$ 更新答案的最大值。

```py [sol-Python3]
class Solution:
    def maximumScore(self, nums: List[int], k: int) -> int:
        n = len(nums)
        ans = min_h = nums[k]
        i = j = k
        for _ in range(n - 1):
            if j == n - 1 or i and nums[i - 1] > nums[j + 1]:
                i -= 1
                min_h = min(min_h, nums[i])
            else:
                j += 1
                min_h = min(min_h, nums[j])
            ans = max(ans, min_h * (j - i + 1))
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumScore(int[] nums, int k) {
        int n = nums.length;
        int ans = nums[k], minH = nums[k];
        int i = k, j = k;
        for (int t = 0; t < n - 1; t++) { // 循环 n-1 次
            if (j == n - 1 || i > 0 && nums[i - 1] > nums[j + 1]) {
                minH = Math.min(minH, nums[--i]);
            } else {
                minH = Math.min(minH, nums[++j]);
            }
            ans = Math.max(ans, minH * (j - i + 1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumScore(vector<int> &nums, int k) {
        int n = nums.size();
        int ans = nums[k], min_h = nums[k];
        int i = k, j = k;
        for (int t = 0; t < n - 1; t++) { // 循环 n-1 次
            if (j == n - 1 || i && nums[i - 1] > nums[j + 1]) {
                min_h = min(min_h, nums[--i]);
            } else {
                min_h = min(min_h, nums[++j]);
            }
            ans = max(ans, min_h * (j - i + 1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumScore(nums []int, k int) int {
    n := len(nums)
    ans, minH := nums[k], nums[k]
    i, j := k, k
    for range n - 1 {
        if j == n-1 || i > 0 && nums[i-1] > nums[j+1] {
            i--
            minH = min(minH, nums[i])
        } else {
            j++
            minH = min(minH, nums[j])
        }
        ans = max(ans, minH*(j-i+1))
    }
    return ans
}
```

```js [sol-JavaScript]
var maximumScore = function(nums, k) {
    const n = nums.length;
    let ans = nums[k], minH = nums[k];
    let i = k, j = k;
    for (let t = 0; t < n - 1; t++) { // 循环 n-1 次
        if (j === n - 1 || i > 0 && nums[i - 1] > nums[j + 1]) {
            minH = Math.min(minH, nums[--i]);
        } else {
            minH = Math.min(minH, nums[++j]);
        }
        ans = Math.max(ans, minH * (j - i + 1));
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_score(nums: Vec<i32>, k: i32) -> i32 {
        let n = nums.len();
        let k = k as usize;
        let mut ans = nums[k];
        let mut min_h = nums[k];
        let mut i = k;
        let mut j = k;
        for _ in 0..n - 1 {
            if j == n - 1 || i > 0 && nums[i - 1] > nums[j + 1] {
                i -= 1;
                min_h = min_h.min(nums[i]);
            } else {
                j += 1;
                min_h = min_h.min(nums[j]);
            }
            ans = ans.max(min_h * (j - i + 1) as i32);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `三、单序列双指针 / §3.4 背向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.4 背向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
