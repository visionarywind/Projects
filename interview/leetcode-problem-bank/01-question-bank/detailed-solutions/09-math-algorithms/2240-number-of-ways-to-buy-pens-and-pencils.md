# 2240. 买钢笔和铅笔的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-buy-pens-and-pencils/
- 题目 slug：`number-of-ways-to-buy-pens-and-pencils`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.12 其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-buy-pens-and-pencils/solutions/1426720/mei-ju-mai-liao-duo-shao-zhi-gang-bi-by-r97vg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举买了多少支钢笔（附 O(log) 类欧几里得算法）](https://leetcode.cn/problems/number-of-ways-to-buy-pens-and-pencils/solutions/1426720/mei-ju-mai-liao-duo-shao-zhi-gang-bi-by-r97vg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-mai-liao-duo-shao-zhi-gang-bi-by-r97vg`
- topic id：`1426720`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

枚举买了 $i$ 支钢笔。

至多能买 $\left\lfloor\dfrac{\textit{total}}{\textit{cost}_1}\right\rfloor$ 支钢笔。

剩余钱数为 $\textit{total} - i * \textit{cost}_1$，至少可以买 $0$ 支铅笔，至多可以买 $\left\lfloor\dfrac{\textit{total} - i * \textit{cost}_1}{\textit{cost}_2}\right\rfloor$ 支铅笔，这一共有 

$$
1 + \left\lfloor\dfrac{\textit{total} - i * \textit{cost}_1}{\textit{cost}_2}\right\rfloor
$$ 

种不同的购买方案。

所以答案为

$$
\sum_{i=0}^{\lfloor \textit{total}/\textit{cost}_1\rfloor} 1 + \left\lfloor\dfrac{\textit{total} - i * \textit{cost}_1}{\textit{cost}_2}\right\rfloor
$$

即

$$
1+\left\lfloor\dfrac{\textit{total}}{\textit{cost}_1}\right\rfloor + \sum_{i=0}^{\lfloor \textit{total}/\textit{cost}_1\rfloor} \left\lfloor\dfrac{\textit{total} - i * \textit{cost}_1}{\textit{cost}_2}\right\rfloor
$$

```py [sol-Python3]
class Solution:
    def waysToBuyPensPencils(self, total: int, cost1: int, cost2: int) -> int:
        n = 1 + total // cost1
        return n + sum((total - cost1 * i) // cost2 for i in range(n))
```

```java [sol-Java]
class Solution {
    public long waysToBuyPensPencils(int total, int cost1, int cost2) {
        long n = 1 + total / cost1, ans = n;
        for (long i = 0; i < n; i++)
            ans += (total - cost1 * i) / cost2;
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long waysToBuyPensPencils(int total, int cost1, int cost2) {
        long long n = 1 + total / cost1, ans = n;
        for (long long i = 0; i < n; i++)
            ans += (total - cost1 * i) / cost2;
        return ans;
    }
};
```

```go [sol-Go]
func waysToBuyPensPencils(total, cost1, cost2 int) int64 {
	n := 1 + total/cost1
	ans := int64(n)
	for i := 0; i < n; i++ {
		ans += int64((total - cost1*i) / cost2)
	}
	return ans
}
```

```js [sol-JavaScript]
var waysToBuyPensPencils = function(total, cost1, cost2) {
    const n = 1 + Math.floor(total / cost1);
    let ans = n;
    for (let i = 0; i < n; i++)
        ans += Math.floor((total - cost1 * i) / cost2);
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}\left(\left\lfloor\dfrac{\textit{total}}{\textit{cost}_1}\right\rfloor\right)$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 附：类欧几里得算法

上面的和式可以直接套用类欧几里德算法解决。

参考：[类欧几里德算法](https://oi-wiki.org/math/number-theory/euclidean/)。

```go
func waysToBuyPensPencils(total, cost1, cost2 int) int64 {
	n := total/cost1 + 1
	return int64(n + floorSum(n, cost2, -cost1, total))
}

// 返回 sum(floor((a*i+b)/m)), i 从 0 到 n-1
func floorSum(n, m, a, b int) (res int) {
	if a < 0 {
		a2 := a%m + m
		res -= n * (n - 1) / 2 * ((a2 - a) / m)
		a = a2
	}
	if b < 0 {
		b2 := b%m + m
		res -= n * ((b2 - b) / m)
		b = b2
	}
	for {
		if a >= m {
			res += n * (n - 1) / 2 * (a / m)
			a %= m
		}
		if b >= m {
			res += n * (b / m)
			b %= m
		}
		yMax := a*n + b
		if yMax < m {
			break
		}
		n = yMax / m
		b = yMax % m
		m, a = a, m
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}\left(\log\left\lfloor\dfrac{\textit{total}}{\textit{cost}_1}\right\rfloor\right)$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

[往期每日一题题解（按 tag 分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.12 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.12 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
