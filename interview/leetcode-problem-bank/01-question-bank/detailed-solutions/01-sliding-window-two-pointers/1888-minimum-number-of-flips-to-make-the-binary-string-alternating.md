# 1888. 使二进制字符串字符交替的最少反转次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-flips-to-make-the-binary-string-alternating/
- 题目 slug：`minimum-number-of-flips-to-make-the-binary-string-alternating`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：2006
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-flips-to-make-the-binary-string-alternating/solutions/815298/cong-qian-wang-hou-pi-pei-cong-hou-wang-uiq6a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[s + s 上的滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-number-of-flips-to-make-the-binary-string-alternating/solutions/815298/cong-qian-wang-hou-pi-pei-cong-hou-wang-uiq6a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-qian-wang-hou-pi-pei-cong-hou-wang-uiq6a`
- topic id：`815298`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

注意题目只要求类型 2 操作次数尽量少，并没有约束类型 1，操作类型 1 多少次都可以。

例如 $s = \texttt{111000}$，不断操作类型 1，可以得到如下字符串（$\_$ 只是为了排版好看）：

$$
\begin{aligned}
& \texttt{111000}\_\_\_\_\_     \\
& \_\texttt{110001}\_\_\_\_     \\
& \_\_\texttt{100011}\_\_\_     \\
& \_\_\_\texttt{000111}\_\_     \\
& \_\_\_\_\texttt{001110}\_     \\
& \_\_\_\_\_\texttt{011100}     \\
\end{aligned}
$$

注意到，这些字符串其实都是 $s+s=\texttt{11100011100}$（去掉最后一个字符）的长为 $n$ 的**子串**。

设 $t=\texttt{11100011100}$。题目要求字符串是交替的：

- 如果改成 $\texttt{0101}\cdots$，那么对于每个 $i$，都要满足 $\texttt{int}(t[i]) = i\bmod 2$。
- 如果改成 $\texttt{1010}\cdots$，那么对于每个 $i$，都要满足 $\texttt{int}(t[i]) \ne i\bmod 2$。

把满足 $\texttt{int}(t[i])= i\bmod 2$ 的 $t[i]$ 视作 $\texttt{.}$ 字符，把满足 $\texttt{int}(t[i])\ne i\bmod 2$ 的 $t[i]$ 视作 $\texttt{X}$ 字符，我们可以得到

$$
\texttt{X.XX.XX.XX.}
$$

- 当子串左端点下标是偶数时：
    - 子串中的 $\texttt{X}$ 的个数就是改成 $\texttt{0101}\cdots$ 的类型 2 操作次数。
    - 子串中的 $\texttt{.}$ 的个数就是改成 $\texttt{1010}\cdots$ 的类型 2 操作次数。
- 当子串左端点下标是奇数时：
    - 子串中的 $\texttt{.}$ 的个数就是改成 $\texttt{0101}\cdots$ 的类型 2 操作次数。
    - 子串中的 $\texttt{X}$ 的个数就是改成 $\texttt{1010}\cdots$ 的类型 2 操作次数。

我们可以统计子串中的 $\texttt{X}$ 的个数 $\textit{cnt}$，那么 $n-\textit{cnt}$ 就是子串中的 $\texttt{.}$ 的个数。

对于每个子串，既可以改成 $\texttt{0101}\cdots$，又可以改成 $\texttt{1010}\cdots$，取最小值，用

$$
\min(\textit{cnt},n-\textit{cnt})
$$

更新答案的最小值。

$\textit{cnt}$ 可以用滑动窗口计算，原理见 [定长滑窗套路](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)。

## 写法一

$\texttt{int}(t[i])\ne i\bmod 2$ 等价于 $t[i]\bmod 2\ne i\bmod 2$，后者用 $t[i]$ 的 ASCII 值参与运算。

$t[i]$ 可以写成 $s[i\bmod n]$，这样无需生成字符串 $t$。

**问**：为什么循环写的是 $i < 2n-1$ 而不是 $i < 2n$？

**答**：因为 $[0,n-1]$ 和 $[n,2n-1]$ 是同一个字符串，不需要重复计算。

```py [sol-Python3]
class Solution:
    def minFlips(self, s: str) -> int:
        ans = n = len(s)
        cnt = 0
        for i in range(n * 2 - 1):
            if ord(s[i % n]) % 2 != i % 2:
                cnt += 1
            left = i - n + 1
            if left < 0:
                continue
            ans = min(ans, cnt, n - cnt)
            if ord(s[left]) % 2 != left % 2:
                cnt -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minFlips(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int ans = n;
        int cnt = 0;
        for (int i = 0; i < n * 2 - 1; i++) {
            if (s[i % n] % 2 != i % 2) {
                cnt++;
            }
            int left = i - n + 1;
            if (left < 0) {
                continue;
            }
            ans = Math.min(ans, Math.min(cnt, n - cnt));
            if (s[left] % 2 != left % 2) {
                cnt--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minFlips(string s) {
        int n = s.size(), ans = n, cnt = 0;
        for (int i = 0; i < n * 2 - 1; i++) {
            if (s[i % n] % 2 != i % 2) {
                cnt++;
            }
            int left = i - n + 1;
            if (left < 0) {
                continue;
            }
            ans = min({ans, cnt, n - cnt});
            if (s[left] % 2 != left % 2) {
                cnt--;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minFlips(s string) int {
	n := len(s)
	ans := n
	cnt := 0
	for i := range n*2 - 1 {
		if int(s[i%n]%2) != i%2 {
			cnt++
		}
		left := i - n + 1
		if left < 0 {
			continue
		}
		ans = min(ans, cnt, n-cnt)
		if int(s[left]%2) != left%2 {
			cnt--
		}
	}
	return ans
}
```

## 写法二（位运算）

$t[i]\bmod 2\ne i\bmod 2$ 等价于，取 $t[i]$ 和 $i$ 的异或结果的最低位，相同则为 $0$，不同则为 $1$。

```py [sol-Python3]
class Solution:
    def minFlips(self, s: str) -> int:
        ans = n = len(s)
        cnt = 0
        for i in range(n * 2 - 1):
            cnt += (ord(s[i % n]) ^ i) & 1
            left = i - n + 1
            if left < 0:
                continue
            ans = min(ans, cnt, n - cnt)
            cnt -= (ord(s[left]) ^ left) & 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minFlips(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int ans = n;
        int cnt = 0;
        for (int i = 0; i < n * 2 - 1; i++) {
            cnt += (s[i % n] ^ i) & 1;
            int left = i - n + 1;
            if (left < 0) {
                continue;
            }
            ans = Math.min(ans, Math.min(cnt, n - cnt));
            cnt -= (s[left] ^ left) & 1;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minFlips(string s) {
        int n = s.size(), ans = n, cnt = 0;
        for (int i = 0; i < n * 2 - 1; i++) {
            cnt += (s[i % n] ^ i) & 1;
            int left = i - n + 1;
            if (left < 0) {
                continue;
            }
            ans = min({ans, cnt, n - cnt});
            cnt -= (s[left] ^ left) & 1;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minFlips(s string) int {
	n := len(s)
	ans := n
	cnt := 0
	for i := range n*2 - 1 {
		cnt += (int(s[i%n]) ^ i) & 1
		left := i - n + 1
		if left < 0 {
			continue
		}
		ans = min(ans, cnt, n-cnt)
		cnt -= (int(s[left]) ^ left) & 1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
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

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
