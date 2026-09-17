# 1. 两数之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/two-sum/
- 题目 slug：`two-sum`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/two-sum/solutions/2326193/dong-hua-cong-liang-shu-zhi-he-zhong-wo-0yvmj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【动画】从两数之和中，我们可以学到什么？（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/two-sum/solutions/2326193/dong-hua-cong-liang-shu-zhi-he-zhong-wo-0yvmj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-hua-cong-liang-shu-zhi-he-zhong-wo-0yvmj`
- topic id：`2326193`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

![lc1-new-c.png](https://pic.leetcode.cn/1724902818-vpHFEf-lc1-new-c.png)

<![lc1-1.png](https://pic.leetcode.cn/1688135757-Sjcxmk-lc1-1.png),![lc1-2.png](https://pic.leetcode.cn/1688135757-cBtvsB-lc1-2.png),![lc1-3.png](https://pic.leetcode.cn/1688135757-rcvJii-lc1-3.png),![lc1-4.png](https://pic.leetcode.cn/1688135757-TVqxKQ-lc1-4.png),![lc1-5.png](https://pic.leetcode.cn/1688135757-gbdDCN-lc1-5.png),![lc1-6-x.png](https://pic.leetcode.cn/1688135962-XlOrzl-lc1-6-x.png),![lc1-7.png](https://pic.leetcode.cn/1688135757-hMIJdl-lc1-7.png),![lc1-8-x.png](https://pic.leetcode.cn/1688136022-tEoGkQ-lc1-8-x.png),![lc1-9-x.png](https://pic.leetcode.cn/1688136187-XXYIeT-lc1-9-x.png),![lc1-10-x.png](https://pic.leetcode.cn/1688136187-wiIlsk-lc1-10-x.png)>

## 答疑

**问**：是什么原因导致了这两种算法的快慢？

**答**：我用「获取了多少信息」来解释。

暴力做法每次拿两个数出来相加，和 $\textit{target}$ 比较，那么花费 $\mathcal{O}(1)$ 的时间，只获取了 $\mathcal{O}(1)$ 的信息。

而哈希表做法，每次查询都能知道 $\mathcal{O}(n)$ 个数中是否有 $\textit{target}-\textit{nums}[j]$，那么花费 $\mathcal{O}(1)$ 的时间，就获取了 $\mathcal{O}(n)$ 的信息。

这就是为什么我们可以把暴力的 $\mathcal{O}(n^2)$ 优化成 $\mathcal{O}(n)$。

**问**：力扣是如何测试题目的？为什么没有 `main` 函数？

**答**：简单来说，力扣评测机内部有 `main` 函数，里面会调用你写的 `twoSum` 函数（方法），传入相应的测试数据，并对比 `twoSum` 的返回值和正确答案是否一致。如果对于所有测试数据，返回值都与正确答案一致，则判定通过。

所以，我们**只需编写核心逻辑**，保证返回结果计算正确即可。

**问**：如何在本地测试代码？

**答**：[见此](https://leetcode.cn/problems/add-two-integers/solutions/1430335/zhen-meng-kai-shi-de-di-fang-by-endlessc-rwxa/)。

## 暴力写法

```py [sol-Python3]
class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        for i, x in enumerate(nums):  # x=nums[i]
            for j in range(i + 1, len(nums)):  # 枚举 i 右边的 j
                if x + nums[j] == target:  # 满足要求
                    return [i, j]  # 返回两个数的下标
        # 这里无需 return，因为题目保证有解
```

```java [sol-Java]
class Solution {
    public int[] twoSum(int[] nums, int target) {
        for (int i = 0; ; i++) { // 枚举 i
            for (int j = i + 1; j < nums.length; j++) { // 枚举 i 右边的 j
                if (nums[i] + nums[j] == target) { // 满足要求
                    return new int[]{i, j}; // 返回两个数的下标
                }
            }
        }
        // 题目保证有解，循环中一定会 return
        // 所以这里无需 return，毕竟代码不会执行到这里
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        for (int i = 0; ; i++) { // 枚举 i
            for (int j = i + 1; j < nums.size(); j++) { // 枚举 i 右边的 j
                if (nums[i] + nums[j] == target) { // 满足要求
                    return {i, j}; // 返回两个数的下标
                }
            }
        }
        // 题目保证有解，循环中一定会 return
        // 所以这里无需 return，毕竟代码不会执行到这里
    }
};
```

```c [sol-C]
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    for (int i = 0; ; i++) { // 枚举 i
        for (int j = i + 1; j < numsSize; j++) { // 枚举 i 右边的 j
            if (nums[i] + nums[j] == target) { // 满足要求
                int* ans = malloc(2 * sizeof(int)); // 分配返回结果的内存空间
                *returnSize = 2; // ans 的长度
                ans[0] = i; // 存储第一个数的下标
                ans[1] = j; // 存储第二个数的下标
                return ans; // 返回结果数组的指针
            }
        }
    }
    // 题目保证有解，循环中一定会 return
    // 所以这里无需 return，毕竟代码不会执行到这里
}
```

```go [sol-Go]
func twoSum(nums []int, target int) []int {
    for i, x := range nums { // 枚举 i
        for j := i + 1; j < len(nums); j++ { // 枚举 i 右边的 j
            if x+nums[j] == target { // 满足要求
                return []int{i, j} // 返回两个数的下标
            }
        }
    }
    return nil // 题目保证一定有解，代码不会执行到这里
}
```

```js [sol-JavaScript]
var twoSum = function(nums, target) {
    for (let i = 0; ; i++) { // 枚举 i
        for (let j = i + 1; j < nums.length; j++) { // 枚举 i 右边的 j
            if (nums[i] + nums[j] === target) { // 满足要求
                return [i, j]; // 返回两个数的下标
            }
        }
    }
    // 题目保证有解，循环中一定会 return
    // 所以这里无需 return，毕竟代码不会执行到这里
};
```

```rust [sol-Rust]
impl Solution {
    pub fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
        for i in 0..nums.len() { // 枚举 i
            for j in i + 1..nums.len() { // 枚举 i 右边的 j
                if nums[i] + nums[j] == target { // 满足要求
                    return vec![i as i32, j as i32]; // 返回两个数的下标
                }
            }
        }
        unreachable!() // 题目保证有解，循环中一定会 return
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 哈希表写法

**问**：为什么下面的代码，要先查询 $\textit{idx}$ 是否有 $\textit{target}- \textit{nums}[j] $，再把 $\textit{nums}[j]$ 和 $j$ 加到 $\textit{idx}$ 中？能不能反过来？

**答**：反过来写是错误的。例如 $\textit{nums}=[2,3,1],\ \textit{target}=4$，如果先把 $\textit{nums}[j]$ 和 $j$ 加到 $\textit{idx}$ 中，我们会认为 $2+2=4$，返回 $[0,0]$，而正确答案应该是 $3+1=4$，也就是返回 $[1,2]$。

原因在于，题目要求「不能使用两次相同的元素」，也就是**两个数的下标必须不同**。我们的做法是枚举右边的数的下标 $j$，去找左边的数的下标 $i$。由于找的是左边的数，如果先把右边的数加到 $\textit{idx}$ 中，找到的数就可能包含右边的数了，不符合题目要求。

```py [sol-Python3]
class Solution:
    def twoSum(self, nums: List[int], target: int) -> List[int]:
        idx = {}  # 创建一个空哈希表（字典）
        for j, x in enumerate(nums):  # x=nums[j]
            if target - x in idx:  # 在左边找 nums[i]，满足 nums[i]+x=target
                return [idx[target - x], j]  # 返回两个数的下标
            idx[x] = j  # 保存 nums[j] 和 j
```

```java [sol-Java]
class Solution {
    public int[] twoSum(int[] nums, int target) {
        Map<Integer, Integer> idx = new HashMap<>(); // 创建一个空哈希表
        for (int j = 0; ; j++) { // 枚举 j
            int x = nums[j];
            // 在左边找 nums[i]，满足 nums[i]+x=target
            if (idx.containsKey(target - x)) { // 找到了
                return new int[]{idx.get(target - x), j}; // 返回两个数的下标
            }
            idx.put(x, j); // 保存 nums[j] 和 j
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> idx; // 创建一个空哈希表
        for (int j = 0; ; j++) { // 枚举 j
            // 在左边找 nums[i]，满足 nums[i]+nums[j]=target
            auto it = idx.find(target - nums[j]);
            if (it != idx.end()) { // 找到了
                return {it->second, j}; // 返回两个数的下标
            }
            idx[nums[j]] = j; // 保存 nums[j] 和 j
        }
    }
};
```

```c [sol-C]
typedef struct {
    int num; // 哈希表的 key
    int idx; // 哈希表的 value
    UT_hash_handle hh; // uthash 需要
} HashEntry;

int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    HashEntry* idx_map = NULL; // 创建一个空哈希表
    HashEntry* e;
    for (int j = 0; ; j++) { // 枚举 j
        // 在左边找 nums[i]，满足 nums[i]+nums[j]=target
        int t = target - nums[j];
        HASH_FIND_INT(idx_map, &t, e);
        if (e) { // 找到了
            int* ans = malloc(2 * sizeof(int)); // 分配返回结果的内存空间
            *returnSize = 2; // ans 的长度
            ans[0] = e->idx; // 存储第一个数的下标
            ans[1] = j; // 存储第二个数的下标

            // 释放哈希表中所有节点，防止内存泄漏
            HashEntry* tmp;
            HASH_ITER(hh, idx_map, e, tmp) {
                HASH_DEL(idx_map, e);
                free(e);
            }

            return ans; // 返回结果数组的指针
        }

        HASH_FIND_INT(idx_map, &nums[j], e);
        if (e == NULL) { // nums[j] 不在哈希表中
            // 保存 nums[j] 和 j 到哈希表中
            e = malloc(sizeof(HashEntry));
            e->num = nums[j];
            e->idx = j;
            HASH_ADD_INT(idx_map, num, e);
        } // else 哈希表有 nums[j]，无需更新
    }
}
```

```go [sol-Go]
func twoSum(nums []int, target int) []int {
    idx := map[int]int{} // 创建一个空哈希表
    for j, x := range nums { // 枚举 j
        // 在左边找 nums[i]，满足 nums[i]+x=target
        if i, ok := idx[target-x]; ok { // 找到了
            return []int{i, j} // 返回两个数的下标
        }
        idx[x] = j // 保存 nums[j] 和 j
    }
    return nil // 题目保证一定有解，代码不会执行到这里
}
```

```js [sol-JavaScript]
var twoSum = function(nums, target) {
    const idx = new Map(); // 创建一个空哈希表
    for (let j = 0; ; j++) { // 枚举 j
        const x = nums[j];
        // 在左边找 nums[i]，满足 nums[i]+x=target
        if (idx.has(target - x)) { // 找到了
            return [idx.get(target - x), j]; // 返回两个数的下标
        }
        idx.set(x, j); // 保存 nums[j] 和 j
    }
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
        let mut idx = HashMap::new(); // 创建一个空哈希表
        for (j, &x) in nums.iter().enumerate() { // 枚举 j
            // 在左边找 nums[i]，满足 nums[i]+x=target
            if let Some(&i) = idx.get(&(target - x)) { // 找到了
                return vec![i as i32, j as i32]; // 返回两个数的下标
            }
            idx.insert(x, j); // 保存 nums[j] 和 j
        }
        unreachable!() // 题目保证有解，循环中一定会 return
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。哈希表需要 $\mathcal{O}(n)$ 的空间。

相比暴力做法，哈希表多消耗了内存空间，但减少了运行时间，这就是「**空间换时间**」。

## 总结 · 练习

很多涉及到「两个变量」的题目，都可以枚举其中一个变量，把它当成**常量**看待，从而转化成「一个变量」的问题。

代码实现时，通常来说「枚举右，寻找左」是更加好写的。

- [1512. 好数对的数目](https://leetcode.cn/problems/number-of-good-pairs/)
- [219. 存在重复元素 II](https://leetcode.cn/problems/contains-duplicate-ii/)
- [1010. 总持续时间可被 60 整除的歌曲](https://leetcode.cn/problems/pairs-of-songs-with-total-durations-divisible-by-60/)
- [2748. 美丽下标对的数目](https://leetcode.cn/problems/number-of-beautiful-pairs/)
- 更多相似题目，请看 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 第零章。

## 思考题

1. 如果 $\textit{nums}$ 是有序的，是否还需要哈希表？换句话说，能否做到 $\mathcal{O}(1)$ 额外空间？
2. 如果要求寻找**三个**数，它们的和等于 $\textit{target}$ 呢？

解答请看[【基础算法精讲】](https://www.bilibili.com/video/BV1bP411c7oJ/)。

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

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
