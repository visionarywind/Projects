# 2320. 统计放置房子的方式数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-number-of-ways-to-place-houses/
- 题目 slug：`count-number-of-ways-to-place-houses`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.2 打家劫舍 / 答疑
- 难度分：1608
- 外部题解来源：https://leetcode.cn/problems/count-number-of-ways-to-place-houses/solutions/1625979/d-by-endlesscheng-gybx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[线性 DP（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/count-number-of-ways-to-place-houses/solutions/1625979/d-by-endlesscheng-gybx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`d-by-endlesscheng-gybx`
- topic id：`1625979`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

单独考虑一侧的房子，定义 $f[i]$ 表示前 $i$ 个地块的放置方案数，其中第 $i$ 个地块可以放房子，也可以不放房子。

考虑第 $i$ 个地块：

- 若不放房子，那么第 $i-1$ 个地块可放可不放，则有 $f[i] = f[i-1]$；
- 若放房子，那么第 $i-1$ 个地块无法放房子，第 $i-2$ 个地块可放可不放，则有 $f[i] = f[i-2]$。

因此

$$
f[i] = f[i-1] + f[i-2]
$$

边界为

- $f[0]=1$，空只有一种选择，就是不放房子。
- $f[1]=2$，放与不放两种方案。

由于两侧的房屋互相独立，根据乘法原理，答案为 $f[n]^2$。

为什么可以在 DP 的计算过程中取模？请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1pW4y1r7xs)。

```py [sol-Python3]
MOD = 1_000_000_007
MX = 10_001

f = [1, 2]
while len(f) < MX:
    f.append((f[-1] + f[-2]) % MOD)

class Solution:
    def countHousePlacements(self, n: int) -> int:
        return f[n] ** 2 % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 10_001;
    private static final int[] f = new int[MX];

    static {
        f[0] = 1;
        f[1] = 2;
        for (int i = 2; i < MX; i++) {
            f[i] = (f[i - 1] + f[i - 2]) % MOD;
        }
    }

    public int countHousePlacements(int n) {
        return (int) ((long) f[n] * f[n] % MOD);
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 10'001;

int f[MX] = {1, 2};

int init = []() {
    for (int i = 2; i < MX; i++) {
        f[i] = (f[i - 1] + f[i - 2]) % MOD;
    }
    return 0;
}();

class Solution {
public:
    int countHousePlacements(int n) {
        return 1LL * f[n] * f[n] % MOD;
    }
};
```

```c [sol-C]
#define MOD 1000000007
#define MX 10001

int f[MX];
bool initialized; // 是否初始化过

void init() {
    if (initialized) {
        return;
    }
    initialized = true;
    f[0] = 1;
    f[1] = 2;
    for (int i = 2; i < MX; i++) {
        f[i] = (f[i - 1] + f[i - 2]) % MOD;
    }
}

int countHousePlacements(int n) {
    init();
    return 1LL * f[n] * f[n] % MOD;
}
```

```go [sol-Go]
const mod = 1_000_000_007

var f = [10_001]int{1, 2}

func init() {
	for i := 2; i < len(f); i++ {
		f[i] = (f[i-1] + f[i-2]) % mod
	}
}

func countHousePlacements(n int) int {
	return f[n] * f[n] % mod
}
```

```js [sol-JavaScript]
const MOD = 1_000_000_007;
const MX = 10_001

const f = Array(MX);
f[0] = 1;
f[1] = 2;
for (let i = 2; i < MX; i++) {
    f[i] = (f[i - 1] + f[i - 2]) % MOD;
}

var countHousePlacements = function(n) {
    return Number(BigInt(f[n]) * BigInt(f[n]) % BigInt(MOD));
};
```

```rust [sol-Rust]
static mut INITIALIZED: bool = false;
static mut F: [i32; 10001] = [0; 10001];

fn init_once() {
    unsafe {
        if INITIALIZED {
            return;
        }
        INITIALIZED = true;

        F[0] = 1;
        F[1] = 2;
        for i in 2..10001 {
            F[i] = (F[i - 1] + F[i - 2]) % 1_000_000_007;
        }
    }
}

impl Solution {
    pub fn count_house_placements(n: i32) -> i32 {
        init_once();
        unsafe {
            let v = F[n as usize] as i64;
            (v * v % 1_000_000_007) as _
        }
    }
}
```

#### 复杂度分析

忽略预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

添加一个约束：同一列不能都盖房子，要怎么做？

欢迎在评论区分享你的思路/代码。

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

本题来自 `一、入门 DP / §1.2 打家劫舍 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.2 打家劫舍 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
