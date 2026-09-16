# 2266. 统计打字方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-number-of-texts/
- 题目 slug：`count-number-of-texts`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.1 爬楼梯
- 难度分：1857
- 外部题解来源：https://leetcode.cn/problems/count-number-of-texts/solutions/1477311/by-endlesscheng-gj8f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分组 + 爬楼梯（Python/Go/C++/Java）](https://leetcode.cn/problems/count-number-of-texts/solutions/1477311/by-endlesscheng-gj8f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-gj8f`
- topic id：`1477311`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

比如 $\textit{pressedKeys}$ 中有连续 $10$ 个字符 $\texttt{2}$，我们需要把这 $10$ 个字符拆分成若干段，每段的长度可以是 $1,2,3$，分别对应字母 $\texttt{a},\texttt{b},\texttt{c}$。比如 $10=2+3+1+2+2$ 就是字符串 $\texttt{bcabb}$。问题相当于把 $10$ 拆分成若干 $1,2,3$ 之和的方案数。

这本质上是 [70. 爬楼梯](https://leetcode.cn/problems/climbing-stairs/)，每次可以跳 $1,2,3$ 个台阶，计算跳 $10$ 个台阶的方案数。

定义一个类似爬楼梯的 DP，即 $f[i]$ 表示长为 $i$ 的只有一种字符的字符串所对应的文字信息种类数，我们可以将末尾的 $1$ 个、$2$ 个或 $3$ 个字符变成一个字母，接下来解决的问题（子问题）为：长为 $i-1,i-2,i-3$ 的只有一种字符的字符串所对应的文字信息种类数，即

$$
f[i] = f[i-1]+f[i-2]+f[i-3]
$$

其中加法是因为三种方案互斥，根据**加法原理**相加。

对于字符为 $\texttt{7}$ 或 $\texttt{9}$ 的情况，同理，定义 $g[i]$ 表示长为 $i$ 的只有一种字符的字符串对应的文字信息种类数，我们可以将末尾的 $1$ 个、$2$ 个、$3$ 个或 $4$ 个字符变成一个字母，接下来解决的问题（子问题）为：长为 $i-1,i-2,i-3,i-4$ 的只有一种字符的字符串所对应的文字信息种类数，即

$$
g[i] = g[i-1]+g[i-2]+g[i-3]+g[i-4]
$$

最后，由于各个组（连续相同子串）的打字方案互相独立，根据**乘法原理**，把各个组的方案数相乘，即为答案。

注意取模。关于取模的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```python [sol-Python3]
MOD = 1_000_000_007
f = [1, 1, 2, 4]
g = [1, 1, 2, 4]
for _ in range(10 ** 5 - 3):  # 预处理所有长度的结果
    f.append((f[-1] + f[-2] + f[-3]) % MOD)
    g.append((g[-1] + g[-2] + g[-3] + g[-4]) % MOD)

class Solution:
    def countTexts(self, pressedKeys: str) -> int:
        ans = 1
        for ch, s in groupby(pressedKeys):
            m = len(list(s))
            ans = ans * (g[m] if ch in "79" else f[m]) % MOD
        return ans
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 100_001;
    private static final long[] f = new long[MX];
    private static final long[] g = new long[MX];
    private static boolean done = false;

    // 这样写比 static block 快很多
    private void init() {
        if (done) {
            return;
        }
        done = true;
        f[0] = g[0] = 1;
        f[1] = g[1] = 1;
        f[2] = g[2] = 2;
        f[3] = g[3] = 4;
        for (int i = 4; i < MX; i++) {
            f[i] = (f[i - 1] + f[i - 2] + f[i - 3]) % MOD;
            g[i] = (g[i - 1] + g[i - 2] + g[i - 3] + g[i - 4]) % MOD;
        }
    }

    public int countTexts(String S) {
        init();
        long ans = 1;
        int cnt = 0;
        char[] s = S.toCharArray();
        for (int i = 0; i < s.length; i++) {
            char c = s[i];
            cnt++;
            if (i == s.length - 1 || c != s[i + 1]) {
                ans = ans * (c != '7' && c != '9' ? f[cnt] : g[cnt]) % MOD;
                cnt = 0;
            }
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 100'001;

long long f[MX], g[MX];

int init = []() {
    f[0] = g[0] = 1;
    f[1] = g[1] = 1;
    f[2] = g[2] = 2;
    f[3] = g[3] = 4;
    for (int i = 4; i < MX; ++i) {
        f[i] = (f[i - 1] + f[i - 2] + f[i - 3]) % MOD;
        g[i] = (g[i - 1] + g[i - 2] + g[i - 3] + g[i - 4]) % MOD;
    }
    return 0;
}();

class Solution {
public:
    int countTexts(string s) {
        long long ans = 1;
        int cnt = 0;
        for (int i = 0; i < s.length(); i++) {
            char c = s[i];
            cnt++;
            if (i == s.length() - 1 || c != s[i + 1]) {
                ans = ans * (c != '7' && c != '9' ? f[cnt] : g[cnt]) % MOD;
                cnt = 0;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const mx = 100_001

var f = [mx]int{1, 1, 2, 4}
var g = f

func init() {
    for i := 4; i < mx; i++ {
        f[i] = (f[i-1] + f[i-2] + f[i-3]) % mod
        g[i] = (g[i-1] + g[i-2] + g[i-3] + g[i-4]) % mod
    }
}

func countTexts(s string) int {
    ans, cnt := 1, 0
    for i, c := range s {
        cnt++
        if i == len(s)-1 || byte(c) != s[i+1] {
            if c != '7' && c != '9' {
                ans = ans * f[cnt] % mod
            } else {
                ans = ans * g[cnt] % mod
            }
            cnt = 0
        }
    }
    return ans
}
```

#### 复杂度分析

忽略预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{pressedKeys}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `一、入门 DP / §1.1 爬楼梯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.1 爬楼梯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
