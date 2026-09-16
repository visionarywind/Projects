# 3926. 有效单词计数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-valid-word-occurrences/
- 题目 slug：`count-valid-word-occurrences`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1608
- 外部题解来源：https://leetcode.cn/problems/count-valid-word-occurrences/solutions/3966164/fen-zu-xun-huan-pythonjavacgo-by-endless-ii4f/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分组循环（Python/Java/C++/Go）](https://leetcode.cn/problems/count-valid-word-occurrences/solutions/3966164/fen-zu-xun-huan-pythonjavacgo-by-endless-ii4f/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-zu-xun-huan-pythonjavacgo-by-endless-ii4f`
- topic id：`3966164`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**适用场景**：按照题目要求，字符串会被分割成若干组，每一组的判断/处理逻辑是相同的。

**核心思想**：

- 外层循环负责遍历组之前的准备工作（记录开始位置），和遍历组之后的统计工作。
- 内层循环负责遍历组，找出这一组最远在哪结束。

这个写法的好处是，各个逻辑块分工明确，也不需要特判最后一组（易错点）。以我的经验，这个写法是所有写法中最不容易出 bug 的，推荐大家记住。

对于本题，单词的第一个字母必须是小写英文字母，作为一组的开始。如果遍历到 `' '` 或者 `"--"` 或者 `"- "`，跳出内层循环。

用哈希表统计每个单词的个数，即可快速回答询问。

[本题视频讲解](https://www.bilibili.com/video/BV1Vb5L6zEgm/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countWordOccurrences(self, chunks: list[str], queries: list[str]) -> list[int]:
        s = ''.join(chunks)
        cnt = defaultdict(int)

        for t in s.split():  # 不用 split 的写法见另一份代码
            n = len(t)
            i = 0
            while i < n:
                if t[i] == '-':
                    i += 1
                    continue
                start = i
                # 遇到 "--"（连续两个 '-'）就跳出循环
                while i < n and (t[i] != '-' or i < n - 1 and t[i + 1] != '-'):
                    i += 1
                cnt[t[start: i]] += 1

        return [cnt[q] for q in queries]
```

```py [sol-Python3 不用 split]
class Solution:
    def countWordOccurrences(self, chunks: list[str], queries: list[str]) -> list[int]:
        s = ''.join(chunks)
        n = len(s)
        cnt = defaultdict(int)

        i = 0
        while i < n:
            if s[i] == ' ' or s[i] == '-':
                i += 1
                continue
            start = i
            # 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
            while i < n and s[i] != ' ' and (s[i] != '-' or i < n - 1 and s[i + 1] != '-' and s[i + 1] != ' '):
                i += 1
            cnt[s[start: i]] += 1

        return [cnt[q] for q in queries]
```

```java [sol-Java]
class Solution {
    public int[] countWordOccurrences(String[] chunks, String[] queries) {
        char[] s = String.join("", chunks).toCharArray();
        int n = s.length;
        Map<String, Integer> cnt = new HashMap<>();

        for (int i = 0; i < n; i++) {
            if (s[i] == ' ' || s[i] == '-') {
                continue;
            }
            int start = i;
            // 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
            while (i < n && s[i] != ' ' && (s[i] != '-' || i < n - 1 && s[i + 1] != '-' && s[i + 1] != ' ')) {
                i++;
            }
            String word = new String(s, start, i - start);
            cnt.merge(word, 1, Integer::sum); // cnt[word]++
        }

        int[] ans = new int[queries.length];
        for (int i = 0; i < queries.length; i++) {
            ans[i] = cnt.getOrDefault(queries[i], 0);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> countWordOccurrences(vector<string>& chunks, vector<string>& queries) {
        string s = chunks | views::join | ranges::to<string>();
        int n = s.size();
        unordered_map<string, int> cnt;

        for (int i = 0; i < n; i++) {
            if (s[i] == ' ' || s[i] == '-') {
                continue;
            }
            int start = i;
            // 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
            while (i < n && s[i] != ' ' && (s[i] != '-' || i < n - 1 && s[i + 1] != '-' && s[i + 1] != ' ')) {
                i++;
            }
            cnt[s.substr(start, i - start)]++;
        }

        vector<int> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            ans[i] = cnt[queries[i]];
        }
        return ans;
    }
};
```

```go [sol-Go]
func countWordOccurrences(chunks []string, queries []string) []int {
	s := strings.Join(chunks, "")
	n := len(s)
	cnt := map[string]int{}

	for i := 0; i < n; i++ {
		if s[i] == ' ' || s[i] == '-' {
			continue
		}
		start := i
		// 遇到 ' ' 或者 "--" 或者 "- " 时，跳出循环
		for i < n && s[i] != ' ' && (s[i] != '-' || i < n-1 && s[i+1] != '-' && s[i+1] != ' ') {
			i++
		}
		cnt[s[start:i]]++
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		ans[i] = cnt[q]
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(C+Q)$，其中 $C$ 是 $\textit{chunks}$ 的字符串长度之和，$Q$ 是 $\textit{queries}$ 的字符串长度之和。
- 空间复杂度：$\mathcal{O}(C)$。返回值不计入。

## 专题训练

见下面双指针题单的「**六、分组循环**」。

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

本题来自 `六、分组循环`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、分组循环`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
