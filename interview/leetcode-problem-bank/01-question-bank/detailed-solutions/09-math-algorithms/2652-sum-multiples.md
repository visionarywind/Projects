# 2652. 倍数求和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-multiples/
- 题目 slug：`sum-multiples`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.3 容斥原理
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sum-multiples/solutions/2241283/o1-rong-chi-yuan-li-by-endlesscheng-yxc4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数学做法：O(1) 容斥原理（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/sum-multiples/solutions/2241283/o1-rong-chi-yuan-li-by-endlesscheng-yxc4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-rong-chi-yuan-li-by-endlesscheng-yxc4`
- topic id：`2241283`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

在 $[1,n]$ 中，$m$ 的倍数有 $k = \left\lfloor\dfrac{n}{m}\right\rfloor$ 个，即

$$
m,2m,\cdots,km
$$

结合等差数列求和公式，这些数的和为

$$
s(m) = \dfrac{k(k+1)}{2} \cdot m
$$

再结合**容斥原理**，可以算出 $3$ **或** $5$ **或** $7$ 的倍数之和，即

$$
s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105)
$$

具体请看 [视频讲解](https://www.bilibili.com/video/BV1Bs4y1A7Wa/) 第二题。

```py [sol-Python3]
class Solution:
    def sumOfMultiples(self, n: int) -> int:
        def s(m: int) -> int:
            return n // m * (n // m + 1) // 2 * m
        return s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105)
```

```java [sol-Java]
class Solution {
    public int sumOfMultiples(int n) {
        return s(n, 3) + s(n, 5) + s(n, 7) - s(n, 15) - s(n, 21) - s(n, 35) + s(n, 105);
    }

    private int s(int n, int m) {
        return n / m * (n / m + 1) / 2 * m;
    }
}
```

```cpp [sol-C++]
class Solution {
    int s(int n, int m) {
        return n / m * (n / m + 1) / 2 * m;
    }
public:
    int sumOfMultiples(int n) {
        return s(n, 3) + s(n, 5) + s(n, 7) - s(n, 15) - s(n, 21) - s(n, 35) + s(n, 105);
    }
};
```

```go [sol-Go]
func s(n, m int) int {
	return n / m * (n/m + 1) / 2 * m
}

func sumOfMultiples(n int) int {
	return s(n, 3) + s(n, 5) + s(n, 7) - s(n, 15) - s(n, 21) - s(n, 35) + s(n, 105)
}
```

```js [sol-JavaScript]
var sumOfMultiples = function(n) {
    function s(m) {
        return Math.floor(n / m) * (Math.floor(n / m) + 1) / 2 * m;
    }
    return s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn sum_of_multiples(n: i32) -> i32 {
        let s = |m| n / m * (n / m + 1) / 2 * m;
        s(3) + s(5) + s(7) - s(15) - s(21) - s(35) + s(105)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [1201. 丑数 III](https://leetcode.cn/problems/ugly-number-iii/)

## 本地原创解析

### 1. 题意重述

本题来自 `二、组合数学 / §2.3 容斥原理`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.3 容斥原理`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
