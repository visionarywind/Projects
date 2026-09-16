# 1686. 石子游戏 VI

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/stone-game-vi/
- 题目 slug：`stone-game-vi`
- 来源专题：数学算法
- 来源分类路径：四、博弈论
- 难度分：2001
- 外部题解来源：https://leetcode.cn/problems/stone-game-vi/solutions/2628498/xiang-xi-jie-shi-wei-shi-yao-yao-an-zhao-0zsg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[详细解释为什么要按照 a[i]+b[i] 排序（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/stone-game-vi/solutions/2628498/xiang-xi-jie-shi-wei-shi-yao-yao-an-zhao-0zsg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xiang-xi-jie-shi-wei-shi-yao-yao-an-zhao-0zsg`
- topic id：`2628498`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 Alice 选的数字之和为 $A$，Bob 选的数字之和为 $B$。如果 $A-B>0$ 那么 Alice 赢，如果 $A-B<0$ 那么 Bob 赢，如果 $A-B=0$ 则平局。所以 Alice 需要最大化 $A-B$，Bob 需要最小化 $A-B$。

下文把数组 $\textit{aliceValues}$ 记作 $a$，把数组 $\textit{bobValues}$ 记作 $b$。

以 $a=[2,4,3,5],\ b=[1,6,7,1]$ 为例说明。

假设 Bob 把所有石子都拿走，则 $A=0,\ B=15,\ A-B=-15$。

先来想一想，如果 Alice 只能拿走一颗石子，应该拿走哪颗呢？

- 拿走第一颗，那么 $A=2,\ B=14,\ A-B=-12$。
- 拿走第二颗，那么 $A=4,\ B=9,\ A-B=-5$。
- 拿走第三颗，那么 $A=3,\ B=8,\ A-B=-5$。
- 拿走第四颗，那么 $A=5,\ B=14,\ A-B=-9$。

对比 Bob 把所有石子都拿走的情况，如果 Alice 拿走第二颗或者第三颗，都可以让 $-15$ 增大为 $-5$，增量为 $10$。由于 $A$ 增加了 $a[i]$，$B$ 减少了 $b[i]$，所以 $A-B$ 的增量等于 

$$
a[i] - (-b[i]) = a[i] + b[i]
$$

所以 Alice 拿走 $a[i]+b[i]$ 最大的石子最优。

如果 Alice 可以拿走两颗石子，应该拿走哪两颗呢？

我们从 Bob 把所有石子都拿走的情况出发，也就是在 $A=0,\ B=15$ 的基础上思考，Alice 拿走哪两颗石子，可以让 $A-B$ 增加的尽量多？

定义 $c[i]=a[i]+b[i]$，那么 $c=[3,10,10,6]$。现在问题变成：给定数组 $c$，**Alice 每回合拿走一个数，Bob 每回合删除一个数，Alice 拿走的数之和最大是多少**？注意 Bob 要让 Alice 拿走的数之和尽量小。

如此转换后，贪心策略就很显然了：Alice 从大到小拿走数字，Bob 也从大到小删除数字。

所以把 $c$ 从大到小排序为 $[10,10,6,3]$，两人从左往右交替取数，那么 Alice 只能拿走下标为偶数的数字，其余数字被 Bob 删除。所以 $A-B$ 最大可以增加 $c[0]+c[2]=10+6=16$，增加后 $A-B=1>0$，Alice 险胜！

## 算法

1. 把数组按照 $a[i]+b[i]$ 从大到小排序。可以创建一个 $(a[i],b[i])$ 的 pair 数组对其排序，也可以创建一个下标数组排序。
2. 用 $\textit{diff}$ 表示 $A-B$，初始化 $\textit{diff}=0$。遍历数组，把偶数下标的 $a[i]$ 加到 $A$ 中，相当于 $\textit{diff}$ 增加了 $a[i]$；把奇数下标的 $b[i]$ 加到 $B$ 中，相当于 $\textit{diff}$ 减少了 $b[i]$。
3. 循环结束后，如果 $\textit{diff}>0$，返回 $1$；如果 $\textit{diff}<0$，返回 $-1$；如果 $\textit{diff}=0$，返回 $0$。

## 答疑

**问**：从这个思路的本质是什么？为什么这样转换一下，问题就变得简单了许多？

**答**：转换前，我们需要同时考虑 $a[i]$ 和 $b[i]$ 这**两个变量**，不好处理。转换成 Bob 先把所有 $b[i]$ 选了，我们就只需要思考 Alice 如何选数字，只有 $c[i]$ 这**一个变量**，更容易处理。从某种程度上来说，这也可以算作一种「正难则反」。

**问**：有没有其它的思考方式？

**答**：也可以这样思考：对比两颗石子 $(a[i],b[i])$ 和 $(a[j],b[j])$。如果 Alice 选 $i$，Bob 选 $j$，那么 $A-B=a[i]-b[j]$；如果 Alice 选 $j$，Bob 选 $i$，那么 $A-B=a[j]-b[i]$。如果 Alice 选 $i$ 更优，则有 $a[i]-b[j] > a[j]-b[i]$，即 $a[i]+b[i] > a[j]+b[j]$，说明 Alice 应当优先选 $a[i]+b[i]$ 更大的石子。

```py [sol-Python3]
class Solution:
    def stoneGameVI(self, a: List[int], b: List[int]) -> int:
        pairs = sorted(zip(a, b), key=lambda p: -p[0] - p[1])
        diff = sum(x if i % 2 == 0 else -y for i, (x, y) in enumerate(pairs))
        return (diff > 0) - (diff < 0)
```

```py [sol-Python3 写法二]
class Solution:
    def stoneGameVI(self, a: List[int], b: List[int]) -> int:
        s = sorted((x + y for x, y in zip(a, b)), reverse=True)
        diff = sum(s[::2]) - sum(b)
        return (diff > 0) - (diff < 0)
```

```java [sol-Java]
class Solution {
    public int stoneGameVI(int[] a, int[] b) {
        int n = a.length;
        Integer[] idx = new Integer[n];
        Arrays.setAll(idx, i -> i);
        Arrays.sort(idx, (i, j) -> a[j] + b[j] - a[i] - b[i]);

        int diff = 0;
        for (int i = 0; i < n; i++) {
            diff += i % 2 == 0 ? a[idx[i]] : -b[idx[i]];
        }
        return Integer.compare(diff, 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int stoneGameVI(vector<int>& a, vector<int>& b) {
        int n = a.size();
        vector<int> idx(n);
        ranges::iota(idx, 0);
        ranges::sort(idx, {}, [&](int i) { return -a[i] - b[i]; });

        int diff = 0;
        for (int i = 0; i < n; i++) {
            diff += i % 2 == 0 ? a[idx[i]] : -b[idx[i]];
        }
        return (diff > 0) - (diff < 0);
    }
};
```

```go [sol-Go]
func stoneGameVI(a, b []int) int {
    type pair struct{ x, y int }
    pairs := make([]pair, len(a))
    for i, x := range a {
        pairs[i] = pair{x, b[i]}
    }
    slices.SortFunc(pairs, func(p, q pair) int { return q.x + q.y - p.x - p.y })

    diff := 0
    for i, p := range pairs {
        if i%2 == 0 {
            diff += p.x
        } else {
            diff -= p.y
        }
    }
    return cmp.Compare(diff, 0)
}
```

```js [sol-JavaScript]
var stoneGameVI = function(a, b) {
    const pairs = _.zip(a, b).sort((p, q) => q[0] + q[1] - p[0] - p[1]);
    let diff = 0;
    for (let i = 0; i < pairs.length; i++) {
        diff += i % 2 === 0 ? pairs[i][0] : -pairs[i][1];
    }
    return Math.sign(diff);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn stone_game_vi(a: Vec<i32>, b: Vec<i32>) -> i32 {
        let mut pairs = a.into_iter().zip(b).collect::<Vec<_>>();
        pairs.sort_unstable_by_key(|(x, y)| -x - y);
        pairs.into_iter()
            .enumerate()
            .map(|(i, (x, y))| if i % 2 == 0 { x } else { -y })
            .sum::<i32>()
            .signum()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $a$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `四、博弈论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、博弈论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
