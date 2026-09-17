# 3980. 变换二进制字符串的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-transform-binary-string/
- 题目 slug：`minimum-operations-to-transform-binary-string`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1846
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-transform-binary-string/solutions/3991773/cong-zuo-dao-you-tan-xin-pythonjavacgo-b-yx03/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从左到右贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-transform-binary-string/solutions/3991773/cong-zuo-dao-you-tan-xin-pythonjavacgo-b-yx03/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-zuo-dao-you-tan-xin-pythonjavacgo-b-yx03`
- topic id：`3991773`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

为方便描述，把 $s_1$ 简记为 $s$，把 $s_2$ 简记为 $t$。

从左到右思考：

- 如果 $s[i]=t[i]$，那么不改，继续向右遍历。
- 如果 $s[i]=\texttt{0}$，$t[i]=\texttt{1}$，那么用操作一即可让 $s[i]=t[i] = \texttt{1}$，操作 $1$ 次。
- 如果 $s[i]=\texttt{1}$，$t[i]=\texttt{0}$。
    - 如果 $i<n-1$ 且 $s[i+1]=\texttt{1}$，那么用操作二即可让 $s[i]=t[i] = \texttt{0}$（$s[i+1]$ 也变成了 $\texttt{0}$），操作 $1$ 次。至于 $s[i+1]$ 怎么操作，在下一轮循环中讨论。为什么不和 $s[i-1]$ 一起操作？理由见后文。
    - 如果 $i<n-1$ 且 $s[i+1]=\texttt{0}$，那么先用操作一把 $s[i+1]$ 变成 $\texttt{1}$，转化成上面的情况。一共操作 $2$ 次。
    - 如果 $i=n-1$，那么只能改 $s[i-1]$。
        - 如果 $s[i-1] = \texttt{0}$，那么先用操作一把 $s[i-1]$ 变成 $\texttt{1}$，再用操作二把 $s[i-1]$ 恢复成 $\texttt{0}$，也是一共操作 $2$ 次。
        - 如果 $s[i-1] = \texttt{1}$，那么先用操作二（$s[i-1]$ 变成 $\texttt{0}$），再用操作一把 $s[i-1]$ 恢复成 $\texttt{1}$，也是一共操作 $2$ 次。

上述分类讨论的最后一段表明，**操作左边**（$s[i-1]$）**不如操作右边**（$s[i+1]$）：

- 操作左边，必须操作 $2$ 次，而且 $s[i+1]$ 还没改。
- 操作右边，如果顺带把 $s[i+1]$ 改成等于 $t[i+1]$，一共只需操作 $1$ 次。如果操作后 $s[i+1]\ne t[i+1]$，这说明此时 $s[i+1]=\texttt{0}$，$t[i+1]=\texttt{1}$，那也只需再用操作一 $1$ 次。
- 所以操作右边不比操作左边差，无脑操作右边即可。

特判 $n=1$ 的情况，如果 $s=\texttt{1}$ 且 $t=\texttt{0}$（示例 3），那么无法操作，返回 $-1$。

[本题视频讲解](https://www.bilibili.com/video/BV1qXTC63EQa/?t=4m39s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minOperations(self, s1: str, t: str) -> int:
        n = len(s1)
        if n == 1 and s1 == "1" and t == "0":
            return -1

        # 也可以用一个布尔变量表示 s1[i] 是否操作过，从而做到 O(1) 空间，见 Python3 写法二
        s = list(s1)
        ans = 0
        for i in range(n):
            if s[i] == t[i]:
                continue
            if s[i] == '0':
                ans += 1
            elif i < n - 1 and s[i + 1] == '1':
                ans += 1
                s[i + 1] = '0'
            else:
                ans += 2
        return ans
```

```py [sol-Python3 写法二]
class Solution:
    def minOperations(self, s: str, t: str) -> int:
        n = len(s)
        if n == 1 and s == "1" and t == "0":
            return -1

        changed = False
        ans = 0
        for i in range(n):
            ch = '0' if changed else s[i]
            changed = False
            if ch == t[i]:
                continue
            if ch == '0':
                ans += 1
            elif i < n - 1 and s[i + 1] == '1':
                ans += 1
                changed = True
            else:
                ans += 2
        return ans
```

```java [sol-Java]
class Solution {
    public int minOperations(String s1, String t) {
        int n = s1.length();
        if (n == 1 && s1.charAt(0) == '1' && t.charAt(0) == '0') {
            return -1;
        }

        // 也可以用一个布尔变量表示 s1[i] 是否操作过，从而做到 O(1) 空间，见 Python3 写法二
        char[] s = s1.toCharArray();
        int ans = 0;
        for (int i = 0; i < n; i++) {
            if (s[i] == t.charAt(i)) {
                continue;
            }
            if (s[i] == '0') {
                ans++;
            } else if (i < n - 1 && s[i + 1] == '1') {
                ans++;
                s[i + 1] = '0';
            } else {
                ans += 2;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(string s, string t) {
        int n = s.size();
        if (n == 1 && s == "1" && t == "0") {
            return -1;
        }

        int ans = 0;
        for (int i = 0; i < n; i++) {
            if (s[i] == t[i]) {
                continue;
            }
            if (s[i] == '0') {
                ans++;
            } else if (i < n - 1 && s[i + 1] == '1') {
                ans++;
                s[i + 1] = '0';
            } else {
                ans += 2;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minOperations(s1, t string) (ans int) {
	n := len(s1)
	if n == 1 && s1 == "1" && t == "0" {
		return -1
	}

	// 也可以用一个布尔变量表示 s1[i] 是否操作过，从而做到 O(1) 空间，见 Python3 写法二
	s := []byte(s1)
	for i := range n {
		if s[i] == t[i] {
			continue
		}
		if s[i] == '0' {
			ans++
		} else if i < n-1 && s[i+1] == '1' {
			ans++
			s[i+1] = '0'
		} else {
			ans += 2
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。可以用一个布尔变量表示是否修改了当前字符，从而做到 $\mathcal{O}(1)$ 空间，见 Python3 写法二。

## 专题训练

见下面贪心题单的「**§1.4 从最左/最右开始贪心**」。

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

本题来自 `一、贪心策略 / §1.4 从最左/最右开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.4 从最左/最右开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
