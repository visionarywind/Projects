# 2860. 让所有学生保持开心的分组方法数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/happy-students/
- 题目 slug：`happy-students`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.2 脑筋急转弯
- 难度分：1626
- 外部题解来源：https://leetcode.cn/problems/happy-students/solutions/2446022/pai-xu-pythonjavacgojs-by-endlesscheng-ptzl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[遇事不决先排序（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/happy-students/solutions/2446022/pai-xu-pythonjavacgojs-by-endlesscheng-ptzl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-pythonjavacgojs-by-endlesscheng-ptzl`
- topic id：`2446022`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

在选择学生人数固定的时候，选择方案是否唯一呢？

假设恰好选 $k$ 个学生，那么：

- 所有 $\textit{nums}[i] < k$ 的学生都要选；
- 所有 $\textit{nums}[i] > k$ 的学生都不能选；
- 不能出现 $\textit{nums}[i] = k$ 的情况，因为每个学生只有选或不选两种可能。

这意味着**在选择学生人数固定的时候，选择方案是唯一的**。把 $\textit{nums}$ 从小到大排序后，唯一性可以更明显地看出来：

- 以 $k$ 为分界线，左边的都要选，右边的都不能选。
  
排序后：

- 如果选了 $\textit{nums}[i]$，那么比 $\textit{nums}[i]$ 更小的学生也要选。
- 如果不选 $\textit{nums}[i]$，那么比 $\textit{nums}[i]$ 更大的学生也不选。

具体地，如果选 $\textit{nums}[i-1]$ 而不选 $\textit{nums}[i]$，由于数组已排序，我们必须要选下标为 $0,1,2,\cdots,i-1$ 的学生，一共 $i$ 个，而下标 $\ge i$ 的学生都不能选，所以需要满足

$$
\textit{nums}[i-1] < i < \textit{nums}[i]
$$

枚举 $i=1,2,\cdots,n-1$（枚举分界线的位置），如果上式成立，就意味着我们可以选 $i$ 个学生，算作一种方案。

特殊情况：

- 如果 $\textit{nums}[0] > 0$，那么可以一个学生都不选。
- 如果 $\textit{nums}[n-1] < n$，那么可以所有学生都选。由于数据范围保证 $\textit{nums}[i]<n$，所以这种方案一定存在。

见 [视频讲解](https://www.bilibili.com/video/BV1Lm4y1N7mf/) 第二题。

```py [sol-Python3]
class Solution:
    def countWays(self, nums: List[int]) -> int:
        nums.sort()
        ans = nums[0] > 0  # 一个学生都不选
        for i, (x, y) in enumerate(pairwise(nums), 1):
            if x < i < y:
                ans += 1
        return ans + 1  # 一定可以都选
```

```java [sol-Java]
class Solution {
    public int countWays(List<Integer> nums) {
        int[] a = nums.stream().mapToInt(i -> i).toArray();
        Arrays.sort(a);
        int ans = a[0] > 0 ? 1 : 0; // 一个学生都不选
        for (int i = 1; i < a.length; i++) {
            if (a[i - 1] < i && i < a[i]) {
                ans++;
            }
        }
        return ans + 1; // 一定可以都选
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countWays(vector<int>& nums) {
        ranges::sort(nums);
        int ans = nums[0] > 0; // 一个学生都不选
        for (int i = 1; i < nums.size(); i++) {
            ans += nums[i - 1] < i && i < nums[i];
        }
        return ans + 1; // 一定可以都选
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int countWays(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = nums[0] > 0; // 一个学生都不选
    for (int i = 1; i < numsSize; i++) {
        ans += nums[i - 1] < i && i < nums[i];
    }
    return ans + 1; // 一定可以都选
}
```

```go [sol-Go]
func countWays(nums []int) (ans int) {
    slices.Sort(nums)
    if nums[0] > 0 { // 一个学生都不选
        ans = 1
    }
    for i := 1; i < len(nums); i++ {
        if nums[i-1] < i && i < nums[i] {
            ans++
        }
    }
    return ans + 1 // 一定可以都选
}
```

```js [sol-JavaScript]
var countWays = function(nums) {
    nums.sort((a, b) => a - b);
    let ans = nums[0] > 0 ? 1 : 0; // 一个学生都不选
    for (let i = 1; i < nums.length; i++) {
        if (nums[i - 1] < i && i < nums[i]) {
            ans++;
        }
    }
    return ans + 1; // 一定可以都选
};
```

```rust [sol-Rust]
impl Solution {
    pub fn count_ways(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let mut ans = (nums[0] > 0) as i32; // 一个学生都不选
        for i in 1..nums.len() {
            let k = i as i32;
            if nums[i - 1] < k && k < nums[i] {
                ans += 1;
            }
        }
        ans + 1 // 一定可以都选
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

注：如果采用计数排序，可以做到 $\mathcal{O}(n)$ 的时间复杂度。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.2 脑筋急转弯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.2 脑筋急转弯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
