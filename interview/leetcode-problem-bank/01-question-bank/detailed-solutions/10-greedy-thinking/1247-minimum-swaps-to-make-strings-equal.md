# 1247. 交换字符使得字符串相同

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-swaps-to-make-strings-equal/
- 题目 slug：`minimum-swaps-to-make-strings-equal`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1597
- 外部题解来源：https://leetcode.cn/problems/minimum-swaps-to-make-strings-equal/solutions/2131832/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-a6r1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-swaps-to-make-strings-equal/solutions/2131832/mei-xiang-ming-bai-yi-zhang-tu-miao-dong-a6r1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-zhang-tu-miao-dong-a6r1`
- topic id：`2131832`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

![1247-2.png](https://pic.leetcode.cn/1677203938-dBhSyu-1247-2.png)

## 答疑

**问**：为什么这种交换方案是最优的？需不需要 $s_1[i]=s_2[i]$ 的字母参与？

**答**：对于不同的字母，这种做法是尽量「内部消化」。对于偶数+偶数的情况，每次交换操作都使 $d$ 减二，这是单次操作的极限；对于奇数+奇数的情况，只额外交换了一次就转换成了偶数+偶数的情况，这是必要的。所以这种交换方案充分利用了每次交换操作，也无需 $s_1[i]=s_2[i]$ 的字母参与。

**问**：为什么奇数+奇数交换一次就变成了偶数+偶数？

**答**：交换一次后，要么 $s_1$ 中的 $\text{x}$ 多一，$\text{y}$ 少一，变成偶数+偶数；要么 $\text{x}$ 少一，$\text{y}$ 多一，也变成偶数+偶数。

```py [sol-Python3]
class Solution:
    def minimumSwap(self, s1: str, s2: str) -> int:
        cnt = Counter(x for x, y in zip(s1, s2) if x != y)
        d = cnt['x'] + cnt['y']
        return -1 if d % 2 else d // 2 + cnt['x'] % 2
```

```java [sol-Java]
class Solution {
    public int minimumSwap(String s1, String s2) {
        int[] cnt = new int[2];
        for (int i = 0, n = s1.length(); i < n; i++)
            if (s1.charAt(i) != s2.charAt(i))
                cnt[s1.charAt(i) % 2]++; // x 和 y ASCII 值的二进制最低位不同
        int d = cnt[0] + cnt[1];
        return d % 2 != 0 ? -1 : d / 2 + cnt[0] % 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumSwap(string s1, string s2) {
        int cnt[2]{};
        for (int i = 0, n = s1.length(); i < n; i++)
            if (s1[i] != s2[i])
                cnt[s1[i] % 2]++; // x 和 y ASCII 值的二进制最低位不同
        int d = cnt[0] + cnt[1];
        return d % 2 ? -1 : d / 2 + cnt[0] % 2;
    }
};
```

```go [sol-Go]
func minimumSwap(s1, s2 string) int {
    cnt := [2]int{}
    for i, x := range s1 {
        if byte(x) != s2[i] {
            cnt[x%2]++ // 'x' 和 'y' ASCII 值的二进制最低位不同
        }
    }
    d := cnt[0] + cnt[1]
    if d%2 > 0 {
        return -1
    }
    return d/2 + cnt[0]%2
}
```

## 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $s_1$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。

## 思考题

如果字符串有超过两种字符，要怎么做呢？

---

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
