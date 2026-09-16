# 503. 下一个更大元素 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/next-greater-element-ii/
- 题目 slug：`next-greater-element-ii`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/next-greater-element-ii/solutions/2820363/shi-pin-dan-diao-zhan-de-liang-chong-xie-k236/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】单调栈的两种写法，附单调栈题单（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/next-greater-element-ii/solutions/2820363/shi-pin-dan-diao-zhan-de-liang-chong-xie-k236/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-dan-diao-zhan-de-liang-chong-xie-k236`
- topic id：`2820363`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 $\textit{nums}$ 是一个循环数组，$\textit{nums}[n-1]$ 右边是 $\textit{nums}[0]$。我们可以把 $\textit{nums}$ 复制一份，拼在 $\textit{nums}$ 右边，这样就把环形数组变成一般数组了。例如 $[1,2,1]$ 变成 $[1,2,1,1,2,1]$。

本题要计算每个元素的下一个更大元素的值（注意是值不是下标），这是单调栈的标准应用，请看 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)。

计算下一个更大元素可以从右到左遍历，也可以从左到右遍历，这两种方法都在视频中讲了。

## 方法一：从右到左

从右往左遍历，栈中记录下一个更大元素的「候选项」。

由于左边更大元素会「挡住」右边更小的元素，所以右边更小的元素是无用信息（不会成为左边元素的下一个更大元素），这会导致栈底（右边）大，栈顶（左边）小。

代码实现时，无需真的把数组复制一份，而是用下标模 $n$ 的方式取到对应的元素值。

```py [sol-Python3]
class Solution:
    def nextGreaterElements(self, nums: List[int]) -> List[int]:
        n = len(nums)
        ans = [-1] * n
        st = []

        for i in range(n * 2 - 1, -1, -1):
            x = nums[i % n]
            while st and x >= st[-1]:
                # 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
                st.pop()
            if st and i < n:
                ans[i] = st[-1]
            st.append(x)

        return ans
```

```java [sol-Java]
class Solution {
    public int[] nextGreaterElements(int[] nums) {
        int n = nums.length;
        int[] ans = new int[n];
        Arrays.fill(ans, -1);
        Deque<Integer> st = new ArrayDeque<>();

        for (int i = n * 2 - 1; i >= 0; i--) {
            int x = nums[i % n];
            while (!st.isEmpty() && x >= st.peek()) {
                // 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
                st.pop();
            }
            if (i < n && !st.isEmpty()) {
                ans[i] = st.peek();
            }
            st.push(x);
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> nextGreaterElements(vector<int>& nums) {
        int n = nums.size();
        vector<int> ans(n, -1);
        stack<int> st;

        for (int i = n * 2 - 1; i >= 0; i--) {
            int x = nums[i % n];
            while (!st.empty() && x >= st.top()) {
                // 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
                st.pop();
            }
            if (i < n && !st.empty()) {
                ans[i] = st.top();
            }
            st.push(x);
        }

        return ans;
    }
};
```

```c [sol-C]
int* nextGreaterElements(int* nums, int numsSize, int* returnSize) {
    *returnSize = numsSize;
    int* ans = malloc(numsSize * sizeof(int));
    memset(ans, -1, numsSize * sizeof(int));
    int* st = malloc(numsSize * sizeof(int));
    int top = 0;

    for (int i = numsSize * 2 - 1; i >= 0; i--) {
        int x = nums[i % numsSize];
        while (top && x >= st[top - 1]) {
            // 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
            top--;
        }
        if (top && i < numsSize) {
            ans[i] = st[top - 1];
        }
        st[top++] = x;
    }

    free(st);
    return ans;
}
```

```go [sol-Go]
func nextGreaterElements(nums []int) []int {
    n := len(nums)
    ans := make([]int, n)
    for i := range ans {
        ans[i] = -1
    }
    st := []int{}

    for i := n*2 - 1; i >= 0; i-- {
        x := nums[i%n]
        for len(st) > 0 && x >= st[len(st)-1] {
            // 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
            st = st[:len(st)-1]
        }
        if i < n && len(st) > 0 {
            ans[i] = st[len(st)-1]
        }
        st = append(st, x)
    }

    return ans
}
```

```js [sol-JavaScript]
var nextGreaterElements = function(nums) {
    const n = nums.length;
    const ans = Array(n).fill(-1);
    const st = [];

    for (let i = n * 2 - 1; i >= 0; i--) {
        const x = nums[i % n];
        while (st.length && x >= st[st.length - 1]) {
            // 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
            st.pop();
        }
        if (i < n && st.length) {
            ans[i] = st[st.length - 1];
        }
        st.push(x);
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn next_greater_elements(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut ans = vec![-1; n];
        let mut st = vec![];

        for i in (0..n * 2).rev() {
            let x = nums[i % n];
            while let Some(&top) = st.last() {
                if x < top {
                    break;
                }
                // 由于 x 的出现，栈顶元素永远不会是左边元素的「下一个更大元素」
                st.pop();
            }
            if i < n && !st.is_empty() {
                ans[i] = *st.last().unwrap();
            }
            st.push(x);
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：从左到右

栈中记录还没算出「下一个更大元素」的那些数的下标。可以把栈视作一个 todolist（待办事项清单）。

只要遍历到比栈顶元素值更大的数，就意味着栈顶元素找到了答案，记录答案，然后弹出栈顶。

```py [sol-Python3]
class Solution:
    def nextGreaterElements(self, nums: List[int]) -> List[int]:
        n = len(nums)
        ans = [-1] * n
        st = []

        for i in range(n * 2):
            x = nums[i % n]
            while st and x > nums[st[-1]]:
                # x 是 nums[st[-1]] 的下一个更大元素
                # 既然 nums[st[-1]] 已经算出答案，则从栈顶弹出
                ans[st.pop()] = x
            if i < n:
                st.append(i)

        return ans
```

```java [sol-Java]
class Solution {
    public int[] nextGreaterElements(int[] nums) {
        int n = nums.length;
        int[] ans = new int[n];
        Arrays.fill(ans, -1);
        Deque<Integer> st = new ArrayDeque<>();

        for (int i = 0; i < n * 2; i++) {
            int x = nums[i % n];
            while (!st.isEmpty() && x > nums[st.peek()]) {
                // x 是 nums[st.peek()] 的下一个更大元素
                // 既然 nums[st.peek()] 已经算出答案，则从栈顶弹出
                ans[st.pop()] = x;
            }
            if (i < n) {
                st.push(i);
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> nextGreaterElements(vector<int>& nums) {
        int n = nums.size();
        vector<int> ans(n, -1);
        stack<int> st;

        for (int i = 0; i < n * 2; i++) {
            int x = nums[i % n];
            while (!st.empty() && x > nums[st.top()]) {
                // x 是 nums[st.top()] 的下一个更大元素
                // 既然 nums[st.top()] 已经算出答案，则从栈顶弹出
                ans[st.top()] = x;
                st.pop();
            }
            if (i < n) {
                st.push(i);
            }
        }

        return ans;
    }
};
```

```c [sol-C]
int* nextGreaterElements(int* nums, int numsSize, int* returnSize) {
    *returnSize = numsSize;
    int* ans = malloc(numsSize * sizeof(int));
    memset(ans, -1, numsSize * sizeof(int));
    int* st = malloc(numsSize * sizeof(int));
    int top = 0;

    for (int i = 0; i < numsSize * 2; i++) {
        int x = nums[i % numsSize];
        while (top && x > nums[st[top - 1]]) {
            // x 是 nums[st[top-1]] 的下一个更大元素
            // 既然 nums[st[top-1]] 已经算出答案，则从栈顶弹出
            ans[st[--top]] = x;
        }
        if (i < numsSize) {
            st[top++] = i;
        }
    }

    free(st);
    return ans;
}
```

```go [sol-Go]
func nextGreaterElements(nums []int) []int {
    n := len(nums)
    ans := make([]int, n)
    for i := range ans {
        ans[i] = -1
    }
    st := []int{}

    for i := range n * 2 {
        x := nums[i%n]
        for len(st) > 0 && x > nums[st[len(st)-1]] {
            // x 是 nums[st[len(st)-1]] 的下一个更大元素
            // 既然 nums[st[len(st)-1]] 已经算出答案，则从栈顶弹出
            ans[st[len(st)-1]] = x
            st = st[:len(st)-1]
        }
        if i < n {
            st = append(st, i)
        }
    }

    return ans
}
```

```js [sol-JavaScript]
var nextGreaterElements = function(nums) {
    const n = nums.length;
    const ans = Array(n).fill(-1);
    const st = [];

    for (let i = 0; i < n * 2; i++) {
        const x = nums[i % n];
        while (st.length && x > nums[st[st.length - 1]]) {
            // x 是 nums[st[st.length-1]] 的下一个更大元素
            // 既然 nums[st[st.length-1]] 已经算出答案，则从栈顶弹出
            ans[st.pop()] = x;
        }
        if (i < n) {
            st.push(i);
        }
    }

    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn next_greater_elements(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut ans = vec![-1; n];
        let mut st = vec![];

        for i in 0..n * 2 {
            let x = nums[i % n];
            while let Some(&top) = st.last() {
                if x <= nums[top] {
                    break;
                }
                // x 是 nums[top] 的下一个更大元素
                // 既然 nums[top] 已经算出答案，则从栈顶弹出
                ans[top] = x;
                st.pop();
            }
            if i < n {
                st.push(i);
            }
        }

        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然我们写了个二重循环，但站在每个元素的视角看，这个元素在二重循环中最多入栈出栈各一次，因此循环次数**之和**是 $\mathcal{O}(n)$，所以时间复杂度是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、单调栈 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、单调栈 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
