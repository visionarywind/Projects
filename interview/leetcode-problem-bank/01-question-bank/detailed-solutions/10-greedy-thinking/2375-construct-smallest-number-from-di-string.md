# 2375. 根据模式串构造最小数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-smallest-number-from-di-string/
- 题目 slug：`construct-smallest-number-from-di-string`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1642
- 外部题解来源：https://leetcode.cn/problems/construct-smallest-number-from-di-string/solutions/1746837/by-endlesscheng-8ee3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 O(n) 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/construct-smallest-number-from-di-string/solutions/1746837/by-endlesscheng-8ee3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-8ee3`
- topic id：`1746837`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1rS4y1s721) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---

贪心策略：

把 $\textit{pattern}$ 按照 $\texttt{III}\cdots \texttt{IDDD}\cdots \texttt{D}$ 分组，每组前一段是 $\texttt{I}$，后一段是 $\texttt{D}$。

遍历每一段，设当前段的长度为 $x$，我们应该把剩余最小的 $x$ 个数字填到该段上（如果是第一段则填最小的 $x+1$ 个数字），从而保证字典序最小。

举例说明，假如第一段为 $\texttt{IIIDDD}$，构造方案如下：

- 前 $2$ 个 $\texttt{I}$ 视作长度为 $3$ 的上升段；
- 剩余的 $\texttt{I}$ 和 $\texttt{D}$ 视作长度为 $4$ 的下降段；
- 最小的 $3$ 个数给上升段，然后剩余最小的 $4$ 个数给下降段；
- 构造结果为 $\texttt{1237654}$。

按照该方案分组模拟即可。

[视频讲解](https://www.bilibili.com/video/BV1rS4y1s721) 中介绍了另外一种更优雅的做法。

```py [sol1-Python3]
class Solution:
    def smallestNumber(self, pattern: str) -> str:
        i, cur, n = 0, 1, len(pattern)
        ans = [''] * (n + 1)
        while i < n:
            if i and pattern[i] == 'I':
                i += 1
            while i < n and pattern[i] == 'I':
                ans[i] = digits[cur]
                cur += 1
                i += 1
            i0 = i
            while i < n and pattern[i] == 'D':
                i += 1
            for j in range(i, i0 - 1, -1):
                ans[j] = digits[cur]
                cur += 1
        return ''.join(ans)
```

```java [sol1-Java]
class Solution {
    public String smallestNumber(String pattern) {
        int i = 0, n = pattern.length();
        var cur = '1';
        var ans = new char[n + 1];
        while (i < n) {
            if (i > 0 && pattern.charAt(i) == 'I') ++i;
            for (; i < n && pattern.charAt(i) == 'I'; ++i) ans[i] = cur++;
            var i0 = i;
            while (i < n && pattern.charAt(i) == 'D') ++i;
            for (var j = i; j >= i0; --j) ans[j] = cur++;
        }
        return new String(ans);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    string smallestNumber(string pattern) {
        int i = 0, n = pattern.length();
        char cur = '1';
        string ans(n + 1, 0);
        while (i < n) {
            if (i && pattern[i] == 'I') ++i;
            for (; i < n && pattern[i] == 'I'; ++i) ans[i] = cur++;
            int i0 = i;
            while (i < n && pattern[i] == 'D') ++i;
            for (int j = i; j >= i0; --j) ans[j] = cur++;
        }
        return ans;
    }
};
```

```go [sol1-Go]
func smallestNumber(pattern string) string {
	n := len(pattern)
	ans := make([]byte, n+1)
	for i, cur := 0, byte('1'); i < n; {
		if i > 0 && pattern[i] == 'I' {
			i++
		}
		for ; i < n && pattern[i] == 'I'; i++ {
			ans[i] = cur
			cur++
		}
		i0 := i
		for ; i < n && pattern[i] == 'D'; i++ {
		}
		for j := i; j >= i0; j-- {
			ans[j] = cur
			cur++
		}
	}
	return string(ans)
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `三、字符串贪心 / §3.1 字典序最小/最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、字符串贪心 / §3.1 字典序最小/最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
