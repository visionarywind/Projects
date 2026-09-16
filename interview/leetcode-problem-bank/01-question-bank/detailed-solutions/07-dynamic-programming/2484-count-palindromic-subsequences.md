# 2484. 统计回文子序列数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-palindromic-subsequences/
- 题目 slug：`count-palindromic-subsequences`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：2223
- 外部题解来源：https://leetcode.cn/problems/count-palindromic-subsequences/solutions/1993115/qian-hou-zhui-fen-jie-o100-chang-shu-kon-51cv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解 + O(100) 常数空间（Python/Java/C++/Go）](https://leetcode.cn/problems/count-palindromic-subsequences/solutions/1993115/qian-hou-zhui-fen-jie-o100-chang-shu-kon-51cv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-o100-chang-shu-kon-51cv`
- topic id：`1993115`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1uG4y157Rc) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

做法同 [1930. 长度为 3 的不同回文子序列](https://leetcode.cn/problems/unique-length-3-palindromic-subsequences/)。

首先倒着遍历 $s$，统计每个字符的出现次数 $\textit{suf}[a]$ 和两个字符的组合个数 $\textit{suf}_2[a][b]$。

例如遍历到 $a=s[i]$ 时，就更新 $\textit{suf}_2[a][0..9] \text{+=} \textit{suf}[0..9]$，然后 $\textit{suf}[a]$ 加一。

然后正着遍历 $s$，统计每个字符的出现次数 $\textit{pre}[a]$ 和两个字符的组合个数 $\textit{pre}_2[a][b]$，更新方法同上。

枚举每个 $s[i]$，当作回文子序列的回文中心，此时的子序列个数就是 $s[0..i-1]$ 中的 $\textit{pre}_2[a][b]$ 与 $s[i+1..n-1]$ 中的 $\textit{suf}_2[a][b]$ 的组合，枚举所有的 $a$ 和 $b$，个数相乘再相加，即为以 $s[i]$ 为回文子序列的回文中心时的答案。

代码实现时，可以一遍遍历一遍计算 $\textit{pre}_2[a][b]$，同时撤销 $\textit{suf}_2[a][b]$ 的统计结果。

注意在最坏情况下（所有字符都相同），答案为

$$
C(n,5) < \dfrac{n^5}{120} < 10^{18}
$$

所以可以最后返回的时候再取模。

```py [sol1-Python3]
class Solution:
    def countPalindromes(self, s: str) -> int:
        suf = [0] * 10
        suf2 = [0] * 100
        for d in map(int, reversed(s)):
            for j, c in enumerate(suf):
                suf2[d * 10 + j] += c
            suf[d] += 1

        ans = 0
        pre = [0] * 10
        pre2 = [0] * 100
        for d in map(int, s):
            suf[d] -= 1
            for j, c in enumerate(suf):
                suf2[d * 10 + j] -= c  # 撤销
            ans += sum(c1 * c2 for c1, c2 in zip(pre2, suf2))  # 枚举所有字符组合
            for j, c in enumerate(pre):
                pre2[d * 10 + j] += c
            pre[d] += 1
        return ans % (10 ** 9 + 7)
```

```java [sol1-Java]
class Solution {
    private static final long MOD = (long) 1e9 + 7;

    public int countPalindromes(String S) {
        var s = S.toCharArray();
        int[] pre = new int[10], suf = new int[10];
        int[][] pre2 = new int[10][10], suf2 = new int[10][10];
        for (var i = s.length - 1; i >= 0; --i) {
            var d = s[i] - '0';
            for (var j = 0; j < 10; ++j)
                suf2[d][j] += suf[j];
            ++suf[d];
        }

        var ans = 0L;
        for (var d : s) {
            d -= '0';
            --suf[d];
            for (var j = 0; j < 10; ++j)
                suf2[d][j] -= suf[j]; // 撤销
            for (var j = 0; j < 10; ++j)
                for (var k = 0; k < 10; ++k)
                    ans += (long) pre2[j][k] * suf2[j][k]; // 枚举所有字符组合
            for (var j = 0; j < 10; ++j)
                pre2[d][j] += pre[j];
            ++pre[d];
        }
        return (int) (ans % MOD);
    }
}
```

```cpp [sol1-C++]
class Solution {
    const long MOD = 1e9 + 7;
public:
    int countPalindromes(string s) {
        int suf[10]{}, suf2[10][10]{}, pre[10]{}, pre2[10][10]{};
        for (int i = s.length() - 1; i >= 0; --i) {
            char d = s[i] - '0';
            for (int j = 0; j < 10; ++j)
                suf2[d][j] += suf[j];
            ++suf[d];
        }

        long ans = 0L;
        for (char d : s) {
            d -= '0';
            --suf[d];
            for (int j = 0; j < 10; ++j)
                suf2[d][j] -= suf[j]; // 撤销
            for (int j = 0; j < 10; ++j)
                for (int k = 0; k < 10; ++k)
                    ans += (long) pre2[j][k] * suf2[j][k]; // 枚举所有字符组合
            for (int j = 0; j < 10; ++j)
                pre2[d][j] += pre[j];
            ++pre[d];
        }
        return ans % MOD;
    }
};
```

```go [sol1-Go]
func countPalindromes(s string) (ans int) {
	const mod int = 1e9 + 7
	n := len(s)
	suf := [10]int{}
	suf2 := [10][10]int{}
	for i := n - 1; i >= 0; i-- {
		d := s[i] - '0'
		for j, c := range suf {
			suf2[d][j] += c
		}
		suf[d]++
	}

	pre := [10]int{}
	pre2 := [10][10]int{}
	for _, d := range s[:n-1] {
		d -= '0'
		suf[d]--
		for j, c := range suf {
			suf2[d][j] -= c // 撤销
		}
		for j, sf := range suf2 {
			for k, c := range sf {
				ans += pre2[j][k] * c // 枚举所有字符组合
			}
		}
		for j, c := range pre {
			pre2[d][j] += c
		}
		pre[d]++
	}
	return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$O(n|\Sigma|^2)$，其中 $n$ 为 $s$ 的长度，$|\Sigma|$ 为字符集合的大小，本题中字符均为数字，所以 $|\Sigma|=10$。
- 空间复杂度：$O(|\Sigma|^2)$。

#### 相似题目

- [1930. 长度为 3 的不同回文子序列](https://leetcode.cn/problems/unique-length-3-palindromic-subsequences/)

## 本地原创解析

### 1. 题意重述

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
