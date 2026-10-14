# 42. 接雨水

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/trapping-rain-water/
- 题目 slug：`trapping-rain-water`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/trapping-rain-water/solutions/1974340/zuo-liao-nbian-huan-bu-hui-yi-ge-shi-pin-ukwm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】三种方法：前后缀分解/双指针/单调栈（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/trapping-rain-water/solutions/1974340/zuo-liao-nbian-huan-bu-hui-yi-ge-shi-pin-ukwm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zuo-liao-nbian-huan-bu-hui-yi-ge-shi-pin-ukwm`
- topic id：`1974340`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 02】](https://www.bilibili.com/video/BV1Qg411q7ia/)，欢迎点赞关注~

## 方法一：前后缀分解

**注**：计算 $\textit{preMax}$ 的循环和计算 $\textit{ans}$ 的循环，可以合并成一个循环。这里为了方便大家阅读，没有合并。

```py [sol-Python3]
class Solution:
    def trap(self, height: List[int]) -> int:
        n = len(height)
        pre_max = [0] * n  # pre_max[i] 表示从 height[0] 到 height[i] 的最大值
        pre_max[0] = height[0]
        for i in range(1, n):
            pre_max[i] = max(pre_max[i - 1], height[i])

        suf_max = [0] * n  # suf_max[i] 表示从 height[i] 到 height[n-1] 的最大值
        suf_max[-1] = height[-1]
        for i in range(n - 2, -1, -1):
            suf_max[i] = max(suf_max[i + 1], height[i])

        ans = 0
        for h, pre, suf in zip(height, pre_max, suf_max):
            ans += min(pre, suf) - h  # 累加每个水桶能接多少水
        return ans
```

```java [sol-Java]
class Solution {
    public int trap(int[] height) {
        int n = height.length;
        int[] preMax = new int[n]; // preMax[i] 表示从 height[0] 到 height[i] 的最大值
        preMax[0] = height[0];
        for (int i = 1; i < n; i++) {
            preMax[i] = Math.max(preMax[i - 1], height[i]);
        }

        int[] sufMax = new int[n]; // sufMax[i] 表示从 height[i] 到 height[n-1] 的最大值
        sufMax[n - 1] = height[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            sufMax[i] = Math.max(sufMax[i + 1], height[i]);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans += Math.min(preMax[i], sufMax[i]) - height[i]; // 累加每个水桶能接多少水
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int trap(vector<int>& height) {
        int n = height.size();
        vector<int> pre_max(n); // pre_max[i] 表示从 height[0] 到 height[i] 的最大值
        pre_max[0] = height[0];
        for (int i = 1; i < n; i++) {
            pre_max[i] = max(pre_max[i - 1], height[i]);
        }

        vector<int> suf_max(n); // suf_max[i] 表示从 height[i] 到 height[n-1] 的最大值
        suf_max[n - 1] = height[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            suf_max[i] = max(suf_max[i + 1], height[i]);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans += min(pre_max[i], suf_max[i]) - height[i]; // 累加每个水桶能接多少水
        }
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int trap(int* height, int heightSize) {
    int* pre_max = malloc(sizeof(int) * heightSize); // pre_max[i] 表示从 height[0] 到 height[i] 的最大值
    pre_max[0] = height[0];
    for (int i = 1; i < heightSize; i++) {
        pre_max[i] = MAX(pre_max[i - 1], height[i]);
    }

    int* suf_max = malloc(sizeof(int) * heightSize); // suf_max[i] 表示从 height[i] 到 height[heightSize-1] 的最大值
    suf_max[heightSize - 1] = height[heightSize - 1];
    for (int i = heightSize - 2; i >= 0; i--) {
        suf_max[i] = MAX(suf_max[i + 1], height[i]);
    }

    int ans = 0;
    for (int i = 0; i < heightSize; i++) {
        ans += MIN(pre_max[i], suf_max[i]) - height[i]; // 累加每个水桶能接多少水
    }

    free(pre_max);
    free(suf_max);
    return ans;
}
```

```go [sol-Go]
func trap(height []int) (ans int) {
    n := len(height)
    preMax := make([]int, n) // preMax[i] 表示从 height[0] 到 height[i] 的最大值
    preMax[0] = height[0]
    for i := 1; i < n; i++ {
        preMax[i] = max(preMax[i-1], height[i])
    }

    sufMax := make([]int, n) // sufMax[i] 表示从 height[i] 到 height[n-1] 的最大值
    sufMax[n-1] = height[n-1]
    for i := n - 2; i >= 0; i-- {
        sufMax[i] = max(sufMax[i+1], height[i])
    }

    for i, h := range height {
        ans += min(preMax[i], sufMax[i]) - h // 累加每个水桶能接多少水
    }
    return
}
```

```js [sol-JavaScript]
var trap = function(height) {
    const n = height.length;
    const preMax = Array(n); // preMax[i] 表示从 height[0] 到 height[i] 的最大值
    preMax[0] = height[0];
    for (let i = 1; i < n; i++) {
        preMax[i] = Math.max(preMax[i - 1], height[i]);
    }

    const sufMax = Array(n); // sufMax[i] 表示从 height[i] 到 height[n-1] 的最大值
    sufMax[n - 1] = height[n - 1];
    for (let i = n - 2; i >= 0; i--) {
        sufMax[i] = Math.max(sufMax[i + 1], height[i]);
    }

    let ans = 0;
    for (let i = 0; i < n; i++) {
        ans += Math.min(preMax[i], sufMax[i]) - height[i]; // 累加每个水桶能接多少水
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn trap(height: Vec<i32>) -> i32 {
        let n = height.len();
        let mut pre_max = vec![0; n]; // pre_max[i] 表示从 height[0] 到 height[i] 的最大值
        pre_max[0] = height[0];
        for i in 1..n {
            pre_max[i] = pre_max[i - 1].max(height[i]);
        }

        let mut suf_max = vec![0; n]; // suf_max[i] 表示从 height[i] 到 height[n-1] 的最大值
        suf_max[n - 1] = height[n - 1];
        for i in (0..n - 1).rev() {
            suf_max[i] = suf_max[i + 1].max(height[i]);
        }

        let mut ans = 0;
        for i in 0..n {
            ans += pre_max[i].min(suf_max[i]) - height[i]; // 累加每个水桶能接多少水
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{height}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：相向双指针（一次遍历）

设现在算出了前缀 $[0, \textit{left}]$ 的最大高度 $\textit{preMax}[\textit{left}]$，以及后缀 $[\textit{right},n-1]$ 的最大高度 $\textit{sufMax}[\textit{right}]$。中间的柱子 $[\textit{left}+1,\textit{right}-1]$ 尚未遍历，不知道有多高。在这种情况下，我们能否直接确定 $\textit{left}$ 或者 $\textit{right}$ 处的接水量？

分类讨论：

- 如果 $\textit{preMax}[\textit{left}] \le \textit{sufMax}[\textit{right}]$，由于 $\textit{sufMax}[\textit{right}]\le \textit{sufMax}[\textit{left}]$（包含的数越多，最大值越大），所以 $\textit{preMax}[\textit{left}] \le \textit{sufMax}[\textit{right}]\le \textit{sufMax}[\textit{left}]$，所以 $\min(\textit{preMax}[\textit{left}],\textit{sufMax}[\textit{left}]) = \textit{preMax}[\textit{left}]$，$\textit{left}$ 处的接水量就是 $\textit{preMax}[\textit{left}] - \textit{height}[\textit{left}]$。
- 如果 $\textit{preMax}[\textit{left}] \ge \textit{sufMax}[\textit{right}]$，由于 $\textit{preMax}[\textit{left}]\le \textit{preMax}[\textit{right}]$（包含的数越多，最大值越大），所以 $\textit{sufMax}[\textit{right}] \le \textit{preMax}[\textit{left}]\le \textit{preMax}[\textit{right}]$，所以 $\min(\textit{preMax}[\textit{right}],\textit{sufMax}[\textit{right}]) = \textit{sufMax}[\textit{right}]$，$\textit{right}$ 处的接水量就是 $\textit{sufMax}[\textit{right}] - \textit{height}[\textit{right}]$。

这意味着，**在没有遍历完 $\textit{height}$ 数组的情况下，也能算出接水量**。这引出了如下相向双指针（一次遍历）做法。

> **注**：代码实现时，$\texttt{while}$ 循环可以不加等号。因为在「谁小移动谁」的规则下，相遇的位置一定是最高的柱子，这个柱子是无法接水的。

```py [sol-Python3]
class Solution:
    def trap(self, height: List[int]) -> int:
        ans = pre_max = suf_max = 0
        left, right = 0, len(height) - 1
        while left < right:
            pre_max = max(pre_max, height[left])  # 前缀最大值
            suf_max = max(suf_max, height[right])  # 后缀最大值
            if pre_max < suf_max:  # 可以确定 left 处的接水量
                ans += pre_max - height[left]
                left += 1  # 搞定了 left，现在问题缩小到 [left+1, right]
            else:  # 可以确定 right 处的接水量
                ans += suf_max - height[right]
                right -= 1  # 搞定了 right，现在问题缩小到 [left, right-1]
        return ans
```

```java [sol-Java]
class Solution {
    public int trap(int[] height) {
        int ans = 0;
        int preMax = 0; // 前缀最大值，随着左指针 left 的移动而更新
        int sufMax = 0; // 后缀最大值，随着右指针 right 的移动而更新
        int left = 0;
        int right = height.length - 1;

        while (left < right) {
            preMax = Math.max(preMax, height[left]);
            sufMax = Math.max(sufMax, height[right]);
            if (preMax < sufMax) { // 可以确定 left 处的接水量
                ans += preMax - height[left];
                left++; // 搞定了 left，现在问题缩小到 [left+1, right]
            } else { // 可以确定 right 处的接水量
                ans += sufMax - height[right];
                right--; // 搞定了 right，现在问题缩小到 [left, right-1]
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int trap(vector<int>& height) {
        int ans = 0;
        int pre_max = 0, suf_max = 0;
        int left = 0, right = height.size() - 1;
        while (left < right) {
            pre_max = max(pre_max, height[left]); // 前缀最大值
            suf_max = max(suf_max, height[right]); // 后缀最大值
            if (pre_max < suf_max) { // 可以确定 left 处的接水量
                ans += pre_max - height[left];
                left++; // 搞定了 left，现在问题缩小到 [left+1, right]
            } else { // 可以确定 right 处的接水量
                ans += suf_max - height[right];
                right--; // 搞定了 right，现在问题缩小到 [left, right-1]
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int trap(int* height, int heightSize) {
    int ans = 0;
    int pre_max = 0, suf_max = 0;
    int left = 0, right = heightSize - 1;
    while (left < right) {
        pre_max = MAX(pre_max, height[left]); // 前缀最大值
        suf_max = MAX(suf_max, height[right]); // 后缀最大值
        if (pre_max < suf_max) { // 可以确定 left 处的接水量
            ans += pre_max - height[left];
            left++; // 搞定了 left，现在问题缩小到 [left+1, right]
        } else { // 可以确定 right 处的接水量
            ans += suf_max - height[right];
            right--; // 搞定了 right，现在问题缩小到 [left, right-1]
        }
    }
    return ans;
}
```

```go [sol-Go]
func trap(height []int) (ans int) {
    preMax, sufMax := 0, 0
    left, right := 0, len(height)-1
    for left < right {
        preMax = max(preMax, height[left]) // 前缀最大值
        sufMax = max(sufMax, height[right]) // 后缀最大值
        if preMax < sufMax { // 可以确定 left 处的接水量
            ans += preMax - height[left]
            left++ // 搞定了 left，现在问题缩小到 [left+1, right]
        } else { // 可以确定 right 处的接水量
            ans += sufMax - height[right]
            right-- // 搞定了 right，现在问题缩小到 [left, right-1]
        }
    }
    return
}
```

```js [sol-JavaScript]
var trap = function(height) {
    let ans = 0;
    let preMax = 0, sufMax = 0;
    let left = 0, right = height.length - 1;
    while (left < right) {
        preMax = Math.max(preMax, height[left]); // 前缀最大值
        sufMax = Math.max(sufMax, height[right]); // 后缀最大值
        if (preMax < sufMax) { // 可以确定 left 处的接水量
            ans += preMax - height[left];
            left++; // 搞定了 left，现在问题缩小到 [left+1, right]
        } else { // 可以确定 right 处的接水量
            ans += sufMax - height[right];
            right--; // 搞定了 right，现在问题缩小到 [left, right-1]
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn trap(height: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut pre_max = 0; // 前缀最大值，随着左指针 left 的移动而更新
        let mut suf_max = 0; // 后缀最大值，随着右指针 right 的移动而更新
        let mut left = 0;
        let mut right = height.len() - 1;

        while left < right {
            pre_max = pre_max.max(height[left]);
            suf_max = suf_max.max(height[right]);
            if pre_max < suf_max { // 可以确定 left 处的接水量
                ans += pre_max - height[left];
                left += 1; // 搞定了 left，现在问题缩小到 [left+1, right]
            } else { // 可以确定 right 处的接水量
                ans += suf_max - height[right];
                right -= 1; // 搞定了 right，现在问题缩小到 [left, right-1]
            };
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{height}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法三：单调栈

请看 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)。

上面的方法相当于「竖着」计算面积，单调栈的做法相当于「横着」计算面积。

这个方法可以总结成 $16$ 个字：**找上一个更大元素，在找的过程中填坑。**

注意 $\texttt{while}$ 中加了等号，这可以让栈中没有重复元素，从而在有很多重复元素的情况下，使用更少的空间。

> **点评**：看复杂度的话，单调栈不如双指针的做法。但如果输入的 $\textit{height}$ 是一个**流**（stream），只能从左到右遍历，那么单调栈（在这种场景下）就是不错的方法了。

```py [sol-Python3]
class Solution:
    def trap(self, height: List[int]) -> int:
        ans = 0
        st = []
        for i, h in enumerate(height):
            while st and height[st[-1]] <= h:
                bottom_h = height[st.pop()]
                if not st:  # 栈是空的
                    break
                left = st[-1]
                dh = min(height[left], h) - bottom_h  # 面积的高
                ans += dh * (i - left - 1)
            st.append(i)
        return ans
```

```java [sol-Java]
class Solution {
    public int trap(int[] height) {
        int ans = 0;
        Deque<Integer> st = new ArrayDeque<>();
        for (int i = 0; i < height.length; i++) {
            int h = height[i];
            while (!st.isEmpty() && height[st.peek()] <= h) {
                int bottomH = height[st.pop()];
                if (st.isEmpty()) {
                    break;
                }
                int left = st.peek();
                int dh = Math.min(height[left], height[i]) - bottomH; // 面积的高
                ans += dh * (i - left - 1);
            }
            st.push(i);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int trap(vector<int>& height) {
        int ans = 0;
        stack<int> st;
        for (int i = 0; i < height.size(); i++) {
            int h = height[i];
            while (!st.empty() && height[st.top()] <= h) {
                int bottom_h = height[st.top()];
                st.pop();
                if (st.empty()) {
                    break;
                }
                int left = st.top();
                int dh = min(height[left], height[i]) - bottom_h; // 面积的高
                ans += dh * (i - left - 1);
            }
            st.push(i);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int trap(int* height, int heightSize) {
    int ans = 0;

    // 用数组模拟栈，栈大小最多为 heightSize
    int* st = malloc(sizeof(int) * heightSize);
    int top = -1; // 栈顶指针，初始为空栈

    for (int i = 0; i < heightSize; i++) {
        int h = height[i];
        while (top >= 0 && height[st[top]] <= h) {
            int bottom_h = height[st[top]];
            top--; // 出栈
            if (top < 0) {
                break;
            }
            int left = st[top];
            int dh = MIN(height[left], height[i]) - bottom_h; // 面积的高
            ans += dh * (i - left - 1);
        }
        st[++top] = i; // 入栈
    }

    free(st);
    return ans;
}
```

```go [sol-Go]
func trap(height []int) (ans int) {
    st := []int{}
    for i, h := range height {
        for len(st) > 0 && height[st[len(st)-1]] <= h {
            bottomH := height[st[len(st)-1]]
            st = st[:len(st)-1]
            if len(st) == 0 {
                break
            }
            left := st[len(st)-1]
            dh := min(height[left], h) - bottomH // 面积的高
            ans += dh * (i - left - 1)
        }
        st = append(st, i)
    }
    return
}
```

```js [sol-JavaScript]
var trap = function(height) {
    let ans = 0;
    const st = [];
    for (let i = 0; i < height.length; i++) {
        const h = height[i];
        while (st.length && height[st[st.length - 1]] <= h) {
            const bottomH = height[st.pop()];
            if (st.length === 0) {
                break;
            }
            const left = st[st.length - 1];
            const dh = Math.min(height[left], height[i]) - bottomH; // 面积的高
            ans += dh * (i - left - 1);
        }
        st.push(i);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn trap(height: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut st: Vec<usize> = Vec::new();
        for (i, &h) in height.iter().enumerate() {
            while !st.is_empty() && height[st[st.len() - 1]] <= h {
                let bottom_h = height[st.pop().unwrap()];
                if st.is_empty() {
                    break;
                }
                let left = st[st.len() - 1];
                let dh = height[left].min(h) - bottom_h;
                ans += dh * ((i - left - 1) as i32);
            }
            st.push(i);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{height}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(\min(n,U))$，其中 $U=\max(\textit{height})-\min(\textit{height})+1$。注意栈中没有重复元素，在 $\textit{height}$ 值域很小的情况下，空间复杂度主要取决于 $\textit{height}$ 的值域范围。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
