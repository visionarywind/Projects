# 2616. 最小化数对的最大差值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-the-maximum-difference-of-pairs/
- 题目 slug：`minimize-the-maximum-difference-of-pairs`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.4 最小化最大值
- 难度分：2155
- 外部题解来源：https://leetcode.cn/problems/minimize-the-maximum-difference-of-pairs/solutions/2216315/er-fen-da-an-tan-xin-by-endlesscheng-dlxv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案 + 贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-the-maximum-difference-of-pairs/solutions/2216315/er-fen-da-an-tan-xin-by-endlesscheng-dlxv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-tan-xin-by-endlesscheng-dlxv`
- topic id：`2216315`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 转化

假设数对差值 $\le \textit{mx}$。

$\textit{mx}$ 越大，要求就越**宽松**，越能找到 $p$ 个数对。

$\textit{mx}$ 越小，要求就越**苛刻**，越不能找到 $p$ 个数对。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题变成一个判定性问题：

- 给定 $\textit{mx}$，能否不重复地选择（至少）$p$ 个数对，每个数对的差值（绝对差）都 $\le \textit{mx}$？

## 思路

为方便计算，先把 $\textit{nums}$ 排序，这样选相邻元素（相差更小）比选不相邻元素更优。例如 $[1,2,3,4]$，如果选 $(1,3)$ 和 $(2,4)$，最大差值是 $2$；而如果选 $(1,2)$ 和 $(3,4)$，最大差值只有 $1$。

最多能选多少个数对？

定义 $f(n)$ 表示在 $\textit{nums}$ 的后 $n$ 个数中选出的最多数对个数。

讨论 $\textit{nums}[0]$ **选或不选**：

- 如果不选 $\textit{nums}[0]$，那么答案等于剩下的「后 $n-1$ 个数的最多数对个数」，即 $f(n-1)$。
- 如果选 $\textit{nums}[0]$ 和 $\textit{nums}[1]$（前提是差值 $\le \textit{mx}$），那么答案等于剩下的「后 $n-2$ 个数的最多数对个数」加一，即 $f(n-2)+1$。
- 两种情况取最大值，得 $f(n) = \max(f(n-1), f(n-2)+1)$。
- 如果无法选 $\textit{nums}[0]$，则 $f(n) = f(n-1)$。

这类似 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/)，可以用动态规划实现。

但实际上，可以贪心。

由于 $f(n-1)$ 比 $f(n-3)$ 只多了两个数，至多比 $f(n-3)$ 多一个下标对，所以有

$$
f(n-1) \le f(n-3)+1
$$

此外，由于元素个数越少，$f(i)$ 越小，所以有

$$
f(n-2)\ge f(n-3)
$$

两边加一，得

$$
f(n-2)+1\ge f(n-3)+1 \ge f(n-1)
$$

因此

$$
f(n) = \max(f(n-1), f(n-2)+1) = f(n-2)+1
$$

所以如果可以选 $\textit{nums}[0]$ 和 $\textit{nums}[1]$，那么直接选 $\textit{nums}[0]$ 和 $\textit{nums}[1]$ 就是最优的。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$-1$。绝对差不可能是负数，一定无法满足要求。
- 开区间右端点初始值：$\max(\textit{nums}) - \min(\textit{nums})$。可以随便选，所以一定满足要求。注意题目保证 $p\le n/2$。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

## 答疑

**问**：为什么二分结束后，答案 $\textit{ans}$ 一定来自 $\textit{nums}$ 中某两个数的差值？

**答**：反证法。假设 $\textit{ans}$ 不来自 $\textit{nums}$ 中某两个数的差值，这意味着最大差值 $\le \textit{ans}-1$，换句话说，$\text{check}(\textit{ans}-1)=\texttt{true}$。但根据循环不变量，二分结束后 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾。故原命题成立。

[本题视频讲解](https://www.bilibili.com/video/BV1iN411w7my/)

```py [sol-Python3]
class Solution:
    def minimizeMax(self, nums: List[int], p: int) -> int:
        nums.sort()
        def check(mx: int) -> bool:
            cnt = i = 0
            while i < len(nums) - 1:
                if nums[i + 1] - nums[i] <= mx:  # 选 nums[i] 和 nums[i+1]
                    cnt += 1
                    i += 2
                else:  # 不选 nums[i]
                    i += 1
            return cnt >= p

        left, right = -1, nums[-1] - nums[0]
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def minimizeMax(self, nums: List[int], p: int) -> int:
        nums.sort()
        def check(mx: int) -> bool:
            cnt = i = 0
            while i < len(nums) - 1:
                if nums[i + 1] - nums[i] <= mx:  # 选 nums[i] 和 nums[i+1]
                    cnt += 1
                    i += 2
                else:  # 不选 nums[i]
                    i += 1
            return cnt >= p
        return bisect_left(range(nums[-1] - nums[0]), True, key=check)
```

```java [sol-Java]
class Solution {
    public int minimizeMax(int[] nums, int p) {
        Arrays.sort(nums);
        int left = -1;
        int right = nums[nums.length - 1] - nums[0];
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (check(mid, nums, p)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int mx, int[] nums, int p) {
        int cnt = 0;
        for (int i = 0; i < nums.length - 1; i++) {
            if (nums[i + 1] - nums[i] <= mx) { // 选 nums[i] 和 nums[i+1]
                cnt++;
                i++;
            }
        }
        return cnt >= p;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimizeMax(vector<int>& nums, int p) {
        ranges::sort(nums);

        auto check = [&](int mx) -> bool {
            int cnt = 0;
            for (int i = 0; i + 1 < nums.size(); i++) {
                if (nums[i + 1] - nums[i] <= mx) { // 选 nums[i] 和 nums[i+1]
                    cnt++;
                    i++;
                }
            }
            return cnt >= p;
        };

        int left = -1, right = nums.back() - nums[0];
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func minimizeMax(nums []int, p int) int {
    slices.Sort(nums)
    n := len(nums)
    return sort.Search(nums[n-1]-nums[0], func(mx int) bool {
        cnt := 0
        for i := 0; i < n-1; i++ {
            if nums[i+1]-nums[i] <= mx { // 选 nums[i] 和 nums[i+1]
                cnt++
                i++
            }
        }
        return cnt >= p
    })
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})-\min(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序时的栈空间。

## 附：与值域无关的做法

由于答案一定是（排序后）相邻元素的差值，我们可以**只在差值中二分**。

具体来说，计算相邻元素的差值，记录到一个 $\textit{diffs}$ 数组中（为兼容 $p=0$ 这种特殊情况，把 $0$ 也加到 $\textit{diffs}$ 中）。然后把 $\textit{diffs}$ 排序。

由于答案一定在 $\textit{diffs}$ 中，我们可以在 $\textit{diffs}$ 中二分下标 $\textit{idx}$，上面代码中的 $\textit{mx} = \textit{diff}[\textit{idx}]$，其余不变。

注意开区间二分右端点是 $n-1$，不是 $n$。因为 $\textit{diffs}[n-1]$ 是最大的差值，一定满足要求。

```py
class Solution:
    def minimizeMax(self, nums: List[int], p: int) -> int:
        nums.sort()
        diffs = [0] + sorted(y - x for x, y in pairwise(nums))

        def check(idx: int) -> int:
            mx = diffs[idx]
            cnt = i = 0
            while i < len(nums) - 1:
                if nums[i + 1] - nums[i] <= mx:  # 选 nums[i] 和 nums[i+1]
                    cnt += 1
                    i += 2
                else:  # 不选 nums[i]
                    i += 1
            return cnt >= p

        left, right = -1, len(nums) - 1
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return diffs[right]
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `二、二分答案 / §2.4 最小化最大值`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.4 最小化最大值`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
