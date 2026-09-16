# 1658. 将 x 减到 0 的最小操作数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-reduce-x-to-zero/
- 题目 slug：`minimum-operations-to-reduce-x-to-zero`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）
- 难度分：1817
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-reduce-x-to-zero/solutions/2048811/ni-xiang-si-wei-pythonjavacgo-by-endless-b4jt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种写法：逆向思维/直接双指针（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-reduce-x-to-zero/solutions/2048811/ni-xiang-si-wei-pythonjavacgo-by-endless-b4jt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-pythonjavacgo-by-endless-b4jt`
- topic id：`2048811`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：逆向思维+滑动窗口

移除的是 $\textit{nums}$ 最左边或最右边的元素，那么剩下的元素是什么？是 $\textit{nums}$ 的**连续子数组**。

移除的元素和 $x$ + 剩余的元素和 = $\textit{nums}$ 的所有元素之和 $s$。

所以剩余的元素和 = $s-x$。

问题变成：

- 从 $\textit{nums}$ 中找最长的子数组（这样移除的数尽量少），满足子数组的元素和恰好等于 $s-x$。

由于本题没有负数，可以用滑动窗口计算，具体原理可以看我的 [滑动窗口【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)，欢迎点赞~

最后答案为 $\textit{nums}$ 的长度减去最长子数组的长度。

⚠**注意**：子数组的长度可以是 $0$，所以下面代码初始化 $\textit{ans}=-1$。如果初始化 $\textit{ans}=0$，就无法区分是否真的存在符合要求的子数组。

```py [sol-Python3]
class Solution:
    def minOperations(self, nums: List[int], x: int) -> int:
        target = sum(nums) - x
        if target < 0:
            return -1  # 全部移除也无法满足要求

        ans = -1
        s = left = 0
        for right, num in enumerate(nums):
            s += num
            while s > target:
                s -= nums[left]
                left += 1  # 缩小子数组长度
            if s == target:
                ans = max(ans, right - left + 1)

        return -1 if ans < 0 else len(nums) - ans
```

```java [sol-Java]
class Solution {
    public int minOperations(int[] nums, int x) {
        int target = -x;
        for (int num : nums) {
            target += num;
        }
        if (target < 0) {
            return -1; // 全部移除也无法满足要求
        }

        int n = nums.length;
        int ans = -1;
        int sum = 0;
        int left = 0;
        for (int right = 0; right < n; right++) {
            sum += nums[right];
            while (sum > target) {
                sum -= nums[left];
                left++; // 缩小子数组长度
            }
            if (sum == target) {
                ans = Math.max(ans, right - left + 1);
            }
        }

        return ans < 0 ? -1 : n - ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(vector<int>& nums, int x) {
        int target = reduce(nums.begin(), nums.end()) - x;
        if (target < 0) {
            return -1; // 全部移除也无法满足要求
        }

        int ans = -1, left = 0, sum = 0, n = nums.size();
        for (int right = 0; right < n; right++) {
            sum += nums[right];
            while (sum > target) {
                sum -= nums[left];
                left++; // 缩小子数组长度
            }
            if (sum == target) {
                ans = max(ans, right - left + 1);
            }
        }

        return ans < 0 ? -1 : n - ans;
    }
};
```

```go [sol-Go]
func minOperations(nums []int, x int) int {
    target := -x
    for _, x := range nums {
        target += x
    }
    if target < 0 { // 全部移除也无法满足要求
        return -1
    }

    ans, left, sum := -1, 0, 0
    for right, num := range nums {
        sum += num
        for sum > target {
            sum -= nums[left]
            left++ // 缩小子数组长度
        }
        if sum == target {
            ans = max(ans, right-left+1)
        }
    }

    if ans < 0 {
        return -1
    }
    return len(nums) - ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然写了个二重循环，但是 `left++` 的执行次数不会超过 $n$ 次，所以总的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：直接双指针

如果要正向计算也是可以的，就是写起来稍微有点麻烦：首先算出最长的元素和不超过 $x$ 的后缀，然后不断枚举前缀长度，另一个指针指向后缀最左元素，保证前缀+后缀的元素和不超过 $x$。答案就是前缀+后缀长度之和的最小值。

```py [sol-Python3]
class Solution:
    def minOperations(self, nums: List[int], x: int) -> int:
        right = n = len(nums)
        s = 0
        while right and s + nums[right - 1] <= x:  # 计算最长后缀
            right -= 1
            s += nums[right]
        if right == 0 and s < x:
            return -1  # 全部移除也无法满足要求

        ans = n - right if s == x else inf
        for left, num in enumerate(nums):
            s += num
            while right < n and s > x:
                s -= nums[right]
                right += 1  # 缩小后缀长度
            if s > x:
                break  # 缩小失败，说明前缀过长
            if s == x:
                ans = min(ans, left + 1 + n - right)  # 前缀+后缀长度

        return ans if ans <= n else -1
```

```java [sol-Java]
class Solution {
    public int minOperations(int[] nums, int x) {
        int n = nums.length;
        int sum = 0;
        int right = n;
        while (right > 0 && sum + nums[right - 1] <= x) { // 计算最长后缀
            right--;
            sum += nums[right];
        }
        if (right == 0 && sum < x) {
            return -1; // 全部移除也无法满足要求
        }

        int ans = sum == x ? n - right : n + 1;
        for (int left = 0; left < n; left++) {
            sum += nums[left];
            while (right < n && sum > x) {
                sum -= nums[right];
                right++; // 缩小后缀长度
            }
            if (sum > x) {
                break; // 缩小失败，说明前缀过长
            }
            if (sum == x) {
                ans = Math.min(ans, left + 1 + n - right); // 前缀+后缀长度
            }
        }

        return ans > n ? -1 : ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(vector<int>& nums, int x) {
        int sum = 0, n = nums.size(), right = n;
        while (right > 0 && sum + nums[right - 1] <= x) { // 计算最长后缀
            right--;
            sum += nums[right];
        }
        if (right == 0 && sum < x) {
            return -1; // 全部移除也无法满足要求
        }

        int ans = sum == x ? n - right : n + 1;
        for (int left = 0; left < n; left++) {
            sum += nums[left];
            while (right < n && sum > x) {
                sum -= nums[right];
                right++; // 缩小后缀长度
            }
            if (sum > x) {
                break; // 缩小失败，说明前缀过长
            }
            if (sum == x) {
                ans = min(ans, left + 1 + n - right); // 前缀+后缀长度
            }
        }

        return ans > n ? -1 : ans;
    }
};
```

```go [sol-Go]
func minOperations(nums []int, x int) int {
    s, n := 0, len(nums)
    right := n
    for right > 0 && s+nums[right-1] <= x { // 计算最长后缀
        right--
        s += nums[right]
    }
    if right == 0 && s < x { // 全部移除也无法满足要求
        return -1
    }

    ans := n + 1
    if s == x {
        ans = n - right
    }
    for left, num := range nums {
        s += num
        for right < n && s > x {
            s -= nums[right]
            right++ // 缩小后缀长度
        }
        if s > x { // 缩小失败，说明前缀过长
            break
        }
        if s == x {
            ans = min(ans, left+1+n-right) // 前缀+后缀长度
        }
    }

    if ans > n {
        return -1
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，理由同上。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
