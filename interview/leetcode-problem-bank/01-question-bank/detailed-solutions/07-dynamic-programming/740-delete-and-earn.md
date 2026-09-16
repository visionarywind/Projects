# 740. 删除并获得点数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/delete-and-earn/
- 题目 slug：`delete-and-earn`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.2 打家劫舍 / 答疑
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/delete-and-earn/solutions/3061028/zhi-yu-da-jia-jie-she-pythonjavaccgojsru-e5gg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[值域打家劫舍（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/delete-and-earn/solutions/3061028/zhi-yu-da-jia-jie-she-pythonjavaccgojsru-e5gg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-yu-da-jia-jie-she-pythonjavaccgojsru-e5gg`
- topic id：`3061028`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

看示例 2，$\textit{nums}=[2,2,3,3,3,4]$。如果我们选了一个等于 $3$ 的数，那么所有等于 $2$ 和等于 $4$ 的数都被删除，也就是都不能选。选了一个 $3$ 后，剩下的 $3$ 可以继续选。所以如果要选 $3$，所有的 $3$ 都要选。

这种「相邻数字不能都选」联想到 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/)。

把 $\textit{nums}$ 转换成一个值域数组 $a$，其中 $a[i]$ 表示 $\textit{nums}$ 中的等于 $i$ 的元素之和。上面的例子中，$a=[0,0,4,9,4]$。因为 $\textit{nums}$ 中有 $3$ 个 $3$，所以 $a[3]=3+3+3=9$。

计算数组 $a$ 的 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/)，即为答案。

下面打家劫舍的代码来自 [我的题解](https://leetcode.cn/problems/house-robber/solutions/2102725/ru-he-xiang-chu-zhuang-tai-ding-yi-he-zh-1wt1/) 中的方法三。

```py [sol-Python3]
class Solution:
    # 198. 打家劫舍
    def rob(self, nums: List[int]) -> int:
        f0 = f1 = 0
        for x in nums:
            f0, f1 = f1, max(f1, f0 + x)
        return f1

    def deleteAndEarn(self, nums: List[int]) -> int:
        a = [0] * (max(nums) + 1)
        for x in nums:
            a[x] += x  # 统计等于 x 的元素之和
        return self.rob(a)
```

```java [sol-Java]
class Solution {
    public int deleteAndEarn(int[] nums) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int[] a = new int[mx + 1];
        for (int x : nums) {
            a[x] += x; // 统计等于 x 的元素之和
        }

        return rob(a);
    }

    // 198. 打家劫舍
    private int rob(int[] nums) {
        int f0 = 0;
        int f1 = 0;
        for (int x : nums) {
            int newF = Math.max(f1, f0 + x);
            f0 = f1;
            f1 = newF;
        }
        return f1;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 198. 打家劫舍
    int rob(vector<int>& nums) {
        int f0 = 0, f1 = 0;
        for (int x : nums) {
            int new_f = max(f1, f0 + x);
            f0 = f1;
            f1 = new_f;
        }
        return f1;
    }

public:
    int deleteAndEarn(vector<int>& nums) {
        int mx = ranges::max(nums);
        vector<int> a(mx + 1);
        for (int x : nums) {
            a[x] += x; // 统计等于 x 的元素之和
        }
        return rob(a);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

// 198. 打家劫舍
int rob(int* nums, int numsSize) {
    int f0 = 0, f1 = 0;
    for (int i = 0; i < numsSize; i++) {
        int new_f = MAX(f1, f0 + nums[i]);
        f0 = f1;
        f1 = new_f;
    }
    return f1;
}

int deleteAndEarn(int* nums, int numsSize) {
    int mx = 0;
    for (int i = 0; i < numsSize; i++) {
        mx = MAX(mx, nums[i]);
    }

    int* a = calloc(mx + 1, sizeof(int));
    for (int i = 0; i < numsSize; i++) {
        a[nums[i]] += nums[i]; // 统计等于 x 的元素之和
    }

    int res = rob(a, mx + 1);
    free(a);
    return res;
}
```

```go [sol-Go]
// 198. 打家劫舍
func rob(nums []int) int {
    f0, f1 := 0, 0
    for _, x := range nums {
        f0, f1 = f1, max(f1, f0+x)
    }
    return f1
}

func deleteAndEarn(nums []int) int {
    a := make([]int, slices.Max(nums)+1)
    for _, x := range nums {
        a[x] += x // 统计等于 x 的元素之和
    }
    return rob(a)
}
```

```js [sol-JavaScript]
// 198. 打家劫舍
var rob = function(nums) {
    let f0 = 0, f1 = 0;
    for (const x of nums) {
        [f0, f1] = [f1, Math.max(f1, f0 + x)]
    }
    return f1;
};

var deleteAndEarn = function(nums) {
    const mx = Math.max(...nums);
    const a = Array(mx + 1).fill(0);
    for (const x of nums) {
        a[x] += x; // 统计等于 x 的元素之和
    }
    return rob(a);
};
```

```rust [sol-Rust]
impl Solution {
    // 198. 打家劫舍
    fn rob(nums: Vec<i32>) -> i32 {
        let mut f0 = 0;
        let mut f1 = 0;
        for x in nums {
            let new_f = f1.max(f0 + x);
            f0 = f1;
            f1 = new_f;
        }
        f1
    }

    pub fn delete_and_earn(nums: Vec<i32>) -> i32 {
        let mx = *nums.iter().max().unwrap();
        let mut a = vec![0; mx as usize + 1];
        for x in nums {
            a[x as usize] += x; // 统计等于 x 的元素之和
        }
        Self::rob(a)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 思考题

如果值域更大，比如 $\textit{nums}[i]\le 10^9$，要怎么做？

这题是 [3186. 施咒的最大总伤害](https://leetcode.cn/problems/maximum-total-damage-with-spell-casting/)。

更多相似题目，见下面动态规划题单中的「**§1.2 打家劫舍**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、入门 DP / §1.2 打家劫舍 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.2 打家劫舍 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
