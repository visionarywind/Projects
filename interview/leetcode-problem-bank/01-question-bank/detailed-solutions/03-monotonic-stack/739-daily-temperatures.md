# 739. 每日温度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/daily-temperatures/
- 题目 slug：`daily-temperatures`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/daily-temperatures/solutions/2470179/shi-pin-jiang-qing-chu-wei-shi-yao-yao-y-k0ks/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】讲清楚为什么要用单调栈！两种写法：从右到左 / 从左到右](https://leetcode.cn/problems/daily-temperatures/solutions/2470179/shi-pin-jiang-qing-chu-wei-shi-yao-yao-y-k0ks/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-jiang-qing-chu-wei-shi-yao-yao-y-k0ks`
- topic id：`2470179`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

为什么要用单调栈？请看 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)，欢迎点赞关注~

## 写法一：从右到左

栈中记录下一个更大元素的「候选项」的下标。

每次循环，我们可以在「候选项」中找到答案。

```py [sol-Python3]
class Solution:
    def dailyTemperatures(self, temperatures: List[int]) -> List[int]:
        n = len(temperatures)
        ans = [0] * n
        st = []
        for i in range(n - 1, -1, -1):
            t = temperatures[i]
            while st and t >= temperatures[st[-1]]:
                st.pop()
            if st:
                ans[i] = st[-1] - i
            st.append(i)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] dailyTemperatures(int[] temperatures) {
        int n = temperatures.length;
        int[] ans = new int[n];
        Deque<Integer> st = new ArrayDeque<>();
        for (int i = n - 1; i >= 0; i--) {
            int t = temperatures[i];
            while (!st.isEmpty() && t >= temperatures[st.peek()]) {
                st.pop();
            }
            if (!st.isEmpty()) {
                ans[i] = st.peek() - i;
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
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        int n = temperatures.size();
        vector<int> ans(n);
        stack<int> st;
        for (int i = n - 1; i >= 0; i--) {
            int t = temperatures[i];
            while (!st.empty() && t >= temperatures[st.top()]) {
                st.pop();
            }
            if (!st.empty()) {
                ans[i] = st.top() - i;
            }
            st.push(i);
        }
        return ans;
    }
};
```

```c [sol-C]
int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    *returnSize = temperaturesSize;
    int* ans = calloc(temperaturesSize, sizeof(int));
    int* st = malloc(temperaturesSize * sizeof(int)); // 用数组模拟栈
    int top = -1; // 栈顶下标
    for (int i = temperaturesSize - 1; i >= 0; i--) {
        int t = temperatures[i];
        while (top >= 0 && t >= temperatures[st[top]]) {
            top--; // 弹出栈顶
        }
        if (top >= 0) {
            ans[i] = st[top] - i;
        }
        st[++top] = i; // 入栈
    }
    free(st);
    return ans;
}
```

```go [sol-Go]
func dailyTemperatures(temperatures []int) []int {
    ans := make([]int, len(temperatures))
    st := []int{}
    for i, t := range slices.Backward(temperatures) {
        for len(st) > 0 && t >= temperatures[st[len(st)-1]] {
            st = st[:len(st)-1]
        }
        if len(st) > 0 {
            ans[i] = st[len(st)-1] - i
        }
        st = append(st, i)
    }
    return ans
}
```

```js [sol-JavaScript]
var dailyTemperatures = function(temperatures) {
    const n = temperatures.length;
    const ans = Array(n).fill(0);
    const st = [];
    for (let i = n - 1; i >= 0; i--) {
        const t = temperatures[i];
        while (st.length && t >= temperatures[st[st.length - 1]]) {
            st.pop();
        }
        if (st.length) {
            ans[i] = st[st.length - 1] - i;
        }
        st.push(i);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn daily_temperatures(temperatures: Vec<i32>) -> Vec<i32> {
        let n = temperatures.len();
        let mut ans = vec![0; n];
        let mut st = vec![];
        for (i, &t) in temperatures.iter().enumerate().rev() {
            while !st.is_empty() && t >= temperatures[st[st.len() - 1]] {
                st.pop();
            }
            if let Some(j) = st.last() {
                ans[i] = (j - i) as i32;
            }
            st.push(i);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{temperatures}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(\min(n,U))$，其中 $U=\max(\textit{temperatures})-\min(\textit{temperatures})+1$。返回值不计入，仅考虑栈的最大空间消耗。

## 写法二：从左到右

栈中记录还没算出下一个更大元素的那些数的下标。

相当于栈是一个 todolist，在循环的过程中，现在还不知道答案是多少，在后面的循环中会算出答案。

```py [sol-Python3]
class Solution:
    def dailyTemperatures(self, temperatures: List[int]) -> List[int]:
        n = len(temperatures)
        ans = [0] * n
        st = []  # todolist
        for i, t in enumerate(temperatures):
            while st and t > temperatures[st[-1]]:
                j = st.pop()
                ans[j] = i - j
            st.append(i)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] dailyTemperatures(int[] temperatures) {
        int n = temperatures.length;
        int[] ans = new int[n];
        Deque<Integer> st = new ArrayDeque<>(); // todolist
        for (int i = 0; i < n; i++) {
            int t = temperatures[i];
            while (!st.isEmpty() && t > temperatures[st.peek()]) {
                int j = st.pop();
                ans[j] = i - j;
            }
            st.push(i);
        }
        return ans;
    }
}
```

```java [sol-Java 数组栈]
class Solution {
    public int[] dailyTemperatures(int[] temperatures) {
        int n = temperatures.length;
        int[] ans = new int[n];
        int[] st = new int[n]; // 数组模拟，效率更高
        int top = -1;
        for (int i = 0; i < n; i++) {
            int t = temperatures[i];
            while (top >= 0 && t > temperatures[st[top]]) {
                int j = st[top--];
                ans[j] = i - j;
            }
            st[++top] = i;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        int n = temperatures.size();
        vector<int> ans(n);
        stack<int> st; // todolist
        for (int i = 0; i < n; i++) {
            int t = temperatures[i];
            while (!st.empty() && t > temperatures[st.top()]) {
                int j = st.top();
                st.pop();
                ans[j] = i - j;
            }
            st.push(i);
        }
        return ans;
    }
};
```

```c [sol-C]
int* dailyTemperatures(int* temperatures, int temperaturesSize, int* returnSize) {
    *returnSize = temperaturesSize;
    int* ans = calloc(temperaturesSize, sizeof(int));
    int* st = malloc(temperaturesSize * sizeof(int)); // 用数组模拟栈
    int top = -1; // 栈顶下标
    for (int i = 0; i < temperaturesSize; i++) {
        int t = temperatures[i];
        while (top >= 0 && t > temperatures[st[top]]) {
            int j = st[top--]; // 弹出栈顶
            ans[j] = i - j;
        }
        st[++top] = i; // 入栈
    }
    free(st);
    return ans;
}
```

```go [sol-Go]
func dailyTemperatures(temperatures []int) []int {
    ans := make([]int, len(temperatures))
    st := []int{} // todolist
    for i, t := range temperatures {
        for len(st) > 0 && t > temperatures[st[len(st)-1]] {
            j := st[len(st)-1]
            st = st[:len(st)-1]
            ans[j] = i - j
        }
        st = append(st, i)
    }
    return ans
}
```

```js [sol-JavaScript]
var dailyTemperatures = function(temperatures) {
    const n = temperatures.length;
    const ans = Array(n).fill(0);
    const st = []; // todolist
    for (let i = 0; i < n; i++) {
        const t = temperatures[i];
        while (st.length && t > temperatures[st[st.length - 1]]) {
            const j = st.pop();
            ans[j] = i - j;
        }
        st.push(i);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn daily_temperatures(temperatures: Vec<i32>) -> Vec<i32> {
        let mut ans = vec![0; temperatures.len()];
        let mut st = vec![]; // todolist
        for (i, &t) in temperatures.iter().enumerate() {
            while !st.is_empty() && t > temperatures[st[st.len() - 1]] {
                let j = st.pop().unwrap();
                ans[j] = (i - j) as i32;
            }
            st.push(i);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{temperatures}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。注意这种写法栈中可以有重复元素。

更多相似题目，请看 [单调栈题单](https://leetcode.cn/circle/discuss/9oZFK9/)。

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
