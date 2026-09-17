# 2862. 完全子集的最大元素和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-element-sum-of-a-complete-subset-of-indices/
- 题目 slug：`maximum-element-sum-of-a-complete-subset-of-indices`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.3 质因数分解
- 难度分：2292
- 外部题解来源：https://leetcode.cn/problems/maximum-element-sum-of-a-complete-subset-of-indices/solutions/2446037/an-zhao-corei-fen-zu-pythonjavacgo-by-en-i6nu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：按照 core 分组 / 枚举 core / 打表预处理（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/maximum-element-sum-of-a-complete-subset-of-indices/solutions/2446037/an-zhao-corei-fen-zu-pythonjavacgo-by-en-i6nu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`an-zhao-corei-fen-zu-pythonjavacgo-by-en-i6nu`
- topic id：`2446037`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：按照下标的 core 值分组

定义 $\text{core}(n)$ 为 $n$ 除去完全平方因子后的剩余结果。

例如 $\text{core}(8)=8/4=2,\ \text{core}(12)=12/4=3, \text{core}(25)=25/25=1, \text{core}(5)=5/1=5$。

计算方式同质因数分解，把 $n$ 的所有出现次数为奇数的质因子相乘，即为 $\text{core}(n)$。

根据题意，如果同一组中有两个数，它们的下标的 $\text{core}$ 值不同，那么这两个数相乘，就不是一个完全平方数。

所以，同一组内的数字下标的 $\text{core}$ 值必须都一样。

那么按照下标的 $\text{core}$ 值分组，累加同一组的元素和，最大元素和即为答案。

[视频讲解](https://www.bilibili.com/video/BV1Lm4y1N7mf/) 第四题。

```py [sol-Python3]
@cache  # 保存 core(n) 的计算结果，测试用例之间可以复用
def core(n: int) -> int:
    i = 2
    while i * i <= n:
        while n % (i * i) == 0:
            n //= i * i
        i += 1
    return n

class Solution:
    def maximumSum(self, nums: List[int]) -> int:
        s = [0] * (len(nums) + 1)
        for i, x in enumerate(nums, 1):
            s[core(i)] += x
        return max(s)
```

```java [sol-Java]
class Solution {
    public long maximumSum(List<Integer> nums) {
        long ans = 0;
        int n = nums.size();
        long[] sum = new long[n + 1];
        for (int i = 0; i < nums.size(); i++) {
            int c = core(i + 1);
            sum[c] += nums.get(i);
            ans = Math.max(ans, sum[c]);
        }
        return ans;
    }

    private int core(int n) {
        for (int i = 2; i * i <= n; i++) {
            while (n % (i * i) == 0) {
                n /= i * i;
            }
        }
        return n;
    }
}
```

```cpp [sol-C++]
class Solution {
    int core(int n) {
        for (int i = 2; i * i <= n; i++) {
            while (n % (i * i) == 0) {
                n /= i * i;
            }
        }
        return n;
    }

public:
    long long maximumSum(vector<int>& nums) {
        vector<long long> sum(nums.size() + 1);
        for (int i = 0; i < nums.size(); i++) {
            sum[core(i + 1)] += nums[i];
        }
        return ranges::max(sum);
    }
};
```

```go [sol-Go]
func core(n int) int {
    for i := 2; i*i <= n; i++ {
        for n%(i*i) == 0 {
            n /= i * i
        }
    }
    return n
}

func maximumSum(nums []int) (ans int64) {
    sum := make([]int64, len(nums)+1)
    for i, x := range nums {
        c := core(i + 1)
        sum[c] += int64(x)
        ans = max(ans, sum[c])
    }
    return
}
```

```js [sol-JavaScript]
function core(n) {
    for (let i = 2; i * i <= n; i++) {
        while (n % (i * i) === 0) {
            n /= i * i;
        }
    }
    return n;
}

var maximumSum = function(nums) {
    const sum = Array(nums.length + 1).fill(0);
    for (let i = 0; i < nums.length; i++) {
        sum[core(i + 1)] += nums[i];
    }
    return Math.max(...sum);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\sqrt{n})$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：枚举 core

我们还可以从 $1$ 开始枚举 $i$ 和所有 $\text{core}$ 值等于 $i$ 的下标，也就是 $ij^2$。

例如 $i=3$ 时，我们可以枚举 

$$
3\times 1, 3\times 4, 3\times 9, 3\times 16,3\times 25,\cdots
$$

这些下标对应的元素值都在同一组中。

可能你会觉得这不对，比如 $i=4$ 时，我们枚举的是

$$
4\times 1, 4\times 4, 4\times 9, 4\times 16,4\times 25,\cdots
$$

而这些数的 $\text{core}$ 值都等于 $1$，而不是 $4$。

没有关系！在 $i=1$ 时，上面这些数都枚举到了（相当于是 $i=1$ 时枚举的数的子集），所以上面这些数的元素和不会超过 $i=1$ 时计算出来的元素和。**我们不会漏掉最大的元素和。**

```py [sol-Python3]
class Solution:
    def maximumSum(self, nums: List[int]) -> int:
        ans = 0
        n = len(nums)
        for i in range(1, n + 1):
            s = 0
            for j in range(1, isqrt(n // i) + 1):
                s += nums[i * j * j - 1]  # -1 是因为数组下标从 0 开始
            ans = max(ans, s)
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumSum(List<Integer> nums) {
        Integer[] a = nums.toArray(Integer[]::new);
        long ans = 0;
        int n = a.length;
        for (int i = 1; i <= n; i++) {
            long sum = 0;
            for (int j = 1; i * j * j <= n; j++) {
                sum += a[i * j * j - 1]; // -1 是因为数组下标从 0 开始
            }
            ans = Math.max(ans, sum);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumSum(vector<int>& nums) {
        long long ans = 0;
        int n = nums.size();
        for (int i = 1; i <= n; i++) {
            long long sum = 0;
            for (int j = 1; i * j * j <= n; j++) {
                sum += nums[i * j * j - 1]; // -1 是因为数组下标从 0 开始
            }
            ans = max(ans, sum);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(nums []int) (ans int64) {
    n := len(nums)
    for i := 1; i <= n; i++ {
        sum := int64(0)
        for j := 1; i*j*j <= n; j++ {
            sum += int64(nums[i*j*j-1]) // -1 是因为数组下标从 0 开始
        }
        ans = max(ans, sum)
    }
    return
}
```

```js [sol-JavaScript]
var maximumSum = function(nums) {
    const n = nums.length;
    let ans = 0;
    for (let i = 1; i <= n; i++) {
        let sum = 0;
        for (let j = 1; i * j * j <= n; j++) {
            sum += nums[i * j * j - 1]; // -1 是因为数组下标从 0 开始
        }
        ans = Math.max(ans, sum);
    }
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。循环次数可近似为 $\sqrt{n}\cdot(1/\sqrt{1} + 1/\sqrt{2} + ... + 1/\sqrt{n})$，由 $f(x)=1/\sqrt{x}$ 的积分可知，$1/\sqrt{1} + 1/\sqrt{2} + ... 1/\sqrt{n} = \mathcal{O}(\sqrt{n})$，所以总的循环次数为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法三：预处理所有 core 值

按照方法二的计算方式，可以直接打表预处理 $10^4$ 内的 $\text{core}$ 值。

```py [sol-Python3]
MX = 10001
core = [0] * MX
for i in range(1, MX):
    if core[i] == 0:  # i 不含完全平方因子，可以作为 core 值
        for j in range(1, isqrt(MX // i) + 1):
            core[i * j * j] = i

class Solution:
    def maximumSum(self, nums: List[int]) -> int:
        s = [0] * (len(nums) + 1)
        for i, x in enumerate(nums, 1):  # 下标从 1 开始
            s[core[i]] += x
        return max(s)
```

```java [sol-Java]
class Solution {
    private final static int MX = 10000;
    private static int[] core = new int[MX + 1];

    static {
        for (int i = 1; i <= MX; i++) {
            if (core[i] == 0) { // i 不含完全平方因子，可以作为 core 值
                for (int j = 1; i * j * j <= MX; j++) {
                    core[i * j * j] = i;
                }
            }
        }
    }

    public long maximumSum(List<Integer> nums) {
        long ans = 0;
        int n = nums.size();
        long[] sum = new long[n + 1];
        for (int i = 1; i <= n; i++) {
            sum[core[i]] += nums.get(i - 1);
            ans = Math.max(ans, sum[core[i]]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
const int MX = 10001;
int core[MX];
int init = [] {
    for (int i = 1; i < MX; ++i) {
        if (core[i] == 0) { // i 不含完全平方因子，可以作为 core 值
            for (int j = 1; i * j * j < MX; ++j) {
                core[i * j * j] = i;
            }
        }
    }
    return 0;
}();

class Solution {
public:
    long long maximumSum(vector<int>& nums) {
        long long ans = 0;
        vector<long long> sum(nums.size() + 1, 0);
        for (int i = 1; i <= nums.size(); ++i) {
            sum[core[i]] += nums[i - 1];
            ans = max(ans, sum[core[i]]);
        }
        return ans;
    }
};
```

```go [sol-Go]
const mx int = 1e4
var core [mx + 1]int
func init() {
    for i := 1; i <= mx; i++ {
        if core[i] == 0 { // i 不含完全平方因子，可以作为 core 值
            for j := 1; i*j*j <= mx; j++ {
                core[i*j*j] = i
            }
        }
    }
}

func maximumSum(nums []int) (ans int64) {
    sum := make([]int64, len(nums)+1)
    for i, x := range nums {
        c := core[i+1]
        sum[c] += int64(x)
        ans = max(ans, sum[c])
    }
    return
}
```

```js [sol-JavaScript]
const MX = 10001;
const core = Array(MX).fill(0);
for (let i = 1; i < MX; i++) {
    if (core[i] === 0) { // i 不含完全平方因子，可以作为 core 值
        for (let j = 1; i * j * j < MX; j++) {
            core[i * j * j] = i;
        }
    }
}

var maximumSum = function(nums) {
    const sum = Array(nums.length + 1).fill(0);
    for (let i = 0; i < nums.length; i++) {
        sum[core[i + 1]] += nums[i];
    }
    return Math.max(...sum);
};
```

#### 复杂度分析

同方法二。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.3 质因数分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.3 质因数分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
