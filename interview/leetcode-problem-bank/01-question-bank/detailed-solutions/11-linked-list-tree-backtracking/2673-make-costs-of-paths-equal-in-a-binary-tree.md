# 2673. 使二叉树所有路径值相等的最小代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-costs-of-paths-equal-in-a-binary-tree/
- 题目 slug：`make-costs-of-paths-equal-in-a-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.16 其他
- 难度分：1917
- 外部题解来源：https://leetcode.cn/problems/make-costs-of-paths-equal-in-a-binary-tree/solutions/2259983/tan-xin-jian-ji-xie-fa-pythonjavacgo-by-5svh1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/make-costs-of-paths-equal-in-a-binary-tree/solutions/2259983/tan-xin-jian-ji-xie-fa-pythonjavacgo-by-5svh1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-jian-ji-xie-fa-pythonjavacgo-by-5svh1`
- topic id：`2259983`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

视频讲解请看[【周赛 344】](https://www.bilibili.com/video/BV1YL41187Rx/)第四题，欢迎点赞投币！

## 提示 1

考虑根到两个互为兄弟节点（父节点相同）的叶子的两条路径。

由于这两条路径除了叶子节点不一样，其余节点都一样，所以为了让这两条路径的路径和相等，必须修改叶子节点的值。

设叶子节点的值分别为 $x$ 和 $y$，假设 $x\le y$，是否需要同时增加 $x$ 和 $y$ 呢？

这是不需要的，把 $x$ 增加 $y-x$ 就行，因为我们可以增加它们的祖先节点的值，使得它们俩的路径和与其它的路径和相等，这样可以节省操作次数。

## 提示 2

对于不是叶子的兄弟节点，又要如何比较和计算呢？

和上面的分析一样，从根到当前节点的路径，除了这两个兄弟节点不一样，其余节点都一样。所以把路径和从叶子往上传，这样就可以按照提示 1 那样比较了。

示例 1 如下图，把节点 $2$ 的路径和视作 $x+5+3=x+8$，节点 $3$ 的路径和视作 $x+2+3=x+5$（其中 $x$ 是在节点 $2,3$ 之上的路径和），这样可以知道需要把节点 $3$ 的值增加 $(x+8)-(x+5)=8-5=3$。

![lc2673.png](https://pic.leetcode.cn/1709024171-NFqAWc-lc2673.png)

代码实现时，可以直接在 $\textit{cost}$ 上累加路径和。由于 $\textit{cost}$ 数组的下标是从 $0$ 开始的，所以节点编号转成下标需要减一。

```py [sol-Python3]
class Solution:
    def minIncrements(self, n: int, cost: List[int]) -> int:
        ans = 0
        for i in range(n // 2, 0, -1):  # 从最后一个非叶节点开始算
            ans += abs(cost[i * 2 - 1] - cost[i * 2])  # 两个子节点变成一样的
            cost[i - 1] += max(cost[i * 2 - 1], cost[i * 2])  # 累加路径和
        return ans
```

```java [sol-Java]
class Solution {
    public int minIncrements(int n, int[] cost) {
        int ans = 0;
        for (int i = n / 2; i > 0; i--) { // 从最后一个非叶节点开始算
            ans += Math.abs(cost[i * 2 - 1] - cost[i * 2]); // 两个子节点变成一样的
            cost[i - 1] += Math.max(cost[i * 2 - 1], cost[i * 2]); // 累加路径和
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minIncrements(int n, vector<int> &cost) {
        int ans = 0;
        for (int i = n / 2; i > 0; i--) { // 从最后一个非叶节点开始算
            ans += abs(cost[i * 2 - 1] - cost[i * 2]); // 两个子节点变成一样的
            cost[i - 1] += max(cost[i * 2 - 1], cost[i * 2]); // 累加路径和
        }
        return ans;
    }
};
```

```go [sol-Go]
func minIncrements(n int, cost []int) (ans int) {
	for i := n / 2; i > 0; i-- { // 从最后一个非叶节点开始算
		left, right := cost[i*2-1], cost[i*2]
		if left > right { // 保证 left <= right
			left, right = right, left
		}
		ans += right - left // 两个子节点变成一样的
		cost[i-1] += right // 累加路径和
	}
	return
}
```

```js [sol-JavaScript]
var minIncrements = function(n, cost) {
    let ans = 0;
    for (let i = Math.floor(n / 2); i > 0; i--) { // 从最后一个非叶节点开始算
        ans += Math.abs(cost[i * 2 - 1] - cost[i * 2]); // 两个子节点变成一样的
        cost[i - 1] += Math.max(cost[i * 2 - 1], cost[i * 2]); // 累加路径和
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_increments(n: i32, mut cost: Vec<i32>) -> i32 {
        let mut ans = 0;
        for i in (1..=n as usize / 2).rev() { // 从最后一个非叶节点开始算
            ans += (cost[i * 2 - 1] - cost[i * 2]).abs(); // 两个子节点变成一样的
            cost[i - 1] += cost[i * 2 - 1].max(cost[i * 2]); // 累加路径和
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{cost}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 思考题

如果还可以对节点值**减一**要怎么做？

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.16 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.16 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
