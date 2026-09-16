# 1884. 鸡蛋掉落-两枚鸡蛋

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/egg-drop-with-2-eggs-and-n-floors/
- 题目 slug：`egg-drop-with-2-eggs-and-n-floors`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/egg-drop-with-2-eggs-and-n-floors/solutions/2945577/liang-chong-fang-fa-dong-tai-gui-hua-shu-hd4i/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：动态规划/数学（Python/Java/C++/Go）](https://leetcode.cn/problems/egg-drop-with-2-eggs-and-n-floors/solutions/2945577/liang-chong-fang-fa-dong-tai-gui-hua-shu-hd4i/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-dong-tai-gui-hua-shu-hd4i`
- topic id：`2945577`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：动态规划

### 一、寻找子问题

假设 $n=10$。

如果第一枚鸡蛋在 $4$ 楼扔下，分类讨论：

- 如果鸡蛋碎了，那么接下来只能依次在 $1,2,3$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $1+3=4$ 次。
- 如果鸡蛋没碎，那么接下来可以在 $5$ 到 $10$ 楼中继续扔第一枚鸡蛋，这等价于在一栋有 $10-4=6$ 层楼的建筑中扔鸡蛋的子问题。这个子问题的结果加上 $1$，就是原问题 $n=10$ 的答案。

这两种情况取最大值，因为在扔之前，我们不知道鸡蛋是否会碎。**为了保证无论在何种情况下，我们都可以确定 $f$ 的值，所以要取最大值。**

一般地，可以枚举第一枚鸡蛋在 $1,2,3,\cdots,10$ 楼扔下，分别计算每种情况需要操作多少次，取其中最小值，作为最终的答案。

比如第一枚鸡蛋在 $4$ 楼扔下，到最终确定 $f$，需要操作 $4$ 次。而第一枚鸡蛋在 $2$ 楼扔下，到最终确定 $f$，需要操作 $5$ 次。那么相比在 $2$ 楼扔，肯定是在 $4$ 楼扔更优。

由于我们会遇到**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

### 二、状态定义与状态转移方程

根据上面的讨论，定义状态为 $\textit{dfs}(i)$，表示在一栋有 $i$ 层楼的建筑中扔鸡蛋，无论 $f$ 等于多少，我们都能确定 $f$ 的最小操作次数。

枚举第一枚鸡蛋在 $j=1,2,3,\cdots,i$ 楼扔下，分类讨论：

- 如果鸡蛋碎了，那么接下来只能依次在 $1,2,3,j-1$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $1+(j-1)=j$ 次。
- 如果鸡蛋没碎，那么接下来可以在 $j+1$ 到 $i$ 楼中继续扔第一枚鸡蛋，这等价于在一栋有 $i-j$ 层楼的建筑中扔鸡蛋的子问题，即 $\textit{dfs}(i-j)$，将其加一即为总操作次数。

这两种情况取最大值，即为在第 $j$ 楼扔下第一枚鸡蛋，到最终确定 $f$，所需要的最小操作次数，即

$$
\max(j, \textit{dfs}(i-j)+1)
$$

对 $j=1,2,3,\cdots,i$ 的上式取最小值，得

$$
\textit{dfs}(i) = \min_{j=1}^i \max(j, \textit{dfs}(i-j)+1)
$$

**递归边界**：$\textit{dfs}(0)=0$。此时 $f$ 一定等于 $0$，无需扔鸡蛋。

**递归入口**：$\textit{dfs}(n)$，也就是答案。

### 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$，但本题除了 $\textit{dfs}(0)$，其余 $\textit{dfs}(i)$ 都是正数，所以初始化成 $0$ 也可以。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int) -> int:
    if i == 0:
        return 0
    return min(max(j, dfs(i - j) + 1) for j in range(1, i + 1))

class Solution:
    def twoEggDrop(self, n: int) -> int:
        return dfs(n)
```

```py [sol-Python3 写法二]
class Solution:
    @cache
    def twoEggDrop(self, n: int) -> int:
        if n == 0:
            return 0
        return min(max(j, self.twoEggDrop(n - j) + 1) for j in range(1, n + 1))
```

```java [sol-Java]
class Solution {
    private static final int[] memo = new int[1001];

    public int twoEggDrop(int n) {
        if (n == 0) {
            return 0;
        }
        if (memo[n] > 0) { // 之前计算过
            return memo[n];
        }
        int res = Integer.MAX_VALUE;
        for (int j = 1; j <= n; j++) {
            res = Math.min(res, Math.max(j, twoEggDrop(n - j) + 1));
        }
        return memo[n] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
int memo[1001];

class Solution {
public:
    int twoEggDrop(int n) {
        if (n == 0) {
            return 0;
        }
        int& res = memo[n]; // 注意这里是引用
        if (res) { // 之前计算过
            return res;
        }
        res = INT_MAX;
        for (int j = 1; j <= n; j++) {
            res = min(res, max(j, twoEggDrop(n - j) + 1));
        }
        return res;
    }
};
```

```go [sol-Go]
var memo [1001]int

func twoEggDrop(n int) int {
    if n == 0 {
        return 0
    }
    p := &memo[n]
    if *p > 0 { // 之前计算过
        return *p
    }
    res := math.MaxInt
    for j := 1; j <= n; j++ {
        res = min(res, max(j, twoEggDrop(n-j)+1))
    }
    *p = res // 记忆化
    return res
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以总的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$ 或者 $\mathcal{O}(N)$，$N=1000$。保存多少状态，就需要多少空间。

### 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是一样的，都表示在一栋有 $i$ 层楼的建筑中扔鸡蛋，无论 $f$ 等于多少，我们都能确定 $f$ 的最小操作次数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \min_{j=1}^i \max(j, f[i-j]+1)
$$

初始值 $f[0]=0$，翻译自递归边界 $\textit{dfs}(0)=0$。

答案为 $f[n]$，翻译自递归入口 $\textit{dfs}(n)$。

代码实现时，可以把 $f$ 数组的计算写在外面（预处理），这样无需每个测试数据都重新计算一遍。

```py [sol-Python3]
f = [0] * 1001
for i in range(1, len(f)):
    f[i] = min(max(j, f[i - j] + 1) for j in range(1, i + 1))

class Solution:
    def twoEggDrop(self, n: int) -> int:
        return f[n]
```

```java [sol-Java]
class Solution {
    private static final int[] f = new int[1001];

    static {
        for (int i = 1; i < f.length; i++) {
            f[i] = Integer.MAX_VALUE;
            for (int j = 1; j <= i; j++) {
                f[i] = Math.min(f[i], Math.max(j, f[i - j] + 1));
            }
        }
    }

    public int twoEggDrop(int n) {
        return f[n];
    }
}
```

```cpp [sol-C++]
const int MX = 1000;
int f[MX + 1];

auto init = [] {
    for (int i = 1; i <= MX; i++) {
        f[i] = INT_MAX;
        for (int j = 1; j <= i; j++) {
            f[i] = min(f[i], max(j, f[i - j] + 1));
        }
    }
    return 0;
}();

class Solution {
public:
    int twoEggDrop(int n) {
        return f[n];
    }
};
```

```go [sol-Go]
var f [1001]int

func init() {
    for i := 1; i < len(f); i++ {
        f[i] = math.MaxInt
        for j := 1; j <= i; j++ {
            f[i] = min(f[i], max(j, f[i-j]+1))
        }
    }
}

func twoEggDrop(n int) int {
    return f[n]
}
```

#### 复杂度分析

- 时间复杂度：预处理 $\mathcal{O}(N^2)$，其中 $N=1000$。
- 空间复杂度：预处理 $\mathcal{O}(N)$。

## 方法二：数学

反过来，如果已知答案（操作次数），$n$ 最大可以是多少？

假设答案是 $5$，也就是我们只能操作 $5$ 次。第一次操作，你打算在几楼扔第一枚鸡蛋？

你可能会想：搏一搏，单车变摩托。楼层越高越好嘛，如果鸡蛋没碎，这对我们会更有利。

最高可以在几楼扔？可以在 $6$ 楼扔吗？

不能。万一~~赌错~~鸡蛋碎了，后面就只能依次在 $1,2,3,4,5$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $6$ 次。

所以第一次操作的最优做法，是在 $5$ 楼扔第一枚鸡蛋：

- 即使鸡蛋碎了，后续也只需要依次在 $1,2,3,4$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $1+4=5$ 次。
- 如果鸡蛋没碎，现在还剩下 $4$ 次扔蛋机会，下一步，你打算在几楼扔鸡蛋？

同理，在 $5+4=9$ 楼扔第一枚鸡蛋：

- 如果鸡蛋碎了，后续只需要依次在 $6,7,8$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $1+1+3=5$ 次。
- 如果鸡蛋没碎，现在还剩下 $3$ 次扔蛋机会，下一步，你打算在几楼扔鸡蛋？

同理，在 $5+4+3=12$ 楼扔第一枚鸡蛋：

- 如果鸡蛋碎了，后续只需要依次在 $10,11$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $1+1+1+2=5$ 次。
- 如果鸡蛋没碎，现在还剩下 $2$ 次扔蛋机会，下一步，你打算在几楼扔鸡蛋？

同理，在 $5+4+3+2=14$ 楼扔第一枚鸡蛋：

- 如果鸡蛋碎了，由于只剩下一次操作机会，只能在 $13$ 楼扔第二枚鸡蛋。如果鸡蛋碎了，说明 $f$ 是 $12$，否则 $f$ 是 $13$。
- 如果鸡蛋没碎，由于只剩下一次操作机会，只能在 $15$ 楼扔第一枚鸡蛋。如果鸡蛋碎了，说明 $f$ 是 $14$；否则，在 $n=15$ 的前提下，说明 $f=n=15$。~~这是颗铁蛋~~
- ⚠**注意**：$n$ 不能大于 $15$。如果 $n=16$，我们在 $15$ 楼扔第一枚鸡蛋，并且鸡蛋没碎，由于没有剩余操作次数，我们无法获知 $f$ 到底是 $15$ 还是 $16$。

综上所述，如果答案是 $5$，也就是可以扔 $5$ 次鸡蛋，那么 $n$ 最大是 $5+4+3+2+1=15$。对于更大的 $n$，比如 $n=16$，我们无法保证只用 $5$ 次操作就能确定 $f$（至少要 $6$ 次操作才能保证可以确定 $f$）。

一般地，设答案为 $x$，必须满足

$$
x+(x-1)+(x-2) + \cdots + 2 + 1 \ge n
$$

即

$$
\dfrac{x(x+1)}{2} \ge n
$$

解一元二次不等式，得

$$
x \ge \dfrac{\sqrt{8n+1}-1}{2}
$$

由于 $x$ 是整数，所以最小的 $x$ 为

$$
\left\lceil\dfrac{\sqrt{8n+1}-1}{2}\right\rceil
$$

```py [sol-Python3]
class Solution:
    def twoEggDrop(self, n: int) -> int:
        return ceil((sqrt(n * 8 + 1) - 1) / 2)
```

```java [sol-Java]
class Solution {
    public int twoEggDrop(int n) {
        return (int) Math.ceil((Math.sqrt(n * 8 + 1) - 1) / 2);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int twoEggDrop(int n) {
        return ceil((sqrt(n * 8 + 1) - 1) / 2);
    }
};
```

```go [sol-Go]
func twoEggDrop(n int) int {
    return int(math.Ceil((math.Sqrt(float64(n*8+1)) - 1) / 2))
}
```

在 $a$ 是整数，$b$ 是正整数的前提下，有如下恒等式

$$
\left\lceil\dfrac{x+a}{b}\right\rceil = \left\lceil\dfrac{\lceil x \rceil + a}{b}\right\rceil
$$

[证明思路](https://zhuanlan.zhihu.com/p/1893240318645732760)。

关于上取整的计算，在 $c$ 是整数，$d$ 是正整数的前提下，有如下恒等式

$$
\left\lceil\dfrac{c}{d}\right\rceil = \left\lfloor\dfrac{c+d-1}{d}\right\rfloor
$$

见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

结合上面两个恒等式，我们有

$$
\left\lceil\dfrac{\sqrt{8n+1}-1}{2}\right\rceil = \left\lceil\dfrac{\lceil \sqrt{8n+1} \rceil -1}{2}\right\rceil = \left\lfloor\dfrac{\lceil \sqrt{8n+1} \rceil}{2}\right\rfloor
$$

对于本题来说，这个优化其实无所谓，但在 [3296. 移山所需的最少秒数](https://leetcode.cn/problems/minimum-number-of-seconds-to-make-mountain-height-zero/) 中，这个优化可以减少运行时间。例如 Java 可以从 16 ms 优化到 10 ms。

```py [sol-Python3]
class Solution:
    def twoEggDrop(self, n: int) -> int:
        return ceil(sqrt(n * 8 + 1)) // 2
```

```java [sol-Java]
class Solution {
    public int twoEggDrop(int n) {
        return (int) Math.ceil(Math.sqrt(n * 8 + 1)) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int twoEggDrop(int n) {
        return (int) ceil(sqrt(n * 8 + 1)) / 2;
    }
};
```

```go [sol-Go]
func twoEggDrop(n int) int {
    return int(math.Ceil(math.Sqrt(float64(n*8+1)))) / 2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。CPU 有专门的计算平方根的指令，可以视作 $\mathcal{O}(1)$。（算平方根和算一次浮点除法差不多快）
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果有 $3$ 枚鸡蛋呢？$k$ 枚鸡蛋呢？DP 代码要如何修改？

见 [887. 鸡蛋掉落](https://leetcode.cn/problems/super-egg-drop/)。

## 相似题目

- [375. 猜数字大小 II](https://leetcode.cn/problems/guess-number-higher-or-lower-ii/)

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

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
