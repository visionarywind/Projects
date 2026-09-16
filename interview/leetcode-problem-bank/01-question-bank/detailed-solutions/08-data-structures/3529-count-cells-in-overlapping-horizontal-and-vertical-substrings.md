# 3529. 统计水平子串和垂直子串重叠格子的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-cells-in-overlapping-horizontal-and-vertical-substrings/
- 题目 slug：`count-cells-in-overlapping-horizontal-and-vertical-substrings`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：2105
- 外部题解来源：https://leetcode.cn/problems/count-cells-in-overlapping-horizontal-and-vertical-substrings/solutions/3663069/kmp-chai-fen-shu-zu-pythonjavacgo-by-end-h5mz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[KMP + 差分数组（Python/Java/C++/Go）](https://leetcode.cn/problems/count-cells-in-overlapping-horizontal-and-vertical-substrings/solutions/3663069/kmp-chai-fen-shu-zu-pythonjavacgo-by-end-h5mz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`kmp-chai-fen-shu-zu-pythonjavacgo-by-end-h5mz`
- topic id：`3663069`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

1. [KMP 原理讲解](https://www.zhihu.com/question/21923021/answer/37475572)。
2. [差分数组原理讲解](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)，推荐和[【图解】从一维差分到二维差分](https://leetcode.cn/problems/stamping-the-grid/solution/wu-nao-zuo-fa-er-wei-qian-zhui-he-er-wei-zwiu/) 一起看。

## 思路

对于水平子串，把 $\textit{grid}$ 每一行首尾相接，得到一个长为 $mn$ 的文本串 $\textit{text}$，我们需要在文本串 $\textit{text}$ 中找到所有模式串 $\textit{pattern}$ 的出现位置，这正是 **KMP 算法**的标准应用。

标记所有在 $\textit{pattern}$ 中的单元格。假设我们在 $\textit{text}[i]$ 处匹配完成，那么用**差分数组**把下标区间 $[i-k+1,i]$ 加一，其中 $k$ 是 $\textit{pattern}$ 的长度。计算差分数组的前缀和，值大于 $0$ 的下标就对应着在 $\textit{pattern}$ 中的单元格。

对于垂直子串，计算方法同理。可以把相关逻辑封装成一个函数，方便垂直子串**复用**。

## 转换公式

把一维数组的下标，转换成一个 $n$ 列矩阵的二维坐标（行列下标），转换关系是

$$
i\to (\left\lfloor i/n \right\rfloor, i\bmod n)
$$

如何理解？想一想，每 $n$ 个数一行，能排多少行？最后一行剩余多少个数？

把 $n$ 列矩阵中的一个单元格的坐标（行列下标），转换到一维数组的下标，转换关系是

$$
(r,c) \to r\cdot n+c
$$

## 应用公式

对于水平子串，我们计算差分数组的前缀和，得到一个长为 $mn$ 的数组 $\textit{inPatternH}$。

对于垂直子串，我们计算差分数组的前缀和，得到一个长为 $mn$ 的数组 $\textit{inPatternV}$。

如果 $\textit{inPatternH}[i]>0$，则表示单元格 $(\left\lfloor i/n \right\rfloor, i\bmod n)$ 在 $\textit{pattern}$ 中。

单元格 $(\left\lfloor i/n \right\rfloor, i\bmod n)$ 在 $\textit{inPatternV}$ 中的哪个位置？由于垂直子串是竖着扫描的，基于一个 $m$ 列的矩阵，$(\left\lfloor i/n \right\rfloor, i\bmod n)$ 在这个矩阵的二维坐标是 $(i\bmod n, \left\lfloor i/n \right\rfloor)$，所以对应到 $\textit{inPatternV}$ 中的下标为

$$
(i\bmod n)\cdot m + \left\lfloor i/n \right\rfloor
$$

具体请看 [视频讲解](https://www.bilibili.com/video/BV1J2jAziENo/?t=7m4s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def calc_pi(self, pattern: str) -> List[int]:
        pi = [0] * len(pattern)
        cnt = 0
        for i in range(1, len(pi)):
            b = pattern[i]
            while cnt > 0 and pattern[cnt] != b:
                cnt = pi[cnt - 1]
            if pattern[cnt] == b:
                cnt += 1
            pi[i] = cnt
        return pi

    def kmp_search(self, text: List[str], pattern: str, pi: List[int]) -> List[int]:
        n, k = len(text), len(pattern)
        diff = [0] * (n + 1)
        cnt = 0
        for i, b in enumerate(text):
            while cnt > 0 and pattern[cnt] != b:
                cnt = pi[cnt - 1]
            if pattern[cnt] == b:
                cnt += 1
            if cnt == k:
                diff[i - k + 1] += 1
                diff[i + 1] -= 1
                cnt = pi[cnt - 1]
        return list(accumulate(diff[:n]))

    def countCells(self, grid: List[List[str]], pattern: str) -> int:
        h_text = [c for row in grid for c in row]
        v_text = [c for col in zip(*grid) for c in col]

        pi = self.calc_pi(pattern)
        in_pattern_h = self.kmp_search(h_text, pattern, pi)
        in_pattern_v = self.kmp_search(v_text, pattern, pi)

        m, n = len(grid), len(grid[0])
        ans = 0
        for i, in_h in enumerate(in_pattern_h):
            if in_h and in_pattern_v[i % n * m + i // n]:
                ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int countCells(char[][] grid, String pattern) {
        int m = grid.length;
        int n = grid[0].length;

        char[] hText = new char[m * n];
        int idx = 0;
        for (char[] row : grid) {
            for (char c : row) {
                hText[idx++] = c;
            }
        }

        char[] vText = new char[m * n];
        idx = 0;
        for (int j = 0; j < n; j++) {
            for (char[] row : grid) {
                vText[idx++] = row[j];
            }
        }

        char[] pat = pattern.toCharArray();
        int[] pi = calcPi(pat);
        int[] inPatternH = kmpSearch(hText, pat, pi);
        int[] inPatternV = kmpSearch(vText, pat, pi);

        int ans = 0;
        for (int i = 0; i < m * n; i++) {
            if (inPatternH[i] > 0 && inPatternV[i % n * m + i / n] > 0) {
                ans++;
            }
        }
        return ans;
    }

    private int[] calcPi(char[] pattern) {
        int[] pi = new int[pattern.length];
        int match = 0;
        for (int i = 1; i < pi.length; i++) {
            char b = pattern[i];
            while (match > 0 && pattern[match] != b) {
                match = pi[match - 1];
            }
            if (pattern[match] == b) {
                match++;
            }
            pi[i] = match;
        }
        return pi;
    }

    private int[] kmpSearch(char[] text, char[] pattern, int[] pi) {
        int n = text.length;
        int[] diff = new int[n + 1];
        int match = 0;
        for (int i = 0; i < n; i++) {
            char b = text[i];
            while (match > 0 && pattern[match] != b) {
                match = pi[match - 1];
            }
            if (pattern[match] == b) {
                match++;
            }
            if (match == pi.length) {
                diff[i - pi.length + 1]++;
                diff[i + 1]--;
                match = pi[match - 1];
            }
        }
        for (int i = 1; i < n; i++) {
            diff[i] += diff[i - 1];
        }
        return diff;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> calc_pi(string& pattern) {
        vector<int> pi(pattern.size());
        int match = 0;
        for (int i = 1; i < pi.size(); i++) {
            char b = pattern[i];
            while (match > 0 && pattern[match] != b) {
                match = pi[match - 1];
            }
            if (pattern[match] == b) {
                match++;
            }
            pi[i] = match;
        }
        return pi;
    }

    vector<int> kmp_search(string& text, string& pattern, vector<int>& pi) {
        int n = text.size(), k = pattern.size();
        vector<int> diff(n + 1);
        int match = 0;
        for (int i = 0; i < n; i++) {
            int b = text[i];
            while (match > 0 && pattern[match] != b) {
                match = pi[match - 1];
            }
            if (pattern[match] == b) {
                match++;
            }
            if (match == k) {
                diff[i - k + 1]++;
                diff[i + 1]--;
                match = pi[match - 1];
            }
        }
        partial_sum(diff.begin(), diff.end(), diff.begin());
        return diff;
    }

public:
    int countCells(vector<vector<char>>& grid, string pattern) {
        int m = grid.size(), n = grid[0].size();
        string h_text;
        for (auto& row : grid) {
            h_text.insert(h_text.end(), row.begin(), row.end());
        }
        string v_text;
        for (int j = 0; j < n; j++) {
            for (auto& row : grid) {
                v_text += row[j];
            }
        }

        vector<int> pi = calc_pi(pattern);
        vector<int> in_pattern_h = kmp_search(h_text, pattern, pi);
        vector<int> in_pattern_v = kmp_search(v_text, pattern, pi);

        int ans = 0;
        for (int i = 0; i < m * n; i++) {
            if (in_pattern_h[i] && in_pattern_v[i % n * m + i / n]) {
                ans++;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func calcPi(pattern string) []int {
	pi := make([]int, len(pattern))
	match := 0
	for i := 1; i < len(pi); i++ {
		b := pattern[i]
		for match > 0 && pattern[match] != b {
			match = pi[match-1]
		}
		if pattern[match] == b {
			match++
		}
		pi[i] = match
	}
	return pi
}

func kmpSearch(text []byte, pattern string, pi []int) []int {
	n := len(text)
	diff := make([]int, n+1)
	match := 0
	for i, b := range text {
		for match > 0 && pattern[match] != b {
			match = pi[match-1]
		}
		if pattern[match] == b {
			match++
		}
		if match == len(pi) {
			diff[i-len(pi)+1]++
			diff[i+1]--
			match = pi[match-1]
		}
	}
	for i := 1; i < n; i++ {
		diff[i] += diff[i-1]
	}
	return diff[:n]
}

func countCells(grid [][]byte, pattern string) (ans int) {
	m, n := len(grid), len(grid[0])
	hText := slices.Concat(grid...)
	vText := make([]byte, 0, m*n)
	for j := range n {
		for _, row := range grid {
			vText = append(vText, row[j])
		}
	}

	pi := calcPi(pattern)
	inPatternH := kmpSearch(hText, pattern, pi)
	inPatternV := kmpSearch(vText, pattern, pi)

	for i, x := range inPatternH {
		if x > 0 && inPatternV[i%n*m+i/n] > 0 {
			ans++
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

更多相似题目，见下面字符串题单的「**一、KMP**」和数据结构题单的「**§2.1 一维差分**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
