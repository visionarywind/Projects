# 875. 爱吃香蕉的珂珂

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/koko-eating-bananas/
- 题目 slug：`koko-eating-bananas`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：1766
- 外部题解来源：https://leetcode.cn/problems/koko-eating-bananas/solutions/2710324/er-fen-da-an-fu-ti-dan-pythonjavacgojsru-eb18/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/koko-eating-bananas/solutions/2710324/er-fen-da-an-fu-ti-dan-pythonjavacgojsru-eb18/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-fu-ti-dan-pythonjavacgojsru-eb18`
- topic id：`2710324`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 转化

看示例 1，$\textit{piles} = [3,6,7,11]$，$h = 8$。

- 如果珂珂能用 $k=4$ 的速度吃掉所有香蕉，那么也能用更快的速度 $k=5,6,\ldots$ 吃掉所有香蕉。
- 如果珂珂不能用 $k=3$ 的速度吃掉所有香蕉，那么也不能用更慢的速度 $k=2,1,\ldots$ 吃掉所有香蕉。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题转化成一个判定性问题：

- 给定 $k$，**判断**珂珂能否用 $k$ 的速度，在 $h$ 小时内吃掉所有香蕉。

如果可以吃掉所有香蕉，说明答案 $\le k$，否则答案 $> k$。

## 思路

假设一堆香蕉有 $p=\textit{piles}[i]$ 根，那么吃完这堆香蕉需要

$$
\left\lceil\dfrac{p}{k}\right\rceil = \left\lfloor\dfrac{p-1}{k}\right\rfloor+1
$$

小时。上式的证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

如果满足

$$
\sum_{i=0}^{n-1} \left(\left\lfloor\dfrac{\textit{piles}[i]-1}{k}\right\rfloor+1\right) \le h
$$

即

$$
n + \sum_{i=0}^{n-1} \left\lfloor\dfrac{\textit{piles}[i]-1}{k}\right\rfloor \le h
$$

那么珂珂能用 $k$ 的速度，在 $h$ 小时内吃掉所有香蕉。

## 细节

下面代码采用开区间二分。使用闭区间或者半闭半开区间也是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$0$。无法吃香蕉。
- 开区间右端点初始值：$\max(\textit{piles})$。此时用 $1$ 小时一定能吃完 $1$ 堆香蕉，所以必然可以在 $h$ 小时内吃掉所有香蕉。注意题目保证 $n\le h$。

> 对于开区间写法，简单来说 `check(mid) == true` 成立时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

```py [sol-Python3]
class Solution:
    def minEatingSpeed(self, piles: List[int], h: int) -> int:
        n = len(piles)
        left = 0  # 恒为 False
        right = max(piles)  # 恒为 True
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if n + sum((p - 1) // mid for p in piles) <= h:
                right = mid  # 循环不变量：恒为 True
            else:
                left = mid  # 循环不变量：恒为 False
        return right  # 最小的 True
```

```py [sol-Python3 库函数]
class Solution:
    def minEatingSpeed(self, piles: List[int], h: int) -> int:
        check = lambda k: sum((p - 1) // k for p in piles) <= h - len(piles)
        left, right = 1, max(piles)  # 左闭右开区间
        return left + bisect_left(range(left, right), True, key=check)
```

```java [sol-Java]
class Solution {
    public int minEatingSpeed(int[] piles, int h) {
        int left = 0;
        int right = 0;
        for (int p : piles) {
            right = Math.max(right, p);
        }
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            if (check(mid, piles, h)) {
                right = mid; // 循环不变量：恒为 true
            } else {
                left = mid; // 循环不变量：恒为 false
            }
        }
        return right; // 最小的 true
    }

    private boolean check(int mid, int[] piles, int h) {
        int sum = piles.length;
        for (int pile : piles) {
            sum += (pile - 1) / mid;
            if (sum > h) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minEatingSpeed(vector<int>& piles, int h) {
        auto check = [&](int k) -> bool {
            int sum = piles.size();
            for (int p : piles) {
                sum += (p - 1) / k;
                if (sum > h) {
                    return false;
                }
            }
            return true;
        };

        int left = 0; // 循环不变量：恒为 false
        int right = ranges::max(piles); // 循环不变量：恒为 true
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right; // 最小的 true
    }
};
```

```c [sol-C]
int minEatingSpeed(int* piles, int pilesSize, int h) {
    int mx = 0;
    for (int i = 0; i < pilesSize; i++) {
        mx = MAX(mx, piles[i]);
    }

    bool check(int k) {
        int sum = pilesSize;
        for (int i = 0; i < pilesSize; i++) {
            sum += (piles[i] - 1) / k;
            if (sum > h) {
                return false;
            }
        }
        return true;
    }

    int left = 0; // 循环不变量：恒为 false
    int right = mx; // 循环不变量：恒为 true
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right; // 最小的 true
}
```

```go [sol-Go]
func minEatingSpeed(piles []int, h int) int {
    n := len(piles)
    left := 0 // 恒为 false
    right := slices.Max(piles) // 恒为 true
    for left+1 < right { // 开区间不为空
        mid := (left + right) / 2
        sum := n
        for _, p := range piles {
            sum += (p - 1) / mid
        }
        if sum <= h {
            right = mid // 循环不变量：恒为 true
        } else {
            left = mid // 循环不变量：恒为 false
        }
    }
    return right // 最小的 true
}
```

```go [sol-Go 库函数]
func minEatingSpeed(piles []int, h int) int {
    return 1 + sort.Search(slices.Max(piles)-1, func(mid int) bool {
        mid++
        sum := len(piles)
        for _, p := range piles {
            sum += (p - 1) / mid
        }
        return sum <= h
    })
}
```

```js [sol-JavaScript]
var minEatingSpeed = function(piles, h) {
    let left = 0; // 恒为 false
    let right = Math.max(...piles); // 恒为 true
    while (left + 1 < right) { // 开区间不为空
        const mid = Math.floor((left + right) / 2);
        let sum = 0;
        for (const p of piles) {
            sum += Math.ceil(p / mid);
        }
        if (sum <= h) {
            right = mid; // 循环不变量：恒为 true
        } else {
            left = mid; // 循环不变量：恒为 false
        }
    }
    return right; // 最小的 true
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_eating_speed(piles: Vec<i32>, h: i32) -> i32 {
        let check = |k: i32| -> bool {
            let mut sum = piles.len() as i32;
            for &p in &piles {
                sum += (p - 1) / k;
                if sum > h {
                    return false;
                }
            }
            true
        };

        let mut left = 0; // 恒为 false
        let mut right = *piles.iter().max().unwrap(); // 恒为 true
        while left + 1 < right { // 开区间不为空
            let mid = left + (right - left) / 2;
            if check(mid) {
                right = mid; // 循环不变量：恒为 true
            } else {
                left = mid; // 循环不变量：恒为 false
            }
        }
        right // 最小的 true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{piles}$ 的长度，$U=\max(\textit{piles})$。二分 $\mathcal{O}(\log U)$ 次，每次 $\mathcal{O}(n)$ 时间。
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

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
