# LCP 30. 魔塔游戏

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/p0NxJO/
- 题目 slug：`p0NxJO`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.5 反悔堆
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/p0NxJO/solutions/2633172/fan-hui-tan-xin-fu-ti-dan-pythonjavacgoj-hxup/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[反悔贪心（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/p0NxJO/solutions/2633172/fan-hui-tan-xin-fu-ti-dan-pythonjavacgoj-hxup/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fan-hui-tan-xin-fu-ti-dan-pythonjavacgoj-hxup`
- topic id：`2633172`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先，如果 $\textit{nums}$ 的元素和小于 $0$，那么即使把所有负数都移到末尾，也无法访问所有房间（最终血量必然小于 $1$），返回 $-1$。

否则遍历数组，能加血就尽管加血，要扣血就直接扣血，但如果血量小于 $1$，我们就「反悔」：从前面的扣血中，拿出一个扣血量最大的数（最小的负数），移到数组的末尾，把之前扣掉的血重新加回来。

> 注：实际上不需要真的移到末尾，因为我们保证了数组元素和非负，最后血量必然是正数，所以无需再次遍历移到数组末尾的数字。

具体来说：

1. 初始化血量 $\textit{hp}=1$。
2. 从左到右遍历数组，把小于 $0$ 的数丢到一个小根堆中。
3. 遍历的同时，把 $\textit{nums}[i]$ 加到 $\textit{hp}$ 中。如果 $\textit{hp}<1$，那么弹出堆顶，$\textit{hp}$ 减去堆顶，相当于把之前扣掉的血重新加回来。同时把调整次数增加一。注意如果 $\textit{hp}<1$，那么必然是由当前这个小于 $0$ 的 $\textit{nums}[i]$ 导致的，这一保证了此时堆不为空，二保证了 $\textit{hp}$ 减去堆顶后必然可以恢复成正数，因为堆顶不会比 $\textit{nums}[i]$ 还大。
4. 返回调整次数。

```py [sol-Python3]
class Solution:
    def magicTower(self, nums: List[int]) -> int:
        if sum(nums) < 0:
            return -1
        ans = 0
        hp = 1
        h = []
        for x in nums:
            if x < 0:
                heappush(h, x)
            hp += x
            if hp < 1:
                # 这意味着 x < 0，所以前面必然会把 x 入堆
                # 所以堆必然不是空的，并且堆顶 <= x
                hp -= heappop(h)  # 反悔
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int magicTower(int[] nums) {
        long sum = 0;
        for (int x : nums) {
            sum += x;
        }
        if (sum < 0) {
            return -1;
        }

        int ans = 0;
        long hp = 1;
        PriorityQueue<Integer> h = new PriorityQueue<>();
        for (int x : nums) {
            if (x < 0) {
                h.offer(x);
            }
            hp += x;
            if (hp < 1) {
                // 这意味着 x < 0，所以前面必然会把 x 入堆
                // 所以堆必然不是空的，并且堆顶 <= x
                hp -= h.poll(); // 反悔
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
    int magicTower(vector<int> &nums) {
        if (accumulate(nums.begin(), nums.end(), 0LL) < 0) {
            return -1;
        }
        int ans = 0;
        long long hp = 1;
        priority_queue<int> h; // 最大堆
        for (int x : nums) {
            if (x < 0) {
                h.push(-x);
            }
            hp += x;
            if (hp < 1) {
                // 这意味着 x < 0，所以前面必然会把 x 入堆
                // 所以堆必然不是空的，并且堆顶 >= -x
                hp += h.top(); // 反悔
                h.pop();
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func magicTower(nums []int) (ans int) {
    sum := 0
    for _, x := range nums {
        sum += x
    }
    if sum < 0 {
        return -1
    }

    hp := 1
    h := &minHeap{}
    for _, x := range nums {
        if x < 0 {
            heap.Push(h, x)
        }
        hp += x
        if hp < 1 {
            // 这意味着 x < 0，所以前面必然会把 x 入堆
            // 所以堆必然不是空的，并且堆顶 <= x
            hp -= heap.Pop(h).(int) // 反悔
            ans++
        }
    }
    return
}

type minHeap struct{ sort.IntSlice } // 继承 Len, Less, Swap
func (h *minHeap) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *minHeap) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var magicTower = function(nums) {
    if (_.sum(nums) < 0) {
        return -1;
    }

    const pq = new MinPriorityQueue();
    let ans = 0;
    let hp = 1;
    for (const x of nums) {
        if (x < 0) {
            pq.enqueue(x);
        }
        hp += x;
        if (hp < 1) {
            // 这意味着 x < 0，所以前面必然会把 x 入堆
            // 所以堆必然不是空的，并且堆顶 <= x
            hp -= pq.dequeue(); // 反悔
            ans++;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn magic_tower(nums: Vec<i32>) -> i32 {
        if nums.iter().map(|&x| x as i64).sum::<i64>() < 0 {
            return -1;
        }
        let mut ans = 0;
        let mut hp = 1i64;
        let mut h = BinaryHeap::new(); // 最大堆
        for &x in &nums {
            if x < 0 {
                h.push(-x);
            }
            hp += x as i64;
            if hp < 1 {
                // 这意味着 x < 0，所以前面必然会把 x 入堆
                // 所以堆必然不是空的，并且堆顶 >= -x
                hp += h.pop().unwrap() as i64; // 反悔
                ans += 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面贪心题单的「**§1.9 反悔贪心**」。

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

本题来自 `五、堆（优先队列） / §5.5 反悔堆`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.5 反悔堆`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
