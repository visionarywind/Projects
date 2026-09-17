# 1033. 移动石子直到连续

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/moving-stones-until-consecutive/
- 题目 slug：`moving-stones-until-consecutive`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1421
- 外部题解来源：https://leetcode.cn/problems/moving-stones-until-consecutive/solutions/2250643/fen-lei-tao-lun-pythonjavacgo-by-endless-2qyo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论（Python/Java/C++/Go）](https://leetcode.cn/problems/moving-stones-until-consecutive/solutions/2250643/fen-lei-tao-lun-pythonjavacgo-by-endless-2qyo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-pythonjavacgo-by-endless-2qyo`
- topic id：`2250643`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

假设 $a,b,c$ 是有序的（从小到大）。

- 最大移动次数：$a$ 和 $c$ 向 $b$ 靠拢，每次只移动一个单位长度，答案就是 $c-a-2$。
- 最小移动次数：
  - 如果 $c-a=2$，已经连续，无需移动。
  - 如果 $b-a=1$ 或者 $c-b=1$，说明有两颗石子已经连续，那么只需移动 $1$ 次另一颗石子。
  - 如果 $b-a=2$ 或者 $c-b=2$，那么把一颗石子移到另外两颗石子之间，只需移动 $1$ 次移动。
  - 否则，$a$ 移动到 $b-1$，$c$ 移动到 $b+1$，一共 $2$ 次移动。 

```py [sol1-Python3]
class Solution:
    def numMovesStones(self, a: int, b: int, c: int) -> List[int]:
        a, b, c = sorted((a, b, c))
        return [0 if c - a == 2 else
                1 if b - a <= 2 or c - b <= 2 else
                2,
                c - a - 2]
```

```java [sol1-Java]
class Solution {
    public int[] numMovesStones(int a, int b, int c) {
        var p = new int[]{a, b, c};
        Arrays.sort(p);
        a = p[0]; b = p[1]; c = p[2];
        return new int[]{c - a == 2 ? 0 :
                         b - a <= 2 || c - b <= 2 ? 1 : 2,
                         c - a - 2};
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<int> numMovesStones(int a, int b, int c) {
        int p[] = {a, b, c};
        sort(p, p + 3);
        a = p[0]; b = p[1]; c = p[2];
        return {c - a == 2 ? 0 :
                b - a <= 2 || c - b <= 2 ? 1 : 2,
                c - a - 2};
    }
};
```

```go [sol1-Go]
func numMovesStones(a, b, c int) []int {
    p := []int{a, b, c}
    sort.Ints(p)
    a, b, c = p[0], p[1], p[2]
    minMove := 0
    if c-a > 2 {
        if b-a <= 2 || c-b <= 2 {
            minMove = 1
        } else {
            minMove = 2
        }
    }
    return []int{minMove, c - a - 2}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 相似题目

一万颗石子要怎么做？（题目要求略有改动）

- [1040. 移动石子直到连续 II](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/)，[题解](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/solution/tu-jie-xia-tiao-qi-pythonjavacgo-by-endl-r1eb/)

[往期每日一题题解](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

---

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

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
