# 1622. 奇妙序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fancy-sequence/
- 题目 slug：`fancy-sequence`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.3 等价转化
- 难度分：2476
- 外部题解来源：https://leetcode.cn/problems/fancy-sequence/solutions/3917656/lan-geng-xin-deng-jie-zhuan-hua-pythonja-csvl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[懒更新 + 等价转化（Python/Java/C++/Go）](https://leetcode.cn/problems/fancy-sequence/solutions/3917656/lan-geng-xin-deng-jie-zhuan-hua-pythonja-csvl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`lan-geng-xin-deng-jie-zhuan-hua-pythonja-csvl`
- topic id：`3917656`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 只有加法

从特殊到一般，先考虑一个简单的问题：只有加法，没有乘法，怎么做？

执行 $\texttt{addAll}(\textit{inc})$ 时，如果把每个数都增加 $\textit{inc}$，就太慢了。

我们可以采用一种「**懒更新**」的想法，等到调用 $\texttt{getIndex}(\textit{idx})$ 时，才做计算。

比如序列 $\textit{vals} = [3,1,4]$，执行 $\texttt{addAll}(2)$ 时，我们不去把 $\textit{vals}$ 的每个数都增加 $2$，而是用一个变量 $\textit{add}$ 表示「每个数都要增加 $\textit{add}$」。执行 $\texttt{addAll}(2)$ 时，只把 $\textit{add}$ 增加 $2$。等到调用 $\texttt{getIndex}(\textit{idx})$ 时，才计算加法：

$$
\textit{vals}[\textit{idx}] + \textit{add}
$$

即为 $\textit{vals}[\textit{idx}]$ 更新后的数值。

如何处理 $\texttt{append}(\textit{val})$ 呢？

为了让 $\textit{val}$ 兼容 $\textit{vals}[\textit{idx}] + \textit{add}$ 这个式子，我们可以先把 $\textit{val}$ **减少** $\textit{add}$，再添加到 $\textit{vals}$ 的末尾，比如 $\textit{val} = 5$，$\textit{add} = 2$，那么往 $\textit{vals}$ 的末尾添加 $5-2=3$，就可以让式子 $\textit{vals}[\textit{idx}] + \textit{add}$ **对所有元素都保持一致**。

## 只有乘法

如果只有乘法，没有加法呢？

同理，用变量 $\textit{mul}$ 表示「每个数都要乘以 $\textit{mul}$」。执行 $\texttt{multAll}(2)$ 时，只把 $\textit{mul}$ 乘以 $2$。等到调用 $\texttt{getIndex}(\textit{idx})$ 时，才计算乘法：

$$
\textit{vals}[\textit{idx}] \cdot \textit{mul}
$$

即为 $\textit{vals}[\textit{idx}]$ 更新后的数值。

如何处理 $\texttt{append}(\textit{val})$ 呢？

为了让 $\textit{val}$ 兼容 $\textit{vals}[\textit{idx}] \cdot \textit{mul}$ 这个式子，我们可以先把 $\textit{val}$ **除以** $\textit{mul}$，再添加到 $\textit{vals}$ 的末尾，比如 $\textit{val} = 6$，$\textit{mul} = 2$，那么往 $\textit{vals}$ 的末尾添加 $6/2=3$，就可以让式子 $\textit{vals}[\textit{idx}] \cdot \textit{mul}$ **对所有元素都保持一致**。

⚠**注意**：在模运算中，除以 $\textit{mul}$ 等价于乘以 $\textit{mul}$ 关于 $M = 10^9+7$ 的**逆元**，即 $\textit{mul}^{M-2}$。原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 加法和乘法

把上述方法结合起来，用 $\textit{add}$ 记录操作 $\texttt{addAll}$，用 $\textit{mul}$ 记录操作 $\texttt{multAll}$。

- 初始值：$\textit{add} = 0$，$\textit{mul}=1$。
- 执行 $\texttt{addAll}(\textit{inc})$ 时，把 $\textit{add}$ 增加 $\textit{inc}$。
- 执行 $\texttt{multAll}(m)$ 时，由于 $(v \cdot \textit{mul} + \textit{add})\cdot m = v \cdot (\textit{mul}\cdot m) + \textit{add}\cdot m$，所以把 $\textit{mul}$ 乘以 $m$，把 $\textit{add}$ 乘以 $m$。

调用 $\texttt{getIndex}(\textit{idx})$ 时，计算

$$
\textit{vals}[\textit{idx}] \cdot \textit{mul} + \textit{add}
$$

即为 $\textit{vals}[\textit{idx}]$ 更新后的数值。

如何处理 $\texttt{append}(\textit{val})$ 呢？

为了让 $\textit{val}$ 兼容 $\textit{vals}[\textit{idx}] \cdot \textit{mul} + \textit{add}$ 这个式子，我们可以先计算 $v = \dfrac{\textit{val} - \textit{add}}{\textit{mul}}$，再把 $v$ 添加到 $\textit{vals}$ 的末尾，就可以让式子 $\textit{vals}[\textit{idx}] \cdot \textit{mul} + \textit{add}$ **对所有元素都保持一致**。

代码实现时，注意取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
MOD = 1_000_000_007

class Fancy:
    def __init__(self):
        self.vals = []
        self.add = 0
        self.mul = 1

    def append(self, val: int) -> None:
        self.vals.append((val - self.add) * pow(self.mul, -1, MOD) % MOD)

    def addAll(self, inc: int) -> None:
        self.add += inc

    def multAll(self, m: int) -> None:
        self.mul = self.mul * m % MOD
        self.add = self.add * m % MOD

    def getIndex(self, idx: int) -> int:
        if idx >= len(self.vals):
            return -1
        return (self.vals[idx] * self.mul + self.add) % MOD
```

```java [sol-Java]
class Fancy {
    private static final int MOD = 1_000_000_007;

    private final List<Integer> vals = new ArrayList<>();
    private long add = 0;
    private long mul = 1;

    public void append(int val) {
        // 注意这里有减法，计算结果可能是负数，+MOD 可以保证计算结果非负
        vals.add((int) ((val - add + MOD) * pow(mul, MOD - 2) % MOD));
    }

    public void addAll(int inc) {
        add = (add + inc) % MOD;
    }

    public void multAll(int m) {
        mul = mul * m % MOD;
        add = add * m % MOD;
    }

    public int getIndex(int idx) {
        if (idx >= vals.size()) {
            return -1;
        }
        return (int) ((vals.get(idx) * mul + add) % MOD);
    }

    private long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Fancy {
    static constexpr int MOD = 1'000'000'007;

    vector<int> vals;
    long long add = 0;
    long long mul = 1;

    long long pow(long long x, int n) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }

public:
    void append(int val) {
        // 注意这里有减法，计算结果可能是负数，+MOD 可以保证计算结果非负
        vals.push_back((val - add + MOD) * pow(mul, MOD - 2) % MOD);
    }

    void addAll(int inc) {
        add = (add + inc) % MOD;
    }

    void multAll(int m) {
        mul = mul * m % MOD;
        add = add * m % MOD;
    }

    int getIndex(int idx) {
        if (idx >= vals.size()) {
            return -1;
        }
        return (vals[idx] * mul + add) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

func pow(x, n int) int {
	res := 1
	for ; n > 0; n /= 2 {
		if n%2 > 0 {
			res = res * x % mod
		}
		x = x * x % mod
	}
	return res
}

type Fancy struct {
	vals []int
	add  int
	mul  int
}

func Constructor() Fancy {
	return Fancy{mul: 1}
}

func (f *Fancy) Append(val int) {
	// 注意这里有减法，计算结果可能是负数，+mod 可以保证计算结果非负
	f.vals = append(f.vals, (val-f.add+mod)*pow(f.mul, mod-2)%mod)
}

func (f *Fancy) AddAll(inc int) {
	f.add = (f.add + inc) % mod
}

func (f *Fancy) MultAll(m int) {
	f.mul = f.mul * m % mod
	f.add = f.add * m % mod
}

func (f *Fancy) GetIndex(idx int) int {
	if idx >= len(f.vals) {
		return -1
	}
	return (f.vals[idx]*f.mul + f.add) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\texttt{append}$ 为 $\mathcal{O}(\log M)$，其余为 $\mathcal{O}(1)$，其中 $M=10^9+7$。
- 空间复杂度：$\mathcal{O}(q)$，其中 $q$ 是 $\texttt{append}$ 的调用次数。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.3 等价转化`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.3 等价转化`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
