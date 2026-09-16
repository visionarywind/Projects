# 1124. 表现良好的最长时间段

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-well-performing-interval/
- 题目 slug：`longest-well-performing-interval`
- 来源专题：单调栈
- 来源分类路径：一、单调栈 / §1.2 进阶
- 难度分：1908
- 外部题解来源：https://leetcode.cn/problems/longest-well-performing-interval/solutions/2110211/liang-chong-zuo-fa-liang-zhang-tu-miao-d-hysl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法，两张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-well-performing-interval/solutions/2110211/liang-chong-zuo-fa-liang-zhang-tu-miao-d-hysl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-zuo-fa-liang-zhang-tu-miao-d-hysl`
- topic id：`2110211`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

# 前置知识：前缀和

对于数组 $\textit{nums}$，定义它的前缀和 $\textit{s}[0]=0$，$\textit{s}[i+1] = \sum\limits_{j=0}^{i}\textit{nums}[j]$。

例如 $\textit{nums}=[1,2,-1,2]$，对应的前缀和数组为 $s=[0,1,3,2,4]$。

通过前缀和，我们可以把**子数组的元素和转换成两个前缀和的差**，即

$$
\sum_{j=\textit{left}}^{\textit{right}}\textit{nums}[j] = \sum\limits_{j=0}^{\textit{right}}\textit{nums}[j] - \sum\limits_{j=0}^{\textit{left}-1}\textit{nums}[j] = \textit{s}[\textit{right}+1] - \textit{s}[\textit{left}]
$$

例如 $\textit{nums}$ 的子数组 $[2,-1,2]$ 的和就可以用 $s[4]-s[1]=4-1=3$ 算出来。

> 注：为方便计算，常用左闭右开区间 $[\textit{left},\textit{right})$ 来表示子数组，此时子数组的和为 $\textit{s}[\textit{right}] - \textit{s}[\textit{left}]$，子数组的长度为 $\textit{right}-\textit{left}$。

# 方法一：单调栈

先把问题转换到我们熟悉的东西上。

「劳累天数大于不劳累天数」等价于「劳累天数减去不劳累天数大于 $0$」。

那么把劳累的一天视作 $\textit{nums}[i]=1$，不劳累的一天视作 $\textit{nums}[i]=-1$，则问题变为：

计算 $\textit{nums}$ 的最长子数组，其元素和大于 $0$。

既然说到了「子数组的元素和」，那么利用前缀和 $s$，将问题变为：

找到两个下标 $i$ 和 $j$，满足 $j<i$ 且 $s[j]<s[i]$，最大化 $i-j$ 的值。

想一想，哪些值可以作为 $j$（最长子数组的左端点）呢？

![1124-3.png](https://pic.leetcode.cn/1676294271-XVWmuP-1124-3.png)

### 答疑

**问**：我怎么觉得这题和我做过的单调栈题目不太一样呢？~~没法快乐套模板~~

**答**：通常的单调栈题目（例如 [496. 下一个更大元素 I](https://leetcode.cn/problems/next-greater-element-i/)）都是求的「最近」或「最短」，本题求的是「最长」，不能一概而论。~~就当成是新的模板~~

```py [sol1-Python3]
class Solution:
    def longestWPI(self, hours: List[int]) -> int:
        n = len(hours)
        s = [0] * (n + 1)  # 前缀和
        st = [0]  # s[0]
        for j, h in enumerate(hours, 1):
            s[j] = s[j - 1] + (1 if h > 8 else -1)
            if s[j] < s[st[-1]]: st.append(j)  # 感兴趣的 j
        ans = 0
        for i in range(n, 0, -1):
            while st and s[i] > s[st[-1]]:
                ans = max(ans, i - st.pop())  # [st[-1],i) 可能是最长子数组
        return ans
```

```java [sol1-Java]
class Solution {
    public int longestWPI(int[] hours) {
        int n = hours.length, ans = 0;
        var s = new int[n + 1]; // 前缀和
        var st = new ArrayDeque<Integer>();
        st.push(0); // s[0]
        for (int j = 1; j <= n; ++j) {
            s[j] = s[j - 1] + (hours[j - 1] > 8 ? 1 : -1);
            if (s[j] < s[st.peek()]) st.push(j); // 感兴趣的 j
        }
        for (int i = n; i > 0; --i)
            while (!st.isEmpty() && s[i] > s[st.peek()])
                ans = Math.max(ans, i - st.pop()); // [栈顶,i) 可能是最长子数组
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int longestWPI(vector<int> &hours) {
        int n = hours.size(), ans = 0, s[n + 1]; // 前缀和
        stack<int> st;
        st.push(s[0] = 0);
        for (int j = 1; j <= n; ++j) {
            s[j] = s[j - 1] + (hours[j - 1] > 8 ? 1 : -1);
            if (s[j] < s[st.top()]) st.push(j); // 感兴趣的 j
        }
        for (int i = n; i; --i)
            while (!st.empty() && s[i] > s[st.top()]) {
                ans = max(ans, i - st.top()); // [栈顶,i) 可能是最长子数组
                st.pop();
            }
        return ans;
    }
};
```

```go [sol1-Go]
func longestWPI(hours []int) (ans int) {
    n := len(hours)
    s := make([]int, n+1) // 前缀和
    st := []int{0} // s[0]
    for j, h := range hours {
        j++
        s[j] = s[j-1]
        if h > 8 {
            s[j]++
        } else {
            s[j]--
        }
        if s[j] < s[st[len(st)-1]] {
            st = append(st, j) // 感兴趣的 j
        }
    }
    for i := n; i > 0; i-- {
        for len(st) > 0 && s[i] > s[st[len(st)-1]] {
            ans = max(ans, i-st[len(st)-1]) // [栈顶,i) 可能是最长子数组
            st = st[:len(st)-1]
        }
    }
    return
}

func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{hours}$ 的长度。注意每个元素至多入栈出栈各一次，因此二重循环的时间复杂度是 $O(n)$ 的。
- 空间复杂度：$O(n)$。

# 方法二：利用前缀和的连续性

虽说方法一更加通用，不过利用 $\textit{nums}$ 中只有 $1$ 和 $-1$ 的特点，可以做到**一次遍历**。

考虑 $s[i]$：

- 如果 $s[i]>0$，那么 $j=0$ 就是最远的左端点，因为 $s[0]=0$，故 $s[i]-s[0]=s[i]>0$，符合要求。
- 如果 $s[i]\le 0$，那么 $j$ 就是 $s[i]-1$ 首次出现的位置。为什么是 $s[i]-1$ 而不是其它更小的数？这是因为前缀和是从 $0$ 开始的，由于 $\textit{nums}$ 中只有 $1$ 和 $-1$，那么相邻前缀和的差都恰好为 $1$，要想算出比 $s[i]-1$ 更小的数，必然会先算出 $s[i]-1$，那么这些更小数必然在 $s[i]-1$ 首次出现的位置的**右边**。

![1124-2-1.png](https://pic.leetcode.cn/1676295694-YLrKeo-1124-2-1.png)

代码实现时，可以用哈希表记录每个 $s[i]$ 首次出现的下标。

不过，由于我们只需要考虑值在闭区间 $[-n,0]$ 内的前缀和，用数组记录是更加高效的。同时，为了避免用负数访问数组，可以在计算过程中把前缀和取反。

```py [sol2-Python3]
class Solution:
    def longestWPI(self, hours: List[int]) -> int:
        pos = [0] * (len(hours) + 2)  # 记录前缀和首次出现的位置
        ans = s = 0
        for i, h in enumerate(hours, 1):
            s -= 1 if h > 8 else -1  # 取反，改为减法
            if s < 0:
                ans = i
            else:
                if pos[s + 1]:  # 原本是 s-1，取反改为 s+1
                    ans = max(ans, i - pos[s + 1])  # 这里手写 if 会更快
                if pos[s] == 0:
                    pos[s] = i
        return ans
```

```java [sol2-Java]
class Solution {
    public int longestWPI(int[] hours) {
        int n = hours.length, ans = 0, s = 0;
        var pos = new int[n + 2]; // 记录前缀和首次出现的位置
        for (int i = 1; i <= n; ++i) {
            s -= hours[i - 1] > 8 ? 1 : -1; // 取反，改为减法
            if (s < 0) ans = i;
            else {
                if (pos[s + 1] > 0) ans = Math.max(ans, i - pos[s + 1]);
                if (pos[s] == 0) pos[s] = i;
            }
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int longestWPI(vector<int> &hours) {
        int n = hours.size(), ans = 0, s = 0, pos[n + 2]; // 记录前缀和首次出现的位置
        memset(pos, 0, sizeof(pos));
        for (int i = 1; i <= n; ++i) {
            hours[i - 1] > 8 ? --s : ++s; // 所有和前缀和相关的都取反
            if (s < 0) ans = i;
            else {
                if (pos[s + 1]) ans = max(ans, i - pos[s + 1]);
                if (pos[s] == 0) pos[s] = i;
            }
        }
        return ans;
    }
};
```

```go [sol2-Go]
func longestWPI(hours []int) (ans int) {
    pos := make([]int, len(hours)+2) // 记录前缀和首次出现的位置
    s := 0 // 前缀和
    for i, h := range hours {
        i++
        if h > 8 {
            s-- // 所有和前缀和相关的都取反
        } else {
            s++
        }
        if s < 0 {
            ans = i
        } else {
            if pos[s+1] > 0 { // 原本是 s-1，取反改为 s+1
                ans = max(ans, i-pos[s+1])
            }
            if pos[s] == 0 {
                pos[s] = i
            }
        }
    }
    return
}

func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{hours}$ 的长度。
- 空间复杂度：$O(n)$。

### 相似题目

单调栈和单调队列背后的思想是类似的，本题求的是「最长」，而 [862. 和至少为 K 的最短子数组](https://leetcode.cn/problems/shortest-subarray-with-sum-at-least-k/) 求的是「最短」。

通过比较这两题的区别，相信各位能对单调栈和单调队列有一个更加深刻的认识。具体请看 [两张图秒懂单调队列](https://leetcode.cn/problems/shortest-subarray-with-sum-at-least-k/solution/liang-zhang-tu-miao-dong-dan-diao-dui-li-9fvh/)。我在这篇题解的末尾附上了大量单调栈和单调队列的题目，供各位练习。

另外，[962. 最大宽度坡](https://leetcode.cn/problems/maximum-width-ramp/) 和本题非常像，你可以用方法一的代码秒杀。~~双倍经验~~

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
