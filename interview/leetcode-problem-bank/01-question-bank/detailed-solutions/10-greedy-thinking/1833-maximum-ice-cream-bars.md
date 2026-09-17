# 1833. 雪糕的最大数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-ice-cream-bars/
- 题目 slug：`maximum-ice-cream-bars`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1253
- 外部题解来源：https://leetcode.cn/problems/maximum-ice-cream-bars/solutions/3986082/cong-xiao-dao-da-tan-xin-pythonjavaccgoj-mxg6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从小到大贪心（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-ice-cream-bars/solutions/3986082/cong-xiao-dao-da-tan-xin-pythonjavaccgoj-mxg6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-xiao-dao-da-tan-xin-pythonjavaccgoj-mxg6`
- topic id：`3986082`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

贪心地，按照价格从低到高购买，是最优的。

**证明**：如果最优解没有按照价格从低到高购买，那么把已买的雪糕替换成没有买的更便宜的雪糕，我们买的雪糕数量不变（仍然最优），花的钱更少了（没有超出预算）。所以存在最优解，是按照价格从低到高购买的。

## 写法一

```py [sol-Python3]
class Solution:
    def maxIceCream(self, costs: List[int], coins: int) -> int:
        costs.sort()

        # 按照价格从低到高买
        for i, cost in enumerate(costs):
            if coins < cost:  # 钱不够
                return i  # 买 [0, i-1] 一共 i 根雪糕
            coins -= cost

        # 可以买所有雪糕
        return len(costs)
```

```java [sol-Java]
class Solution {
    public int maxIceCream(int[] costs, int coins) {
        Arrays.sort(costs);
        int n = costs.length;

        // 按照价格从低到高买
        for (int i = 0; i < n; i++) {
            int cost = costs[i];
            if (coins < cost) { // 钱不够
                return i; // 买 [0, i-1] 一共 i 根雪糕
            }
            coins -= cost;
        }

        // 可以买所有雪糕
        return n;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxIceCream(vector<int>& costs, int coins) {
        ranges::sort(costs);
        int n = costs.size();

        // 按照价格从低到高买
        for (int i = 0; i < n; i++) {
            int cost = costs[i];
            if (coins < cost) { // 钱不够
                return i; // 买 [0, i-1] 一共 i 根雪糕
            }
            coins -= cost;
        }

        // 可以买所有雪糕
        return n;
    }
};
```

```c [sol-C]
int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int maxIceCream(int* costs, int costsSize, int coins) {
    qsort(costs, costsSize, sizeof(int), cmp);

    // 按照价格从低到高买
    for (int i = 0; i < costsSize; i++) {
        int cost = costs[i];
        if (coins < cost) { // 钱不够
            return i; // 买 [0, i-1] 一共 i 根雪糕
        }
        coins -= cost;
    }

    // 可以买所有雪糕
    return costsSize;
}
```

```go [sol-Go]
func maxIceCream(costs []int, coins int) int {
	slices.Sort(costs)

	// 按照价格从低到高买
	for i, cost := range costs {
		if coins < cost { // 钱不够
			return i // 买 [0, i-1] 一共 i 根雪糕
		}
		coins -= cost
	}

	// 可以买所有雪糕
	return len(costs)
}
```

```js [sol-JavaScript]
var maxIceCream = function(costs, coins) {
    costs.sort((a, b) => a - b);
    const n = costs.length; 

    // 按照价格从低到高买
    for (let i = 0; i < n; i++) {
        const cost = costs[i];
        if (coins < cost) { // 钱不够
            return i; // 买 [0, i-1] 一共 i 根雪糕
        }
        coins -= cost;
    }

    // 可以买所有雪糕
    return n;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_ice_cream(mut costs: Vec<i32>, mut coins: i32) -> i32 {
        costs.sort_unstable();

        // 按照价格从低到高买
        for (i, &cost) in costs.iter().enumerate() {
            if coins < cost { // 钱不够
                return i as _; // 买 [0, i-1] 一共 i 根雪糕
            }
            coins -= cost;
        }

        // 可以买所有雪糕
        costs.len() as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{costs}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 写法二：计数排序

```py [sol-Python3]
class Solution:
    def maxIceCream(self, costs: List[int], coins: int) -> int:
        mx = max(costs)
        cnt = [0] * (mx + 1)
        for cost in costs:
            cnt[cost] += 1

        # 按照价格从低到高买
        ans = 0
        for cost in range(1, mx + 1):
            if coins < cost:  # 钱不够
                break
            num = min(cnt[cost], coins // cost)
            coins -= cost * num  # 买 num 根雪糕
            ans += num
        return ans
```

```java [sol-Java]
class Solution {
    public int maxIceCream(int[] costs, int coins) {
        int mx = 0;
        for (int cost : costs) {
            mx = Math.max(mx, cost);
        }

        int[] cnt = new int[mx + 1];
        for (int cost : costs) {
            cnt[cost]++;
        }

        // 按照价格从低到高买
        int ans = 0;
        for (int cost = 1; cost <= mx && cost <= coins; cost++) {
            int num = Math.min(cnt[cost], coins / cost);
            coins -= cost * num; // 买 num 根雪糕
            ans += num;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxIceCream(vector<int>& costs, int coins) {
        int mx = ranges::max(costs);
        vector<int> cnt(mx + 1);
        for (int cost : costs) {
            cnt[cost]++;
        }

        // 按照价格从低到高买
        int ans = 0;
        for (int cost = 1; cost <= mx && cost <= coins; cost++) {
            int num = min(cnt[cost], coins / cost);
            coins -= cost * num; // 买 num 根雪糕
            ans += num;
        }
        return ans;
    }
};
```

```c [sol-C]
int maxIceCream(int* costs, int costsSize, int coins) {
    int mx = 0;
    for (int i = 0; i < costsSize; i++) {
        mx = MAX(mx, costs[i]);
    }

    int* cnt = calloc(mx + 1, sizeof(int));
    for (int i = 0; i < costsSize; i++) {
        cnt[costs[i]]++;
    }

    // 按照价格从低到高买
    int ans = 0;
    for (int cost = 1; cost <= mx && cost <= coins; cost++) {
        int num = MIN(cnt[cost], coins / cost);
        coins -= cost * num; // 买 num 根雪糕
        ans += num;
    }

    free(cnt);
    return ans;
}
```

```go [sol-Go]
func maxIceCream(costs []int, coins int) (ans int) {
	mx := slices.Max(costs)
	cnt := make([]int, mx+1)
	for _, cost := range costs {
		cnt[cost]++
	}

	// 按照价格从低到高买
	for cost := 1; cost <= mx && cost <= coins; cost++ {
		num := min(cnt[cost], coins/cost)
		coins -= cost * num // 买 num 根雪糕
		ans += num
	}
	return
}
```

```js [sol-JavaScript]
var maxIceCream = function(costs, coins) {
    const mx = Math.max(...costs);
    const cnt = Array(mx + 1).fill(0);
    for (const cost of costs) {
        cnt[cost]++;
    }

    // 按照价格从低到高买
    let ans = 0;
    for (let cost = 1; cost <= mx && cost <= coins; cost++) {
        const num = Math.min(cnt[cost], Math.floor(coins / cost));
        coins -= cost * num; // 买 num 根雪糕
        ans += num;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_ice_cream(costs: Vec<i32>, mut coins: i32) -> i32 {
        let mx = *costs.iter().max().unwrap();
        let mut cnt = vec![0; mx as usize + 1];
        for cost in costs {
            cnt[cost as usize] += 1;
        }

        // 按照价格从低到高买
        let mut ans = 0;
        for cost in 1..=mx {
            if coins < cost { // 钱不够
                break;
            }
            let num = cnt[cost as usize].min(coins / cost);
            coins -= cost * num; // 买 num 根雪糕
            ans += num;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + U)$，其中 $n$ 是 $\textit{costs}$ 的长度，$U=\max(\textit{costs})$。
- 空间复杂度：$\mathcal{O}(U)$。

## 专题训练

见下面贪心题单的「**§1.1 从最小/最大开始贪心**」。

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

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
