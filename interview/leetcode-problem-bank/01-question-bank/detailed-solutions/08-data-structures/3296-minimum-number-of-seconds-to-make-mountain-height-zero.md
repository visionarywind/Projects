# 3296. 移山所需的最少秒数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-seconds-to-make-mountain-height-zero/
- 题目 slug：`minimum-number-of-seconds-to-make-mountain-height-zero`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1695
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-seconds-to-make-mountain-height-zero/solutions/2925848/er-fen-da-an-pythonjavacgo-by-endlessche-myg4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：最小堆模拟/二分答案（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-seconds-to-make-mountain-height-zero/solutions/2925848/er-fen-da-an-pythonjavacgo-by-endlessche-myg4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-pythonjavacgo-by-endlessche-myg4`
- topic id：`2925848`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：最小堆模拟

循环 $\textit{mountainHeight}$ 次，每次选一个「工作后总用时」最短的工人，把山的高度降低 $1$。

注意工人们是**同时**工作的，这个算法不是贪心，是按照事件发生顺序的模拟。

[本题视频讲解](https://www.bilibili.com/video/BV1WRtDejEjD/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minNumberOfSeconds(self, mountainHeight: int, workerTimes: List[int]) -> int:
        h = [(t, t, t) for t in workerTimes]
        heapify(h)

        for _ in range(mountainHeight):
            # 工作后总用时，当前工作（山高度降低 1）用时，workerTimes[i]
            total, cur, base = h[0]
            heapreplace(h, (total + cur + base, cur + base, base))
        return total  # 最后一个出堆的 total 即为答案
```

```java [sol-Java]
class Solution {
    public long minNumberOfSeconds(int mountainHeight, int[] workerTimes) {
        PriorityQueue<long[]> pq = new PriorityQueue<>((a, b) -> Long.compare(a[0], b[0]));
        for (int t : workerTimes) {
            pq.offer(new long[]{t, t, t});
        }

        long ans = 0;
        while (mountainHeight-- > 0) {
            // 工作后总用时，当前工作（山高度降低 1）用时，workerTimes[i]
            long[] top = pq.poll();
            long total = top[0], cur = top[1], base = top[2];
            ans = total; // 最后一个出堆的 total 即为答案
            pq.offer(new long[]{total + cur + base, cur + base, base});
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minNumberOfSeconds(int mountainHeight, vector<int>& workerTimes) {
        priority_queue<tuple<long long, long long, int>, vector<tuple<long long, long long, int>>, greater<>> pq;
        for (int t : workerTimes) {
            pq.emplace(t, t, t);
        }

        long long ans = 0;
        while (mountainHeight--) {
            // 工作后总用时，当前工作（山高度降低 1）用时，workerTimes[i]
            auto [total, cur, base] = pq.top(); pq.pop();
            ans = total; // 最后一个出堆的 total 即为答案
            pq.emplace(total + cur + base, cur + base, base);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minNumberOfSeconds(mountainHeight int, workerTimes []int) int64 {
	h := make(hp, len(workerTimes))
	for i, t := range workerTimes {
		h[i] = worker{t, t, t}
	}
	heap.Init(&h)

	ans := 0
	for range mountainHeight {
		ans = h[0].total // 最后一个出堆的 total 即为答案
		h[0].cur += h[0].base
		h[0].total += h[0].cur
		heap.Fix(&h, 0)
	}
	return int64(ans)
}

// 工作后总用时，当前工作（山高度降低 1）用时，workerTimes[i]
type worker struct{ total, cur, base int }
type hp []worker
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].total < h[j].total }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (hp) Push(any)             {}
func (hp) Pop() (_ any)         { return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\textit{mountainHeight}\log n)$，其中 $n$ 是 $\textit{workerTimes}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：二分答案

由于花的时间越多，能够降低的高度也越多，所以有**单调性**，可以二分答案。

问题变成：

- 每个工人**至多**花费 $m$ 秒，总共降低的高度是多少？能否大于等于 $\textit{mountainHeight}$？

遍历 $\textit{workerTimes}$，设 $t=\textit{workerTimes}[i]$，那么有

$$
t + 2t+ \cdots + xt = t\cdot \dfrac{x(x+1)}{2} \le m
$$

即

$$
\dfrac{x(x+1)}{2} \le \left\lfloor\dfrac{m}{t}\right\rfloor = k
$$

解得

$$
x \le \dfrac{-1 + \sqrt{1 + 8k}}{2}
$$

所以第 $i$ 名工人可以把山的高度降低

$$
\left\lfloor \dfrac{-1 + \sqrt{1 + 8k}}{2} \right\rfloor = \left\lfloor \dfrac{-1 + \lfloor\sqrt{1 + 8k}\rfloor}{2} \right\rfloor
$$

上式是个关于下取整的恒等式，理由见 [下取整恒等式及其应用](https://zhuanlan.zhihu.com/p/1893240318645732760)。

累加上式，如果和 $\ge \textit{mountainHeight}$，则说明答案 $\le m$，否则说明答案 $> m$。

最后，讨论二分的上下界。这里用开区间二分，其他二分写法也是可以的。

- 开区间二分下界：$0$，无法把山的高度降低到 $0$。
- 开区间二分上界：设 $\textit{maxT}$ 为 $\textit{workerTimes}$ 的最大值，假设每个工人都是最慢的 $\textit{maxT}$，那么单个工人要把山降低 $h=\left\lceil\dfrac{mountainHeight}{n}\right\rceil$，耗时 $\textit{maxT}\cdot(1+2+\cdots+h)=\textit{maxT}\cdot\dfrac{h(h+1)}{2}$，将其作为开区间的二分上界，一定可以把山的高度降低到 $\le 0$。

关于上取整的计算，当 $a$ 和 $b$ 均为正整数时，我们有

$$
\left\lceil\dfrac{a}{b}\right\rceil = \left\lfloor\dfrac{a-1}{b}\right\rfloor + 1
$$

证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

```py [sol-Python3]
class Solution:
    def minNumberOfSeconds(self, mountainHeight: int, workerTimes: List[int]) -> int:
        def check(m: int) -> bool:
            left_h = mountainHeight
            for t in workerTimes:
                left_h -= (isqrt(m // t * 8 + 1) - 1) // 2
                if left_h <= 0:
                    return True
            return False

        max_t = max(workerTimes)
        h = (mountainHeight - 1) // len(workerTimes) + 1
        return bisect_left(range(max_t * h * (h + 1) // 2), True, 1, key=check)
```

```py [sol-Python3 写法二]
class Solution:
    def minNumberOfSeconds(self, mountainHeight: int, workerTimes: List[int]) -> int:
        f = lambda m: sum((isqrt(m // t * 8 + 1) - 1) // 2 for t in workerTimes)
        max_t = max(workerTimes)
        h = (mountainHeight - 1) // len(workerTimes) + 1
        return bisect_left(range(max_t * h * (h + 1) // 2), mountainHeight, 1, key=f)
```

```java [sol-Java]
class Solution {
    public long minNumberOfSeconds(int mountainHeight, int[] workerTimes) {
        int maxT = 0;
        for (int t : workerTimes) {
            maxT = Math.max(maxT, t);
        }
        int h = (mountainHeight - 1) / workerTimes.length + 1;
        long left = 0;
        long right = (long) maxT * h * (h + 1) / 2;
        while (left + 1 < right) {
            long mid = (left + right) / 2;
            if (check(mid, mountainHeight, workerTimes)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(long m, int leftH, int[] workerTimes) {
        for (int t : workerTimes) {
            leftH -= ((int) Math.sqrt(m / t * 8 + 1) - 1) / 2;
            if (leftH <= 0) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minNumberOfSeconds(int mountainHeight, vector<int>& workerTimes) {
        auto check = [&](long long m) {
            int left_h = mountainHeight;
            for (int t : workerTimes) {
                left_h -= ((int) sqrt(m / t * 8 + 1) - 1) / 2;
                if (left_h <= 0) {
                    return true;
                }
            }
            return false;
        };

        int max_t = ranges::max(workerTimes);
        int h = (mountainHeight - 1) / workerTimes.size() + 1;
        long long left = 0, right = (long long) max_t * h * (h + 1) / 2;
        while (left + 1 < right) {
            long long mid = (left + right) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func minNumberOfSeconds(mountainHeight int, workerTimes []int) int64 {
	maxT := slices.Max(workerTimes)
	h := (mountainHeight-1)/len(workerTimes) + 1
	ans := 1 + sort.Search(maxT*h*(h+1)/2-1, func(m int) bool {
		m++
		leftH := mountainHeight
		for _, t := range workerTimes {
			leftH -= (int(math.Sqrt(float64(m/t*8+1))) - 1) / 2
			if leftH <= 0 {
				return true
			}
		}
		return false
	})
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{workerTimes}$ 的长度，$U\le 5\cdot 10^{10}(10^5+1)$ 是二分上界。二分 $\mathcal{O}(\log U)$ 次，每次 $\mathcal{O}(n)$ 时间。开平方有专门的 CPU 指令，可以视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面二分题单的「**二分答案：求最小**」。

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
