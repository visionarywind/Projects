# 2178. 拆分成最多数目的正偶数之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-split-of-positive-even-integers/
- 题目 slug：`maximum-split-of-positive-even-integers`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1538
- 外部题解来源：https://leetcode.cn/problems/maximum-split-of-positive-even-integers/solutions/1277705/tan-xin-jian-ji-xie-fa-by-endlesscheng-uxbg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 简洁写法（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/maximum-split-of-positive-even-integers/solutions/1277705/tan-xin-jian-ji-xie-fa-by-endlesscheng-uxbg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-jian-ji-xie-fa-by-endlesscheng-uxbg`
- topic id：`1277705`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

由于 $\textit{finalSum}$ 只能分解成偶数之和，而偶数+偶数=偶数，所以 $\textit{finalSum}$ 也必须是偶数。

既然要尽量多的分解，且分解出的偶数互不相同，那么可以按照 $2,4,6,8,\cdots$ 的顺序分解，一边分解一边减少 $\textit{finalSum}$，直到 $\textit{finalSum}$ 小于要分解出的数为止。

最后把剩余的 $\textit{finalSum}$ 加到最后一个分解出的偶数上，即完成了分解。

```py [sol-Python3]
class Solution:
    def maximumEvenSplit(self, finalSum: int) -> List[int]:
        if finalSum % 2:
            return []
        ans = []
        i = 2
        while i <= finalSum:
            ans.append(i)
            finalSum -= i
            i += 2
        ans[-1] += finalSum
        return ans
```

```java [sol-Java]
class Solution {
    public List<Long> maximumEvenSplit(long finalSum) {
        if (finalSum % 2 > 0) return List.of();
        var ans = new ArrayList<Long>();
        for (long i = 2; i <= finalSum; i += 2) {
            ans.add(i);
            finalSum -= i;
        }
        int back = ans.size() - 1;
        ans.set(back, ans.get(back) + finalSum);
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> maximumEvenSplit(long long finalSum) {
        if (finalSum % 2) return {};
        vector<long long> ans;
        for (long long i = 2; i <= finalSum; i += 2) {
            ans.push_back(i);
            finalSum -= i;
        }
        ans.back() += finalSum;
        return ans;
    }
};
```

```go [sol-Go]
func maximumEvenSplit(finalSum int64) (ans []int64) {
	if finalSum%2 == 0 {
		for i := int64(2); i <= finalSum; i += 2 {
			ans = append(ans, i)
			finalSum -= i
		}
		ans[len(ans)-1] += finalSum
	}
	return
}
```

```js [sol-JavaScript]
var maximumEvenSplit = function (finalSum) {
    if (finalSum % 2) 
        return [];
    let ans = [];
    for (let i = 2; i <= finalSum; i += 2) {
        ans.push(i);
        finalSum -= i;
    }
    ans[ans.length - 1] += finalSum;
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\sqrt{\textit{finalSum}})$。设数组长度为 $x$，根据等差数列之和，$2+4+6+8+\cdots+2x = x(x+1) \le \textit{finalSum}$，可知 $x$ 约为 $\sqrt{\textit{finalSum}}$，所以循环次数为 $\mathcal{O}(\sqrt{\textit{finalSum}})$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

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
