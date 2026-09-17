# 55. 跳跃游戏

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/jump-game/
- 题目 slug：`jump-game`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.5 合并区间
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/jump-game/solutions/2798996/liang-chong-li-jie-fang-shi-wei-hu-zui-y-q67s/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种理解方式：维护最右可达位置/合并区间（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/jump-game/solutions/2798996/liang-chong-li-jie-fang-shi-wei-hu-zui-y-q67s/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-li-jie-fang-shi-wei-hu-zui-y-q67s`
- topic id：`2798996`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

**题意**：从 $i$ 可以跳到闭区间 $[i,i+\textit{nums}_i]$ 中的任意整数。问：能不能从 $0$ 跳到 $n-1$？

先看示例 2，$\textit{nums}=[3,2,1,0,4]$。我们在遍历数组的同时，维护最右可以到达的位置 $\textit{mx}$，如下表：

| $i$  | $\textit{nums}_i$  | $i+\textit{nums}_i$  |  $\textit{mx}$ |
|---|---|---|---|
| $0$  | $3$  | $3$  | $3$  |
| $1$  | $2$  | $3$  | $3$  |
| $2$  | $1$  | $3$  | $3$  |
| $3$  | $0$  | $3$  | $3$  |
| $4$  | $4$  | $8$  | 失败  | 

从 $0$ 可以跳到 $1,2,3$，但是无法从 $1,2,3$ 中的任何位置跳到 $4$。当我们遍历到 $i=4$ 时，发现

$$
i > \textit{mx}
$$

这意味着 $i$ 是无法到达的，返回 $\texttt{false}$。

然后来看示例 1，$\textit{nums}=[2,3,1,1,4]$，在遍历数组的同时，维护最远可以到达的位置 $\textit{mx}$，如下表：

| $i$  | $\textit{nums}_i$  | $i+\textit{nums}_i$  |  $\textit{mx}$ |
|---|---|---|---|
| $0$  | $2$  | $2$  | $2$  |
| $1$  | $3$  | $4$  | $4$  |
| $2$  | $1$  | $3$  | $4$  |
| $3$  | $1$  | $4$  | $4$  |
| $4$  | $4$  | $8$  | $8$  |

从 $0$ 可以跳到 $1,2$，最远可以到达的位置 $\textit{mx}=2$。能否跳到更远的位置？那就看从 $1$ 能跳到哪些位置，从 $2$ 能跳到哪些位置。

从 $1$ 可以跳到 $2,3,4$，$\textit{mx}$ 更新成 $4$。

从 $2$ 可以跳到 $3$，$\textit{mx}$ 不变。

从 $3$ 可以跳到 $4$，$\textit{mx}$ 不变。

到达 $4$，返回 $\texttt{true}$。

一般地，算法如下：

1. 从左到右遍历 $\textit{nums}$，同时维护能跳到的最远位置 $\textit{mx}$，初始值为 $0$。
2. 如果 $i > \textit{mx}$，说明无法跳到 $i$，返回 $\texttt{false}$。
3. 否则，用 $i+\textit{nums}_i$ 更新 $\textit{mx}$ 的最大值。
4. 如果循环中没有返回 $\texttt{false}$，那么最后返回 $\texttt{true}$。

另一种理解方式是，把每个 $\textit{nums}_i$ 看成闭区间 $[i,i+\textit{nums}_i]$，问题变成判定这 $n$ 个区间能否合并成一个大区间（而不是多个区间），这可以用 [56. 合并区间](https://leetcode.cn/problems/merge-intervals/) 的 [算法](https://leetcode.cn/problems/merge-intervals/solution/jian-dan-zuo-fa-yi-ji-wei-shi-yao-yao-zh-f2b3/) 解决。

```py [sol-Python3]
class Solution:
    def canJump(self, nums: List[int]) -> bool:
        mx = 0
        for i, jump in enumerate(nums):
            if i > mx:  # 无法到达 i
                return False
            mx = max(mx, i + jump)  # 从 i 最右可以跳到 i+jump
        return True
```

```java [sol-Java]
class Solution {
    public boolean canJump(int[] nums) {
        int mx = 0;
        for (int i = 0; i < nums.length; i++) {
            if (i > mx) { // 无法到达 i
                return false;
            }
            mx = Math.max(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canJump(vector<int>& nums) {
        int mx = 0;
        for (int i = 0; i < nums.size(); i++) {
            if (i > mx) { // 无法到达 i
                return false;
            }
            mx = max(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
        }
        return true;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

bool canJump(int* nums, int numsSize) {
    int mx = 0;
    for (int i = 0; i < numsSize; i++) {
        if (i > mx) { // 无法到达 i
            return false;
        }
        mx = MAX(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
    }
    return true;
}
```

```go [sol-Go]
func canJump(nums []int) bool {
    mx := 0
    for i, jump := range nums {
        if i > mx { // 无法到达 i
            return false
        }
        mx = max(mx, i+jump) // 从 i 最右可以跳到 i + jump
    }
    return true
}
```

```js [sol-JavaScript]
var canJump = function(nums) {
    let mx = 0;
    for (let i = 0; i < nums.length; i++) {
        if (i > mx) { // 无法到达 i
            return false;
        }
        mx = Math.max(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_jump(nums: Vec<i32>) -> bool {
        let mut mx = 0;
        for (i, &jump) in nums.iter().enumerate() {
            if i > mx { // 无法到达 i
                return false;
            }
            mx = mx.max(i + jump as usize); // 从 i 最右可以跳到 i + jump
        }
        true
    }
}
```

也可以在 $\textit{mx}\ge n-1$ 时就返回 $\texttt{true}$，这可以让我们提前退出循环。

```py [sol-Python3]
class Solution:
    def canJump(self, nums: List[int]) -> bool:
        mx = 0
        for i, jump in enumerate(nums):
            if i > mx:  # 无法到达 i
                return False
            mx = max(mx, i + jump)  # 从 i 最右可以跳到 i + jump
            if mx >= len(nums) - 1:  # 可以跳到 n-1
                return True
```

```java [sol-Java]
class Solution {
    public boolean canJump(int[] nums) {
        int mx = 0;
        for (int i = 0; mx < nums.length - 1; i++) {
            if (i > mx) { // 无法到达 i
                return false;
            }
            mx = Math.max(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool canJump(vector<int>& nums) {
        int mx = 0;
        for (int i = 0; mx < nums.size() - 1; i++) {
            if (i > mx) { // 无法到达 i
                return false;
            }
            mx = max(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
        }
        return true;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

bool canJump(int* nums, int numsSize) {
    int mx = 0;
    for (int i = 0; mx < numsSize - 1; i++) {
        if (i > mx) { // 无法到达 i
            return false;
        }
        mx = MAX(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
    }
    return true;
}
```

```go [sol-Go]
func canJump(nums []int) bool {
    mx := 0
    for i, jump := range nums {
        if i > mx { // 无法到达 i
            return false
        }
        mx = max(mx, i+jump) // 从 i 最右可以跳到 i + jump
        if mx >= len(nums)-1 { // 可以跳到 n-1
            break
        }
    }
    return true
}
```

```js [sol-JavaScript]
var canJump = function(nums) {
    let mx = 0;
    for (let i = 0; mx < nums.length - 1; i++) {
        if (i > mx) { // 无法到达 i
            return false;
        }
        mx = Math.max(mx, i + nums[i]); // 从 i 最右可以跳到 i + nums[i]
    }
    return true;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn can_jump(nums: Vec<i32>) -> bool {
        let mut mx = 0;
        for (i, &jump) in nums.iter().enumerate() {
            if i > mx { // 无法到达 i
                return false;
            }
            mx = mx.max(i + jump as usize); // 从 i 最右可以跳到 i + jump
            if mx >= nums.len() - 1 { // 可以跳到 n-1
                break;
            }
        }
        true
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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.5 合并区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.5 合并区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
