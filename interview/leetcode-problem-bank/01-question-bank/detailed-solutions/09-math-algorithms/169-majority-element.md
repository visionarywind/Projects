# 169. 多数元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/majority-element/
- 题目 slug：`majority-element`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.8 摩尔投票法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/majority-element/solutions/3744717/on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 武林大会打擂台，严谨证明（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/majority-element/solutions/3744717/on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-mo-er-tou-piao-fa-yan-jin-zheng-ming-ww1zv`
- topic id：`3744717`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

**题意**：计算 $\textit{nums}$ 的**绝对众数**。绝对众数的出现次数，比其余所有元素的出现次数加起来还多。题目保证 $\textit{nums}$ 一定存在绝对众数。

尝试设计一个 $\mathcal{O}(n)$ 一次遍历，同时只用到 $\mathcal{O}(1)$ 额外空间的算法。

想象一众武林高手比武，谁会笑到最后？

我用「擂台赛」打比方：

1. **擂主登场**：$\textit{nums}[0]$ 成为初始擂主，生命值为 $1$。
2. **挑战者出现**：遍历后续元素，作为挑战者。
3. **比武**：如果挑战者与擂主属于同一门派（值相同），那么擂主生命值加 $1$，否则擂主生命值减 $1$。
4. **擂主更迭**：如果比武后，擂主生命值降为 $0$（同归于尽），那么下一个挑战者成为新的擂主，生命值为 $1$。
5. 最后在擂台上的那人，便是武林盟主（绝对众数）。

为什么这样做是对的？

设出现次数最多的元素的出现次数为 $a$，其余元素的出现次数之和为 $b=n-a$。题目保证 $a>b$。

**证明**：上述过程中，每次擂主的生命值降为 $0$ 时，相当于开了一个新的擂台赛，在 $\textit{nums}[i-1]$ 和 $\textit{nums}[i]$ 之间切一刀。这会把 $\textit{nums}$ 分成若干段。依次考察这些段：

- 对于除了最后一段的每一段（注意这些段的擂主不一定是绝对众数，比如绝对众数是 $9$，这一段是 $[1,1,2,9]$），设绝对众数在其中出现了 $x$ 次，其余元素的出现次数之和为 $y$，则必然有 $x\le y$。这可以用反证法证明，如果 $x>y$，那么绝对众数血多，不可能被其余元素同归于尽，绝对众数的生命值在这段结束时必然大于 $0$，矛盾。由此可得 $a-x > b-y$，意思是，把 $a$ 减去 $x$，$b$ 减去 $y$，所得到的 $a'$ 和 $b'$ 仍然满足 $a'>b'$。依此类推，每一段结束时，在剩余元素（未遍历到的元素）中，设出现次数最多的元素的出现次数为 $a'$，其余元素的出现次数之和为 $b'$，那么 $a'>b'$ 始终成立。
- 对于最后一段，由于 $a'>b'$，绝对众数血多，不可能被其余元素同归于尽，绝对众数的生命值最终必然大于 $0$，所以最后在擂台上的是绝对众数。

```py [sol-Python3]
class Solution:
    def majorityElement(self, nums: List[int]) -> int:
        ans = hp = 0
        for x in nums:
            if hp == 0:  # x 是初始擂主，生命值为 1
                ans, hp = x, 1
            else:  # 比武，同门加血，否则扣血
                hp += 1 if x == ans else -1
        return ans
```

```java [sol-Java]
class Solution {
    public int majorityElement(int[] nums) {
        int ans = 0;
        int hp = 0;
        for (int x : nums) {
            if (hp == 0) { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else { // 比武，同门加血，否则扣血
                hp += x == ans ? 1 : -1;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int majorityElement(vector<int>& nums) {
        int ans = 0, hp = 0;
        for (int x : nums) {
            if (hp == 0) { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else { // 比武，同门加血，否则扣血
                hp += x == ans ? 1 : -1;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int majorityElement(int* nums, int numsSize) {
    int ans = 0, hp = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        if (hp == 0) { // x 是初始擂主，生命值为 1
            ans = x;
            hp = 1;
        } else { // 比武，同门加血，否则扣血
            hp += x == ans ? 1 : -1;
        }
    }
    return ans;
}
```

```go [sol-Go]
func majorityElement(nums []int) (ans int) {
    hp := 0
    for _, x := range nums {
        if hp == 0 { // x 是初始擂主，生命值为 1
            ans, hp = x, 1
        } else if x == ans { // 比武，同门加血，否则扣血
            hp++
        } else {
            hp--
        }
    }
    return
}
```

```js [sol-JavaScript]
var majorityElement = function(nums) {
    let ans = 0, hp = 0;
    for (const x of nums) {
        if (hp === 0) { // x 是初始擂主，生命值为 1
            ans = x;
            hp = 1;
        } else { // 比武，同门加血，否则扣血
            hp += x === ans ? 1 : -1;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn majority_element(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut hp = 0;
        for x in nums {
            if hp == 0 { // x 是初始擂主，生命值为 1
                ans = x;
                hp = 1;
            } else if x == ans { // 比武，同门加血，否则扣血
                hp += 1;
            } else {
                hp -= 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

给定数组 $\textit{nums}$，判断 $\textit{nums}$ 是否存在绝对众数。

欢迎在评论区分享你的思路/代码。

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

本题来自 `七、杂项 / §7.8 摩尔投票法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.8 摩尔投票法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
