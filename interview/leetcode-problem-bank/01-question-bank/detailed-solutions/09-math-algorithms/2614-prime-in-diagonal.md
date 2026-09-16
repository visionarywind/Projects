# 2614. 对角线上的质数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/prime-in-diagonal/
- 题目 slug：`prime-in-diagonal`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.1 判断质数
- 难度分：1375
- 外部题解来源：https://leetcode.cn/problems/prime-in-diagonal/solutions/2216347/pan-duan-zhi-shu-by-endlesscheng-m6nt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[判断质数（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/prime-in-diagonal/solutions/2216347/pan-duan-zhi-shu-by-endlesscheng-m6nt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pan-duan-zhi-shu-by-endlesscheng-m6nt`
- topic id：`2216347`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

遍历两条对角线上的元素，如果是质数则更新答案的最大值。

注意 $1$ 不是质数。

## 如何判断质数

对于整数 $n$，暴力做法是枚举 $[2,n-1]$ 中的整数 $i$，判断 $n\bmod i=0$ 是否成立，如果成立则说明 $i$ 是 $n$ 的一个因子，$n$ 不是质数。

但其实只需枚举 $[2,\left\lfloor\sqrt{n}\right\rfloor]$ 中的整数 $i$。

**证明**：反证法。从小到大枚举，假设我们枚举到了一个超过 $\left\lfloor\sqrt{n}\right\rfloor$ 的整数 $j$，且 $j$ 是 $n$ 的因子，那么 $\dfrac{n}{j}$ 也是 $n$ 的因子。由于 $\dfrac{n}{j} < j$，我们会先枚举到 $i=\dfrac{n}{j}$，再枚举到 $i=j$。但由于 $\dfrac{n}{j}$ 是 $n$ 的因子，我们会在 $i=\dfrac{n}{j}$ 时停止枚举，不可能继续枚举到 $i=j$，矛盾，所以原命题成立。

## 细节

如果元素 $x$ 没有超过答案 $\textit{ans}$，那么无需判断 $x$ 是否为质数，因为它不会让答案变得更大。

```py [sol-Python3]
class Solution:
    def is_prime(self, n: int) -> bool:
        for i in range(2, isqrt(n) + 1):
            if n % i == 0:
                return False
        return n >= 2  # 1 不是质数

    def diagonalPrime(self, nums: List[List[int]]) -> int:
        ans = 0
        for i, row in enumerate(nums):
            for x in row[i], row[-1 - i]:
                if x > ans and self.is_prime(x):
                    ans = x
        return ans
```

```java [sol-Java]
class Solution {
    public int diagonalPrime(int[][] nums) {
        int n = nums.length;
        int ans = 0;
        for (int i = 0; i < n; i++) {
            int x = nums[i][i];
            if (x > ans && isPrime(x)) {
                ans = x;
            }
            x = nums[i][n - 1 - i];
            if (x > ans && isPrime(x)) {
                ans = x;
            }
        }
        return ans;
    }

    private boolean isPrime(int n) {
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return n >= 2; // 1 不是质数
    }
}
```

```cpp [sol-C++]
class Solution {
    bool is_prime(int n) {
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return n >= 2; // 1 不是质数
    }

public:
    int diagonalPrime(vector<vector<int>>& nums) {
        int n = nums.size(), ans = 0;
        for (int i = 0; i < n; i++) {
            for (int x : {nums[i][i], nums[i][n - 1 - i]}) {
                if (x > ans && is_prime(x)) {
                    ans = x;
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int isPrime(int n) {
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return n >= 2; // 1 不是质数
}

int diagonalPrime(int** nums, int numsSize, int* numsColSize) {
    int ans = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i][i];
        if (x > ans && isPrime(x)) {
            ans = x;
        }
        x = nums[i][numsSize - 1 - i];
        if (x > ans && isPrime(x)) {
            ans = x;
        }
    }
    return ans;
}
```

```go [sol-Go]
func isPrime(n int) bool {
    for i := 2; i*i <= n; i++ {
        if n%i == 0 {
            return false
        }
    }
    return n >= 2 // 1 不是质数
}

func diagonalPrime(nums [][]int) (ans int) {
    for i, row := range nums {
        if x := row[i]; x > ans && isPrime(x) {
            ans = x
        }
        if x := row[len(nums)-1-i]; x > ans && isPrime(x) {
            ans = x
        }
    }
    return
}
```

```js [sol-JavaScript]
var isPrime = function(n) {
    for (let i = 2; i * i <= n; i++) {
        if (n % i === 0) {
            return false;
        }
    }
    return n >= 2; // 1 不是质数
};

var diagonalPrime = function(nums) {
    const n = nums.length;
    let ans = 0;
    for (let i = 0; i < n; i++) {
        for (const x of [nums[i][i], nums[i][n - 1 - i]]) {
            if (x > ans && isPrime(x)) {
                ans = x;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    fn is_prime(n: i32) -> bool {
        for i in 2..=((n as f64).sqrt() as i32) {
            if n % i == 0 {
                return false;
            }
        }
        n >= 2 // 1 不是质数
    }

    pub fn diagonal_prime(nums: Vec<Vec<i32>>) -> i32 {
        let n = nums.len();
        let mut ans = 0;
        for (i, row) in nums.iter().enumerate() {
            for x in [row[i], row[n - 1 - i]] {
                if x > ans && Self::is_prime(x) {
                    ans = x;
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\sqrt{U})$，其中 $n$ 为 $\textit{nums}$ 的长度，$U$ 为两条对角线上的最大值。
- 空间复杂度：$\mathcal{O}(1)$。

**注**：本题 $n$ 很小，$U$ 比较大，直接暴力判断，比筛质数更好。

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
9. 【本题相关】[数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.1 判断质数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.1 判断质数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
