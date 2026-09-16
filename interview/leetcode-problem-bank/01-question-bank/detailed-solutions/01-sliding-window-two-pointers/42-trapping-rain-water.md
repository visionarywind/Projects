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

## 本地原创解析

### 1. 题意重述

本题来自 `三、单序列双指针 / §3.2 相向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.2 相向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
