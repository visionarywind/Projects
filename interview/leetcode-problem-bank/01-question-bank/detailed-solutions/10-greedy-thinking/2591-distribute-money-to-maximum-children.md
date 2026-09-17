# 2591. 将钱分给最多的儿童

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distribute-money-to-maximum-children/
- 题目 slug：`distribute-money-to-maximum-children`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1531
- 外部题解来源：https://leetcode.cn/problems/distribute-money-to-maximum-children/solutions/2177180/fen-lei-tao-lun-o1-zuo-fa-by-endlesschen-95ef/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论 + O(1) 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/distribute-money-to-maximum-children/solutions/2177180/fen-lei-tao-lun-o1-zuo-fa-by-endlesschen-95ef/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-o1-zuo-fa-by-endlesschen-95ef`
- topic id：`2177180`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

### 本题视频讲解

见[【双周赛 100】](https://www.bilibili.com/video/BV1WM411H7UE/)。

### 思路

首先，每人至少分配 $1$ 美元，把 $\textit{money}$ 减少 $\textit{children}$。

如果 $\textit{money}<0$，返回 $-1$。

然后不断给每个人 $7$ 美元（前面分配了 $1$ 美元），这样可以分给至多

$$
\textit{ans}=\min\left(\left\lfloor\dfrac{\textit{money}}{7}\right\rfloor,\textit{children}\right)
$$

个人。然后更新剩余 $\textit{money}$ 和剩余未分配的人数。

最后，分类讨论：

- 如果剩余 $0$ 人，且 $\textit{money}>0$，那么必须分给一个已经分到 $8$ 美元的人，$\textit{ans}$ 减一。
- 如果剩余 $1$ 人，且 $\textit{money}=3$，为避免分配 $4$ 美元，那么必须分给一个已经分到 $8$ 美元的人，$\textit{ans}$ 减一。（注意输入的 $\textit{children}\ge 2$）
- 其它情况全部给一个人，如果这个人分配到 $4$ 美元，他再给另一个人 $1$ 美元，这样 $\textit{ans}$ 不变。

```py [sol1-Python3]
class Solution:
    def distMoney(self, money: int, children: int) -> int:
        money -= children  # 每人至少 1 美元
        if money < 0: return -1
        ans = min(money // 7, children)  # 初步分配，让尽量多的人分到 8 美元
        money -= ans * 7
        children -= ans
        # children == 0 and money：必须找一个前面分了 8 美元的人，分配完剩余的钱
        # children == 1 and money == 3：不能有人恰好分到 4 美元
        if children == 0 and money or \
           children == 1 and money == 3:
            ans -= 1
        return ans
```

```java [sol1-Java]
class Solution {
    public int distMoney(int money, int children) {
        money -= children; // 每人至少 1 美元
        if (money < 0) return -1;
        int ans = Math.min(money / 7, children); // 初步分配，让尽量多的人分到 8 美元
        money -= ans * 7;
        children -= ans;
        if (children == 0 && money > 0 || // 必须找一个前面分了 8 美元的人，分完剩余的钱
            children == 1 && money == 3) // 不能有人恰好分到 4 美元
            --ans;
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int distMoney(int money, int children) {
        money -= children; // 每人至少 1 美元
        if (money < 0) return -1;
        int ans = min(money / 7, children); // 初步分配，让尽量多的人分到 8 美元
        money -= ans * 7;
        children -= ans;
        if (children == 0 && money || // 必须找一个前面分了 8 美元的人，分完剩余的钱
            children == 1 && money == 3) // 不能有人恰好分到 4 美元
            --ans;
        return ans;
    }
};
```

```go [sol1-Go]
func distMoney(money, children int) int {
	money -= children // 每人至少 1 美元
	if money < 0 {
		return -1
	}
	ans := min(money/7, children) // 初步分配，让尽量多的人分到 8 美元
	money -= ans * 7
	children -= ans
	if children == 0 && money > 0 || // 必须找一个前面分了 8 美元的人，分完剩余的钱
		children == 1 && money == 3 { // 不能有人恰好分到 4 美元
		ans--
	}
	return ans
}

func min(a, b int) int { if a > b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(1)$。
- 空间复杂度：$O(1)$。仅用到若干额外变量。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
