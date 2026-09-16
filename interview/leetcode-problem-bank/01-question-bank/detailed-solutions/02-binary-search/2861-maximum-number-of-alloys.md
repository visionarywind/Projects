# 2861. 最大合金数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-alloys/
- 题目 slug：`maximum-number-of-alloys`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.2 求最大
- 难度分：1981
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-alloys/solutions/2446024/er-fen-da-an-fu-ti-dan-by-endlesscheng-3jdr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案，附题单（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/maximum-number-of-alloys/solutions/2446024/er-fen-da-an-fu-ti-dan-by-endlesscheng-3jdr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-fu-ti-dan-by-endlesscheng-3jdr`
- topic id：`2446024`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**挨个判断**每台机器最多可以制造多少份合金。

假设要制造 $\textit{num}$ 份合金，由于 $\textit{num}$ 越小，花费的钱越少，$\textit{num}$ 越多，花费的钱越多，可以据此**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

对于第 $j$ 类金属：

- 如果 $\textit{composition}[i][j]\cdot \textit{num} \le \textit{stock}[j]$，那么无需购买额外的金属。
- 如果 $\textit{composition}[i][j]\cdot \textit{num} > \textit{stock}[j]$，那么需要购买额外的金属，花费为

$$
(\textit{composition}[i][j]\cdot \textit{num} - \textit{stock}[j])\cdot \textit{cost}[j]
$$

遍历每类金属，计算总花费。如果总花费超过 $\textit{budget}$，则无法制造 $\textit{num}$ 份合金，否则可以制造。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$0$。不需要造合金，直接就满足要求了。
- 开区间左端点初始值（优化）：更巧妙的做法是，设当前答案为 $\textit{ans}$，那么开区间左端点可以初始化为 $\textit{ans}$，因为算出小于等于 $\textit{ans}$ 的值是没有意义的，不会让 $\textit{ans}$ 变得更大。如果这台机器无法制造出至少 $\textit{ans}+1$ 份合金，那么二分循环结束后的结果为 $\textit{ans}$，不影响答案。
- 开区间右端点初始值：粗略估计即可。假设 $\textit{composition}[i][j]$ 和 $\textit{cost}[j]$ 都是 $1$，且只有一种金属需要购买，我们可以制造的合金数最多为 $\min(\textit{stock}) + \textit{budget}$。再加一就一定无法满足要求了。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

[本题视频讲解](https://www.bilibili.com/video/BV1Lm4y1N7mf/)

```py [sol-Python3]
class Solution:
    def maxNumberOfAlloys(self, n: int, k: int, budget: int, composition: List[List[int]], stock: List[int], cost: List[int]) -> int:
        ans = 0
        mx = min(stock) + budget
        for comp in composition:
            def check(num: int) -> bool:
                money = 0
                for s, base, c in zip(stock, comp, cost):
                    if s < base * num:
                        money += (base * num - s) * c
                        if money > budget:
                            return False
                return True

            left, right = ans, mx + 1
            while left + 1 < right:  # 开区间写法
                mid = (left + right) // 2
                if check(mid):
                    left = mid
                else:
                    right = mid
            ans = left
        return ans
```

```py [sol-Python3 库函数]
class Solution:
    def maxNumberOfAlloys(self, n: int, k: int, budget: int, composition: List[List[int]], stock: List[int], cost: List[int]) -> int:
        ans = 0
        mx = min(stock) + budget
        for comp in composition:
            def f(num: int) -> int:
                money = 0
                for s, base, c in zip(stock, comp, cost):
                    if s < base * num:
                        money += (base * num - s) * c
                        if money > budget:
                            break
                return money
            ans += bisect_right(range(ans + 1, mx + 1), budget, key=f)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxNumberOfAlloys(int n, int k, int budget, List<List<Integer>> composition, List<Integer> Stock, List<Integer> Cost) {
        int mx = Collections.min(Stock) + budget;
        Integer[] stock = Stock.toArray(Integer[]::new); // 转成数组更快
        Integer[] cost = Cost.toArray(Integer[]::new);

        int ans = 0;
        for (List<Integer> Comp : composition) {
            Integer[] comp = Comp.toArray(Integer[]::new);
            int left = ans;
            int right = mx + 1;
            while (left + 1 < right) { // 开区间写法
                int mid = (left + right) >>> 1;
                long money = 0;
                for (int i = 0; i < n && money <= budget; i++) {
                    if (stock[i] < (long) comp[i] * mid) {
                        money += ((long) comp[i] * mid - stock[i]) * cost[i];
                    }
                }
                if (money <= budget) {
                    left = mid;
                } else {
                    right = mid;
                }
            }
            ans = left;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxNumberOfAlloys(int n, int, int budget, vector<vector<int>>& composition, vector<int>& stock, vector<int>& cost) {
        int ans = 0;
        int mx = ranges::min(stock) + budget;
        for (auto& comp : composition) {
            auto check = [&](long long num) -> bool {
                long long money = 0;
                for (int i = 0; i < n; i++) {
                    if (stock[i] < comp[i] * num) {
                        money += (comp[i] * num - stock[i]) * cost[i];
                        if (money > budget) {
                            return false;
                        }
                    }
                }
                return true;
            };
            int left = ans, right = mx + 1;
            while (left + 1 < right) { // 开区间写法
                int mid = left + (right - left) / 2;
                (check(mid) ? left : right) = mid;
            }
            ans = left;
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxNumberOfAlloys(_, _, budget int, composition [][]int, stock, cost []int) (ans int) {
	mx := slices.Min(stock) + budget
	for _, comp := range composition {
		ans += sort.Search(mx-ans, func(num int) bool {
			num += ans + 1
			money := 0
			for i, s := range stock {
				if s < comp[i]*num {
					money += (comp[i]*num - s) * cost[i]
					if money > budget {
						return true
					}
				}
			}
			return false
		})
	}
	return
}
```

```js [sol-JavaScript]
var maxNumberOfAlloys = function(n, k, budget, composition, stock, cost) {
    let ans = 0;
    const mx = Math.min(...stock) + budget;
    for (const comp of composition) {
        function check(num) {
            let money = 0;
            for (let i = 0; i < n; i++) {
                if (stock[i] < comp[i] * num) {
                    money += (comp[i] * num - stock[i]) * cost[i];
                    if (money > budget) {
                        return false;
                    }
                }
            }
            return true;
        }
        let left = ans, right = mx + 1;
        while (left + 1 < right) { // 开区间写法
            const mid = Math.floor((left + right) / 2);
            if (check(mid)) {
                left = mid;
            } else {
                right = mid;
            }
        }
        ans = left;
    }
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(kn\log U)$，其中 $U=\min(\textit{stock}) + \textit{budget}$。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. 【本题相关】[二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.2 求最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.2 求最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
