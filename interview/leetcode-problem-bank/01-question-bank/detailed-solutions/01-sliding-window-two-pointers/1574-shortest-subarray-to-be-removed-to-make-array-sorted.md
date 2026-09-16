# 1574. 删除最短的子数组使剩余数组有序

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-subarray-to-be-removed-to-make-array-sorted/
- 题目 slug：`shortest-subarray-to-be-removed-to-make-array-sorted`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.3 同向双指针
- 难度分：1932
- 外部题解来源：https://leetcode.cn/problems/shortest-subarray-to-be-removed-to-make-array-sorted/solutions/2189149/dong-hua-yi-xie-jiu-cuo-liang-chong-xie-iijwz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】两种双指针写法（Python/Java/C++/Go）](https://leetcode.cn/problems/shortest-subarray-to-be-removed-to-make-array-sorted/solutions/2189149/dong-hua-yi-xie-jiu-cuo-liang-chong-xie-iijwz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-hua-yi-xie-jiu-cuo-liang-chong-xie-iijwz`
- topic id：`2189149`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分析

去掉子数组后，需要满足：

1. 剩余的前缀必须是递增的（允许相邻元素相等）。
2. 剩余的后缀必须是递增的（允许相邻元素相等）。
3. 此外，剩余前缀的最后一个数必须小于等于剩余后缀的第一个数。

## 方法一：枚举左端点，移动右端点

**核心思路**：枚举 $\textit{left}$，增大 $\textit{right}$ 直到 $\textit{arr}[\textit{left}]\le\textit{arr}[\textit{right}]$，此时更新子数组长度的最小值。

<![1574-1-1.png](https://pic.leetcode.cn/1679668011-zwxpOg-1574-1-1.png),![1574-1-2.png](https://pic.leetcode.cn/1679668011-ZQZjdj-1574-1-2.png),![1574-1-3.png](https://pic.leetcode.cn/1679668011-hosbtm-1574-1-3.png),![1574-1-4.png](https://pic.leetcode.cn/1679668011-lJHOkO-1574-1-4.png),![1574-1-5.png](https://pic.leetcode.cn/1679668011-HKzuIa-1574-1-5.png),![1574-1-6.png](https://pic.leetcode.cn/1679668011-BvlOQV-1574-1-6.png),![1574-1-7.png](https://pic.leetcode.cn/1679668011-AwSybW-1574-1-7.png)>

#### 答疑

**问**：为什么枚举一个新的 $\textit{left}$ 时，$\textit{right}$ 不会往左移？或者说，是否需要再次枚举之前枚举过的 $\textit{arr}[\textit{right}]$？

**答**：在向右移动时，由于 $\textit{arr}[\textit{left}]$ 和 $\textit{arr}[\textit{right}]$ 都是非递减的，所以 $\textit{right}$ 左侧之前枚举过的元素必然小于 $\textit{arr}[\textit{left}]$，无需再次枚举。这也是本题可以使用同向双指针（不定长滑动窗口）的前提。

**问**：在计算子数组长度时，我经常分不清下标是否要 $+1$ 或 $-1$，请问如何解决？

**答**：第一，时刻把握住 $\textit{left}$ 和 $\textit{right}$ 的含义，对于本题来说是开区间 $(\textit{left},\textit{right})$，这两个指针指向的元素不能删除。第二，可以代入一些数据来验证，比如代入 $\textit{left}=1,\textit{right}=3$，此时只需要删除**一个** $\textit{arr}[2]$，所以公式 $\textit{right}-\textit{left}-1$ 才是符合要求的。

**问**：为什么不用判断 $\textit{left}<\textit{right}$，难道不会出现 $\textit{left}\ge\textit{right}$ 的情况吗？

**答**：由于提前判断了 $\textit{arr}$ 是非递减数组的情况，后面的循环 $\textit{left}$ 必定小于 $\textit{right}$。反证：如果某个时刻 $\textit{left}$ 达到了 $\textit{right}$，就说明整个数组是有序的，但这种情况已经提前判断了。

**问**：能不能先把 $\textit{left}$ 的最大值算出来，然后再去枚举 $\textit{left}$ 或 $\textit{right}$？

**答**：可以。根据对称性，这种做法和先算 $\textit{right}$ 的最小值的做法是一样的，只不过枚举的顺序相反而已。

```py [sol1-Python3]
class Solution:
    def findLengthOfShortestSubarray(self, arr: List[int]) -> int:
        n = len(arr)
        right = n - 1
        while right and arr[right - 1] <= arr[right]:
            right -= 1
        if right == 0:  # arr 已经是非递减数组
            return 0
        # 此时 arr[right-1] > arr[right]
        ans = right  # 删除 arr[:right]
        left = 0  # 枚举 left
        while left == 0 or arr[left - 1] <= arr[left]:
            while right < n and arr[right] < arr[left]:
                right += 1
            # 此时 arr[left] <= arr[right]，删除 arr[left+1:right]
            ans = min(ans, right - left - 1)
            left += 1
        return ans
```

```java [sol1-Java]
class Solution {
    public int findLengthOfShortestSubarray(int[] arr) {
        int n = arr.length, right = n - 1;
        while (right > 0 && arr[right - 1] <= arr[right])
            --right;
        if (right == 0) return 0; // arr 已经是非递减数组
        // 此时 arr[right-1] > arr[right]
        int ans = right; // 删除 0 到 right-1
        for (int left = 0; left == 0 || arr[left - 1] <= arr[left]; ++left) {
            while (right < n && arr[right] < arr[left])
                ++right;
            // 此时 arr[left] <= arr[right]，从 left+1 到 right-1 可以删除
            ans = Math.min(ans, right - left - 1);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int findLengthOfShortestSubarray(vector<int> &arr) {
        int n = arr.size(), right = n - 1;
        while (right && arr[right - 1] <= arr[right])
            --right;
        if (right == 0) return 0; // arr 已经是非递减数组
        // 此时 arr[right-1] > arr[right]
        int ans = right; // 删除 0 到 right-1
        for (int left = 0; left == 0 || arr[left - 1] <= arr[left]; ++left) {
            while (right < n && arr[right] < arr[left])
                ++right;
            // 此时 arr[left] <= arr[right]，从 left+1 到 right-1 可以删除
            ans = min(ans, right - left - 1);
        }
        return ans;
    }
};
```

```go [sol1-Go]
func findLengthOfShortestSubarray(arr []int) int {
    n := len(arr)
    right := n - 1
    for right > 0 && arr[right-1] <= arr[right] {
        right--
    }
    if right == 0 { // arr 已经是非递减数组
        return 0
    }
    // 此时 arr[right-1] > arr[right]
    ans := right // 删除 arr[:right]
    for left := 0; left == 0 || arr[left-1] <= arr[left]; left++ {
        for right < n && arr[right] < arr[left] {
            right++
        }
        ans = min(ans, right-left-1) // 删除 arr[left+1:right]
    }
    return ans
}
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{right}$ 加一的**总**执行次数不会超过 $n$ 次，所以总的时间复杂度为 $O(n)$。
- 空间复杂度：$O(1)$。

## 方法二：枚举右端点，移动左端点

核心思路：枚举 $\textit{right}$，增大 $\textit{left}$ 直到 $\textit{arr}[\textit{left}]>\textit{arr}[\textit{right}]$。在增大过程中去更新子数组长度的最小值。

<![1574-2-1.png](https://pic.leetcode.cn/1679668016-oXiiDv-1574-2-1.png),![1574-2-2.png](https://pic.leetcode.cn/1679668016-PvvbiN-1574-2-2.png),![1574-2-3.png](https://pic.leetcode.cn/1679668016-jmpvYh-1574-2-3.png),![1574-2-4.png](https://pic.leetcode.cn/1679668016-HKMsiU-1574-2-4.png),![1574-2-5.png](https://pic.leetcode.cn/1679668016-iuArrU-1574-2-5.png),![1574-2-6.png](https://pic.leetcode.cn/1679668016-IPrstM-1574-2-6.png),![1574-2-7.png](https://pic.leetcode.cn/1679668016-mQSJdI-1574-2-7.png),![1574-2-8.png](https://pic.leetcode.cn/1679668016-FAMJBf-1574-2-8.png)>

#### 答疑

**问**：为什么枚举一个新的 $\textit{right}$ 时，$\textit{left}$ 不会往左移？或者说，是否需要再次枚举之前枚举过的 $\textit{arr}[\textit{left}]$？

**答**：在向右移动时，由于 $\textit{arr}[\textit{left}]$ 和 $\textit{arr}[\textit{right}]$ 都是非递减的，所以 $\textit{left}$ 左侧之前枚举过的元素必然小于等于 $\textit{arr}[\textit{right}]$，由于这样的子数组长度更长，无需再次枚举。这也是本题可以使用同向双指针（不定长滑动窗口）的前提。

**问**：为什么循环一定会结束？

**答**：代码中提前判断了 $\textit{arr}$ 已经是非递减数组的情况，所以后面的循环一定存在 $\textit{left}$，使得 $\textit{arr}[\textit{left}]>\textit{arr}[\textit{left}+1]$ 成立。

> 注：最坏情况下，当 $\textit{right}=n$ 时才会去移动 $\textit{left}$。

```py [sol2-Python3]
class Solution:
    def findLengthOfShortestSubarray(self, arr: List[int]) -> int:
        n = len(arr)
        right = n - 1
        while right and arr[right - 1] <= arr[right]:
            right -= 1
        if right == 0:  # arr 已经是非递减数组
            return 0
        # 此时 arr[right-1] > arr[right]
        ans = right  # 删除 arr[:right]
        left = 0
        while True:  # 枚举 right
            while right == n or arr[left] <= arr[right]:
                ans = min(ans, right - left - 1)  # 删除 arr[left+1:right]
                if arr[left] > arr[left + 1]:
                    return ans
                left += 1
            right += 1
```

```java [sol2-Java]
class Solution {
    public int findLengthOfShortestSubarray(int[] arr) {
        int n = arr.length, right = n - 1;
        while (right > 0 && arr[right - 1] <= arr[right])
            --right;
        if (right == 0) return 0; // arr 已经是非递减数组
        // 此时 arr[right-1] > arr[right]
        int ans = right; // 删除 0 到 right-1
        for (int left = 0; ; ++right) // 枚举 right
            while (right == n || arr[left] <= arr[right]) {
                // 中间 left+1 到 right-1 可以删除
                ans = Math.min(ans, right - left - 1);
                if (arr[left] > arr[left + 1]) return ans;
                ++left;
            }
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int findLengthOfShortestSubarray(vector<int> &arr) {
        int n = arr.size(), right = n - 1;
        while (right && arr[right - 1] <= arr[right])
            --right;
        if (right == 0) return 0; // arr 已经是非递减数组
        // 此时 arr[right-1] > arr[right]
        int ans = right; // 删除 0 到 right-1
        for (int left = 0; ; ++right) // 枚举 right
            while (right == n || arr[left] <= arr[right]) {
                // 中间 left+1 到 right-1 可以删除
                ans = min(ans, right - left - 1);
                if (arr[left] > arr[left + 1]) return ans;
                ++left;
            }
    }
};
```

```go [sol2-Go]
func findLengthOfShortestSubarray(arr []int) int {
    n := len(arr)
    right := n - 1
    for right > 0 && arr[right-1] <= arr[right] {
        right--
    }
    if right == 0 { // arr 已经是非递减数组
        return 0
    }
    // 此时 arr[right-1] > arr[right]
    ans := right // 删除 arr[:right]
    for left := 0; ; right++ { // 枚举 right
        for right == n || arr[left] <= arr[right] {
            ans = min(ans, right-left-1) // 删除 arr[left+1:right]
            if arr[left] > arr[left+1] {
                return ans
            }
            left++
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $n$ 次，所以总的时间复杂度为 $O(n)$。
- 空间复杂度：$O(1)$。

这两种写法，你更喜欢哪种呢？本题还有其它实现方案，欢迎在评论区发表你的写法。

## 思考题（变形题）

**问**：至少修改多少个数，使得修改后的数组是非递减的？（等价于删除一个最短的**子序列**）

**答**：先计算最长非递减子序列，再用 $n$ 减去这个子序列的长度，就得到了删除子序列的最短长度。具体见[【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)。

**问**：至少修改多少个数，使得修改后的数组是**严格递增**的？

**答**：严格递增等价于对于任意 $i>j$，有

$$
\textit{arr}[i]-\textit{arr}[j]\ge i-j
$$

变形得 

$$
\textit{arr}[i]-i\ge \textit{arr}[j]-j
$$

故构造 $b[i]=\textit{arr}[i]-i$，求 $b$ 的最长非递减子序列，再用 $n$ 减去这个子序列的长度，就得到了答案。

为什么不能直接求 $\textit{arr}$ 的最长严格递增子序列？试试 $\textit{arr}=[1,2,2,3]$，必须修改 $2$ 个数才能变成严格递增数组。

> 注意在严格递增情况下，修改和删除不是等价的。

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

本题来自 `三、单序列双指针 / §3.3 同向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.3 同向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
