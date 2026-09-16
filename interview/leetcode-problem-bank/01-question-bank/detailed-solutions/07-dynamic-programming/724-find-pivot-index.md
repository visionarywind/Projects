# 724. 寻找数组的中心下标

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-pivot-index/
- 题目 slug：`find-pivot-index`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-pivot-index/solutions/2834687/jian-ji-xie-fa-o1-e-wai-kong-jian-python-tz0p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法，O(1) 额外空间（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-pivot-index/solutions/2834687/jian-ji-xie-fa-o1-e-wai-kong-jian-python-tz0p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-o1-e-wai-kong-jian-python-tz0p`
- topic id：`2834687`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $\textit{nums}$ 的元素之和为 $s$。

设中心下标为 $i$，其左侧元素和为 $\textit{leftS} = \textit{nums}[0] + \textit{nums}[1] + \cdots + \textit{nums}[i-1]$，那么右侧元素和为 $s - \textit{nums}[i] - \textit{leftS}$。

由于左侧元素和等于右侧元素和，所以有

$$
\textit{leftS} = s - \textit{nums}[i] - \textit{leftS}
$$

即

$$
2\cdot \textit{leftS} = s - \textit{nums}[i]
$$

从左到右遍历数组，一边遍历，一边累加元素更新 $\textit{leftS}$。每次累加前，检查是否满足上式，满足则返回 $i$。

如果不存在这样的 $i$，返回 $-1$。

```py [sol-Python3]
class Solution:
    def pivotIndex(self, nums: List[int]) -> int:
        s = sum(nums)
        left_s = 0
        for i, x in enumerate(nums):
            if left_s * 2 == s - x:
                return i
            left_s += x
        return -1
```

```java [sol-Java]
class Solution {
    public int pivotIndex(int[] nums) {
        int s = 0;
        for (int num : nums) {
            s += num;
        }
        int leftS = 0;
        for (int i = 0; i < nums.length; i++) {
            if (leftS * 2 == s - nums[i]) {
                return i;
            }
            leftS += nums[i];
        }
        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int pivotIndex(vector<int>& nums) {
        int s = reduce(nums.begin(), nums.end());
        int left_s = 0;
        for (int i = 0; i < nums.size(); i++) {
            if (left_s * 2 == s - nums[i]) {
                return i;
            }
            left_s += nums[i];
        }
        return -1;
    }
};
```

```c [sol-C]
int pivotIndex(int* nums, int numsSize) {
    int s = 0;
    for (int i = 0; i < numsSize; i++) {
        s += nums[i];
    }
    int left_s = 0;
    for (int i = 0; i < numsSize; i++) {
        if (left_s * 2 == s - nums[i]) {
            return i;
        }
        left_s += nums[i];
    }
    return -1;
}
```

```go [sol-Go]
func pivotIndex(nums []int) int {
    s := 0
    for _, num := range nums {
        s += num
    }
    leftS := 0
    for i, x := range nums {
        if leftS*2 == s-x {
            return i
        }
        leftS += x
    }
    return -1
}
```

```js [sol-JS]
var pivotIndex = function(nums) {
    const s = _.sum(nums);
    let leftS = 0;
    for (let i = 0; i < nums.length; i++) {
        if (leftS * 2 === s - nums[i]) {
            return i;
        }
        leftS += nums[i];
    }
    return -1;
}
```

```rust [sol-Rust]
impl Solution {
    pub fn pivot_index(nums: Vec<i32>) -> i32 {
        let s = nums.iter().sum::<i32>();
        let mut left_s = 0;
        for (i, &x) in nums.iter().enumerate() {
            if left_s * 2 == s - x {
                return i as _;
            }
            left_s += x;
        }
        -1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

1. 把数组分成 $3$ 段，使得这 $3$ 段的元素和都相等，有多少种分割方案？注意数组中有负数，也有 $0$。
2. 把数组分成 $3$ 段，使得第一段的元素和小于等于第二段的元素和，第二段的元素和小于等于第三段的元素和，有多少种分割方案？见 [1712. 将数组分成三个子数组的方案数](https://leetcode.cn/problems/ways-to-split-array-into-three-subarrays/)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
