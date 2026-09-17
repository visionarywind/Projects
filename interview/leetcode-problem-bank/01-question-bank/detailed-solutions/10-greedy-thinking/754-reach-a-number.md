# 754. 到达终点数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reach-a-number/
- 题目 slug：`reach-a-number`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.7 其他数学贪心
- 难度分：2000
- 外部题解来源：https://leetcode.cn/problems/reach-a-number/solutions/1947254/fen-lei-tao-lun-xiang-xi-zheng-ming-jian-sqj2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论+详细证明+简洁写法+优化（Python/Java/C++/Go）](https://leetcode.cn/problems/reach-a-number/solutions/1947254/fen-lei-tao-lun-xiang-xi-zheng-ming-jian-sqj2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-xiang-xi-zheng-ming-jian-sqj2`
- topic id：`1947254`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

如果**不回头**地往终点走 $n$ 步，并恰好能走到终点，那么答案就是 $n$。

如果无法恰好走到终点，就需要仔细讨论了：

![754-2.png](https://pic.leetcode.cn/1667384831-ILuyCm-754-2.png)

#### 答疑

为方便描述，如果终点 $\textit{target}<0$，根据对称性，可将其变为 $-\textit{target}$，这不影响结果。

**问**：对于情况四，为什么多走两步一定可以把到终点的距离变为偶数？

**答**：此时相距为奇数，多走两步，奇数再加上两个相邻数字（必定一偶一奇），即奇数+偶数+奇数，可以得到偶数。

**问**：为什么相距为偶数时，一定可以通过把**某些**步反向，恰好到达终点？

**答**：设走了 $n$ 步后，步长之和 $s=1+2+\cdots+n=\dfrac{n(n+1)}{2}$。
&emsp;&emsp;这里需要一个小结论：$[1,s]$ 内的每个数字都可以由若干个不同的 $[1,n]$ 内的数组成。
&emsp;&emsp;证明：$n=1,2$ 时显然。假设 $n=k\ (k\ge 2)$ 时结论成立，此时已经得到了 $[1,s]$ 内的每个数字（这里 $s=\dfrac{k(k+1)}{2}$），我们可以用 $[s-k,s]$ 内的数字加上 $k+1$，从而得到 $[s+1,s+k+1]$ 内的每个数字，即得到了 $[1,s+k+1]$ 内的每个数字，即证明了当 $n=k+1$ 时结论也成立。根据数学归纳法，原结论成立。
&emsp;&emsp;由于相距 $d=s-\textit{target}<s$，根据结论，在 $d$ 为偶数时，一定可以选择某些步，满足这些步长的和为 $\dfrac{d}{2}$，将这些步反向，就能恰好到达终点。

**问**：为什么按图中的方法，走的步数一定是最小的？

**答**：将原问题转换成一个等价问题「先一直往终点走，然后选择某些步反向，所需要的最小步数」，然后在该问题上讨论。
&emsp;&emsp;情况一二可以用反证法，不走 $n$ 步根本无法到达终点（这里第 $n$ 步恰好到达或越过终点）。
&emsp;&emsp;情况三四，由于反向操作只能将 $s$ 减少偶数，无法处理相距奇数的情况，必须多走一两步，将相距变为偶数，才能处理。

代码实现时，我们可以不断循环，累加当前步长 $n$，当到达（越过）终点且相距偶数时停止。最后一步的步长即为答案。

```py [sol1-Python3]
class Solution:
    def reachNumber(self, target: int) -> int:
        target = abs(target)
        s = n = 0
        while s < target or (s - target) % 2:  # 没有到达（越过）终点，或者相距奇数
            n += 1
            s += n
        return n
```

```java [sol1-Java]
class Solution {
    public int reachNumber(int target) {
        target = Math.abs(target);
        int s = 0, n = 0;
        while (s < target || (s - target) % 2 == 1) // 没有到达（越过）终点，或者相距奇数
            s += ++n;
        return n;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int reachNumber(int target) {
        target = abs(target);
        int s = 0, n = 0;
        while (s < target || (s - target) % 2) // 没有到达（越过）终点，或者相距奇数
            s += ++n;
        return n;
    }
};
```

```go [sol1-Go]
func reachNumber(target int) (n int) {
    if target < 0 {
        target = -target
    }
    s := 0
    for s < target || (s-target)%2 == 1 { // 没有到达（越过）终点，或者相距奇数
        n++
        s += n
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$O(\sqrt{|\textit{target}|})$。
- 空间复杂度：$O(1)$，仅用到若干变量。

#### 优化

进一步地，由于 $n$ 是最小的满足 $\dfrac{n(n+1)}{2} \ge |\textit{target}|$ 的 $n$，解得

$$
n = \left\lceil\dfrac{-1+\sqrt{8\cdot |\textit{target}|+1}}{2}\right\rceil
$$

这样可以省去循环判断。

情况三四也可以进一步讨论：

- 如果是情况三，说明下一步 $n+1$ 是奇数，即 $n$ 为偶数，此时答案为 $n+1$；
- 如果是情况四，说明下一步 $n+1$ 是偶数，即 $n$ 为奇数，此时答案为 $n+2$。

因此情况三四可以合并为一个公式：

$$
n+1+(n\bmod 2)
$$

```py [sol2-Python3]
class Solution:
    def reachNumber(self, target: int) -> int:
        target = abs(target)
        n = ceil((-1 + (8 * target + 1) ** 0.5) / 2)
        return n if (n * (n + 1) // 2 - target) % 2 == 0 else n + 1 + n % 2
```

```java [sol2-Java]
class Solution {
    public int reachNumber(int target) {
        target = Math.abs(target);
        int n = (int) Math.ceil((-1 + Math.sqrt(8L * target + 1)) / 2); // 注意 8*target 会超过 int 范围
        return (n * (n + 1) / 2 - target) % 2 == 0 ? n : n + 1 + n % 2;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int reachNumber(int target) {
        target = abs(target);
        int n = ceil((-1 + sqrt(8L * target + 1)) / 2); // 注意 8*target 会超过 int 范围
        return (n * (n + 1) / 2 - target) % 2 == 0 ? n : n + 1 + n % 2;
    }
};
```

```go [sol2-Go]
func reachNumber(target int) int {
    if target < 0 {
        target = -target
    }
    n := int(math.Ceil((-1 + math.Sqrt(float64(8*target+1))) / 2))
    if (n*(n+1)/2-target)%2 == 0 {
        return n
    }
    return n + 1 + n%2
}
```

#### 复杂度分析

- 时间复杂度：$O(1)$。
- 空间复杂度：$O(1)$，仅用到若干变量。

---

欢迎关注我的B站频道：[灵茶山艾府](https://space.bilibili.com/206214)，每周更新算法讲解视频~

## 本地原创解析

### 1. 题意重述

本题来自 `四、数学贪心 / §4.7 其他数学贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.7 其他数学贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
