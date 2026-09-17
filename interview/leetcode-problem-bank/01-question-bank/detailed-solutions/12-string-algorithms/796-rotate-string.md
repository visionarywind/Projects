# 796. 旋转字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/rotate-string/
- 题目 slug：`rotate-string`
- 来源专题：字符串
- 来源分类路径：一、KMP（前缀的后缀）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/rotate-string/solutions/3962376/liang-chong-fang-fa-bao-li-kmppythonjava-6n68/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：暴力 / KMP（Python/Java/C++/Go）](https://leetcode.cn/problems/rotate-string/solutions/3962376/liang-chong-fang-fa-bao-li-kmppythonjava-6n68/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bao-li-kmppythonjava-6n68`
- topic id：`3962376`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 12:05:27 +0800

## 方法一：暴力

例如 $s=\texttt{abcde}$，旋转一次变成 $\texttt{bcdea}$，再旋转一次变成 $\texttt{cdeab}$。旋转后的字符串是 $s$ 的后缀加上 $s$ 的前缀，例如 $\texttt{cdeab} = \texttt{cde} + \texttt{ab}$。于是构造 $s+s = \texttt{abcdeabcde}$，这个字符串的任意长为 $n$ 的子串，都是 $s$ 的后缀加上 $s$ 的前缀。所以无论 $s$ 如何旋转，旋转后的字符串一定是 $s+s$ 的子串。

于是问题等价于：

- 判断 $s+s$ 是否包含 $\textit{goal}$。

注意题目没有保证 $s$ 和 $\textit{goal}$ 长度相等，如果不等，直接返回 $\texttt{false}$。

```py [sol-Python3]
class Solution:
    def rotateString(self, s: str, goal: str) -> bool:
        return len(s) == len(goal) and goal in s + s
```

```java [sol-Java]
class Solution {
    public boolean rotateString(String s, String goal) {
        return s.length() == goal.length() && (s + s).contains(goal);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool rotateString(string s, string goal) {
        return s.size() == goal.size() && (s + s).contains(goal);
    }
};
```

```go [sol-Go]
func rotateString(s, goal string) bool {
	return len(s) == len(goal) && strings.Contains(s+s, goal)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：字符串匹配

用 KMP、Z 函数、字符串哈希等算法，都可以 $\mathcal{O}(n)$ 判断 $s+s$ 是否包含 $\textit{goal}$。

下面用的 KMP 算法，原理见 [KMP 算法讲解](https://www.zhihu.com/question/21923021/answer/37475572)。

```py [sol-Python3]
# 下面是 KMP 模板。对于本题来说，找到一个就可以返回了。为方便大家使用，我保留了完整的模板。
# 在文本串 text 中查找模式串 pattern，返回所有成功匹配的位置（pattern[0] 在 text 中的下标）
def kmp_search(text: str, pattern: str) -> List[int]:
    m = len(pattern)
    pi = [0] * m
    cnt = 0
    for i in range(1, m):
        b = pattern[i]
        while cnt and pattern[cnt] != b:
            cnt = pi[cnt - 1]
        if pattern[cnt] == b:
            cnt += 1
        pi[i] = cnt

    pos = []
    cnt = 0
    for i, b in enumerate(text):
        while cnt and pattern[cnt] != b:
            cnt = pi[cnt - 1]
        if pattern[cnt] == b:
            cnt += 1
        if cnt == len(pattern):
            pos.append(i - m + 1)
            cnt = pi[cnt - 1]
    return pos

class Solution:
    def rotateString(self, s: str, goal: str) -> bool:
        return len(s) == len(goal) and len(kmp_search(s + s, goal)) > 0
```

```java [sol-Java]
class Solution {
    public boolean rotateString(String s, String goal) {
        return s.length() == goal.length() && 
               !kmpSearch((s + s).toCharArray(), goal.toCharArray()).isEmpty();
    }

    // 下面是 KMP 模板。对于本题来说，找到一个就可以返回了。为方便大家使用，我保留了完整的模板。
    // 在文本串 text 中查找模式串 pattern，返回所有成功匹配的位置（pattern[0] 在 text 中的下标）
    private List<Integer> kmpSearch(char[] text, char[] pattern) {
        int m = pattern.length;
        int[] pi = new int[m];
        int cnt = 0;
        for (int i = 1; i < m; i++) {
            char b = pattern[i];
            while (cnt > 0 && pattern[cnt] != b) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == b) {
                cnt++;
            }
            pi[i] = cnt;
        }

        List<Integer> pos = new ArrayList<>();
        cnt = 0;
        for (int i = 0; i < text.length; i++) {
            char b = text[i];
            while (cnt > 0 && pattern[cnt] != b) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == b) {
                cnt++;
            }
            if (cnt == m) {
                pos.add(i - m + 1);
                cnt = pi[cnt - 1];
            }
        }
        return pos;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 下面是 KMP 模板。对于本题来说，找到一个就可以返回了。为方便大家使用，我保留了完整的模板。
    // 在文本串 text 中查找模式串 pattern，返回所有成功匹配的位置（pattern[0] 在 text 中的下标）
    vector<int> kmp_search(const string& text, const string& pattern) {
        int m = pattern.size();
        vector<int> pi(m);
        int cnt = 0;
        for (int i = 1; i < m; i++) {
            char b = pattern[i];
            while (cnt && pattern[cnt] != b) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == b) {
                cnt++;
            }
            pi[i] = cnt;
        }
    
        vector<int> pos;
        cnt = 0;
        for (int i = 0; i < text.size(); i++) {
            char b = text[i];
            while (cnt && pattern[cnt] != b) {
                cnt = pi[cnt - 1];
            }
            if (pattern[cnt] == b) {
                cnt++;
            }
            if (cnt == m) {
                pos.push_back(i - m + 1);
                cnt = pi[cnt - 1];
            }
        }
        return pos;
    }

public:
    bool rotateString(string s, string goal) {
        return s.size() == goal.size() && !kmp_search(s + s, goal).empty();
    }
};
```

```go [sol-Go]
// 下面是 KMP 模板。对于本题来说，找到一个就可以返回了。为方便大家使用，我保留了完整的模板。
// 在文本串 text 中查找模式串 pattern，返回所有成功匹配的位置（pattern[0] 在 text 中的下标）
func kmpSearch(text, pattern string) (pos []int) {
	m := len(pattern)
	pi := make([]int, m)
	cnt := 0
	for i := 1; i < m; i++ {
		b := pattern[i]
		for cnt > 0 && pattern[cnt] != b {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == b {
			cnt++
		}
		pi[i] = cnt
	}

	cnt = 0
	for i, b := range text {
		for cnt > 0 && pattern[cnt] != byte(b) {
			cnt = pi[cnt-1]
		}
		if pattern[cnt] == byte(b) {
			cnt++
		}
		if cnt == m {
			pos = append(pos, i-m+1)
			cnt = pi[cnt-1]
		}
	}
	return
}

func rotateString(s, goal string) bool {
	return len(s) == len(goal) && kmpSearch(s+s, goal) != nil
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面字符串题单的「**一、KMP**」。

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

本题来自 `一、KMP（前缀的后缀）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、KMP（前缀的后缀）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
