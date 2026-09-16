# 3387. 两天自由外汇交易后的最大货币数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-amount-after-two-days-of-conversions/
- 题目 slug：`maximize-amount-after-two-days-of-conversions`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.1 深度优先搜索（DFS）
- 难度分：1788
- 外部题解来源：https://leetcode.cn/problems/maximize-amount-after-two-days-of-conversions/solutions/3020665/jian-tu-zi-ding-xiang-xia-dfspythonjavac-um05/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两次 DFS，简洁写法，附思考题（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-amount-after-two-days-of-conversions/solutions/3020665/jian-tu-zi-ding-xiang-xia-dfspythonjavac-um05/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-tu-zi-ding-xiang-xia-dfspythonjavac-um05`
- topic id：`3020665`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

1. 根据 $\textit{pairs}_1$ 和 $\textit{rates}_1$ 建图。
2. 从 $\textit{initialCurrency}$ 开始，自顶向下 DFS 这张图，递归的同时维护金额。记录把 $\textit{initialCurrency}$ 兑换成其他货币的金额 $\textit{day1Amount}$。
3. 根据 $\textit{pairs}_2$ 和 $\textit{rates}_2$ 建图。
4. 同样地，从 $\textit{initialCurrency}$ 开始，自顶向下 DFS 这张图，递归的同时维护金额。记录把 $\textit{initialCurrency}$ 兑换成其他货币的金额 $\textit{day2Amount}$。金额的倒数，就是从其他货币兑换成 $\textit{initialCurrency}$ 的金额。

枚举中转货币 $x$，答案为 $\dfrac{\textit{day1Amount}[x]}{\textit{day2Amount}[x]}$ 的最大值。

```py [sol-Python3]
class Solution:
    def calc_amount(self, pairs: List[List[str]], rates: List[float], initialCurrency: str) -> Dict[str, float]:
        g = defaultdict(list)
        for (x, y), r in zip(pairs, rates):
            g[x].append((y, r))
            g[y].append((x, 1.0 / r))

        amount = {}
        def dfs(x: str, cur_amount: float) -> None:
            amount[x] = cur_amount
            for to, rate in g[x]:
                # 每个节点只需递归一次（重复递归算出来的结果是一样的，因为题目保证汇率没有矛盾）
                if to not in amount:
                    dfs(to, cur_amount * rate)
        dfs(initialCurrency, 1.0)
        return amount

    def maxAmount(self, initialCurrency: str, pairs1: List[List[str]], rates1: List[float], pairs2: List[List[str]], rates2: List[float]) -> float:
        day1_amount = self.calc_amount(pairs1, rates1, initialCurrency)
        day2_amount = self.calc_amount(pairs2, rates2, initialCurrency)
        return max(day1_amount.get(x, 0.0) / a2 for x, a2 in day2_amount.items())
```

```java [sol-Java]
class Solution {
    public double maxAmount(String initialCurrency, List<List<String>> pairs1, double[] rates1, List<List<String>> pairs2, double[] rates2) {
        Map<String, Double> day1Amount = calcAmount(pairs1, rates1, initialCurrency);
        Map<String, Double> day2Amount = calcAmount(pairs2, rates2, initialCurrency);

        double ans = 0;
        for (Map.Entry<String, Double> e : day2Amount.entrySet()) {
            ans = Math.max(ans, day1Amount.getOrDefault(e.getKey(), 0.0) / e.getValue());
        }
        return ans;
    }

    private record Pair(String to, double rate) {
    }

    private Map<String, Double> calcAmount(List<List<String>> pairs, double[] rates, String initialCurrency) {
        Map<String, List<Pair>> g = new HashMap<>();
        for (int i = 0; i < pairs.size(); i++) {
            String x = pairs.get(i).get(0);
            String y = pairs.get(i).get(1);
            double r = rates[i];
            g.computeIfAbsent(x, k -> new ArrayList<>()).add(new Pair(y, r));
            g.computeIfAbsent(y, k -> new ArrayList<>()).add(new Pair(x, 1 / r));
        }

        Map<String, Double> amount = new HashMap<>();
        dfs(initialCurrency, 1, g, amount);
        return amount;
    }

    private void dfs(String x, double curAmount, Map<String, List<Pair>> g, Map<String, Double> amount) {
        amount.put(x, curAmount);
        if (!g.containsKey(x)) {
            return;
        }
        for (Pair p : g.get(x)) {
            // 每个节点只需递归一次（重复递归算出来的结果是一样的，因为题目保证汇率没有矛盾）
            if (!amount.containsKey(p.to)) {
                dfs(p.to, curAmount * p.rate, g, amount);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double maxAmount(string initialCurrency, vector<vector<string>>& pairs1, vector<double>& rates1, vector<vector<string>>& pairs2, vector<double>& rates2) {
        auto calc_amount = [&](vector<vector<string>>& pairs, vector<double>& rates) {
            unordered_map<string, vector<pair<string, double>>> g;
            for (int i = 0; i < pairs.size(); i++) {
                auto& p = pairs[i];
                double r = rates[i];
                g[p[0]].emplace_back(p[1], r);
                g[p[1]].emplace_back(p[0], 1 / r);
            }

            unordered_map<string, double> amount;
            auto dfs = [&](this auto&& dfs, const string& x, double cur_amount) -> void {
                amount[x] = cur_amount;
                for (auto& [to, rate] : g[x]) {
                    // 每个节点只需递归一次（重复递归算出来的结果是一样的，因为题目保证汇率没有矛盾）
                    if (!amount.contains(to)) {
                        dfs(to, cur_amount * rate);
                    }
                }
            };
            dfs(initialCurrency, 1);
            return amount;
        };

        auto day1_amount = calc_amount(pairs1, rates1);
        auto day2_amount = calc_amount(pairs2, rates2);

        double ans = 0;
        for (auto& [x, a2] : day2_amount) {
            ans = max(ans, day1_amount[x] / a2);
        }
        return ans;
    }
};
```

```go [sol-Go]
type pair struct {
	to   string
	rate float64
}

func calcAmount(pairs [][]string, rates []float64, initialCurrency string) map[string]float64 {
	g := map[string][]pair{}
	for i, p := range pairs {
		x, y, r := p[0], p[1], rates[i]
		g[x] = append(g[x], pair{y, r})
		g[y] = append(g[y], pair{x, 1 / r})
	}

	amount := map[string]float64{}
	var dfs func(string, float64)
	dfs = func(x string, curAmount float64) {
		amount[x] = curAmount
		for _, e := range g[x] {
			// 每个节点只需递归一次（重复递归算出来的结果是一样的，因为题目保证汇率没有矛盾）
			if amount[e.to] == 0 {
				dfs(e.to, curAmount*e.rate)
			}
		}
	}
	dfs(initialCurrency, 1)
	return amount
}

func maxAmount(initialCurrency string, pairs1 [][]string, rates1 []float64, pairs2 [][]string, rates2 []float64) (ans float64) {
	day1Amount := calcAmount(pairs1, rates1, initialCurrency)
	day2Amount := calcAmount(pairs2, rates2, initialCurrency)
	for x, a2 := range day2Amount {
		ans = max(ans, day1Amount[x]/a2)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)L)$，其中 $n$ 是 $\textit{pairs}_1$ 的长度，$m$ 是 $\textit{pairs}_2$ 的长度，$L$ 是单个字符串的长度（不超过 $3$）。
- 空间复杂度：$\mathcal{O}((n+m)L)$。

## 思考题

如果有 $3$ 天或者更多天呢？怎么建图最合适？请看 [视频讲解](https://www.bilibili.com/video/BV1pnqZYKEqr/?t=6m10s)，欢迎点赞关注~

更多相似题目，见下面图论题单中的「**DFS**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. 【本题相关】[图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、图的遍历 / §1.1 深度优先搜索（DFS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.1 深度优先搜索（DFS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
