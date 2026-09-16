# 454. 四数相加 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/4sum-ii/
- 题目 slug：`4sum-ii`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/4sum-ii/solutions/3038338/on2-jian-ji-xie-fa-pythonjavacgojsrust-b-paps/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n^2) 简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/4sum-ii/solutions/3038338/on2-jian-ji-xie-fa-pythonjavacgojsrust-b-paps/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on2-jian-ji-xie-fa-pythonjavacgojsrust-b-paps`
- topic id：`3038338`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

题目要求

$$
\textit{nums}_1[i] + \textit{nums}_2[j] + \textit{nums}_3[k] + \textit{nums}_4[l] == 0
$$

将其变形为

$$
\textit{nums}_1[i] + \textit{nums}_2[j] = -\textit{nums}_3[k] - \textit{nums}_4[l]
$$

我们可以先统计出所有 $\textit{nums}_1[i] + \textit{nums}_2[j]$ 的出现次数，记在一个哈希表 $\textit{cnt}$ 中。

然后遍历所有 $\textit{nums}_3[k]$ 和 $\textit{nums}_4[l]$，比如 $-\textit{nums}_3[k] - \textit{nums}_4[l] = 10$，那么我们需要知道有多少个 $\textit{nums}_1[i] + \textit{nums}_2[j]$ 也等于 $10$，这可以直接 $\mathcal{O}(1)$ 从哈希表中获取到，即 $\textit{cnt}[10]$。所以累加 $\textit{cnt}[-\textit{nums}_3[k] - \textit{nums}_4[l]]$ 的值，即为答案。

```py [sol-Python3]
class Solution:
    def fourSumCount(self, nums1: List[int], nums2: List[int], nums3: List[int], nums4: List[int]) -> int:
        cnt = Counter(x + y for x in nums1 for y in nums2)
        return sum(cnt[-x - y] for x in nums3 for y in nums4)
```

```java [sol-Java]
class Solution {
    public int fourSumCount(int[] nums1, int[] nums2, int[] nums3, int[] nums4) {
        HashMap<Integer, Integer> cnt = new HashMap<>();
        for (int x : nums1) {
            for (int y : nums2) {
                cnt.merge(x + y, 1, Integer::sum); // cnt[x+y]++
            }
        }

        int ans = 0;
        for (int x : nums3) {
            for (int y : nums4) {
                ans += cnt.getOrDefault(-x - y, 0);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int fourSumCount(vector<int>& nums1, vector<int>& nums2, vector<int>& nums3, vector<int>& nums4) {
        unordered_map<int, int> cnt;
        for (int x : nums1) {
            for (int y : nums2) {
                cnt[x + y]++;
            }
        }

        int ans = 0;
        for (int x : nums3) {
            for (int y : nums4) {
                ans += cnt[-x - y];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func fourSumCount(nums1, nums2, nums3, nums4 []int) (ans int) {
    cnt := map[int]int{}
    for _, x := range nums1 {
        for _, y := range nums2 {
            cnt[x+y]++
        }
    }

    for _, x := range nums3 {
        for _, y := range nums4 {
            ans += cnt[-x-y]
        }
    }
    return
}
```

```js [sol-JavaScript]
var fourSumCount = function(nums1, nums2, nums3, nums4) {
    const cnt = new Map();
    for (const x of nums1) {
        for (const y of nums2) {
            cnt.set(x + y, (cnt.get(x + y) ?? 0) + 1);
        }
    }

    let ans = 0;
    for (const x of nums3) {
        for (const y of nums4) {
            ans += cnt.get(-x - y) ?? 0;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn four_sum_count(nums1: Vec<i32>, nums2: Vec<i32>, nums3: Vec<i32>, nums4: Vec<i32>) -> i32 {
        let mut cnt = HashMap::new();
        for x in nums1 {
            for &y in &nums2 {
                *cnt.entry(x + y).or_insert(0) += 1;
            }
        }

        let mut ans = 0;
        for x in nums3 {
            for &y in &nums4 {
                ans += cnt.get(&(-x - y)).unwrap_or(&0);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}_i$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 思考题

改成 $i<j<k<l$ 怎么做？

思路类似 [3404. 统计特殊子序列的数目](https://leetcode.cn/problems/count-special-subsequences/)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
