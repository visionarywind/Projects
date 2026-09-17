# 3955. 成本限制的有效二进制字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-binary-strings-with-cost-limit/
- 题目 slug：`valid-binary-strings-with-cost-limit`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.2 子集型回溯
- 难度分：1429
- 外部题解来源：https://leetcode.cn/problems/valid-binary-strings-with-cost-limit/solutions/3980522/yu-chu-li-mei-ge-shu-de-cheng-ben-python-u4et/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理每个数的成本（Python/Java/C++/Go）](https://leetcode.cn/problems/valid-binary-strings-with-cost-limit/solutions/3980522/yu-chu-li-mei-ge-shu-de-cheng-ben-python-u4et/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-mei-ge-shu-de-cheng-ben-python-u4et`
- topic id：`3980522`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

对于 `x` 和 `x >> 1` 这两个二进制数，同一个比特位上的数字，对应着 $x$ 中一对相邻比特位上的数字。所以计算 `x & (x >> 1)` 等价于计算 $x$ 的所有相邻比特位的 `&`。所有相邻的 $11$ 变成 $1$，其余变成 $0$。

例如二进制数 $x = 10011001110$，`x & (x >> 1)` 为 $1000110$。

$x$ 不含 $11$，等价于 `n & (n >> 1)` 等于 $0$。

计算 $x$ 的成本，我们可以去掉 $x$ 中的一个比特位（最低位还是最高位都可以），计算剩余二进制数的成本，这是一个规模更小的子问题，所以可以 DP。

[本题视频讲解](https://www.bilibili.com/video/BV1yfEx6YEBx/)，欢迎点赞关注~

```py [sol-Python3]
cost = [0] * (1 << 12)
for x in range(1, len(cost)):
    if x & (x >> 1):  # 有两个连续的 1
        cost[x] = inf  # 不合法
    else:
        # 去掉 x 中的一个比特位（最低位还是最高位都可以），计算 DP
        i = x.bit_length() - 1
        cost[x] = cost[x ^ (1 << i)] + i

class Solution:
    def generateValidStrings(self, n: int, k: int) -> List[str]:
        ans = []
        s = [''] * n
        for x in range(1 << n):
            if cost[x] > k:
                continue
            for j in range(n):  # 注意左边是低位，右边是高位
                s[j] = str(x & 1)
                x >>= 1
            ans.append(''.join(s))
        return ans
```

```py [sol-Python3 库函数]
cost = [0] * (1 << 12)
for x in range(1, len(cost)):
    if x & (x >> 1):  # 有两个连续的 1
        cost[x] = inf  # 不合法
    else:
        # 去掉 x 中的一个比特位（最低位还是最高位都可以），计算 DP
        i = x.bit_length() - 1
        cost[x] = cost[x ^ (1 << i)] + i

class Solution:
    def generateValidStrings(self, n: int, k: int) -> List[str]:
        ans = []
        for x in range(1 << n):
            if cost[x] > k:
                continue
            s = bin(x)[2:].zfill(n)[::-1]  # 或者 format(x, f"0{n}b")[::-1]
            ans.append(s)
        return ans
```

```java [sol-Java]
class Solution {
    private static final int[] cost = new int[1 << 12];
    private static boolean initialized = false;

    // 这样写比 static block 快
    public Solution() {
        if (initialized) {
            return;
        }
        initialized = true;

        for (int x = 1; x < cost.length; x++) {
            if ((x & (x >> 1)) > 0) { // 有两个连续的 1
                cost[x] = Integer.MAX_VALUE; // 不满足要求
            } else {
                // 去掉 x 中的一个比特位（最低位还是最高位都可以），计算 DP
                cost[x] = cost[x & (x - 1)] + Integer.numberOfTrailingZeros(x);
            }
        }
    }

    public List<String> generateValidStrings(int n, int k) {
        List<String> ans = new ArrayList<>();
        char[] s = new char[n];
        for (int x = 0; x < (1 << n); x++) {
            if (cost[x] > k) {
                continue;
            }
            int y = x;
            for (int j = 0; j < n; j++) { // 注意左边是低位，右边是高位
                s[j] = (char) ('0' + (y & 1));
                y >>= 1;
            }
            ans.add(new String(s));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
constexpr int MX = 1 << 12;
int cost[MX];

int init = [] {
    for (int x = 1; x < MX; x++) {
        if (x & (x >> 1)) { // 有两个连续的 1
            cost[x] = INT_MAX; // 不满足要求
        } else {
            // 去掉 x 中的一个比特位（最低位还是最高位都可以），计算 DP
            cost[x] = cost[x & (x - 1)] + countr_zero(1u * x);
        }
    }
    return 0;
}();

class Solution {
public:
    vector<string> generateValidStrings(int n, int k) {
        vector<string> ans;
        string s(n, 0);
        for (int x = 0; x < (1 << n); x++) {
            if (cost[x] > k) {
                continue;
            }
            int y = x;
            for (int j = 0; j < n; j++) { // 注意左边是低位，右边是高位
                s[j] = '0' + (y & 1);
                y >>= 1;
            }
            ans.push_back(s);
        }
        return ans;
    }
};
```

```go [sol-Go]
var cost [1 << 12]int

func init() {
	for x := 1; x < len(cost); x++ {
		if x&(x>>1) > 0 { // 有两个连续的 1
			cost[x] = math.MaxInt // 不合法
		} else {
			// 去掉 x 中的一个比特位（最低位还是最高位都可以），计算 DP
			cost[x] = cost[x&(x-1)] + bits.TrailingZeros(uint(x))
		}
	}
}

func generateValidStrings(n, k int) (ans []string) {
	s := make([]byte, n)
	for x, c := range cost[:1<<n] {
		if c > k {
			continue
		}
		for j := range s { // 注意左边是低位，右边是高位
			s[j] = '0' + byte(x&1)
			x >>= 1
		}
		ans = append(ans, string(s))
	}
	return
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n2^n)$。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。返回值不计入。

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

本题来自 `四、回溯 / §4.2 子集型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.2 子集型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
