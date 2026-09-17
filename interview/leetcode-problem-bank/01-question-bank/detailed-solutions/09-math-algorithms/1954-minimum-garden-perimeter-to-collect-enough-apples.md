# 1954. 收集足够苹果的最小花园周长

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-garden-perimeter-to-collect-enough-apples/
- 题目 slug：`minimum-garden-perimeter-to-collect-enough-apples`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1759
- 外部题解来源：https://leetcode.cn/problems/minimum-garden-perimeter-to-collect-enough-apples/solutions/2577652/tu-jie-o1-zuo-fa-pythonjavacgojsrust-by-oms4k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】O(1) 做法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-garden-perimeter-to-collect-enough-apples/solutions/2577652/tu-jie-o1-zuo-fa-pythonjavacgojsrust-by-oms4k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-o1-zuo-fa-pythonjavacgojsrust-by-oms4k`
- topic id：`2577652`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

![LC1954-c.png](https://pic.leetcode.cn/1703216655-CnqSqn-LC1954-c.png)

设正方形的**周长**为 $8n$，则其**边长**为 $2n$。

问题相当于求最小的 $n$，满足

$$
2n(n+1)(2n+1)\ge \textit{neededApples}
$$

上式变形为

$$
n(n+1)(n+\dfrac{1}{2})\ge \dfrac{1}{4}\textit{neededApples}
$$

设 $m = \left\lfloor \sqrt[3]{\dfrac{1}{4}\textit{neededApples}} \right\rfloor$。

- 由于 $(m-1)m(m-\dfrac{1}{2}) < m^3 \le \dfrac{1}{4}\textit{neededApples}$，所以 $m-1$ 必不满足要求。
- 由于 $(m+1)(m+2)(m+\dfrac{3}{2}) > (m+1)^3 > \dfrac{1}{4}\textit{neededApples}$，所以 $m+1$ 必满足要求。注意 $m+1>\left\lceil\sqrt[3]{\dfrac{1}{4}\textit{neededApples}}\right\rceil$。
- $m$ 是否满足要求？计算一下就知道了。

因此，直接计算出 $n = \left\lfloor \sqrt[3]{\dfrac{1}{4}\textit{neededApples}} \right\rfloor$，如果 $2n(n+1)(2n+1)< \textit{neededApples}$ 则将 $n$ 加一。

> 注：在本题的数据范围下，`cbrt` 算出的整数部分是正确的，不会因为浮点误差导致对 `xxx.999999` 下取整的错误。

```py [sol-Python3]
class Solution:
    def minimumPerimeter(self, neededApples: int) -> int:
        n = int(cbrt(neededApples / 4))
        if 2 * n * (n + 1) * (2 * n + 1) < neededApples:
            n += 1
        return 8 * n
```

```java [sol-Java]
class Solution {
    public long minimumPerimeter(long neededApples) {
        long n = (long) Math.cbrt(neededApples / 4.0);
        if (2 * n * (n + 1) * (2 * n + 1) < neededApples) {
            n++;
        }
        return 8 * n;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumPerimeter(long long neededApples) {
        long long n = cbrt(neededApples / 4.0);
        if (2 * n * (n + 1) * (2 * n + 1) < neededApples) {
            n++;
        }
        return 8 * n;
    }
};
```

```go [sol-Go]
func minimumPerimeter(neededApples int64) int64 {
    n := int64(math.Cbrt(float64(neededApples) / 4))
    if 2*n*(n+1)*(2*n+1) < neededApples {
        n++
    }
    return 8 * n
}
```

```js [sol-JavaScript]
var minimumPerimeter = function(neededApples) {
    let n = Math.floor(Math.cbrt(neededApples / 4));
    if (2 * n * (n + 1) * (2 * n + 1) < neededApples) {
        n++;
    }
    return 8 * n;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_perimeter(needed_apples: i64) -> i64 {
        let mut n = (needed_apples as f64 / 4.0).cbrt() as i64;
        if 2 * n * (n + 1) * (2 * n + 1) < needed_apples {
            n += 1;
        }
        8 * n
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。开立方有专用的计算函数 `cbrt`，时间可以视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

#### 相似题目

- [1739. 放置盒子](https://leetcode.cn/problems/building-boxes/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `七、杂项 / §7.10 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.10 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
