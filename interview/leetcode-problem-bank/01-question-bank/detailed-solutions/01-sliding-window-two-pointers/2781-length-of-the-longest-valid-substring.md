# 2781. 最长合法子字符串的长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/length-of-the-longest-valid-substring/
- 题目 slug：`length-of-the-longest-valid-substring`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）
- 难度分：2204
- 外部题解来源：https://leetcode.cn/problems/length-of-the-longest-valid-substring/solutions/2345796/ha-xi-biao-shuang-zhi-zhen-pythonjavacgo-bcez/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[哈希表+滑动窗口（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/length-of-the-longest-valid-substring/solutions/2345796/ha-xi-biao-shuang-zhi-zhen-pythonjavacgo-bcez/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ha-xi-biao-shuang-zhi-zhen-pythonjavacgo-bcez`
- topic id：`2345796`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1DM4y1x7bR/) 第四题。

## 提示 1

$\textit{forbidden}[i]$ 的长度至多为 $10$。

## 提示 2

[滑动窗口](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

## 提示 3

初始化子串左端点 $\textit{left}=0$，枚举子串右端点 $\textit{right}$。

对于示例 2，只要 $\textit{right}\ge 1$，那么合法子串是不能包含 $\texttt{le}$ 的，所以左端点 $\textit{left}$ 必须向右移，不可能再回到 $0$（否则就包含 $\texttt{le}$ 了）。因为左端点只会向右移动，不会向左移动，这样的**单调性**保证了算法的效率。

当 $\textit{right}$ 右移到一个新的字母时，**枚举**以该字母为右端点的 $\textit{forbidden}[i]$ 的最短长度。如果发现子串 $\textit{word}[i]$ 到 $\textit{word}[\textit{right}]$ 在 $\textit{forbidden}$ 中（用哈希表实现），那么更新 $\textit{left}=i+1$ 并结束枚举，从而避免合法子串包含 $\textit{forbidden}$ 中的字符串。枚举结束后，更新答案为合法子串长度 $\textit{right}-\textit{left}+1$ 的最大值。

```py [sol-Python3]
class Solution:
    def longestValidSubstring(self, word: str, forbidden: List[str]) -> int:
        fb = set(forbidden)
        ans = left = 0
        for right in range(len(word)):
            for i in range(right, max(right - 10, left - 1), -1):
                if word[i: right + 1] in fb:
                    left = i + 1  # 当子串右端点 >= right 时，合法子串一定不能包含 word[i]
                    break
            ans = max(ans, right - left + 1)
        return ans
```

```java [sol-Java]
class Solution {
    public int longestValidSubstring(String word, List<String> forbidden) {
        var fb = new HashSet<String>();
        fb.addAll(forbidden);
        int ans = 0, left = 0, n = word.length();
        for (int right = 0; right < n; right++) {
            for (int i = right; i >= left && i > right - 10; i--) {
                if (fb.contains(word.substring(i, right + 1))) {
                    left = i + 1; // 当子串右端点 >= right 时，合法子串一定不能包含 word[i]
                    break;
                }
            }
            ans = Math.max(ans, right - left + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestValidSubstring(string word, vector<string> &forbidden) {
        unordered_set<string> fb{forbidden.begin(), forbidden.end()};
        int ans = 0, left = 0, n = word.length();
        for (int right = 0; right < n; right++) {
            for (int i = right; i >= left && i > right - 10; i--) {
                if (fb.count(word.substr(i, right - i + 1))) {
                    left = i + 1; // 当子串右端点 >= right 时，合法子串一定不能包含 word[i]
                    break;
                }
            }
            ans = max(ans, right - left + 1);
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestValidSubstring(word string, forbidden []string) (ans int) {
	has := make(map[string]bool, len(forbidden))
	for _, s := range forbidden {
		has[s] = true
	}

	left := 0
	for right := range word {
		for i := right; i >= left && i > right-10; i-- {
			if has[word[i:right+1]] {
				left = i + 1 // 当子串右端点 >= right 时，合法子串一定不能包含 word[i]
				break
			}
		}
		ans = max(ans, right-left+1)
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```

```js [sol-JavaScript]
var longestValidSubstring = function (word, forbidden) {
    let fb = new Set();
    for (const f of forbidden) fb.add(f);
    const n = word.length;
    let ans = 0, left = 0;
    for (let right = 0; right < n; right++) {
        for (let i = right; i >= left && i > right - 10; i--) {
            if (fb.has(word.substring(i, right + 1))) {
                left = i + 1; // 当子串右端点 >= right 时，合法子串一定不能包含 word[i]
                break;
            }
        }
        ans = Math.max(ans, right - left + 1);
    }
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L+nM^2)$，其中 $L$ 为所有 $\textit{forbidden}[i]$ 的长度之和，$n$ 为 $\textit{word}$ 的长度，$M=10$ 表示 $\textit{forbidden}[i]$ 的最长长度。请注意，在哈希表中查询一个长为 $M$ 的字符串的时间是 $\mathcal{O}(M)$，每次移动右指针会执行至多 $M$ 次这样的查询。
- 空间复杂度：$\mathcal{O}(L)$。

更多滑窗题目，请看[【题单】滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)

## 本地原创解析

### 1. 题意重述

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
