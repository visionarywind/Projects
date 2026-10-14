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
