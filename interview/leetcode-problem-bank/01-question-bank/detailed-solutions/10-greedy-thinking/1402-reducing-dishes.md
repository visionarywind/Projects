# 1402. 做菜顺序

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reducing-dishes/
- 题目 slug：`reducing-dishes`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.3 排序不等式
- 难度分：1679
- 外部题解来源：https://leetcode.cn/problems/reducing-dishes/solutions/2492854/mei-ju-zuo-ji-dao-cai-tan-xin-pythonjava-k7w2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举做几道菜+贪心（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/reducing-dishes/solutions/2492854/mei-ju-zuo-ji-dao-cai-tan-xin-pythonjava-k7w2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-zuo-ji-dao-cai-tan-xin-pythonjava-k7w2`
- topic id：`2492854`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

为了简化描述，下文把 $\textit{satisfaction}$ 记作 $a$。

## 提示 1

枚举做 $k=0,1,2,\cdots,n$ 道菜。

假设要做 $2$ 道菜，应该选择 $a$ 中的哪两个数呢？

由于 $a[i]$ 越大，like-time 系数越大，所以应该选择 $a$ 中最大的两个数。

## 提示 2

假设 $a$ 中最大的两个数分别是 $4$ 和 $3$，先做哪道菜，后做哪道菜？

- 如果先 $4$ 后 $3$，总和为 $1\cdot 4 + 2\cdot 3 = 10$。
- 如果先 $3$ 后 $4$，总和为 $1\cdot 3 + 2\cdot 4 = 11$。

这说明 $a[i]$ 大的菜应该后做。可以根据 [排序不等式](https://baike.baidu.com/item/%E6%8E%92%E5%BA%8F%E4%B8%8D%E7%AD%89%E5%BC%8F/7775728) 得到该结论。

## 提示 3

为了方便计算，把 $a$ **从大到小**排序。

来看看做 $k=1,2,3$ 道菜时，对应的总和 $f(k)$ 是多少。

- $k=1$ 时，总和为 $f(1)=a[0]$。
- $k=2$ 时，总和为 $f(2)=2\cdot a[0] + a[1]$。
- $k=3$ 时，总和为 $f(3)=3\cdot a[0] + 2\cdot a[1] + a[2]$。

为了快速地算出每个 $f(k)$，我们需要找到 $f(k)$ 的**递推式**。观察上面列出的式子，你能找到递推式吗？

先把 $f(k)$ 的式子列出来：

$$
f(k)=k\cdot a[0] + (k-1)\cdot a[1] + \cdots + 2\cdot a[k-2] + a[k-1]
$$

每一项去掉一个 $a[i]$，得到：

$$
(k-1)\cdot a[0] + (k-2)\cdot a[1] + \cdots + a[k-2]
$$

这正是 $f(k-1)$。

所以有

$$
f(k) = f(k-1) + (a[0] + a[1] + \cdots + a[k-1])
$$

右边的和式是 $a$ 的前缀和，我们可以一边遍历 $a$，一边把 $a[i]$ 累加到一个变量 $s$ 中。这样就可以 $\mathcal{O}(1)$ 地从 $f(k-1)$ 递推得到 $f(k)$ 了。

答案为 $f(0), f(1), f(2),\cdots, f(n)$ 中的最大值。

## 实现细节

想一想 $s$ 是怎么变化的。由于数组是从大到小排序的，（一般地）会先遇到正数，再遇到负数，所以（一般地）$s$ 会先变大，再变小。

如果 $s\le 0$，那么后面的 $a[i]$ 必然都是负数，我们不可能得到更大的 $f(k)$，退出循环。

代码实现时，可以只用一个变量表示 $f$。由于在退出循环之前 $s$ 都是大于 $0$ 的，所以 $f(k) > f(k-1)$，因此退出循环时的 $f$ 就是最终答案。

```py [sol-Python3]
class Solution:
    def maxSatisfaction(self, satisfaction: List[int]) -> int:
        satisfaction.sort(reverse=True)
        f = 0  # f(0) = 0
        for s in accumulate(satisfaction):  # satisfaction 的前缀和
            if s <= 0:  # 后面不可能找到更大的 f(k)
                break
            f += s  # f(k) = f(k-1) + s
        return f
```

```java [sol-Java]
class Solution {
    public int maxSatisfaction(int[] satisfaction) {
        Arrays.sort(satisfaction);
        int f = 0; // f(0) = 0
        int s = 0;
        for (int i = satisfaction.length - 1; i >= 0; i--) {
            s += satisfaction[i];
            if (s <= 0) { // 后面不可能找到更大的 f(k)
                break;
            }
            f += s; // f(k) = f(k-1) + s
        }
        return f;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSatisfaction(vector<int>& satisfaction) {
        ranges::sort(satisfaction, greater()); // 从大到小排序
        int f = 0; // f(0) = 0
        int s = 0; // satisfaction 的前缀和
        for (int x : satisfaction) {
            s += x;
            if (s <= 0) { // 后面不可能找到更大的 f(k)
                break;
            }
            f += s; // f(k) = f(k-1) + s
        }
        return f;
    }
};
```

```go [sol-Go]
func maxSatisfaction(satisfaction []int) int {
    slices.SortFunc(satisfaction, func(a, b int) int { return b - a })
    f := 0 // f(0) = 0
    s := 0 // satisfaction 的前缀和
    for _, x := range satisfaction {
        s += x
        if s <= 0 { // 后面不可能找到更大的 f(k)
            break
        }
        f += s // f(k) = f(k-1) + s
    }
    return f
}
```

```js [sol-JavaScript]
var maxSatisfaction = function(satisfaction) {
    satisfaction.sort((a, b) => b - a);
    let f = 0; // f(0) = 0
    let s = 0; // satisfaction 的前缀和
    for (const x of satisfaction) {
        s += x;
        if (s <= 0) { // 后面不可能找到更大的 f(k)
            break;
        }
        f += s; // f(k) = f(k-1) + s
    }
    return f;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_satisfaction(mut satisfaction: Vec<i32>) -> i32 {
        satisfaction.sort_unstable_by(|a, b| b.cmp(a));
        let mut f = 0; // f(0) = 0
        let mut s = 0; // satisfaction 的前缀和
        for &x in &satisfaction {
            s += x;
            if s <= 0 { // 后面不可能找到更大的 f(k)
                break;
            }
            f += s; // f(k) = f(k-1) + s
        }
        f
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $a$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

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

本题来自 `四、数学贪心 / §4.3 排序不等式`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.3 排序不等式`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
