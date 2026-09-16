# 75. 颜色分类

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sort-colors/
- 题目 slug：`sort-colors`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.5 原地修改
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sort-colors/solutions/3679069/on-cha-ru-pai-xu-jian-ji-xie-fa-pythonja-zk60/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 插入排序，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/sort-colors/solutions/3679069/on-cha-ru-pai-xu-jian-ji-xie-fa-pythonja-zk60/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-cha-ru-pai-xu-jian-ji-xie-fa-pythonja-zk60`
- topic id：`3679069`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

不让用 $\texttt{sort}$ 吗？有意思……

## 技巧：O(1) 插入元素

假设现在有一个有序数组 $a=[0,0,1,1,2,2]$。在 $a$ 中插入一个 $0$，同时保证 $a$ 是有序的，你会怎么做？

最暴力的想法是，把 $0$ 插在数组的最左边，原来的元素全体右移一位，得到 $[0,0,0,1,1,2,2]$。这样做是 $\mathcal{O}(n)$ 的。

实际上，我们可以「狸猫换太子」：**不是插入元素，而是修改元素**！

对比一下插入前后：

- 插入前 $[0,0,1,1,2,2]$。
- 插入后 $[0,0,0,1,1,2,2]$。

竖着看，其实**只有三个位置上的数变了**：

1. $a[2]$ 从 $1$ 改成 $0$。
2. $a[4]$ 从 $2$ 改成 $1$。
3. 末尾新增一个 $2$，相当于 $a[6]=2$。

怎么知道这些位置？

1. 维护 $a$ 中 $0$ 的个数，即为改成 $0$ 的位置，记作 $p_0$。上例中 $p_0=2$。把 $a[p_0]$ 改成 $0$。
2. 维护 $a$ 中 $0$ 和 $1$ 的个数，即为改成 $1$ 的位置，记作 $p_1$。上例中 $p_1=4$。把 $a[p_1]$ 改成 $1$。
3. 末尾新增的位置记作 $i$，把 $a[i]$ 改成 $2$。

## 细节

如果 $a$ 中没有 $2$ 呢？上面第三步就错了。

比如 $a=[1]$，插入一个 $0$，结果为 $[0,1]$。但如果按照上面三步走，第三步把 $a[1]$ 改成 $2$，得到的是错误的 $[0,2]$。

要用很多 $\texttt{if-else}$ 特判这些特殊情况吗？

不需要，我们可以**倒过来**算：先把 $a[1]$ 改成 $2$，再把 $a[1]$ 改成 $1$（覆盖），最后 $a[0]$ 改成 $0$，得到 $[0,1]$。这种「覆盖」等价于「没有 $2$ 的时候不改成 $2$」。

如果插入的是 $1$ 呢？

跳过「把 $a[p_0]$ 改成 $0$」这一步。

如果插入的是 $2$ 呢？

只需要把 $a[i]$ 改成 $2$。

## 本题思路

对 $\textit{nums}$ 执行插入排序，也就是对 $i=0,1,2,\ldots,n-1$ 依次执行如下过程：

- 现在前缀 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 是有序的，把 $\textit{nums}[i]$ 插入这个有序前缀，从而把前缀 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 变成有序的。
- 算法执行完后，$\textit{nums}$ 就是一个有序数组了。

```py [sol-Python3]
class Solution:
    def sortColors(self, nums: List[int]) -> None:
        p0 = p1 = 0
        for i, x in enumerate(nums):
            nums[i] = 2
            if x <= 1:
                nums[p1] = 1
                p1 += 1
            if x == 0:
                nums[p0] = 0
                p0 += 1
```

```java [sol-Java]
class Solution {
    public void sortColors(int[] nums) {
        int p0 = 0;
        int p1 = 0;
        for (int i = 0; i < nums.length; i++) {
            int x = nums[i];
            nums[i] = 2;
            if (x <= 1) {
                nums[p1++] = 1;
            }
            if (x == 0) {
                nums[p0++] = 0;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    void sortColors(vector<int>& nums) {
        int p0 = 0, p1 = 0;
        for (int i = 0; i < nums.size(); i++) {
            int x = nums[i];
            nums[i] = 2;
            if (x <= 1) {
                nums[p1++] = 1;
            }
            if (x == 0) {
                nums[p0++] = 0;
            }
        }
    }
};
```

```c [sol-C]
void sortColors(int* nums, int numsSize) {
    int p0 = 0, p1 = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        nums[i] = 2;
        if (x <= 1) {
            nums[p1++] = 1;
        }
        if (x == 0) {
            nums[p0++] = 0;
        }
    }
}
```

```go [sol-Go]
func sortColors(nums []int) {
    p0, p1 := 0, 0
    for i, x := range nums {
        nums[i] = 2
        if x <= 1 {
            nums[p1] = 1
            p1++
        }
        if x == 0 {
            nums[p0] = 0
            p0++
        }
    }
}
```

```js [sol-JavaScript]
var sortColors = function(nums) {
    let p0 = 0, p1 = 0;
    for (let i = 0; i < nums.length; i++) {
        const x = nums[i];
        nums[i] = 2;
        if (x <= 1) {
            nums[p1++] = 1;
        }
        if (x === 0) {
            nums[p0++] = 0;
        }
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn sort_colors(nums: &mut Vec<i32>) {
        let mut p0 = 0;
        let mut p1 = 0;
        for i in 0..nums.len() {
            let x = nums[i];
            nums[i] = 2;
            if x <= 1 {
                nums[p1] = 1;
                p1 += 1;
            }
            if x == 0 {
                nums[p0] = 0;
                p0 += 1;
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、单序列双指针 / §3.5 原地修改`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.5 原地修改`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
