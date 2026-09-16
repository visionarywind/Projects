# 907. 子数组的最小值之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-subarray-minimums/
- 题目 slug：`sum-of-subarray-minimums`
- 来源专题：单调栈
- 来源分类路径：三、贡献法
- 难度分：1976
- 外部题解来源：https://leetcode.cn/problems/sum-of-subarray-minimums/solutions/1930857/gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贡献法+单调栈，从三次遍历到一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-subarray-minimums/solutions/1930857/gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gong-xian-fa-dan-diao-zhan-san-chong-shi-gxa5`
- topic id：`1930857`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：单调栈

请看视频 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)

## 提示 1：计算以 arr[i] 为最小值的子数组的个数

暴力做法是枚举所有子数组，算出每个子数组的最小值，但这太慢了。

不妨换个视角，对每个数，算出它是哪些子数组的最小值。

例如 $\textit{arr}=[1,4,2,3,1]$，其中 $2$ 是子数组 $[2],[4,2],[2,3],[4,2,3]$ 的最小值，那么 $2$ 对答案的**贡献**就是 $2\cdot 4=8$。

由于以 $2$ 为最小值的子数组，绝对不能包含比 $2$ 小的数字，因此我们可以找到 $2$ 左右两侧比它小的数的下标，从而确定子数组的**边界**。$2$ 对应的边界为开区间 $(0,4)$，即闭区间 $[1,3]$，只要在闭区间 $[1,3]$ 范围内且包含下标 $2$ 的子数组，就是以 $2$ 为最小值的子数组。

一般地，设 $\textit{arr}[i]$ 对应的边界为**开区间** $(L,R)$，由于子数组必须包含 $\textit{arr}[i]$：

- 子数组的左端点可以是 $L+1,L+2,\cdots,i$，共有 $i-L$ 个；
- 子数组的右端点可以是 $i,i+1,\cdots,R-1$，共有 $R-i$ 个。

因此，**在 $\textit{arr}$ 不含重复元素的前提下**，根据**乘法原理**，我们可以得出如下结论：

以 $\textit{arr}[i]$ 为最小值的子数组的个数为 $(i-L)\cdot(R-i)$，对答案的贡献为 $\textit{arr}[i]\cdot(i-L)\cdot(R-i)$。

> 注：如果左侧没有比 $\textit{arr}[i]$ 小的元素，则 $L=-1$；如果右侧没有比 $\textit{arr}[i]$ 小的元素，则 $R=n$。

下文讨论的边界均指的是开区间 $(L,R)$。

## 提示 2：修改边界定义，避免重复统计子数组

如果 $\textit{arr}$ 有重复元素，例如 $\textit{arr}=[1,2,4,2,3,1]$，其中第一个 $2$ 和第二个 $2$ 对应的边界都是开区间 $(0,5)$，子数组 $[2,4,2,3]$ 都包含这两个 $2$，这样在计算答案时就会重复统计同一个子数组，算出错误的结果。 

为避免重复统计，可以修改边界的定义，把右边界改为「找**小于或等于** $\textit{arr}[i]$ 的数的下标」，那么：

- 第一个 $2$ 对应的边界是 $(0,3)$，子数组需要在 $(0,3)$ 范围内且包含下标 $1$；
- 第二个 $2$ 对应的边界是 $(0,5)$，子数组需要在 $(0,5)$ 范围内且包含下标 $3$。

这样以第一个 $2$ 为最小值的子数组，就不会「越界」包含第二个 $2$ 了，从而解决了重复统计子数组的问题。

> 注：也可以把左边界改为 $\le$，右边界不变（仍为 $<$）。根据对称性，算出来的答案是一样的。

## 提示 3：高效计算边界

最后需要解决的问题是，如何高效计算每个 $\textit{arr}[i]$ 对应的左右边界？

以计算左边界为例，从左到右遍历 $\textit{arr}$，同时用某个合适的数据结构维护遍历过的元素，并**及时移除无用的元素**，如下图（$\textit{arr}$ 简写为 $a$）：

![907-2.png](https://pic.leetcode.cn/1666863950-pzFCnp-907-2.png)

移除无用元素后，再把 $\textit{arr}[i]$ 加到这个数据结构中。

由于该优化保证了数据结构中的元素会形成一个递增的序列，因此我们移除的是数据结构中的最右端的若干元素。我们需要一个数据结构，它支持移除最右端的元素，以及在最右端添加元素，故选用**栈**。

移除栈中 $\ge\textit{arr}[i]$ 的元素后，栈顶的下标就是 $\textit{arr}[i]$ 的左边界，如果此时栈为空，那么左边界为 $-1$。

> 注 1：在遍历过程中会维护一个栈，移除的是当前的栈顶元素。遍历到 $\textit{arr}[i]$ 时，$\textit{arr}[i]$ 及其右侧元素尚未入栈。
> 注 2：由于栈内元素始终保持单调递增，因此这种数据结构也叫做**单调栈**。

右边界的计算是类似的，从右往左遍历 $\textit{arr}$ 可以算出（根据提示 2，需要修改一下比较的符号，具体见代码）。

算出左右边界后，根据提示 1 的公式累加每个 $\textit{arr}[i]$ 的贡献，即为答案。

代码实现时，有三种实现版本。注意实际入栈的是元素的下标。

## 第一版：三次遍历

```py [sol1-Python3]
class Solution:
    def sumSubarrayMins(self, arr: List[int]) -> int:
        n = len(arr)
        # 左边界 left[i] 为左侧严格小于 arr[i] 的最近元素位置（不存在时为 -1）
        left, st = [-1] * n, []
        for i, x in enumerate(arr):
            while st and arr[st[-1]] >= x:
                st.pop()  # 移除无用数据
            if st: left[i] = st[-1]
            st.append(i)

        # 右边界 right[i] 为右侧小于等于 arr[i] 的最近元素位置（不存在时为 n）
        right, st = [n] * n, []
        for i in range(n - 1, -1, -1):
            while st and arr[st[-1]] > arr[i]:
                st.pop()  # 移除无用数据
            if st: right[i] = st[-1]
            st.append(i)

        ans = 0
        for i, (x, l, r) in enumerate(zip(arr, left, right)):
            ans += x * (i - l) * (r - i)  # 累加贡献
        return ans % (10 ** 9 + 7)
```

```java [sol1-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;

    public int sumSubarrayMins(int[] arr) {
        int n = arr.length;
        // 左边界 left[i] 为左侧严格小于 arr[i] 的最近元素位置（不存在时为 -1）
        int[] left = new int[n];
        Deque<Integer> st = new ArrayDeque<>(); // 注：推荐用 ArrayDeque 实现栈
        st.push(-1); // 方便赋值 left
        for (int i = 0; i < n; ++i) {
            while (st.size() > 1 && arr[st.peek()] >= arr[i])
                st.pop(); // 移除无用数据
            left[i] = st.peek();
            st.push(i);
        }

        // 右边界 right[i] 为右侧小于等于 arr[i] 的最近元素位置（不存在时为 n）
        int[] right = new int[n];
        st.clear();
        st.push(n); // 方便赋值 right
        for (int i = n - 1; i >= 0; --i) {
            while (st.size() > 1 && arr[st.peek()] > arr[i])
                st.pop(); // 移除无用数据
            right[i] = st.peek();
            st.push(i);
        }

        long ans = 0;
        for (int i = 0; i < n; ++i)
            ans += (long) arr[i] * (i - left[i]) * (right[i] - i); // 累加贡献
        return (int) (ans % MOD);
    }
}
```

```cpp [sol1-C++]
class Solution {
    const int MOD = 1e9 + 7;
public:
    int sumSubarrayMins(vector<int> &arr) {
        int n = arr.size();
        // 左边界 left[i] 为左侧严格小于 arr[i] 的最近元素位置（不存在时为 -1）
        vector<int> left(n, -1);
        stack<int> st;
        for (int i = 0; i < n; ++i) {
            while (!st.empty() && arr[st.top()] >= arr[i])
                st.pop(); // 移除无用数据
            if (!st.empty()) left[i] = st.top();
            st.push(i);
        }

        // 右边界 right[i] 为右侧小于等于 arr[i] 的最近元素位置（不存在时为 n）
        vector<int> right(n, n);
        while (!st.empty()) st.pop();
        for (int i = n - 1; i >= 0; --i) {
            while (!st.empty() && arr[st.top()] > arr[i])
                st.pop(); // 移除无用数据
            if (!st.empty()) right[i] = st.top();
            st.push(i);
        }

        long ans = 0L;
        for (int i = 0; i < n; ++i)
            ans += (long) arr[i] * (i - left[i]) * (right[i] - i); // 累加贡献
        return ans % MOD;
    }
};
```

```go [sol1-Go]
func sumSubarrayMins(arr []int) (ans int) {
    n := len(arr)
    // 左边界 left[i] 为左侧严格小于 arr[i] 的最近元素位置（不存在时为 -1）
    left := make([]int, n)
    st := []int{-1} // 方便赋值 left
    for i, x := range arr {
        for len(st) > 1 && arr[st[len(st)-1]] >= x {
            st = st[:len(st)-1] // 移除无用数据
        }
        left[i] = st[len(st)-1]
        st = append(st, i)
    }

    // 右边界 right[i] 为右侧小于等于 arr[i] 的最近元素位置（不存在时为 n）
    right := make([]int, n)
    st = []int{n} // 方便赋值 right
    for i := n - 1; i >= 0; i-- {
        for len(st) > 1 && arr[st[len(st)-1]] > arr[i] {
            st = st[:len(st)-1] // 移除无用数据
        }
        right[i] = st[len(st)-1]
        st = append(st, i)
    }

    for i, x := range arr {
        ans += x * (i - left[i]) * (right[i] - i) // 累加贡献
    }
    return ans % (1e9 + 7)
}
```

## 第二版：两次遍历

注意到在计算 $\textit{left}$ 的过程中，如果栈顶元素 $\ge\textit{arr}[i]$，那么 $i$ 就是栈顶元素的右边界，因此前两个循环可以合并。

更详细的解释：对于栈顶元素 $t$，如果 $t$ 右侧有多个小于或等于 $t$ 的元素，那么 $t$ 只会因为**右侧第一个**小于或等于 $t$ 的元素而出栈，这恰好符合右边界的定义。

```py [sol2-Python3]
class Solution:
    def sumSubarrayMins(self, arr: List[int]) -> int:
        n = len(arr)
        left, right, st = [-1] * n, [n] * n, []
        for i, x in enumerate(arr):
            while st and arr[st[-1]] >= x:
                right[st.pop()] = i  # i 恰好是栈顶的右边界
            if st: left[i] = st[-1]
            st.append(i)

        ans = 0
        for i, (x, l, r) in enumerate(zip(arr, left, right)):
            ans += x * (i - l) * (r - i)  # 累加贡献
        return ans % (10 ** 9 + 7)
```

```java [sol2-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;

    public int sumSubarrayMins(int[] arr) {
        int n = arr.length;
        int[] left = new int[n];
        int[] right = new int[n];
        Arrays.fill(right, n);
        Deque<Integer> st = new ArrayDeque<>();
        st.push(-1); // 方便赋值 left
        for (int i = 0; i < n; ++i) {
            while (st.size() > 1 && arr[st.peek()] >= arr[i])
                right[st.pop()] = i; // i 恰好是栈顶的右边界
            left[i] = st.peek();
            st.push(i);
        }

        long ans = 0;
        for (int i = 0; i < n; ++i)
            ans += (long) arr[i] * (i - left[i]) * (right[i] - i); // 累加贡献
        return (int) (ans % MOD);
    }
}
```

```cpp [sol2-C++]
class Solution {
    const int MOD = 1e9 + 7;
public:
    int sumSubarrayMins(vector<int> &arr) {
        int n = arr.size();
        vector<int> left(n, -1), right(n, n);
        stack<int> st;
        for (int i = 0; i < n; ++i) {
            while (!st.empty() && arr[st.top()] >= arr[i]) {
                right[st.top()] = i; // i 恰好是栈顶的右边界
                st.pop();
            }
            if (!st.empty()) left[i] = st.top();
            st.push(i);
        }

        long ans = 0L;
        for (int i = 0; i < n; ++i)
            ans += (long) arr[i] * (i - left[i]) * (right[i] - i); // 累加贡献
        return ans % MOD;
    }
};
```

```go [sol2-Go]
func sumSubarrayMins(arr []int) (ans int) {
    n := len(arr)
    left := make([]int, n)
    right := make([]int, n)
    for i := range right {
        right[i] = n
    }
    st := []int{-1} // 方便赋值 left
    for i, x := range arr {
        for len(st) > 1 && arr[st[len(st)-1]] >= x {
            right[st[len(st)-1]] = i // i 恰好是栈顶的右边界
            st = st[:len(st)-1]
        }
        left[i] = st[len(st)-1]
        st = append(st, i)
    }

    for i, x := range arr {
        ans += x * (i - left[i]) * (right[i] - i) // 累加贡献
    }
    return ans % (1e9 + 7)
}
```

## 第三版：一次遍历

进一步地，由于栈顶下面的元素正好也是栈顶的左边界，所以甚至连 $\textit{left}$ 和 $\textit{right}$ 数组都可以不要，直接在出栈的时候计算贡献。

为简化代码逻辑，可以在遍历前，往栈底加一个 $-1$，当作哨兵。栈底加了哨兵后，代码中的「$i$ 减去栈顶」的逻辑无需判断栈为空的情况，因为有哨兵，栈一定不是空。如果栈中只剩下哨兵，那么我们要计算的 $\textit{left}=-1$，这正好就是哨兵的值。

此外，也可以往 $\textit{arr}$ 的末尾加个 $-1$，这可以保证遍历到这个 $-1$ 的时候把栈清空（除了哨兵）。如果不加 $-1$，可能循环结束后栈中还有未处理的数据，还需要额外处理。

```py [sol3-Python3]
class Solution:
    def sumSubarrayMins(self, arr: List[int]) -> int:
        arr.append(-1)
        ans, st = 0, [-1]  # 哨兵
        for r, x in enumerate(arr):
            # 也可以 while arr[st[-1]] > x，效率略高一点
            while len(st) > 1 and arr[st[-1]] >= x:
                i = st.pop()
                ans += arr[i] * (i - st[-1]) * (r - i)  # 累加贡献
            st.append(r)
        return ans % (10 ** 9 + 7)
```

```java [sol3-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;

    public int sumSubarrayMins(int[] arr) {
        long ans = 0;
        Deque<Integer> st = new ArrayDeque<>();
        st.push(-1); // 哨兵
        for (int r = 0; r <= arr.length; ++r) {
            int x = r < arr.length ? arr[r] : -1; // 假设 arr 末尾有个 -1
            while (st.size() > 1 && arr[st.peek()] >= x) {
                int i = st.pop();
                ans += (long) arr[i] * (i - st.peek()) * (r - i); // 累加贡献
            }
            st.push(r);
        }
        return (int) (ans % MOD);
    }
}
```

```cpp [sol3-C++]
class Solution {
    const int MOD = 1e9 + 7;
public:
    int sumSubarrayMins(vector<int> &arr) {
        long ans = 0L;
        arr.push_back(-1);
        stack<int> st;
        st.push(-1); // 哨兵
        for (int r = 0; r < arr.size(); ++r) {
            while (st.size() > 1 && arr[st.top()] >= arr[r]) {
                int i = st.top();
                st.pop();
                ans += (long) arr[i] * (i - st.top()) * (r - i); // 累加贡献
            }
            st.push(r);
        }
        return ans % MOD;
    }
};
```

```go [sol3-Go]
func sumSubarrayMins(arr []int) (ans int) {
    arr = append(arr, -1)
    st := []int{-1} // 哨兵
    for r, x := range arr {
        for len(st) > 1 && arr[st[len(st)-1]] >= x {
            i := st[len(st)-1]
            st = st[:len(st)-1]
            ans += arr[i] * (i - st[len(st)-1]) * (r - i) // 累加贡献
        }
        st = append(st, r)
    }
    return ans % (1e9 + 7)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。虽然我们写了个二重循环，但站在 $\textit{arr}[i]$ 的视角看，$i$ 在二重循环中最多入栈出栈各一次，因此整个二重循环的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，单调栈里面有 $\mathcal{O}(n)$ 个元素。 

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

本题来自 `三、贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
