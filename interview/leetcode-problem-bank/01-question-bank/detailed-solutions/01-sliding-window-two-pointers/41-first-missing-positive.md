# 41. 缺失的第一个正数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/first-missing-positive/
- 题目 slug：`first-missing-positive`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.5 原地修改
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/first-missing-positive/solutions/3655377/huan-zuo-wei-tong-guo-li-zi-li-jie-suan-qa94e/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 换座位，通过例子理解算法思想（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/first-missing-positive/solutions/3655377/huan-zuo-wei-tong-guo-li-zi-li-jie-suan-qa94e/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`huan-zuo-wei-tong-guo-li-zi-li-jie-suan-qa94e`
- topic id：`3655377`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 思路

想象有一间教室，座位从左到右编号为 $1$ 到 $n$。

有 $n$ 个学生坐在教室的座位上，把 $\textit{nums}[i]$ 当作坐在第 $i$ 个座位上的学生的学号。我们要做的事情，就是让学号在 $1$ 到 $n$ 中的学生，都坐到编号与自己学号相同的座位上（学号与座位编号匹配）。学号不在 $[1,n]$ 中的学生可以忽略。

学生们交换座位后，从左往右看，第一个学号与座位编号不匹配的学生，其座位编号就是答案。

特别地，如果所有学生都坐在正确的座位上，那么答案是 $n+1$。

### 第一个例子

为方便描述思路，假设数组的下标是从 $1$ 开始的。

假设 $\textit{nums}=[2,3,1]$。

1. 从 $\textit{nums}[1]$ 开始。这个座位上的学生，学号是 $2$，他应当坐在 $\textit{nums}[2]$ 上，所以他和 $\textit{nums}[2]$ 交换。交换后 $\textit{nums}=[3,2,1]$。
2. 仍然看 $\textit{nums}[1]$，这个座位上的学生，学号是 $3$，他应当坐在 $\textit{nums}[3]$ 上，所以他和 $\textit{nums}[3]$ 交换。交换后 $\textit{nums}=[1,2,3]$。
3. 仍然看 $\textit{nums}[1]$，这个座位上的学生，学号是 $1$，他坐在正确的座位上。
4. 向后遍历，$\textit{nums}[2]=2$，他坐在正确的座位上。
5. 向后遍历，$\textit{nums}[3]=3$，他坐在正确的座位上。
6. 换座位过程结束。
7. 再次遍历 $\textit{nums}$，发现 $\textit{nums}[i]=i$ 都满足，说明数组中 $1,2,3$ 都有，所以缺失的第一个正数是 $4$。

### 第二个例子

假设 $\textit{nums}=[3,4,-1,1]$，这是题目中的示例 2。

1. 从 $\textit{nums}[1]$ 开始。这个座位上的学生，学号是 $3$，他应当坐在 $\textit{nums}[3]$ 上，所以他和 $\textit{nums}[3]$ 交换。交换后 $\textit{nums}=[-1,4,3,1]$。
2. 仍然看 $\textit{nums}[1]$，这个座位上的学生，学号是 $-1$，忽略。
3. 向后遍历，$\textit{nums}[2]=4$，他应当坐在 $\textit{nums}[4]$ 上，所以他和 $\textit{nums}[4]$ 交换。交换后 $\textit{nums}=[-1,1,3,4]$。
4. 仍然看 $\textit{nums}[2]=1$，他应当坐在 $\textit{nums}[1]$ 上，所以他和 $\textit{nums}[1]$ 交换。交换后 $\textit{nums}=[1,-1,3,4]$。
5. 仍然看 $\textit{nums}[2]$，这个座位上的学生，学号是 $-1$，忽略。
6. 向后遍历，$\textit{nums}[3]=3$，他坐在正确的座位上。
7. 向后遍历，$\textit{nums}[4]=4$，他坐在正确的座位上。
8. 换座位过程结束。
9. 再次遍历 $\textit{nums}$，发现 $\textit{nums}[2]=-1\ne 2$，说明教室中没有学号为 $2$ 的学生（否则他会坐在 $\textit{nums}[2]$ 上），所以答案是 $2$。

### 第三个例子

注意 $\textit{nums}$ 中可能有重复元素。在这种情况下，算法仍然是正确的吗？

假设 $\textit{nums}=[1,1,2]$。

1. 从 $\textit{nums}[1]$ 开始。这个座位上的学生坐在正确的座位上。
2. 继续遍历，$\textit{nums}[2]=1$，这是 $1$ 号学生的影分身。由于 $1$ 号学生的真身已经坐在正确的座位上，**我们可以在第二次遍历中知道「数组中有 $1$」这个信息**，所以可以忽略 $\textit{nums}[2]$，向后遍历。
3. $\textit{nums}[3]=2$，他应当坐在 $\textit{nums}[2]$ 上，所以他和 $\textit{nums}[2]$ 交换。交换后 $\textit{nums}=[1,2,1]$。
4. 仍然看 $\textit{nums}[3]=1$，同样地，由于 $1$ 号学生已经坐在正确的座位上，所以可以忽略 $\textit{nums}[3]$。
5. 换座位过程结束。
6. 再次遍历 $\textit{nums}$，发现 $\textit{nums}[3]=1\ne 3$，说明教室中没有学号为 $3$ 的学生，所以答案是 $3$。

### 细节

判断「学生是否坐在正确的座位上」，能用 $\textit{nums}[i]=i$ 判断吗？注意有影分身（重复元素）。

在第三个例子中，虽然 $\textit{nums}[2]=1\ne 2$，但由于 $\textit{nums}[\textit{nums}[2]] = \textit{nums}[1] = 1$，所以 $\textit{nums}[2]$ 是个影分身，并且其真身坐在了正确的座位上，所以可以忽略 $\textit{nums}[2]$，向后遍历。注意这种情况是不能交换的，因为 $\textit{nums}[2]=\textit{nums}[1]$，交换后 $\textit{nums}=[1,1,2]$ 是不变的，这会导致**死循环**。

为避免死循环，可以改成判断 $\textit{nums}[2]$ 和 $\textit{nums}[\textit{nums}[2]]$ 是不是一样的。如果一样，就不执行交换，继续向后遍历。

一般地，为了兼容「当前学生是真身，坐在正确的座位上」和「当前学生是影分身，且其真身坐在正确的座位上」两种情况，我们可以把 $i=\textit{nums}[i]$ 套一层 $\textit{nums}$，用 $\textit{nums}[i]=\textit{nums}[\textit{nums}[i]]$ 判断。

- 无论「当前学生是真身，坐在正确的座位上」还是「当前学生是影分身，且其真身坐在正确的座位上」，上式都是成立的。
- 如果「当前学生是真身，不坐在正确的座位上」，那么上式左边是当前学生的学号，右边是要交换的学生的学号。
- 如果「当前学生是影分身，且其真身不坐在正确的座位上」，那么上式左边是当前学生的学号，右边是要交换的学生的学号。虽然是用影分身交换的，但交换后，可以认为真身已经坐在了正确的座位上。

代码实现时，由于 $\textit{nums}$ 的下标是从 $0$ 开始的，**通过学号访问下标，要把学号减一**。

> **注**：用 Python 的同学请注意，下面代码中的 `nums[i], nums[j] = nums[j], nums[i]` 不能写成 `nums[i], nums[nums[i] - 1] = nums[nums[i] - 1], nums[i]`。这会先更新 `nums[i]` 为 `nums[nums[i] - 1]`，然后再更新 `nums[nums[i] - 1]`，但此时 `nums[i] - 1` 已经不是原来的值了。

```py [sol-Python3]
class Solution:
    def firstMissingPositive(self, nums: list[int]) -> int:
        n = len(nums)
        for i in range(n):
            # 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
            while 1 <= nums[i] <= n and nums[nums[i] - 1] != nums[i]:
                # 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
                j = nums[i] - 1  # 减一是因为数组下标从 0 开始
                nums[i], nums[j] = nums[j], nums[i]

        # 找第一个学号与座位编号不匹配的学生
        for i in range(n):
            if nums[i] != i + 1:
                return i + 1

        # 所有学生都坐在正确的座位上
        return n + 1
```

```java [sol-Java]
class Solution {
    public int firstMissingPositive(int[] nums) {
        int n = nums.length;
        for (int i = 0; i < n; i++) {
            // 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
            while (1 <= nums[i] && nums[i] <= n && nums[nums[i] - 1] != nums[i]) {
                // 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
                int j = nums[i] - 1; // 减一是因为数组下标从 0 开始
                int tmp = nums[i];
                nums[i] = nums[j];
                nums[j] = tmp;
            }
        }

        // 找第一个学号与座位编号不匹配的学生
        for (int i = 0; i < n; i++) {
            if (nums[i] != i + 1) {
                return i + 1;
            }
        }

        // 所有学生都坐在正确的座位上
        return n + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int firstMissingPositive(vector<int>& nums) {
        int n = nums.size();
        for (int i = 0; i < n; i++) {
            // 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
            while (1 <= nums[i] && nums[i] <= n && nums[nums[i] - 1] != nums[i]) {
                // 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
                int j = nums[i] - 1; // 减一是因为数组下标从 0 开始
                swap(nums[i], nums[j]);
            }
        }

        // 找第一个学号与座位编号不匹配的学生
        for (int i = 0; i < n; i++) {
            if (nums[i] != i + 1) {
                return i + 1;
            }
        }

        // 所有学生都坐在正确的座位上
        return n + 1;
    }
};
```

```c [sol-C]
int firstMissingPositive(int* nums, int numsSize) {
    for (int i = 0; i < numsSize; i++) {
        // 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
        while (1 <= nums[i] && nums[i] <= numsSize && nums[nums[i] - 1] != nums[i]) {
            // 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
            int j = nums[i] - 1; // 减一是因为数组下标从 0 开始
            int tmp = nums[i];
            nums[i] = nums[j];
            nums[j] = tmp;
        }
    }

    // 找第一个学号与座位编号不匹配的学生
    for (int i = 0; i < numsSize; i++) {
        if (nums[i] != i + 1) {
            return i + 1;
        }
    }

    // 所有学生都坐在正确的座位上
    return numsSize + 1;
}
```

```go [sol-Go]
func firstMissingPositive(nums []int) int {
    n := len(nums)
    for i := range n {
        // 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
        for 1 <= nums[i] && nums[i] <= n && nums[nums[i]-1] != nums[i] {
            // 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
            j := nums[i] - 1 // 减一是因为数组下标从 0 开始
            nums[i], nums[j] = nums[j], nums[i]
        }
    }

    // 找第一个学号与座位编号不匹配的学生
    for i := range n {
        if nums[i] != i+1 {
            return i + 1
        }
    }

    // 所有学生都坐在正确的座位上
    return n + 1
}
```

```js [sol-JavaScript]
var firstMissingPositive = function(nums) {
    const n = nums.length;
    for (let i = 0; i < n; i++) {
        // 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
        while (1 <= nums[i] && nums[i] <= n && nums[nums[i] - 1] !== nums[i]) {
            // 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
            const j = nums[i] - 1; // 减一是因为数组下标从 0 开始
            [nums[i], nums[j]] = [nums[j], nums[i]];
        }
    }

    // 找第一个学号与座位编号不匹配的学生
    for (let i = 0; i < n; i++) {
        if (nums[i] !== i + 1) {
            return i + 1;
        }
    }

    // 所有学生都坐在正确的座位上
    return n + 1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn first_missing_positive(mut nums: Vec<i32>) -> i32 {
        let n = nums.len();
        for i in 0..n {
            // 如果当前学生的学号在 [1,n] 中，但（真身）没有坐在正确的座位上
            while 1 <= nums[i] && nums[i] as usize <= n && nums[(nums[i] - 1) as usize] != nums[i] {
                // 那么就交换 nums[i] 和 nums[j]，其中 j 是 i 的学号
                let j = (nums[i] - 1) as usize; // 减一是因为数组下标从 0 开始
                nums.swap(i, j);
            }
        }

        // 找第一个学号与座位编号不匹配的学生
        for i in 0..n {
            if nums[i] != (i + 1) as i32 {
                return (i + 1) as _;
            }
        }

        // 所有学生都坐在正确的座位上
        (n + 1) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然我们写了个二重循环，但**每次交换都会把一个学生换到正确的座位上**，所以总交换次数至多为 $n$，所以内层循环的**总**循环次数是 $\mathcal{O}(n)$ 的，所以时间复杂度是 $\mathcal{O}(n)$。
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
