# 2696. 删除子串后的字符串最小长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-string-length-after-removing-substrings/
- 题目 slug：`minimum-string-length-after-removing-substrings`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.3 邻项消除
- 难度分：1282
- 外部题解来源：https://leetcode.cn/problems/minimum-string-length-after-removing-substrings/solutions/2278343/liang-chong-suan-fa-bao-li-ti-huan-zhan-vto0o/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种算法：暴力替换/栈（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-string-length-after-removing-substrings/solutions/2278343/liang-chong-suan-fa-bao-li-ti-huan-zhan-vto0o/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-suan-fa-bao-li-ti-huan-zhan-vto0o`
- topic id：`2278343`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 算法一：暴力

本题思路和 [20. 有效的括号](https://leetcode.cn/problems/valid-parentheses/) 是一样的。把 AB 看成一对括号 `()`，CD 看成另一对括号 `{}`。

暴力做法是不断把 AB 和 CD 去掉，直到 $s$ 中没有 AB 和 CD 为止。

请看[【周赛 346】](https://www.bilibili.com/video/BV1Qm4y1t7cx/)第一题，欢迎点赞关注~

```py [sol1-Python3]
class Solution:
    def minLength(self, s: str) -> int:
        while "AB" in s or "CD" in s:
            s = s.replace("AB", "").replace("CD", "")
        return len(s)
```

```java [sol1-Java]
class Solution {
    public int minLength(String s) {
        while (s.contains("AB") || s.contains("CD"))
            s = s.replace("AB", "").replace("CD", "");
        return s.length();
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minLength(string s) {
        regex ab("AB"), cd("CD");
        while (s.find("AB") != string::npos || s.find("CD") != string::npos) {
            s = regex_replace(s, ab, "");
            s = regex_replace(s, cd, "");
        }
        return s.length();
    }
};
```

```go [sol1-Go]
func minLength(s string) int {
	for strings.Contains(s, "AB") || strings.Contains(s, "CD") {
		s = strings.ReplaceAll(s, "AB", "")
		s = strings.ReplaceAll(s, "CD", "")
	}
	return len(s)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s$ 的长度。对于 AAA...BBB 这样的字符串，需要循环 $\mathcal{O}(n)$ 次，每次需要 $\mathcal{O}(n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。替换过程中生成的字符串需要 $\mathcal{O}(n)$ 的空间。

## 算法二：栈

用栈记录遍历过的，没有删除的字母。

如果当前字母是 B，且栈顶为 A，那么这两个字母都可以删除。同理，如果当前字母是 D，且栈顶为 C，那么这两个字母都可以删除。

否则，把当前字母入栈。

```py [sol-Python3]
class Solution:
    def minLength(self, s: str) -> int:
        st = []
        for c in s:
            if st and (c == 'B' and st[-1] == 'A' or c == 'D' and st[-1] == 'C'):
                st.pop()
            else:
                st.append(c)
        return len(st)
```

```java [sol-Java]
class Solution {
    public int minLength(String s) {
        var st = new ArrayDeque<Character>();
        for (var c : s.toCharArray()) {
            if (!st.isEmpty() && (c == 'B' && st.peek() == 'A' || c == 'D' && st.peek() == 'C'))
                st.pop();
            else
                st.push(c);
        }
        return st.size();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minLength(string s) {
        stack<char> st;
        for (char c: s) {
            if (!st.empty() && (c == 'B' && st.top() == 'A' || c == 'D' && st.top() == 'C'))
                st.pop();
            else
                st.push(c);
        }
        return st.size();
    }
};
```

```go [sol-Go]
func minLength(s string) int {
	st := []rune{}
	for _, c := range s {
		if len(st) > 0 && (c == 'B' && st[len(st)-1] == 'A' || c == 'D' && st[len(st)-1] == 'C') {
			st = st[:len(st)-1]
		} else {
			st = append(st, c)
		}
	}
	return len(st)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，请看下面数据结构题单中的「**§3.3 邻项消除**」。

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

本题来自 `三、栈 / §3.3 邻项消除`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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
    int maxVowels(string s, int k) {
        auto isVowel = [](char c) {
            return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        };
        int cur = 0, ans = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            cur += isVowel(s[i]);
            if (i >= k) cur -= isVowel(s[i - k]);
            if (i + 1 >= k) ans = max(ans, cur);
        }
        return ans;
    }
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

回到来源分类 `三、栈 / §3.3 邻项消除`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
