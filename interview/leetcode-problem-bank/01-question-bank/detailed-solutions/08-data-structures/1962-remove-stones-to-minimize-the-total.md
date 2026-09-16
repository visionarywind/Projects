# 1962. 移除石子使总数最小

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-stones-to-minimize-the-total/
- 题目 slug：`remove-stones-to-minimize-the-total`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1419
- 外部题解来源：https://leetcode.cn/problems/remove-stones-to-minimize-the-total/solutions/2576668/yuan-di-dui-hua-o1-kong-jian-pythonjavac-29iz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[原地堆化 O(1) 空间（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/remove-stones-to-minimize-the-total/solutions/2576668/yuan-di-dui-hua-o1-kong-jian-pythonjavac-29iz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yuan-di-dui-hua-o1-kong-jian-pythonjavac-29iz`
- topic id：`2576668`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

每次操作，应当选数组中最大的数，移除它的一半（下取整）。

动态维护数组的最大值，可以**最大堆**模拟。

循环 $k$ 次。每次循环，弹出堆顶 $\textit{x}$，然后把 $x-\left\lfloor\dfrac{x}{2}\right\rfloor = \left\lceil\dfrac{x}{2}\right\rceil$ 入堆。

循环结束后，堆中所有元素之和就是答案。

#### 优化

1. 如果堆顶等于 $1$，说明堆中所有元素都为 $1$，操作无法修改任何元素，可以直接退出循环。
2. 原地堆化（heapify）可以做到 $\mathcal{O}(1)$ 的空间复杂度。部分语言用的标准库自带的堆化函数。关于堆化是如何实现的，可以看下面的 Java 代码。

```py [sol-Python3]
class Solution:
    def minStoneSum(self, piles: List[int], k: int) -> int:
        heapify_max(piles)  # 原地堆化
        while k and piles[0] != 1:
            heapreplace_max(piles, (piles[0] + 1) // 2)
            k -= 1
        return sum(piles)
```

```java [sol-Java]
class Solution {
    public int minStoneSum(int[] piles, int k) {
        heapify(piles); // 原地堆化（最大堆）
        while (k-- > 0 && piles[0] != 1) {
            piles[0] -= piles[0] / 2; // 直接修改堆顶
            sink(piles, 0); // 堆化（只需要把 piles[0] 下沉）
        }

        int ans = 0;
        for (int x : piles) {
            ans += x;
        }
        return ans;
    }

    // 原地堆化（最大堆）
    // 堆化可以保证 h[0] 是堆顶元素，且 h[i] >= max(h[2*i+1], h[2*i+2])
    private void heapify(int[] h) {
        // 倒着遍历，从而保证 i 的左右子树一定是堆，那么 sink(h, i) 就可以把左右子树合并成一个堆
        // 下标 >= h.length / 2 的元素是二叉树的叶子，无需下沉
        for (int i = h.length / 2 - 1; i >= 0; i--) {
            sink(h, i);
        }
    }

    // 把 h[i] 不断下沉，每次找左右儿子中最大的交换，直到 i 的左右儿子都 <= h[i] 时停止
    private void sink(int[] h, int i) {
        int n = h.length;
        while (2 * i + 1 < n) {
            int j = 2 * i + 1; // i 的左儿子
            if (j + 1 < n && h[j + 1] > h[j]) { // i 的右儿子比 i 的左儿子大
                j++;
            }
            if (h[j] <= h[i]) { // 说明 i 的左右儿子都 <= h[i]，停止下沉
                break;
            }
            swap(h, i, j); // 下沉
            i = j;
        }
    }

    // 交换 h[i] 和 h[j]
    private void swap(int[] h, int i, int j) {
        int tmp = h[i];
        h[i] = h[j];
        h[j] = tmp;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minStoneSum(vector<int> &piles, int k) {
        make_heap(piles.begin(), piles.end()); // 原地堆化（最大堆）
        while (k-- && piles[0] != 1) {
            pop_heap(piles.begin(), piles.end()); // 弹出堆顶并移到末尾
            piles.back() -= piles.back() / 2;
            push_heap(piles.begin(), piles.end()); // 把末尾元素入堆
        }
        return accumulate(piles.begin(), piles.end(), 0);
    }
};
```

```go [sol-Go]
func minStoneSum(piles []int, k int) (ans int) {
    h := &hp{piles}
    heap.Init(h) // 原地堆化
    for ; k > 0 && piles[0] != 1; k-- {
        piles[0] -= piles[0] / 2 // 直接修改堆顶
        heap.Fix(h, 0)
    }
    for _, x := range piles {
        ans += x
    }
    return
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] } // 最大堆
func (hp) Pop() (_ any)         { return }
func (hp) Push(any)             {}
```

```js [sol-JavaScript]
var minStoneSum = function (piles, k) {
    heapify(piles); // 堆化
    while (k-- && piles[0] !== 1) {
        piles[0] -= Math.floor(piles[0] / 2); // 直接修改堆顶
        sink(piles, 0); // 堆化（只需要把 piles[0] 下沉）
    }
    return _.sum(piles);
};

// 原地堆化（最大堆）
// 堆化可以保证 h[0] 是堆顶元素，且 h[i] >= max(h[2*i+1], h[2*i+2])
function heapify(h) {
    // 倒着遍历，从而保证 i 的左右子树一定是堆，那么 sink(h, i) 就可以把左右子树合并成一个堆
    // 下标 >= h.length / 2 的元素是二叉树的叶子，无需下沉
    for (let i = Math.floor(h.length / 2) - 1; i >= 0; i--) {
        sink(h, i);
    }
}

// 把 h[i] 不断下沉，直到 i 的左右儿子都 <= h[i]
function sink(h, i) {
    const n = h.length;
    while (2 * i + 1 < n) {
        let j = 2 * i + 1; // i 的左儿子
        if (j + 1 < n && h[j + 1] > h[j]) { // i 的右儿子比 i 的左儿子大
            j++;
        }
        if (h[j] <= h[i]) { // 说明 i 的左右儿子都 <= h[i]，停止下沉
            break;
        }
        [h[i], h[j]] = [h[j], h[i]]; // 下沉
        i = j;
    }
}
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn min_stone_sum(piles: Vec<i32>, k: i32) -> i32 {
        let mut h = BinaryHeap::from(piles); // 原地堆化（最大堆）
        for _ in 0..k {
            let top = h.pop().unwrap();
            h.push((top + 1) / 2);
            if *h.peek().unwrap() == 1 {
                break;
            }
        }
        h.iter().sum()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + k\log n)$，其中 $n$ 为 $\textit{piles}$ 的长度。堆化需要 $\mathcal{O}(n)$ 的时间（证明见下）。每次修改堆顶，需要 $\mathcal{O}(\log n)$ 的时间。所以总的时间复杂度为 $\mathcal{O}(n + k\log n)$。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

#### 关于堆化的时间复杂度

从 Java 代码的实现上看，堆化（heapify）的时间复杂度「似乎」是 $\mathcal{O}(n\log n)$ 的，果真如此吗？注意堆中大部分节点都集中在下层，`sink` 中的循环没几次就结束了。假设堆是一棵满二叉树，那么倒数第二层有 $\left\lceil\dfrac{n}{4}\right\rceil$ 个节点，这些节点在 `sink` 中至多循环一次。倒数第三层有 $\left\lceil\dfrac{n}{8}\right\rceil$ 个节点，这些节点在 `sink` 中至多循环两次。依此类推，总的循环次数至多为

$$
\left\lceil\dfrac{n}{2^2}\right\rceil\cdot 1 + \left\lceil\dfrac{n}{2^3}\right\rceil\cdot 2 + \left\lceil\dfrac{n}{2^4}\right\rceil\cdot 3 + \cdots + \left\lceil\dfrac{n}{2^{k+1}}\right\rceil\cdot k
$$

其中 $k = \left\lfloor\log_2{n}\right\rfloor$。

在计算大 $\mathcal{O}$ 记号时，上取整符号可以忽略，从而得到

$$
\mathcal{O}\left(n\cdot\left(\dfrac{1}{2^2} + \dfrac{2}{2^3} + \cdots + \dfrac{k}{2^{k+1}}\right)\right)
$$

设 

$$
S = \dfrac{1}{2^2} + \dfrac{2}{2^3} +  \dfrac{3}{2^4} + \cdots + \dfrac{k}{2^{k+1}}
$$

那么

$$
2S = \dfrac{1}{2} + \dfrac{2}{2^2} + \dfrac{3}{2^3}\cdots + \dfrac{k}{2^k}
$$

所以

$$
S = 2S - S = \dfrac{1}{2} + \dfrac{1}{2^2} + \dfrac{1}{2^3} + \cdots + \dfrac{1}{2^k} - \dfrac{k}{2^{k+1}} < 1- \dfrac{k}{2^{k+1}} < 1
$$

所以堆化的时间复杂度为 $\mathcal{O}(n)$。

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

本题来自 `五、堆（优先队列） / §5.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
