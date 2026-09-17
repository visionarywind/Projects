# 45. 跳跃游戏 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/jump-game-ii/
- 题目 slug：`jump-game-ii`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.4 区间覆盖
- 难度分：1700
- 外部题解来源：https://leetcode.cn/problems/jump-game-ii/solutions/2926993/tu-jie-yi-zhang-tu-miao-dong-tiao-yue-yo-h2d4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一张图秒懂跳跃游戏！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/jump-game-ii/solutions/2926993/tu-jie-yi-zhang-tu-miao-dong-tiao-yue-yo-h2d4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-yi-zhang-tu-miao-dong-tiao-yue-yo-h2d4`
- topic id：`2926993`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

![lc45-c.png](https://pic.leetcode.cn/1765859329-rZnxlH-lc45-c.png){:width=550}

⚠**注意**：不是在无路可走的那个位置造桥，而是当发现无路可走的时候，时光倒流到能跳到最远点的那个位置造桥。换句话说，**在无路可走之前，我们只是在默默地收集信息**。当发现无路可走的时候，才从收集到的信息中，选择最远点造桥。所建造的这座桥的左端点（起跳位置）可能在我们当前走的这座桥的中间。

> 也可以理解成，当发现无路可走的时候，往回跳，选择合适的位置造桥。我们可以在最终算账的时候，把往回跳的情况擦掉，只保留往右跳，也能到达终点。所以往回跳不计入跳跃次数。

## 答疑

**问**：为什么这个贪心策略是对的？

**答**：其实这个做法是对暴力的优化。题目让我们计算到达 $n-1$ 的最小跳跃次数，暴力的做法是，用暴力搜索（或者 BFS）算出跳一次最远可以到哪，跳两次最远可以到哪，跳三次最远可以到哪……跳到 $n-1$ 的最小跳跃次数就是答案。设跳 $k$ 次最远可以到达 $\textit{curEnd}$，那么下标在 $[0,\textit{curEnd}]$ 中的最大的 $i+\textit{nums}[i]$，就是跳 $k+1$ 次最远可以到达的位置 $\textit{nextEnd}$。当我们遍历到 $i=\textit{curEnd}$ 时，通过计算遍历过的 $i+\textit{nums}[i]$ 的最大值，可以算出再跳一步最远能到哪里。这本质是一个递推式（$k\to k+1$），根据这个递推式，就能从跳 $k=0$ 步开始（此时最远到达 $0$），依次递推算出跳 $k=1,2,3,\ldots$ 步后的最远位置。

**问**：为什么代码只遍历到 $n-2$？

**答**：这题 $n-1$ 是终点，$\textit{nums}[n-1]$ 这个数没有任何意义，完全可以把这个数删了，在长为 $n-1$ 的数组上跑这个算法。或者说，遍历到 $\textit{nums}[n-2]$ 时，要么已经可以到达终点，要么需要造最后一座桥。$n-1$ 已经是终点了，不需要造桥。

**问**：如果题目没有保证一定能到达 $n-1$，代码要怎么改？

**答**：见 [1326. 灌溉花园的最少水龙头数目](https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/)，[我的题解](https://leetcode.cn/problems/minimum-number-of-taps-to-open-to-water-a-garden/solutions/2123855/yi-zhang-tu-miao-dong-pythonjavacgo-by-e-wqry/)。

```py [sol-Python3]
class Solution:
    def jump(self, nums: List[int]) -> int:
        ans = 0
        cur_end = 0  # 已建造的桥的右端点
        next_end = 0  # 下一座桥的右端点的最大值
        for i in range(len(nums) - 1):
            # 遍历的过程中，记录下一座桥的最远点
            next_end = max(next_end, i + nums[i])
            if i == cur_end:  # 无路可走，必须建桥
                cur_end = next_end  # 建桥后，最远可以到达 next_end
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int jump(int[] nums) {
        int ans = 0;
        int curEnd = 0; // 已建造的桥的右端点
        int nextEnd = 0; // 下一座桥的右端点的最大值
        for (int i = 0; i < nums.length - 1; i++) {
            // 遍历的过程中，记录下一座桥的最远点
            nextEnd = Math.max(nextEnd, i + nums[i]);
            if (i == curEnd) { // 无路可走，必须建桥
                curEnd = nextEnd; // 建桥后，最远可以到达 nextEnd
                ans++;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int jump(vector<int>& nums) {
        int ans = 0;
        int cur_end = 0; // 已建造的桥的右端点
        int next_end = 0; // 下一座桥的右端点的最大值
        for (int i = 0; i + 1 < nums.size(); i++) {
            // 遍历的过程中，记录下一座桥的最远点
            next_end = max(next_end, i + nums[i]);
            if (i == cur_end) { // 无路可走，必须建桥
                cur_end = next_end; // 建桥后，最远可以到达 next_end
                ans++;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int jump(int* nums, int numsSize) {
    int ans = 0;
    int cur_end = 0; // 已建造的桥的右端点
    int next_end = 0; // 下一座桥的右端点的最大值
    for (int i = 0; i < numsSize - 1; i++) {
        // 遍历的过程中，记录下一座桥的最远点
        next_end = MAX(next_end, i + nums[i]);
        if (i == cur_end) { // 无路可走，必须建桥
            cur_end = next_end; // 建桥后，最远可以到达 next_end
            ans++;
        }
    }
    return ans;
}
```

```go [sol-Go]
func jump(nums []int) (ans int) {
    curEnd := 0  // 已建造的桥的右端点
    nextEnd := 0 // 下一座桥的右端点的最大值
    for i, num := range nums[:len(nums)-1] {
        // 遍历的过程中，记录下一座桥的最远点
        nextEnd = max(nextEnd, i+num)
        if i == curEnd { // 无路可走，必须建桥
            curEnd = nextEnd // 建桥后，最远可以到达 nextEnd
            ans++
        }
    }
    return
}
```

```js [sol-JavaScript]
var jump = function(nums) {
    let ans = 0;
    let curEnd = 0; // 已建造的桥的右端点
    let nextEnd = 0; // 下一座桥的右端点的最大值
    for (let i = 0; i < nums.length - 1; i++) {
        // 遍历的过程中，记录下一座桥的最远点
        nextEnd = Math.max(nextEnd, i + nums[i]);
        if (i === curEnd) { // 无路可走，必须建桥
            curEnd = nextEnd; // 建桥后，最远可以到达 nextEnd
            ans++;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn jump(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut cur_end = 0; // 已建造的桥的右端点
        let mut next_end = 0; // 下一座桥的右端点的最大值
        for i in 0..nums.len()-1 {
            // 遍历的过程中，记录下一座桥的最远点
            next_end = next_end.max(i as i32 + nums[i]);
            if i as i32 == cur_end { // 无路可走，必须建桥
                cur_end = next_end; // 建桥后，最远可以到达 next_end
                ans += 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果要输出具体的跳跃方案（每一步跳到哪个位置），怎么做？

欢迎在评论区分享你的思路/代码。

更多相似题目，见下面贪心题单中的「**区间贪心**」。

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

本题来自 `二、区间贪心 / §2.4 区间覆盖`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.4 区间覆盖`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
