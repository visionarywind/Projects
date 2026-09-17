# 2223. 构造字符串的总得分和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-scores-of-built-strings/
- 题目 slug：`sum-of-scores-of-built-strings`
- 来源专题：字符串
- 来源分类路径：二、Z 函数（后缀的前缀）
- 难度分：2220
- 外部题解来源：https://leetcode.cn/problems/sum-of-scores-of-built-strings/solutions/1390110/on-kuo-zhan-kmp-by-endlesscheng-9nhs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 扩展 KMP](https://leetcode.cn/problems/sum-of-scores-of-built-strings/solutions/1390110/on-kuo-zhan-kmp-by-endlesscheng-9nhs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-kuo-zhan-kmp-by-endlesscheng-9nhs`
- topic id：`1390110`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

题目求的就是扩展 KMP（Z 数组）的所有元素之和。

指路 -> https://oi-wiki.org/string/z-func/

```Python [sol1-Python3]
class Solution:
    def sumScores(self, s: str) -> int:
        n = len(s)
        z = [0] * n
        ans, l, r = n, 0, 0
        for i in range(1, n):
            z[i] = max(min(z[i - l], r - i + 1), 0)  # 注：不用 min max，拆开用 < > 比较会更快（仅限于 Python）
            while i + z[i] < n and s[z[i]] == s[i + z[i]]:
                l, r = i, i + z[i]
                z[i] += 1
            ans += z[i]
        return ans
```

```java [sol1-Java]
class Solution {
    public long sumScores(String s) {
        var n = s.length();
        var z = new int[n];
        long ans = n;
        for (int i = 1, l = 0, r = 0; i < n; i++) {
            z[i] = Math.max(Math.min(z[i - l], r - i + 1), 0);
            while (i + z[i] < n && s.charAt(z[i]) == s.charAt(i + z[i])) {
                l = i;
                r = i + z[i];
                z[i]++;
            }
            ans += z[i];
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long sumScores(string s) {
        int n = s.length();
        long ans = n;
        vector<int> z(n);
        for (int i = 1, l = 0, r = 0; i < n; ++i) {
            z[i] = max(min(z[i - l], r - i + 1), 0);
            while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
                l = i;
                r = i + z[i];
                ++z[i];
            }
            ans += z[i];
        }
        return ans;
    }
};
```

```go [sol1-Go]
func sumScores(s string) int64 {
	n := len(s)
	z := make([]int, n)
	ans := n
	for i, l, r := 1, 0, 0; i < n; i++ {
		z[i] = max(min(z[i-l], r-i+1), 0)
		for i+z[i] < n && s[z[i]] == s[i+z[i]] {
			l, r = i, i+z[i]
			z[i]++
		}
		ans += z[i]
	}
	return int64(ans)
}

func min(a, b int) int { if a > b { return b }; return a }
func max(a, b int) int { if a < b { return b }; return a }
```

## 本地原创解析

### 1. 题意重述

本题来自 `二、Z 函数（后缀的前缀）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、Z 函数（后缀的前缀）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
