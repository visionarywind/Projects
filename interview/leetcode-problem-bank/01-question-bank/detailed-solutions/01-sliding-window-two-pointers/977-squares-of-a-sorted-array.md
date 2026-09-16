# 977. 有序数组的平方

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/squares-of-a-sorted-array/
- 题目 slug：`squares-of-a-sorted-array`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.2 相向双指针
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/squares-of-a-sorted-array/solutions/2806253/xiang-xiang-shuang-zhi-zhen-cong-da-dao-blda6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[相向双指针：从大到小合并（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/squares-of-a-sorted-array/solutions/2806253/xiang-xiang-shuang-zhi-zhen-cong-da-dao-blda6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xiang-xiang-shuang-zhi-zhen-cong-da-dao-blda6`
- topic id：`2806253`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

看示例 1，把 $[-4,-1,0,3,10]$ 分成负数和非负数两部分：

- **负数**：$[-4,-1]$，计算平方得 $[16,1]$，反过来看就是 $[1,16]$。
- **非负数**：$[0,3,10]$，计算平方得 $[0,9,100]$。

仿照 [88. 合并两个有序数组](https://leetcode.cn/problems/merge-sorted-array/) 的 [思路](https://leetcode.cn/problems/merge-sorted-array/solution/dao-xu-shuang-zhi-zhen-wei-shi-yao-dao-x-xxkp/)，把上述两个有序数组合并，得到答案 $[0,1,9,16,100]$。

与其从中间开始向两边合并，不如**从两边开始向中间合并**，这样无需计算从中间的哪个位置开始。

具体算法如下：

1. 初始化一个长为 $n$ 的空数组 $\textit{ans}$。
2. 初始化左指针 $i=0$，右指针 $j=n-1$。初始化下标 $p=n-1$，表示要往 $\textit{ans}[p]$ 填入数据。
3. 设 $x=\textit{nums}[i]^2,\ y = \textit{nums}[j]^2$。
4. 如果 $x>y$，将 $x$ 填入 $\textit{ans}[p]$，把 $i$ 加一，$p$ 减一。
5. 如果 $x\le y$，将 $y$ 填入 $\textit{ans}[p]$，把 $j$ 减一，$p$ 减一。
6. 循环直到 $p=-1$，即所有数据都填入 $\textit{ans}$。
7. 返回 $\textit{ans}$。

在上述过程中，由于我们总是把更大的数放在 $\textit{ans}$ 更靠右的位置，所以得到的答案是非递减的。

## 写法一

```py [sol-Python3]
class Solution:
    def sortedSquares(self, nums: List[int]) -> List[int]:
        n = len(nums)
        ans = [0] * n
        i, j = 0, n - 1
        for p in range(n - 1, -1, -1):
            x = nums[i] * nums[i]
            y = nums[j] * nums[j]
            if x > y:  # 更大的数放右边
                ans[p] = x
                i += 1
            else:
                ans[p] = y
                j -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] sortedSquares(int[] nums) {
        int n = nums.length;
        int[] ans = new int[n];
        int i = 0;
        int j = n - 1;
        for (int p = n - 1; p >= 0; p--) {
            int x = nums[i] * nums[i];
            int y = nums[j] * nums[j];
            if (x > y) {
                ans[p] = x;
                i++;
            } else {
                ans[p] = y;
                j--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> sortedSquares(vector<int>& nums) {
        int n = nums.size();
        vector<int> ans(n);
        int i = 0, j = n - 1;
        for (int p = n - 1; p >= 0; p--) {
            int x = nums[i] * nums[i];
            int y = nums[j] * nums[j];
            if (x > y) {
                ans[p] = x;
                i++;
            } else {
                ans[p] = y;
                j--;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int* sortedSquares(int* nums, int numsSize, int* returnSize) {
    int* ans = malloc(numsSize * sizeof(int));
    *returnSize = numsSize;
    int i = 0, j = numsSize - 1;
    for (int p = numsSize - 1; p >= 0; p--) {
        int x = nums[i] * nums[i];
        int y = nums[j] * nums[j];
        if (x > y) {
            ans[p] = x;
            i++;
        } else {
            ans[p] = y;
            j--;
        }
    }
    return ans;
}
```

```go [sol-Go]
func sortedSquares(nums []int) []int {
    n := len(nums)
    ans := make([]int, n)
    i, j := 0, n-1
    for p := n - 1; p >= 0; p-- {
        x := nums[i] * nums[i]
        y := nums[j] * nums[j]
        if x > y {
            ans[p] = x
            i++
        } else {
            ans[p] = y
            j--
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var sortedSquares = function(nums) {
    const n = nums.length;
    const ans = Array(n);
    let i = 0, j = n - 1;
    for (let p = n - 1; p >= 0; p--) {
        const x = nums[i] * nums[i];
        const y = nums[j] * nums[j];
        if (x > y) {
            ans[p] = x;
            i++;
        } else {
            ans[p] = y;
            j--;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn sorted_squares(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut ans = vec![0; n];
        let mut i = 0;
        let mut j = n - 1;
        for p in (0..n).rev() {
            let x = nums[i] * nums[i];
            let y = nums[j] * nums[j];
            if x > y {
                ans[p] = x;
                i += 1;
            } else {
                ans[p] = y;
                j -= 1;
            }
        }
        ans
    }
}
```

## 写法二

改成比较 $-\textit{nums}[i]$ 和 $\textit{nums}[j]$ 的大小：

- 如果 $-\textit{nums}[i] > \textit{nums}[j]$，那么填入 $\textit{ans}[p] = \textit{nums}[i]^2$。
- 如果 $-\textit{nums}[i] \le \textit{nums}[j]$，那么填入 $\textit{ans}[p] = \textit{nums}[j]^2$。

为什么这样做是对的？

1. 如果 $\textit{nums}$ 中的元素均为负数，那么 $-\textit{nums}[i] > 0 > \textit{nums}[j]$ 恒成立，我们按照（平方后）从大到小的顺序填入答案。
2. 如果 $\textit{nums}$ 中的元素均为非负数，那么 $-\textit{nums}[i] \le 0 \le \textit{nums}[j]$ 恒成立，我们按照（平方后）从大到小的顺序填入答案。
3. 否则，$\textit{nums}[i] < 0$ 且 $\textit{nums}[j] \ge 0$，那么 $-\textit{nums}[i] > \textit{nums}[j]$ 等价于 $\textit{nums}[i]^2 > \textit{nums}[j]^2$，反之亦然。填入答案，移动指针后，转换成这三种情况之一。所以我们仍然按照（平方后）从大到小的顺序填入答案。

注：这种写法每次循环只需要计算一次乘法。

```py [sol-Python3]
class Solution:
    def sortedSquares(self, nums: List[int]) -> List[int]:
        n = len(nums)
        ans = [0] * n
        i, j = 0, n - 1
        for p in range(n - 1, -1, -1):
            x, y = nums[i], nums[j]
            if -x > y:
                ans[p] = x * x
                i += 1
            else:
                ans[p] = y * y
                j -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] sortedSquares(int[] nums) {
        int n = nums.length;
        int[] ans = new int[n];
        int i = 0;
        int j = n - 1;
        for (int p = n - 1; p >= 0; p--) {
            int x = nums[i];
            int y = nums[j];
            if (-x > y) {
                ans[p] = x * x;
                i++;
            } else {
                ans[p] = y * y;
                j--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> sortedSquares(vector<int>& nums) {
        int n = nums.size();
        vector<int> ans(n);
        int i = 0, j = n - 1;
        for (int p = n - 1; p >= 0; p--) {
            int x = nums[i], y = nums[j];
            if (-x > y) {
                ans[p] = x * x;
                i++;
            } else {
                ans[p] = y * y;
                j--;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int* sortedSquares(int* nums, int numsSize, int* returnSize) {
    int* ans = malloc(numsSize * sizeof(int));
    *returnSize = numsSize;
    int i = 0, j = numsSize - 1;
    for (int p = numsSize - 1; p >= 0; p--) {
        int x = nums[i], y = nums[j];
        if (-x > y) {
            ans[p] = x * x;
            i++;
        } else {
            ans[p] = y * y;
            j--;
        }
    }
    return ans;
}
```

```go [sol-Go]
func sortedSquares(nums []int) []int {
    n := len(nums)
    ans := make([]int, n)
    i, j := 0, n-1
    for p := n - 1; p >= 0; p-- {
        x, y := nums[i], nums[j]
        if -x > y {
            ans[p] = x * x
            i++
        } else {
            ans[p] = y * y
            j--
        }
    }
    return ans
}
```

```js [sol-JavaScript]
var sortedSquares = function(nums) {
    const n = nums.length;
    const ans = Array(n);
    let i = 0, j = n - 1;
    for (let p = n - 1; p >= 0; p--) {
        const x = nums[i], y = nums[j];
        if (-x > y) {
            ans[p] = x * x;
            i++;
        } else {
            ans[p] = y * y;
            j--;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn sorted_squares(nums: Vec<i32>) -> Vec<i32> {
        let n = nums.len();
        let mut ans = vec![0; n];
        let mut i = 0;
        let mut j = n - 1;
        for p in (0..n).rev() {
            let x = nums[i];
            let y = nums[j];
            if -x > y {
                ans[p] = x * x;
                i += 1;
            } else {
                ans[p] = y * y;
                j -= 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

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

本题来自 `三、单序列双指针 / §3.2 相向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.2 相向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
