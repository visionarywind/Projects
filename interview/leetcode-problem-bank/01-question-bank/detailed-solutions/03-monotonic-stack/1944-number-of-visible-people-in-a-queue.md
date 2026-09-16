# 1944. 队列中可以看到的人数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-visible-people-in-a-queue/
- 题目 slug：`number-of-visible-people-in-a-queue`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：2105
- 外部题解来源：https://leetcode.cn/problems/number-of-visible-people-in-a-queue/solutions/2591558/dan-diao-zhan-de-ben-zhi-ji-shi-qu-diao-8tp3s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[单调栈的本质：及时去掉无用数据（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/number-of-visible-people-in-a-queue/solutions/2591558/dan-diao-zhan-de-ben-zhi-ji-shi-qu-diao-8tp3s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dan-diao-zhan-de-ben-zhi-ji-shi-qu-diao-8tp3s`
- topic id：`2591558`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 前置知识

请看视频讲解：[单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)

### 思路

为方便描述，下文将 $\textit{heights}$ 简记为 $h$。

如果暴力枚举，对于每个 $i$，我们需要遍历 $i$ 右边所有的 $j$，一个一个地判断是否满足题目要求。

怎么优化？如果某个 $j$ 一定不会被 $i$ 看到，我们就不需要去遍历这样的 $j$ 了。根据题目要求，如果在 $i$ 和 $j$ 之间，存在一个 $k$，使得 $h[k]>h[j]$，那么 $j$ 就一定不会被 $i$ 看到。注意题目保证所有元素互不相同，无需处理身高相等的情况。

从右往左遍历 $h$，如果发现一个 $h[k]$ 比右边的数 $h[j]$ 大，那么 $h[k]$ 就把 $h[j]$「挡住」，$k$ 左边的人**再也不会**看到 $j$ 了。

这启发我们用一个数据结构（栈）维护**没有被挡住**的人的身高。对于那些被挡住的人，永远不会去遍历他们了（及时去掉无用数据）。

看示例 1：

| $i$ | $h[i]$ | 入栈前  | 入栈后  | $\textit{ans}[i]$  |解释|
|---|:---:|---|---|:---:|---|
| $5$ | $9$  | $[]$  | $[9]$   | $0$  ||
| $4$ | $11$  |  $[9]$ |  $[11]$ | $1$  |$11$ 挡住 $9$|
| $3$ | $5$  | $[11]$  | $[11,5]$  | $1$  ||
| $2$ | $8$  | $[11,5]$  | $[11,8]$  | $2$  |$8$ 挡住 $5$|
| $1$ | $6$  |  $[11,8]$ | $[11,8,6]$  | $1$  ||
| $0$ | $10$  |  $[11,8,6]$ | $[11,10]$  | $3$  |$10$ 挡住 $8,6$|

从右往左遍历到 $i$ 时，如果栈顶比 $h[i]$ 小，就不断弹出栈顶，直到栈为空或者栈顶大于 $h[i]$。统计弹出的元素个数，作为 $i$ 能看到的人数 $\textit{ans}[i]$。出栈结束后，若栈不为空，说明第 $i$ 个人还可以再看到一个人（栈顶），把 $\textit{ans}[i]$ 加一。

对于比栈顶更靠右的人，由于栈顶比 $h[i]$ 大，根据题目要求，$i$ 无法看到比栈顶更靠右的人，所以无需遍历这些更靠右的人。

```py [sol-Python3]
class Solution:
    def canSeePersonsCount(self, heights: List[int]) -> List[int]:
        n = len(heights)
        ans = [0] * n
        st = []
        for i in range(n - 1, -1, -1):
            while st and st[-1] < heights[i]:
                st.pop()
                ans[i] += 1
            if st:  # 还可以再看到一个人
                ans[i] += 1
            st.append(heights[i])
        return ans
```

```java [sol-Java]
// 更快的写法见数组版本
class Solution {
    public int[] canSeePersonsCount(int[] heights) {
        int n = heights.length;
        int[] ans = new int[n];
        Deque<Integer> st = new ArrayDeque<>();
        for (int i = n - 1; i >= 0; i--) {
            while (!st.isEmpty() && st.peek() < heights[i]) {
                st.pop();
                ans[i]++;
            }
            if (!st.isEmpty()) { // 还可以再看到一个人
                ans[i]++;
            }
            st.push(heights[i]);
        }
        return ans;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int[] canSeePersonsCount(int[] heights) {
        int n = heights.length;
        int[] ans = new int[n];
        int[] st = new int[n];
        int top = -1;
        for (int i = n - 1; i >= 0; i--) {
            while (top >= 0 && st[top] < heights[i]) {
                top--;
                ans[i]++;
            }
            if (top >= 0) { // 还可以再看到一个人
                ans[i]++;
            }
            st[++top] = heights[i];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> canSeePersonsCount(vector<int>& heights) {
        int n = heights.size();
        vector<int> ans(n);
        stack<int> st;
        for (int i = n - 1; i >= 0; i--) {
            while (!st.empty() && st.top() < heights[i]) {
                st.pop();
                ans[i]++;
            }
            if (!st.empty()) { // 还可以再看到一个人
                ans[i]++;
            }
            st.push(heights[i]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func canSeePersonsCount(heights []int) []int {
    n := len(heights)
    ans := make([]int, n)
    st := []int{math.MaxInt} // 哨兵，下面不用判空
    for i := n - 1; i >= 0; i-- {
        for st[len(st)-1] < heights[i] {
            st = st[:len(st)-1]
            ans[i]++
        }
        if len(st) > 1 { // 还可以再看到一个人
            ans[i]++
        }
        st = append(st, heights[i])
    }
    return ans
}
```

```js [sol-JavaScript]
var canSeePersonsCount = function(heights) {
    const n = heights.length;
    const ans = Array(n).fill(0);
    const st = [];
    for (let i = n - 1; i >= 0; i--) {
        while (st.length && st[st.length - 1] < heights[i]) {
            st.pop();
            ans[i]++;
        }
        if (st.length) { // 还可以再看到一个人
            ans[i]++;
        }
        st.push(heights[i]);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_see_persons_count(heights: Vec<i32>) -> Vec<i32> {
        let n = heights.len();
        let mut ans = vec![0; n];
        let mut st = Vec::new();
        for (i, &h) in heights.iter().enumerate().rev() {
            while !st.is_empty() && *st.last().unwrap() < h {
                st.pop();
                ans[i] += 1;
            }
            if !st.is_empty() { // 还可以再看到一个人
                ans[i] += 1;
            }
            st.push(h);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $h$ 的长度。每个人至多入栈出栈各一次，所以二重循环的总循环次数是 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

[【题单】单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、单调栈 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、单调栈 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
