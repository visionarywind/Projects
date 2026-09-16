# 137. 只出现一次的数字 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/single-number-ii/
- 题目 slug：`single-number-ii`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/single-number-ii/solutions/2482832/dai-ni-yi-bu-bu-tui-dao-chu-wei-yun-suan-wnwy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步推导出位运算公式！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/single-number-ii/solutions/2482832/dai-ni-yi-bu-bu-tui-dao-chu-wei-yun-suan-wnwy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dai-ni-yi-bu-bu-tui-dao-chu-wei-yun-suan-wnwy`
- topic id：`2482832`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 暴力思路

设只出现一次的那个数为 $x$。用**二进制**思考：

- 如果 $x$ 的某个比特是 $0$，由于其余数字都出现了 $3$ 次，所以 $\textit{nums}$ 的所有元素在这个比特位上的 $1$ 的个数是 $3$ 的倍数。
- 如果 $x$ 的某个比特是 $1$，由于其余数字都出现了 $3$ 次，所以 $\textit{nums}$ 的所有元素在这个比特位上的 $1$ 的个数除 $3$ 余 $1$。

这启发我们**统计每个比特位上有多少个** $1$。下图比较了 [136. 只出现一次的数字](https://leetcode.cn/problems/single-number/) 与本题的异同：

![](https://pic.leetcode.cn/1697077395-HGUOiR-lc137-c.png)

先来看看如何实现「统计每个比特位的 $1$ 的个数」。

代码中用到了一些位运算技巧，请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class Solution:
    def singleNumber(self, nums: List[int]) -> int:
        ans = 0
        for i in range(31):
            cnt1 = sum(x >> i & 1 for x in nums)
            ans |= cnt1 % 3 << i
        # 最高位是符号位，下面这行相当于统计负数的个数
        cnt1 = sum(x >> 31 & 1 for x in nums)
        # 如果 cnt1 % 3 == 1，那么答案的最高位是 1，否则是 0
        # Python 只能通过减法把最高位置为 1
        return ans - (cnt1 % 3 << 31)
```

```java [sol-Java]
class Solution {
    public int singleNumber(int[] nums) {
        int ans = 0;
        for (int i = 0; i < 32; i++) {
            int cnt1 = 0;
            for (int x : nums) {
                cnt1 += x >> i & 1;
            }
            ans |= cnt1 % 3 << i;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int ans = 0;
        for (int i = 0; i < 32; i++) {
            int cnt1 = 0;
            for (int x : nums) {
                cnt1 += x >> i & 1;
            }
            ans |= cnt1 % 3 << i;
        }
        return ans;
    }
};
```

```go [sol-Go]
func singleNumber(nums []int) int {
    ans := int32(0)
    for i := 0; i < 32; i++ {
        cnt1 := int32(0)
        for _, x := range nums {
            // 力扣的 int 是 int64，需要转成 int32 从而正确获取到符号位
            cnt1 += int32(x) >> i & 1
        }
        ans |= cnt1 % 3 << i
    }
    return int(ans)
}
```

```js [sol-JavaScript]
var singleNumber = function(nums) {
    let ans = 0;
    for (let i = 0; i < 32; i++) {
        let cnt1 = 0;
        for (const x of nums) {
            cnt1 += x >> i & 1;
        }
        ans |= cnt1 % 3 << i;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_number(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        for i in 0..32 {
            let mut cnt1 = 0;
            for &x in &nums {
                cnt1 += x >> i & 1;
            }
            ans |= cnt1 % 3 << i;
        }
        ans
    }
}
```

## 位运算的魔法：模 3 加法

对于异或（模 $2$ 加法）来说，把一个数不断地异或 $1$，相当于在 $0$ 和 $1$ 之间不断转换，即：

$$
0 \rightarrow 1 \rightarrow 0 \rightarrow 1 \rightarrow \cdots
$$

类似地，模 $3$ 加法就是在 $0,1,2$ 之间不断转换，即：

$$
0 \rightarrow 1 \rightarrow 2 \rightarrow 0 \rightarrow 1 \rightarrow 2 \rightarrow \cdots
$$

由于 $0,1,2$ 需要两个比特才能表示，设这两个比特分别为 $a$ 和 $b$，即：

- $a=0,\ b=0$ 表示数字 $0$；
- $a=0,\ b=1$ 表示数字 $1$；
- $a=1,\ b=0$ 表示数字 $2$。

那么转换规则就是：

$$
(0,0) \rightarrow (0,1) \rightarrow (1,0) \rightarrow (0,0) \rightarrow  (0,1) \rightarrow (1,0) \rightarrow  \cdots
$$

这其中有大量 $0$ 和 $1$ 之间的转换，我们已经知道，这可以用异或运算实现，写成代码就是：

```
a = a ^ 1
b = b ^ 1
```

**除了两个例外情况**：

- 当 $a=0$ 且 $b=0$ 时，$a$ 必须保持不变，仍然为 $0$。
- 当 $a=1$ 时（此时 $b$ 一定是 $0$），$b$ 必须保持不变，仍然为 $0$。

换句话说，我们可以在异或运算的基础上，增加一些「约束」：

- 当 `(a | b) == 0` 时，把 $0$ 赋值给 $a$，否则（`(a | b) == 1`）可以执行异或操作。写成代码就是 `a = (a ^ 1) & (a | b)`。
- 当 `a == 1` 时，把 $0$ 赋值给 $b$，否则（`~a == 1`）可以执行异或操作。写成代码就是 `b = (b ^ 1) & ~a`。

其中 `&` 运算相当于为异或运算添加了一个约束：当……为 $1$ 时，才执行异或操作。

如果模 $3$ 加法遇到了 $0$，那么 $a$ 和 $b$ 都应当维持不变。好在把 $1$ 改成 $0$，我们的代码仍然是正确的，也就是：

```cpp
// 注意 a 和 b 是同时计算的
a = (a ^ x) & (a | b)
b = (b ^ x) & ~a
```

该代码在 $x=0$ 和 $x=1$ 的情况下都是成立的（注意 $a$ 和 $b$ 不可能都为 $1$）。

由于位运算具有「并行计算」的特点，上述运算规则可以推广到多个比特的情况。遍历 $\textit{nums}$，利用上式计算出最终的 $a$ 和 $b$。

最后，由于模 $3$ 加法的结果要么是 $0$，要么是 $1$，没有 $2$，那么根据 $b$ 的定义，这刚好就是 $b$，所以最后返回 $b$。

```py [sol-Python3]
class Solution:
    def singleNumber(self, nums: List[int]) -> int:
        a = b = 0
        for x in nums:
            a, b = (a ^ x) & (a | b), (b ^ x) & ~a
        return b
```

```java [sol-Java]
class Solution {
    public int singleNumber(int[] nums) {
        int a = 0, b = 0;
        for (int x : nums) {
            int tmpA = a;
            a = (a ^ x) & (a | b);
            b = (b ^ x) & ~tmpA;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int a = 0, b = 0;
        for (int x : nums) {
            int tmp_a = a;
            a = (a ^ x) & (a | b);
            b = (b ^ x) & ~tmp_a;
        }
        return b;
    }
};
```

```go [sol-Go]
func singleNumber(nums []int) int {
    a, b := 0, 0
    for _, x := range nums {
        a, b = (a^x)&(a|b), (b^x)&^a
    }
    return b
}
```

```js [sol-JavaScript]
var singleNumber = function(nums) {
    let a = 0, b = 0;
    for (const x of nums) {
        [a, b] = [(a ^ x) & (a | b), (b ^ x) & ~a]
    }
    return b;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_number(nums: Vec<i32>) -> i32 {
        let mut a = 0;
        let mut b = 0;
        for x in nums {
            let tmp_a = a;
            a = (a ^ x) & (a | b);
            b = (b ^ x) & !tmp_a;
        }
        b
    }
}
```

## 优化：更简洁的代码

也可以先算 $b$ 再算 $a$，那么 $a$ 的计算规则修改成：

- 当 $a=0$ 且 $b=1$ 时，$a$ 必须保持不变，仍然为 $0$。注意这里的 $b$ 是更新后的。

由于 $b=1$ 的情况在 $(0,0) \rightarrow (0,1) \rightarrow (1,0)$ 中只会出现一次，所以只要 $b=1$ 我们可以断定 $a=0$，所以

- 当 `b == 1` 时，把 $0$ 赋值给 $a$，否则（`~b == 1`）可以执行异或操作。

于是得到：

```cpp
// 先算 b 再算 a
b = (b ^ x) & ~a
a = (a ^ x) & ~b
```

这样写出来的代码比上面的更简洁：

```py [sol-Python3]
class Solution:
    def singleNumber(self, nums: List[int]) -> int:
        a = b = 0
        for x in nums:
            b = (b ^ x) & ~a
            a = (a ^ x) & ~b
        return b
```

```java [sol-Java]
class Solution {
    public int singleNumber(int[] nums) {
        int a = 0, b = 0;
        for (int x : nums) {
            b = (b ^ x) & ~a;
            a = (a ^ x) & ~b;
        }
        return b;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int a = 0, b = 0;
        for (int x : nums) {
            b = (b ^ x) & ~a;
            a = (a ^ x) & ~b;
        }
        return b;
    }
};
```

```go [sol-Go]
func singleNumber(nums []int) int {
    a, b := 0, 0
    for _, x := range nums {
        b = (b ^ x) &^ a
        a = (a ^ x) &^ b
    }
    return b
}
```

```js [sol-JavaScript]
var singleNumber = function(nums) {
    let a = 0, b = 0;
    for (const x of nums) {
        b = (b ^ x) & ~a;
        a = (a ^ x) & ~b;
    }
    return b;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_number(nums: Vec<i32>) -> i32 {
        let mut a = 0;
        let mut b = 0;
        for x in nums {
            b = (b ^ x) & !a;
            a = (a ^ x) & !b;
        }
        b
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 进阶问题

改成除了一个元素出现**一次**，其余元素都出现**五次**呢？

欢迎在评论区发表你的思路/代码。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
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

本题来自 `九、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
