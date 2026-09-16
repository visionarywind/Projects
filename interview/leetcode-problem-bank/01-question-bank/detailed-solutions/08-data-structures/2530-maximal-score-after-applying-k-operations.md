# 2530. 执行 K 次操作后的最大分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximal-score-after-applying-k-operations/
- 题目 slug：`maximal-score-after-applying-k-operations`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1386
- 外部题解来源：https://leetcode.cn/problems/maximal-score-after-applying-k-operations/solutions/2487446/o1-kong-jian-zuo-fa-pythonjavacgojsrust-ztx6f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(1) 空间做法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximal-score-after-applying-k-operations/solutions/2487446/o1-kong-jian-zuo-fa-pythonjavacgojsrust-ztx6f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-kong-jian-zuo-fa-pythonjavacgojsrust-ztx6f`
- topic id：`2487446`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

用一个最大堆模拟。循环 $k$ 次，每次循环把堆顶加入答案，然后把堆顶 $\textit{top}$ 更新为 $\left\lceil\dfrac{\textit{top}}{3}\right\rceil = \left\lfloor\dfrac{\textit{top}+2}{3}\right\rfloor$。

原地堆化（heapify）可以做到 $\mathcal{O}(1)$ 额外空间。

部分语言用的标准库自带的堆化函数，具体实现可以看下面的 Java 代码。

[视频讲解](https://www.bilibili.com/video/BV1KG4y1j73o/) 第二题。

```py [sol-Python3]
class Solution:
    def maxKelements(self, nums: List[int], k: int) -> int:
        heapify_max(nums)  # 原地堆化
        ans = 0
        for _ in range(k):
            ans += heapreplace_max(nums, (nums[0] + 2) // 3)
        return ans
```

```java [sol-Java]
class Solution {
    public long maxKelements(int[] nums, int k) {
        heapify(nums); // 原地堆化（最大堆）
        long ans = 0;
        while (k-- > 0) {
            ans += nums[0]; // 堆顶
            nums[0] = (nums[0] + 2) / 3;
            sink(nums, 0); // 堆化（只需要把 nums[0] 下沉）
        }
        return ans;
    }

    // 原地堆化（最大堆）
    // 堆化可以保证 h[0] 是堆顶元素，且 h[i] >= max(h[2*i+1], h[2*i+2])
    private void heapify(int[] h) {
        // 下标 >= h.length / 2 的元素是二叉树的叶子，无需下沉
        // 倒着遍历，从而保证 i 的左右子树一定是堆，那么 sink(h, i) 就可以把左右子树合并成一个堆
        for (int i = h.length / 2 - 1; i >= 0; i--) {
            sink(h, i);
        }
    }

    // 把 h[i] 不断下沉，直到 i 的左右儿子都 <= h[i]
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
    long long maxKelements(vector<int> &nums, int k) {
        priority_queue<int> pq(less<int>(), move(nums));
        long long ans = 0;
        while (k--) {
            int x = pq.top();
            pq.pop();
            ans += x;
            pq.push((x + 2) / 3);
        }
        return ans;
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    long long maxKelements(vector<int> &nums, int k) {
        make_heap(nums.begin(), nums.end()); // 原地堆化（最大堆）
        long long ans = 0;
        while (k--) {
            pop_heap(nums.begin(), nums.end()); // 把堆顶移到末尾
            ans += nums.back();
            nums.back() = (nums.back() + 2) / 3;
            push_heap(nums.begin(), nums.end()); // 把末尾元素入堆
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxKelements(nums []int, k int) (ans int64) {
	h := hp{nums}
	heap.Init(&h) // 原地堆化
	for ; k > 0; k-- {
		ans += int64(h.IntSlice[0]) // 堆顶
		h.IntSlice[0] = (h.IntSlice[0] + 2) / 3
		heap.Fix(&h, 0)
	}
	return
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] } // 最大堆
func (hp) Push(any)             {}
func (hp) Pop() (_ any)         { return }
```

```js [sol-JavaScript]
var maxKelements = function (nums, k) {
    heapify(nums); // 堆化
    let ans = 0;
    while (k--) {
        ans += nums[0]; // 堆顶
        nums[0] = Math.floor((nums[0] + 2) / 3);
        sink(nums, 0); // 堆化（只需要把 nums[0] 下沉）
    }
    return ans;
};

// 原地堆化（最大堆）
// 堆化可以保证 h[0] 是堆顶元素，且 h[i] >= max(h[2*i+1], h[2*i+2])
function heapify(h) {
    // 下标 >= h.length / 2 的元素是二叉树的叶子，无需下沉
    // 倒着遍历，从而保证 i 的左右子树一定是堆，那么 sink(h, i) 就可以把左右子树合并成一个堆
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
    pub fn max_kelements(nums: Vec<i32>, k: i32) -> i64 {
        let mut h = BinaryHeap::from(nums); // 原地堆化（最大堆）
        let mut ans = 0i64;
        for _ in 0..k {
            let mx = h.pop().unwrap();
            ans += mx as i64;
            h.push((mx + 2) / 3);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + k\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。把 $\textit{nums}$ 堆化需要 $\mathcal{O}(n)$ 时间。
- 空间复杂度：$\mathcal{O}(1)$。

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
