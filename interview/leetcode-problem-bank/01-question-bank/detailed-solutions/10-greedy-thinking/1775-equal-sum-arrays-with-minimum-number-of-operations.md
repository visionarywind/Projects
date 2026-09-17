# 1775. 通过最少操作次数使数组的和相等

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/equal-sum-arrays-with-minimum-number-of-operations/
- 题目 slug：`equal-sum-arrays-with-minimum-number-of-operations`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1850
- 外部题解来源：https://leetcode.cn/problems/equal-sum-arrays-with-minimum-number-of-operations/solutions/2009786/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-ocuu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一个动画秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/equal-sum-arrays-with-minimum-number-of-operations/solutions/2009786/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-ocuu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-ocuu`
- topic id：`2009786`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

<![1775-2-1.png](https://pic.leetcode.cn/1670310507-OiJYJT-1775-2-1.png),![1775-2-2.png](https://pic.leetcode.cn/1670310507-KAyzey-1775-2-2.png),![1775-2-3.png](https://pic.leetcode.cn/1670310507-jPVURx-1775-2-3.png),![1775-2-4.png](https://pic.leetcode.cn/1670372581-UmZRAr-1775-2-4.png),![1775-3-5.png](https://pic.leetcode.cn/1670311068-PFFLrS-1775-3-5.png)>

#### 详细算法流程

不妨设 $\textit{nums}_1$ 的元素和小于 $\textit{nums}_2$ 的元素和（如果不是则交换这两数组），元素和的差为 $d$。

那么 $\textit{nums}_1$ 的元素需要变大，$\textit{nums}_2$ 的元素需要变小。

计算每个元素的**最大变化量**：

- $\textit{nums}_1[i]$ 最大能变成 $6$，最大变化量为 $6-\textit{nums}_1[i]$；
- $\textit{nums}_2[i]$ 最小能变成 $1$，最大变化量为 $\textit{nums}_2[i]-1$。

统计这些变化量的个数，记到一个哈希表或长为 $6$ 的数组 $\textit{cnt}$ 中，也就是有 $\textit{cnt}[i]$ 个数可以使 $d$ 减少 $i$。

那么从大到小枚举 $i=5,4,3,2,1$：

- 如果 $d > i\cdot\textit{cnt}[i]$，那么应该把这 $\textit{cnt}[i]$ 个数的变化量拉满，并更新 $d$ 为 $d-i\cdot\textit{cnt}[i]$；
- 否则，可以通过修改其中的 $\left\lceil\dfrac{d}{i}\right\rceil$ 个数，使 $d$ 恰好为 $0$，退出循环。

累加需要修改的数的个数，即为答案。如果无法使 $d=0$，返回 $-1$。

#### 优化

假设 $\textit{nums}_1$ 的元素和小于 $\textit{nums}_2$ 的元素和。

把 $\textit{nums}_1$ 的所有数都改成 $6$，$\textit{nums}_2$ 的所有数都改成 $1$，如果 $\textit{nums}_1$ 的元素和仍然小于 $\textit{nums}_2$ 的元素和，则说明无论怎么操作，都无法使这两个数组的元素和相等。

对于 $\textit{nums}_1$ 的元素和大于 $\textit{nums}_2$ 的元素和的情况，也同理。

因此，设 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度，我们可以在一开始就判断下：如果 $6n<m$ 或 $6m<n$，则直接返回 $-1$。否则，一定可以使两个数组的和相等，这是因为从「$\textit{nums}_1$ 的元素和小于 $\textit{nums}_2$ 的元素和」变到「$\textit{nums}_1$ 的元素和大于等于 $\textit{nums}_2$ 的元素和」，由于元素值可以变成 $[1,6]$ 中的任意值，我们可以每次操作只把一个元素增大 $1$ 或减小 $1$，这样必然会遇到元素和相差为 $0$ 的情况。

```py [sol1-Python3]
class Solution:
    def minOperations(self, nums1: List[int], nums2: List[int]) -> int:
        if 6 * len(nums1) < len(nums2) or 6 * len(nums2) < len(nums1):
            return -1  # 优化
        d = sum(nums2) - sum(nums1)  # 数组元素和的差，我们要让这个差变为 0
        if d < 0:
            d = -d
            nums1, nums2 = nums2, nums1  # 统一让 nums1 的数变大，nums2 的数变小
        ans = 0
        # 统计每个数的最大变化量（nums1 的变成 6，nums2 的变成 1）
        cnt = Counter(6 - x for x in nums1) + Counter(x - 1 for x in nums2)
        for i in range(5, 0, -1):  # 从大到小枚举最大变化量 5 4 3 2 1
            if i * cnt[i] >= d:  # 可以让 d 变为 0
                return ans + (d + i - 1) // i
            ans += cnt[i]  # 需要所有最大变化量为 i 的数
            d -= i * cnt[i]
```

```java [sol1-Java]
class Solution {
    public int minOperations(int[] nums1, int[] nums2) {
        if (6 * nums1.length < nums2.length || 6 * nums2.length < nums1.length)
            return -1; // 优化
        // int d = Arrays.stream(nums2).sum() - Arrays.stream(nums1).sum();
        int d = 0; // 数组元素和的差，我们要让这个差变为 0
        for (int x : nums2) d += x;
        for (int x : nums1) d -= x;
        if (d < 0) {
            d = -d;
            int[] tmp = nums1;
            nums1 = nums2;
            nums2 = tmp; // 交换，统一让 nums1 的数变大，nums2 的数变小
        }
        int[] cnt = new int[6]; // 统计每个数的最大变化量
        for (int x : nums1) ++cnt[6 - x]; // nums1 的变成 6
        for (int x : nums2) ++cnt[x - 1]; // nums2 的变成 1
        for (int i = 5, ans = 0; ; --i) { // 从大到小枚举最大变化量 5 4 3 2 1
            if (i * cnt[i] >= d) // 可以让 d 变为 0
                return ans + (d + i - 1) / i;
            ans += cnt[i]; // 需要所有最大变化量为 i 的数
            d -= i * cnt[i];
        }
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minOperations(vector<int> &nums1, vector<int> &nums2) {
        if (6 * nums1.size() < nums2.size() || 6 * nums2.size() < nums1.size())
            return -1; // 优化
        int d = accumulate(nums2.begin(), nums2.end(), 0) - accumulate(nums1.begin(), nums1.end(), 0);
        if (d < 0) {
            d = -d;
            swap(nums1, nums2); // 统一让 nums1 的数变大，nums2 的数变小
        }
        int cnt[6]{}; // 统计每个数的最大变化量
        for (int x: nums1) ++cnt[6 - x]; // nums1 的变成 6
        for (int x: nums2) ++cnt[x - 1]; // nums2 的变成 1
        for (int i = 5, ans = 0;; --i) { // 从大到小枚举最大变化量 5 4 3 2 1
            if (i * cnt[i] >= d) // 可以让 d 变为 0
                return ans + (d + i - 1) / i;
            ans += cnt[i]; // 需要所有最大变化量为 i 的数
            d -= i * cnt[i];
        }
    }
};
```

```go [sol1-Go]
func minOperations(nums1, nums2 []int) (ans int) {
    if 6*len(nums1) < len(nums2) || 6*len(nums2) < len(nums1) {
        return -1 // 优化
    }
    d := 0 // 数组元素和的差，我们要让这个差变为 0
    for _, x := range nums2 { d += x }
    for _, x := range nums1 { d -= x }
    if d < 0 {
        d = -d
        nums1, nums2 = nums2, nums1 // 统一让 nums1 的数变大，nums2 的数变小
    }
    cnt := [6]int{} // 统计每个数的最大变化量
    for _, x := range nums1 { cnt[6-x]++ } // nums1 的变成 6
    for _, x := range nums2 { cnt[x-1]++ } // nums2 的变成 1
    for i := 5; ; i-- { // 从大到小枚举最大变化量 5 4 3 2 1
        if i*cnt[i] >= d { // 可以让 d 变为 0
            return ans + (d+i-1)/i
        }
        ans += cnt[i] // 需要所有最大变化量为 i 的数
        d -= i * cnt[i]
    }
}
```

#### 复杂度分析

- 时间复杂度：$O(n+m)$，其中 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度。
- 空间复杂度：$O(C)$。本题 $C=6$。

---

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)，最近正在连载 [【基础算法精讲】](https://www.bilibili.com/video/BV1AP41137w7/)，欢迎收看~

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
