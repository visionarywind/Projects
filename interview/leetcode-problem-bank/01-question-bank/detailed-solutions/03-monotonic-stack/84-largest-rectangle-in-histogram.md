# 84. 柱状图中最大的矩形

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-rectangle-in-histogram/
- 题目 slug：`largest-rectangle-in-histogram`
- 来源专题：单调栈
- 来源分类路径：二、矩形
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/largest-rectangle-in-histogram/solutions/2695467/dan-diao-zhan-fu-ti-dan-pythonjavacgojsr-89s7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步优化：从三次遍历到一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/largest-rectangle-in-histogram/solutions/2695467/dan-diao-zhan-fu-ti-dan-pythonjavacgojsr-89s7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dan-diao-zhan-fu-ti-dan-pythonjavacgojsr-89s7`
- topic id：`2695467`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分析

![单调栈原理讲解](https://pic.leetcode.cn/1710804602-sheXGS-lc84.jpg)

首先，面积最大矩形的高度**一定是** $\textit{heights}$ **中的元素**。这可以用反证法证明：假如高度不在 $\textit{heights}$ 中，比如 $4$，那我们可以增加高度直到触及某根柱子的顶部，比如增加到 $5$，由于矩形底边长不变，高度增加，我们得到了面积更大的矩形，矛盾，所以面积最大矩形的高度一定是 $\textit{heights}$ 中的元素。

枚举每个 $h=\textit{heights}[i]$，作为矩形的高。那么矩形的宽最大是多少？我们需要知道：

- 在 $i$ 左侧的**小于** $h$ 的最近元素的下标 $\textit{left}$，如果不存在则为 $-1$。求出了 $\textit{left}$，那么 $\textit{left}+1$ 就是矩形最左边那根柱子。如果 $\textit{left}=-1$，那么加一后是 $0$，就是整个 $\textit{heights}$ 最左边的柱子。
- 在 $i$ 右侧的**小于** $h$ 的最近元素的下标 $\textit{right}$，如果不存在则为 $n$。求出了 $\textit{right}$，那么 $\textit{right}-1$ 就是矩形最右边那根柱子。如果 $\textit{right}=n$，那么减一后是 $n-1$，就是整个 $\textit{heights}$ 最右边的柱子。

比如示例 1（上图），选择 $i=2$ 这个柱子作为矩形的高，那么左边小于 $\textit{heights}[2]=5$ 的最近元素的下标为 $\textit{left}=1$，右边小于 $\textit{heights}[2]=5$ 的最近元素的下标为 $\textit{right}=4$。矩形的宽就是 $\textit{right}-\textit{left}-1 = 4-1-1=2$，矩形面积为 $h\cdot (\textit{right}-\textit{left}-1) = 5\cdot 2 = 10$。

枚举 $i$，计算对应的矩形面积，更新答案的最大值。

为什么这样做不会漏掉答案？题目本质是计算高×宽的最大值，高是我们枚举的，所以只要保证宽尽量大，答案就一定能被我们枚举计算到。

如何快速计算 $\textit{left}$ 和 $\textit{right}$？可以用**单调栈**。原理请看视频：[单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)，欢迎点赞关注~

## 答疑

**问**：为什么一定要找「最近」的？

**答**：看上面的图，比如 $5$ 右侧小于 $5$ 的最近高度是 $2$，如果我们找的不是高度 $2$ 而是更远的高度 $3$，那么我们会误认为矩形右边界最远可以是高度 $2$（$3$ 的左边是 $2$），然而这样的矩形已经超出柱状图的范围了，不符合题目要求。

## 写法一：三次遍历

```py [sol-Python3]
class Solution:
    def largestRectangleArea(self, heights: List[int]) -> int:
        n = len(heights)
        left = [-1] * n
        st = []
        for i, h in enumerate(heights):
            while st and heights[st[-1]] >= h:
                st.pop()
            if st:
                left[i] = st[-1]
            st.append(i)

        right = [n] * n
        st.clear()
        for i in range(n - 1, -1, -1):
            h = heights[i]
            while st and heights[st[-1]] >= h:
                st.pop()
            if st:
                right[i] = st[-1]
            st.append(i)

        ans = 0
        for h, l, r in zip(heights, left, right):
            ans = max(ans, h * (r - l - 1))
        return ans
```

```java [sol-Java]
class Solution {
    public int largestRectangleArea(int[] heights) {
        int n = heights.length;
        int[] left = new int[n];
        Deque<Integer> st = new ArrayDeque<>();
        for (int i = 0; i < n; i++) {
            int h = heights[i];
            while (!st.isEmpty() && heights[st.peek()] >= h) {
                st.pop();
            }
            left[i] = st.isEmpty() ? -1 : st.peek();
            st.push(i);
        }

        int[] right = new int[n];
        st.clear();
        for (int i = n - 1; i >= 0; i--) {
            int h = heights[i];
            while (!st.isEmpty() && heights[st.peek()] >= h) {
                st.pop();
            }
            right[i] = st.isEmpty() ? n : st.peek();
            st.push(i);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, heights[i] * (right[i] - left[i] - 1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestRectangleArea(vector<int> &heights) {
        int n = heights.size();
        vector<int> left(n, -1);
        stack<int> st;
        for (int i = 0; i < n; i++) {
            int h = heights[i];
            while (!st.empty() && heights[st.top()] >= h) {
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
            int h = heights[i];
            while (!st.empty() && heights[st.top()] >= h) {
                st.pop();
            }
            if (!st.empty()) {
                right[i] = st.top();
            }
            st.push(i);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = max(ans, heights[i] * (right[i] - left[i] - 1));
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int largestRectangleArea(int* heights, int n) {
    int* left = malloc(sizeof(int) * n);
    int* stack = malloc(sizeof(int) * (n + 1));
    int top = -1; // 栈顶下标（-1 表示栈为空）
    for (int i = 0; i < n; i++) {
        int h = heights[i];
        while (top >= 0 && heights[stack[top]] >= h) {
            top--; // 出栈
        }
        left[i] = top < 0 ? -1 : stack[top];
        stack[++top] = i; // 入栈
    }

    int* right = malloc(sizeof(int) * n);
    top = -1; // 清空栈
    for (int i = n - 1; i >= 0; i--) {
        int h = heights[i];
        while (top >= 0 && heights[stack[top]] >= h) {
            top--; // 出栈
        }
        right[i] = top < 0 ? n : stack[top];
        stack[++top] = i; // 入栈
    }

    int ans = 0;
    for (int i = 0; i < n; i++) {
        ans = MAX(ans, heights[i] * (right[i] - left[i] - 1));
    }

    free(left);
    free(right);
    free(stack);
    return ans;
}
```

```go [sol-Go]
func largestRectangleArea(heights []int) (ans int) {
    n := len(heights)
    left := make([]int, n)
    st := []int{-1} // 哨兵，简化 left[i] = ... 的逻辑
    for i, h := range heights {
        for len(st) > 1 && heights[st[len(st)-1]] >= h {
            st = st[:len(st)-1]
        }
        left[i] = st[len(st)-1] // 因为有哨兵，这里不需要判空
        st = append(st, i)
    }

    right := make([]int, n)
    st = st[:1]
    st[0] = n // 哨兵
    for i, h := range slices.Backward(heights) {
        for len(st) > 1 && heights[st[len(st)-1]] >= h {
            st = st[:len(st)-1]
        }
        right[i] = st[len(st)-1]
        st = append(st, i)
    }

    for i, h := range heights {
        ans = max(ans, h*(right[i]-left[i]-1))
    }
    return
}
```

```js [sol-JavaScript]
var largestRectangleArea = function(heights) {
    const n = heights.length;
    const left = Array(n).fill(-1);
    const st = [];
    for (let i = 0; i < n; i++) {
        const h = heights[i];
        while (st.length && heights[st[st.length - 1]] >= h) {
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
        const h = heights[i];
        while (st.length && heights[st[st.length - 1]] >= h) {
            st.pop();
        }
        if (st.length) {
            right[i] = st[st.length - 1];
        }
        st.push(i);
    }

    let ans = 0;
    for (let i = 0; i < n; i++) {
        ans = Math.max(ans, heights[i] * (right[i] - left[i] - 1));
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn largest_rectangle_area(heights: Vec<i32>) -> i32 {
        let n = heights.len();
        let mut left = vec![-1; n];
        let mut st = vec![];
        for (i, &h) in heights.iter().enumerate() {
            while !st.is_empty() && heights[*st.last().unwrap()] >= h {
                st.pop();
            }
            if let Some(&j) = st.last() {
                left[i] = j as i32;
            }
            st.push(i);
        }

        let mut right = vec![n as i32; n];
        st.clear();
        for (i, &h) in heights.iter().enumerate().rev() {
            while !st.is_empty() && heights[*st.last().unwrap()] >= h {
                st.pop();
            }
            if let Some(&j) = st.last() {
                right[i] = j as i32;
            }
            st.push(i);
        }

        let mut ans = 0;
        for ((h, l), r) in heights.into_iter().zip(left).zip(right) {
            ans = ans.max(h * (r - l - 1));
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{heights}$ 的长度。每个元素入栈出栈各至多一次，所以二重循环是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二：两次遍历

为了做到两次遍历，以及写法三的一次遍历，首先，把 $\textit{right}[i]$ 的定义略作修改，调整为：在 $i$ 右侧的**小于或等于** $h=\textit{heights}[i]$ 的最近元素的下标。

如果 $\textit{heights}$ 中没有相同的元素，这样修改不影响 $\textit{right}[i]$。

如果 $\textit{heights}$ 中有相同的元素呢？比如 $\textit{heights}=[1,3,4,3,2]$，左边那个 $3$ 的 $\textit{right}[i]$ 会变小，导致矩形面积变小，这是否会导致计算错误？

不会。注意在这种情况下，这两个高为 $3$ 的柱子，对应的矩形面积（在写法一中）是一样大的，虽然（在写法二中）左边那个 $3$ 的矩形面积变小了，但右边那个 $3$ 的矩形面积是不变的，所以我们**不会错过正确答案**。

修改 $\textit{right}[i]$ 的定义后，我们可以把 $\textit{left}$ 和 $\textit{right}$ 合在一起计算，从而减少一次遍历：

- 在计算 $\textit{left}$ 的过程中，如果栈顶元素 $\ge\textit{heights}[i]$，那么 $i$ 就是栈顶元素的 $\textit{right}$。

```py [sol-Python3]
class Solution:
    def largestRectangleArea(self, heights: List[int]) -> int:
        n = len(heights)
        left = [-1] * n
        right = [n] * n
        st = []
        for i, h in enumerate(heights):
            while st and heights[st[-1]] >= h:
                right[st.pop()] = i
            if st:
                left[i] = st[-1]
            st.append(i)

        ans = 0
        for h, l, r in zip(heights, left, right):
            ans = max(ans, h * (r - l - 1))
        return ans
```

```java [sol-Java]
class Solution {
    public int largestRectangleArea(int[] heights) {
        int n = heights.length;
        int[] left = new int[n];
        int[] right = new int[n];
        Arrays.fill(right, n);
        Deque<Integer> st = new ArrayDeque<>();
        for (int i = 0; i < n; i++) {
            int h = heights[i];
            while (!st.isEmpty() && heights[st.peek()] >= h) {
                right[st.pop()] = i;
            }
            left[i] = st.isEmpty() ? -1 : st.peek();
            st.push(i);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, heights[i] * (right[i] - left[i] - 1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestRectangleArea(vector<int> &heights) {
        int n = heights.size();
        vector<int> left(n, -1);
        vector<int> right(n, n);
        stack<int> st;
        for (int i = 0; i < n; i++) {
            int h = heights[i];
            while (!st.empty() && heights[st.top()] >= h) {
                right[st.top()] = i;
                st.pop();
            }
            if (!st.empty()) {
                left[i] = st.top();
            }
            st.push(i);
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = max(ans, heights[i] * (right[i] - left[i] - 1));
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int largestRectangleArea(int* heights, int n) {
    int* left = malloc(sizeof(int) * n);
    int* right = malloc(sizeof(int) * n);
    int* stack = malloc(sizeof(int) * (n + 1));
    int top = -1; // 栈顶下标（-1 表示栈为空）
    for (int i = 0; i < n; i++) {
        int h = heights[i];
        while (top >= 0 && heights[stack[top]] >= h) {
            right[stack[top--]] = i;
        }
        left[i] = top < 0 ? -1 : stack[top];
        stack[++top] = i; // 入栈
    }

    // 栈中剩余元素的 right 都是 n
    for (int i = 0; i <= top; i++) {
        right[stack[i]] = n;
    }

    int ans = 0;
    for (int i = 0; i < n; i++) {
        ans = MAX(ans, heights[i] * (right[i] - left[i] - 1));
    }

    free(left);
    free(right);
    free(stack);
    return ans;
}
```

```go [sol-Go]
func largestRectangleArea(heights []int) (ans int) {
    n := len(heights)
    left := make([]int, n)
    right := make([]int, n)
    st := []int{-1} // 哨兵，简化 left[i] = ... 的逻辑
    for i, h := range heights {
        for len(st) > 1 && heights[st[len(st)-1]] >= h {
            right[st[len(st)-1]] = i
            st = st[:len(st)-1]
        }
        left[i] = st[len(st)-1] // 因为有哨兵，这里不需要判空
        st = append(st, i)
    }

    // 栈中剩余元素的 right 都是 n
    for _, i := range st[1:] {
        right[i] = n
    }

    for i, h := range heights {
        ans = max(ans, h*(right[i]-left[i]-1))
    }
    return
}
```

```js [sol-JavaScript]
var largestRectangleArea = function(heights) {
    const n = heights.length;
    const left = Array(n).fill(-1);
    const right = Array(n).fill(n);
    const st = [];
    for (let i = 0; i < n; i++) {
        const h = heights[i];
        while (st.length && heights[st[st.length - 1]] >= h) {
            right[st.pop()] = i;
        }
        if (st.length) {
            left[i] = st[st.length - 1];
        }
        st.push(i);
    }

    let ans = 0;
    for (let i = 0; i < n; i++) {
        ans = Math.max(ans, heights[i] * (right[i] - left[i] - 1));
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn largest_rectangle_area(heights: Vec<i32>) -> i32 {
        let n = heights.len();
        let mut left = vec![-1; n];
        let mut right = vec![n as i32; n];
        let mut st = vec![];
        for (i, &h) in heights.iter().enumerate() {
            while !st.is_empty() && heights[*st.last().unwrap()] >= h {
                right[st.pop().unwrap()] = i as i32;
            }
            if let Some(&j) = st.last() {
                left[i] = j as i32;
            }
            st.push(i);
        }

        let mut ans = 0;
        for ((h, l), r) in heights.into_iter().zip(left).zip(right) {
            ans = ans.max(h * (r - l - 1));
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{heights}$ 的长度。每个元素入栈出栈各至多一次，所以二重循环是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法三：一次遍历

写法二告诉我们，栈顶出栈时，当前下标就是栈顶的 $\textit{right}$。

如果此刻能顺带求出栈顶的 $\textit{left}$，那不就能一步到位，一次遍历就搞定了？

想一想，栈顶的 $\textit{left}$ 在哪？

由于单调栈是底小顶大的，栈顶下面那个柱子的高度一定比栈顶小，所以栈顶下面的值就是 $\textit{left}$。

为简化代码逻辑，可以在一开始把 $-1$ 入栈，当作哨兵。当栈中只有一个数的时候，栈顶下面那个数刚好就是 $-1$，对应 $\textit{left}[i]=-1$ 的情况。

此外，循环结束的时候，栈中还有数据，这些数据也要计算矩形面积。处理这种情况可以再写一个循环，但更简单的办法是，往 $\textit{heights}$ 的末尾加一个 $-1$（或者任意 $\le \min(\textit{heights})$ 的数），从而保证循环结束的时候，栈一定是空的（不包括哨兵）。

```py [sol-Python3]
class Solution:
    def largestRectangleArea(self, heights: List[int]) -> int:
        heights.append(-1)  # 最后大火收汁，用 -1 把栈清空
        st = [-1]  # 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
        ans = 0
        for right, h in enumerate(heights):
            while len(st) > 1 and heights[st[-1]] >= h:
                i = st.pop()  # 矩形的高（的下标）
                left = st[-1]  # 栈顶下面那个数就是 left
                ans = max(ans, heights[i] * (right - left - 1))
            st.append(right)
        return ans
```

```java [sol-Java]
class Solution {
    public int largestRectangleArea(int[] heights) {
        int n = heights.length;
        Deque<Integer> st = new ArrayDeque<>();
        st.push(-1); // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
        int ans = 0;
        for (int right = 0; right <= n; right++) {
            int h = right < n ? heights[right] : -1;
            while (st.size() > 1 && heights[st.peek()] >= h) {
                int i = st.pop(); // 矩形的高（的下标）
                int left = st.peek(); // 栈顶下面那个数就是 left
                ans = Math.max(ans, heights[i] * (right - left - 1));
            }
            st.push(right);
        }
        return ans;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int largestRectangleArea(int[] heights) {
        int n = heights.length;
        int[] st = new int[n + 1];
        int top = -1; // 栈顶下标
        st[++top] = -1; // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
        int ans = 0;
        for (int right = 0; right <= n; right++) {
            int h = right < n ? heights[right] : -1;
            while (top > 0 && heights[st[top]] >= h) {
                int i = st[top--]; // 矩形的高（的下标）
                int left = st[top]; // 栈顶下面那个数就是 left
                ans = Math.max(ans, heights[i] * (right - left - 1));
            }
            st[++top] = right;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestRectangleArea(vector<int>& heights) {
        heights.push_back(-1); // 最后大火收汁，用 -1 把栈清空
        stack<int> st;
        st.push(-1); // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
        int ans = 0;
        for (int right = 0; right < heights.size(); right++) {
            int h = heights[right];
            while (st.size() > 1 && heights[st.top()] >= h) {
                int i = st.top(); // 矩形的高（的下标）
                st.pop();
                int left = st.top(); // 栈顶下面那个数就是 left
                ans = max(ans, heights[i] * (right - left - 1));
            }
            st.push(right);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int largestRectangleArea(int* heights, int n) {
    int* stack = malloc((n + 1) * sizeof(int));
    int top = -1; // 栈顶下标
    stack[++top] = -1; // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
    int ans = 0;

    for (int right = 0; right <= n; right++) {
        int h = right < n ? heights[right] : -1;
        while (top > 0 && heights[stack[top]] >= h) {
            int i = stack[top--]; // 矩形的高（的下标）
            int left = stack[top]; // 栈顶下面那个数就是 left
            ans = MAX(ans, heights[i] * (right - left - 1));
        }
        stack[++top] = right;
    }

    free(stack);
    return ans;
}
```

```go [sol-Go]
func largestRectangleArea(heights []int) (ans int) {
    heights = append(heights, -1) // 最后大火收汁，用 -1 把栈清空
    st := []int{-1} // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
    for right, h := range heights {
        for len(st) > 1 && heights[st[len(st)-1]] >= h {
            i := st[len(st)-1] // 矩形的高（的下标）
            st = st[:len(st)-1]
            left := st[len(st)-1] // 栈顶下面那个数就是 left
            ans = max(ans, heights[i]*(right-left-1))
        }
        st = append(st, right)
    }
    return
}
```

```js [sol-JavaScript]
var largestRectangleArea = function(heights) {
    heights.push(-1); // 最后大火收汁，用 -1 把栈清空
    const st = [-1]; // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
    let ans = 0;
    for (let right = 0; right < heights.length; right++) {
        const h = heights[right];
        while (st.length > 1 && heights[st[st.length - 1]] >= h) {
            const i = st.pop(); // 矩形的高（的下标）
            const left = st[st.length - 1]; // 栈顶下面那个数就是 left
            ans = Math.max(ans, heights[i] * (right - left - 1));
        }
        st.push(right);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn largest_rectangle_area(mut heights: Vec<i32>) -> i32 {
        heights.push(-1); // 最后大火收汁，用 -1 把栈清空
        let mut st = vec![-1]; // 在栈中只有一个数的时候，栈顶的「下面那个数」是 -1，对应 left[i] = -1 的情况
        let mut ans = 0;
        for (right, &h) in heights.iter().enumerate() {
            let right = right as i32;
            while st.len() > 1 && heights[*st.last().unwrap() as usize] >= h {
                let i = st.pop().unwrap() as usize; // 矩形的高（的下标）
                let left = *st.last().unwrap(); // 栈顶下面那个数就是 left
                ans = ans.max(heights[i] * (right - left - 1));
            }
            st.push(right);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{heights}$ 的长度。每个元素入栈出栈各至多一次，所以二重循环是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(\min(n,U))$。其中 $U$ 为 $\textit{heights}$ 中的不同元素个数。注意栈中没有重复元素。

## 专题训练

见下面单调栈题单的「**二、矩形**」。

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

本题来自 `二、矩形`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、矩形`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
