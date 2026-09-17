# 88. 合并两个有序数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/merge-sorted-array/
- 题目 slug：`merge-sorted-array`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.1 双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/merge-sorted-array/solutions/2385610/dao-xu-shuang-zhi-zhen-wei-shi-yao-dao-x-xxkp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[倒序双指针：为什么倒序就一定能避免覆盖？（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/merge-sorted-array/solutions/2385610/dao-xu-shuang-zhi-zhen-wei-shi-yao-dao-x-xxkp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dao-xu-shuang-zhi-zhen-wei-shi-yao-dao-x-xxkp`
- topic id：`2385610`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 错误的思路

如果从左往右地把 $\textit{nums}_2$ 合并到 $\textit{nums}_1$ 中，假设 $\textit{nums}_2[0] < \textit{nums}_1[0]$，那么 $\textit{nums}_2[0]$ 会直接覆盖掉 $\textit{nums}_1[0]$，这不是我们期望看到的。

## 正确的思路

如果从右往左地把 $\textit{nums}_2$ 合并到 $\textit{nums}_1$ 中，是否会发生错误的覆盖呢？我们来看几个例子：

- $\textit{nums}_1=[1,2,3,*,*,*], \textit{nums}_2=[4,5,6]$。这里我用 $*$ 表示可以填入的空位。在这个例子中，$\textit{nums}_2$ 可以直接填入 $\textit{nums}_1$ 后面的 $3$ 个空位，得到 $[1,2,3,4,5,6]$，没有任何错误覆盖。
- $\textit{nums}_1=[1,2,6,*,*,*], \textit{nums}_2=[3,4,5]$。这里 $\textit{nums}_1$ 中的 $6$ 是最大的，应当填入末尾。现在 $\textit{nums}_1=[1,2,*,*,*,6]$，注意 $\textit{nums}_1[2]$ 这个位置现在空出了。然后把 $\textit{nums}_2$ 中的数字填入空位，得到 $[1,2,3,4,5,6]$，没有任何错误覆盖。
- 上面的例子表明，把 $\textit{nums}_1$ 中的数字**移到另一个空位，又产生了一个新的空位，所以剩余空位的个数是不变的**，我们总是有空位可以让 $\textit{nums}_2$ 的数字填入，不会发生错误覆盖，这是如下算法正确的前提。

## 算法

1. 初始化三个指针 $p_1=m-1$ 指向 $\textit{nums}_1$ 的末尾，$p_2=n-1$ 指向 $\textit{nums}_2$ 的末尾，$p=m+n-1$ 指向合并后的数组末尾。
2. 不断比较 $\textit{nums}_1[p_1]$ 和 $\textit{nums}_2[p_2]$ 的大小，将较大的值放入 $\textit{nums}_1[p]$。如果 $p_1\ge 0$ 且 $\textit{nums}_1[p_1]$ 更大，那么放入后 $p_1$ 和 $p$ 减一，否则 $p_2$ 和 $p$ 减一。注意 $\textit{nums}_1[p_1]=\textit{nums}_2[p_2]$ 时放入谁都可以，不妨规定放入 $\textit{nums}_2[p_2]$。这样在数组元素都相等的情况下，只需要把 $\textit{nums}_2$ 的数据填入 $\textit{nums}_1$ 中，效率更高。
3. 循环直到 $p_2<0$，此时 $\textit{nums}_2$ 的所有元素均已填入 $\textit{nums}_1$。你可能会想，如果 $\textit{nums}_1$ 还有元素没有移动呢？注意到当 $\textit{nums}_2$ 都合并到 $\textit{nums}_1$ 时，$\textit{nums}_1$ 剩余未移动的元素，它要移动的目标位置就是它自己所处的位置，所以无需移动，合并完毕。这可以算作一个小优化，比如 $\textit{nums}_1=[1,2,3,*,*,*], \textit{nums}_2=[4,5,6]$，其实只要把 $\textit{nums}_2$ 的所有数都填入 $\textit{nums}_1$ 中，合并就已经结束了，即便此时 $p_1=2$ 仍然 $\ge 0$。

```py [sol-Python3]
class Solution:
    def merge(self, nums1: List[int], m: int, nums2: List[int], n: int) -> None:
        p1, p2, p = m - 1, n - 1, m + n - 1
        while p2 >= 0:  # nums2 还有要合并的元素
            # 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
            if p1 >= 0 and nums1[p1] > nums2[p2]:
                nums1[p] = nums1[p1]  # 填入 nums1[p1]
                p1 -= 1
            else:
                nums1[p] = nums2[p2]  # 填入 nums2[p2]
                p2 -= 1
            p -= 1  # 下一个要填入的位置
```

```java [sol-Java]
class Solution {
    public void merge(int[] nums1, int m, int[] nums2, int n) {
        int p1 = m - 1;
        int p2 = n - 1;
        int p = m + n - 1;
        while (p2 >= 0) { // nums2 还有要合并的元素
            // 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
            if (p1 >= 0 && nums1[p1] > nums2[p2]) {
                nums1[p--] = nums1[p1--]; // 填入 nums1[p1]
            } else {
                nums1[p--] = nums2[p2--]; // 填入 nums2[p2]
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
        int p1 = m - 1, p2 = n - 1, p = m + n - 1;
        while (p2 >= 0) { // nums2 还有要合并的元素
            // 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
            if (p1 >= 0 && nums1[p1] > nums2[p2]) {
                nums1[p--] = nums1[p1--]; // 填入 nums1[p1]
            } else {
                nums1[p--] = nums2[p2--]; // 填入 nums2[p2]
            }
        }
    }
};
```

```c [sol-C]
// 入参 nums1Size 和 nums2Size 是多余的
void merge(int* nums1, int, int m, int* nums2, int, int n) {
    int p1 = m - 1, p2 = n - 1, p = m + n - 1;
    while (p2 >= 0) { // nums2 还有要合并的元素
        // 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
        if (p1 >= 0 && nums1[p1] > nums2[p2]) {
            nums1[p--] = nums1[p1--]; // 填入 nums1[p1]
        } else {
            nums1[p--] = nums2[p2--]; // 填入 nums2[p2]
        }
    }
}
```

```go [sol-Go]
func merge(nums1 []int, m int, nums2 []int, n int) {
    p1, p2, p := m-1, n-1, m+n-1
    for p2 >= 0 { // nums2 还有要合并的元素
        // 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
        if p1 >= 0 && nums1[p1] > nums2[p2] {
            nums1[p] = nums1[p1] // 填入 nums1[p1]
            p1--
        } else {
            nums1[p] = nums2[p2] // 填入 nums2[p2]
            p2--
        }
        p-- // 下一个要填入的位置
    }
}
```

```js [sol-JavaScript]
var merge = function(nums1, m, nums2, n) {
    let p1 = m - 1, p2 = n - 1, p = m + n - 1;
    while (p2 >= 0) { // nums2 还有要合并的元素
        // 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
        if (p1 >= 0 && nums1[p1] > nums2[p2]) {
            nums1[p--] = nums1[p1--]; // 填入 nums1[p1]
        } else {
            nums1[p--] = nums2[p2--]; // 填入 nums2[p2]
        }
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn merge(nums1: &mut Vec<i32>, m: i32, nums2: &mut Vec<i32>, n: i32) {
        let m = m as usize;
        let n = n as usize;
        let mut p1 = m - 1;
        let mut p2 = n - 1;
        let mut p = m + n - 1;
        while p2 < n { // nums2 还有要合并的元素
            // 如果 p1 < 0，那么走 else 分支，把 nums2 合并到 nums1 中
            if p1 < m && nums1[p1] > nums2[p2] {
                nums1[p] = nums1[p1]; // 填入 nums1[p1]
                p1 -= 1;
            } else {
                nums1[p] = nums2[p2]; // 填入 nums2[p2]
                p2 -= 1;
            }
            p -= 1;
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m+n)$。最坏情况形如 $\textit{nums}_1=[4,5,6,*,*,*],\textit{nums}_2=[1,2,3]$，每个数都需要移动一次。
- 空间复杂度：$\mathcal{O}(1)$。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、双序列双指针 / §4.1 双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、双序列双指针 / §4.1 双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
