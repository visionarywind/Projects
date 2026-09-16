# 68. 文本左右对齐

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/text-justification/
- 题目 slug：`text-justification`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/text-justification/solutions/535122/golang-jian-ji-xie-fa-by-endlesscheng-180a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分组循环，把困难题变成简单题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/text-justification/solutions/535122/golang-jian-ji-xie-fa-by-endlesscheng-180a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`golang-jian-ji-xie-fa-by-endlesscheng-180a`
- topic id：`535122`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分组循环

**适用场景**：按照题目要求，数组会被分割成若干组，每一组的判断/处理逻辑是相同的。

**核心思想**：

- 外层循环负责遍历组之前的准备工作（记录开始位置），和遍历组之后的统计工作（更新答案）。
- 内层循环负责遍历组，找出这一组最远在哪结束。

这个写法的好处是，各个逻辑块分工明确，也不需要特判最后一组（易错点）。以我的经验，这个写法是所有写法中最不容易出 bug 的，推荐大家记住。

## 内层循环开始前

记录当前位置 $\textit{start} = i$，这也是这一行第一个单词的下标。

初始化这一行的最小长度 $\textit{sumLen}$ 为 $\textit{words}[i]$ 的长度。

## 内层循环

从这一行的第二个单词 $i+1$ 开始循环。

从第二个单词开始，每个单词之前必须有一个空格。

所以每个单词占用的长度是单词长度加一，加给 $\textit{sumLen}$。

如果 $\textit{sumLen} + \text{len}(\textit{words}[i]) + 1  > \textit{maxWidth}$，退出循环。

## 内层循环结束后

这一行还剩下 $\textit{extraSpaces} = \textit{maxWidth} - \textit{sumLen}$ 个空格没有分配。

这一行单词之间的空隙个数 $\textit{gaps} = i - \textit{start} - 1$，即单词个数减一。

首先处理**特殊情况**。如果只有一个单词，或者现在是最后一行，那么根据题目要求，所有单词左对齐，单词之间只有一个空格，末尾补上 $\textit{extraSpaces}$ 个空格。

然后处理**一般情况**。

单词之间至少有 $\left\lfloor\dfrac{\textit{extraSpaces}}{\textit{gaps}}\right\rfloor+1$ 个空格。其中 $+1$ 是因为单词之间必须有一个空格，不算在 $\textit{extraSpaces}$ 中，这里重新加进来。

还剩下 $\textit{rem} = \textit{extraSpaces}\bmod \textit{gaps}$ 个空格，分配给前 $\textit{rem}$ 个空隙。换句话说，前 $\textit{rem}+1$ 个单词之间的空格个数多 $1$。

```py [sol-Python3]
class Solution:
    def fullJustify(self, words: List[str], maxWidth: int) -> List[str]:
        ans = []
        n = len(words)
        i = 0
        while i < n:
            start = i  # 这一行第一个单词的下标
            sum_len = -1  # 第一个单词之前没有空格
            while i < n and sum_len + len(words[i]) + 1 <= maxWidth:
                sum_len += len(words[i]) + 1  # 单词之间至少要有一个空格
                i += 1

            extra_spaces = maxWidth - sum_len  # 这一行剩余未分配的空格个数
            gaps = i - start - 1  # 这一行单词之间的空隙个数（单词个数减一）

            # 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
            if gaps == 0 or i == n:
                row = ' '.join(words[start: i]) + ' ' * extra_spaces  # 末尾补空格
                ans.append(row)
                continue

            # 一般情况：把 extra_spaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
            avg, rem = divmod(extra_spaces, gaps)
            spaces = ' ' * (avg + 1)  # +1 表示加上单词之间已有的一个空格
            # 前 rem 个空隙多一个空格
            row = (spaces + ' ').join(words[start: start + rem + 1]) + \
                  spaces + spaces.join(words[start + rem + 1: i])
            ans.append(row)
        return ans
```

```java [sol-Java]
class Solution {
    public List<String> fullJustify(String[] words, int maxWidth) {
        List<String> ans = new ArrayList<>();
        int n = words.length;
        for (int i = 0; i < n; ) {
            int start = i; // 这一行第一个单词的下标
            int sumLen = words[i].length(); // 第一个单词的长度
            for (i++; i < n && sumLen + words[i].length() + 1 <= maxWidth; i++) {
                sumLen += words[i].length() + 1; // 单词之间至少要有一个空格
            }

            int extraSpaces = maxWidth - sumLen; // 这一行剩余未分配的空格个数
            int gaps = i - start - 1; // 这一行单词之间的空隙个数（单词个数减一）

            // 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
            if (gaps == 0 || i == n) {
                String row = join(words, start, i, " ") + " ".repeat(extraSpaces); // 末尾补空格
                ans.add(row);
                continue;
            }

            // 一般情况：把 extraSpaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
            int avg = extraSpaces / gaps;
            int rem = extraSpaces % gaps;
            String spaces = " ".repeat(avg + 1); // +1 表示加上单词之间已有的一个空格
            String row = join(words, start, start + rem + 1, spaces + ' ') + // 前 rem 个空隙多一个空格
                    spaces + join(words, start + rem + 1, i, spaces);
            ans.add(row);
        }
        return ans;
    }

    // 用 StringBuilder 的写法见【Java 写法二】
    private String join(String[] words, int start, int end, String sep) {
        return String.join(sep, Arrays.copyOfRange(words, start, end));
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public List<String> fullJustify(String[] words, int maxWidth) {
        List<String> ans = new ArrayList<>();
        int n = words.length;
        for (int i = 0; i < n; ) {
            int start = i; // 这一行第一个单词的下标
            int sumLen = words[i].length(); // 第一个单词的长度
            for (i++; i < n && sumLen + words[i].length() + 1 <= maxWidth; i++) {
                sumLen += words[i].length() + 1; // 单词之间至少要有一个空格
            }

            int extraSpaces = maxWidth - sumLen; // 这一行剩余未分配的空格个数
            int gaps = i - start - 1; // 这一行单词之间的空隙个数（单词个数减一）

            // 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
            if (gaps == 0 || i == n) {
                StringBuilder row = join(words, start, i, " ");
                row.repeat(' ', extraSpaces); // 末尾补空格
                ans.add(row.toString());
                continue;
            }

            // 一般情况：把 extraSpaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
            int avg = extraSpaces / gaps;
            int rem = extraSpaces % gaps;
            String spaces = " ".repeat(avg + 1); // +1 表示加上单词之间已有的一个空格
            StringBuilder row = join(words, start, start + rem + 1, spaces + ' '); // 前 rem 个空隙多一个空格
            row.append(spaces);
            row.append(join(words, start + rem + 1, i, spaces));
            ans.add(row.toString());
        }
        return ans;
    }

    private StringBuilder join(String[] words, int start, int end, String sep) {
        StringBuilder res = new StringBuilder();
        for (int i = start; i < end; i++) {
            if (i > start) {
                res.append(sep);
            }
            res.append(words[i]);
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<string> fullJustify(vector<string>& words, int maxWidth) {
        auto join = [&](int start, int end, const string& sep) -> string {
            string res;
            for (int i = start; i < end; i++) {
                if (i > start) {
                    res += sep;
                }
                res += words[i];
            }
            return res;
        };

        vector<string> ans;
        int n = words.size();
        for (int i = 0; i < n;) {
            int start = i; // 这一行第一个单词的下标
            int sum_len = words[i].size(); // 第一个单词的长度
            for (i++; i < n && sum_len + words[i].size() + 1 <= maxWidth; i++) {
                sum_len += words[i].size() + 1; // 单词之间至少要有一个空格
            }

            int extra_spaces = maxWidth - sum_len; // 这一行剩余未分配的空格个数
            int gaps = i - start - 1; // 这一行单词之间的空隙个数（单词个数减一）

            // 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
            if (gaps == 0 || i == n) {
                string row = join(start, i, " ");
                row += string(extra_spaces, ' '); // 末尾补空格
                ans.push_back(move(row));
                continue;
            }

            // 一般情况：把 extra_spaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
            int avg = extra_spaces / gaps;
            int rem = extra_spaces % gaps;
            string spaces(avg + 1, ' '); // +1 表示加上单词之间已有的一个空格
            string row = join(start, start + rem + 1, spaces + " "); // 前 rem 个空隙多一个空格
            row += spaces;
            row += join(start + rem + 1, i, spaces);
            ans.push_back(move(row));
        }
        return ans;
    }
};
```

```go [sol-Go]
func fullJustify(words []string, maxWidth int) (ans []string) {
	n := len(words)
	for i := 0; i < n; {
		start := i // 这一行第一个单词的下标
		sumLen := len(words[i]) // 第一个单词的长度
		for i++; i < n && sumLen+len(words[i])+1 <= maxWidth; i++ {
			sumLen += len(words[i]) + 1 // 单词之间至少要有一个空格
		}

		extraSpaces := maxWidth - sumLen // 这一行剩余未分配的空格个数
		gaps := i - start - 1 // 这一行单词之间的空隙个数（单词个数减一）

		// 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
		if gaps == 0 || i == n {
			row := strings.Join(words[start:i], " ") +
				strings.Repeat(" ", extraSpaces) // 末尾补空格
			ans = append(ans, row)
			continue
		}

		// 一般情况：把 extraSpaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
		avg, rem := extraSpaces/gaps, extraSpaces%gaps
		spaces := strings.Repeat(" ", avg+1) // +1 表示加上单词之间已有的一个空格
		row := strings.Join(words[start:start+rem+1], spaces+" ") + // 前 rem 个空隙多一个空格
			spaces + strings.Join(words[start+rem+1:i], spaces)
		ans = append(ans, row)
	}
	return
}
```

```js [sol-JavaScript]
var fullJustify = function(words, maxWidth) {
    const ans = [];
    const n = words.length;
    for (let i = 0; i < n;) {
        const start = i; // 这一行第一个单词的下标
        let sumLen = words[i].length; // 第一个单词的长度
        for (i++; i < n && sumLen + words[i].length + 1 <= maxWidth; i++) {
            sumLen += words[i].length + 1; // 单词之间至少要有一个空格
        }

        const extraSpaces = maxWidth - sumLen; // 这一行剩余未分配的空格个数
        const gaps = i - start - 1; // 这一行单词之间的空隙个数（单词个数减一）

        // 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
        if (gaps === 0 || i === n) {
            const row = words.slice(start, i).join(" ") + " ".repeat(extraSpaces); // 末尾补空格
            ans.push(row);
            continue;
        }

        // 一般情况：把 extraSpaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
        const avg = Math.floor(extraSpaces / gaps);
        const rem = extraSpaces % gaps;
        const spaces = " ".repeat(avg + 1); // +1 表示加上单词之间已有的一个空格
        const row = words.slice(start, start + rem + 1).join(spaces + " ") + // 前 rem 个空隙多一个空格
            spaces + words.slice(start + rem + 1, i).join(spaces);
        ans.push(row);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn full_justify(words: Vec<String>, max_width: i32) -> Vec<String> {
        let n = words.len();
        let max_width = max_width as usize;
        let mut ans = vec![];
        let mut i = 0;
        while i < n {
            let start = i; // 这一行第一个单词的下标
            let mut sum_len = words[i].len(); // 第一个单词的长度
            i += 1;
            while i < n && sum_len + 1 + words[i].len() <= max_width {
                sum_len += words[i].len() + 1; // 单词之间至少要有一个空格
                i += 1;
            }

            let extra_spaces = max_width - sum_len; // 这一行剩余未分配的空格个数
            let gaps = i - start - 1; // 这一行单词之间的空隙个数（单词个数减一）

            // 特殊情况：如果只有一个单词，或者是最后一行，那么左对齐，末尾补空格
            if gaps == 0 || i == n {
                let row = words[start..i].join(" ") + &" ".repeat(extra_spaces); // 末尾补空格
                ans.push(row);
                continue;
            }

            // 一般情况：把 extra_spaces 个空格均匀分配到 gaps 个空隙中（靠左的空格更多）
            let avg = extra_spaces / gaps;
            let rem = extra_spaces % gaps;
            let spaces = &" ".repeat(avg + 1); // +1 表示加上单词之间已有的一个空格
            let row = words[start..start + rem + 1].join(&(spaces.clone() + " ")) + // 前 rem 个空隙多一个空格
                spaces + &words[start + rem + 1..i].join(spaces);
            ans.push(row);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nL + \textit{maxWidth})$，其中 $n$ 是 $\textit{words}$ 的长度，$L\le 20$ 是 $\textit{words}[i]$ 的最大长度。虽然写了个二重循环，但是内层循环中对 $i$ 加一的**总**执行次数不会超过 $n$ 次，所以总的循环次数是 $\mathcal{O}(n)$ 的。最后一行需要 $\mathcal{O}(\textit{maxWidth})$ 的时间，其余行消耗的时间**之和**是 $\mathcal{O}(nL)$。你可能会问，如果每一行都只有一个单词呢？这种情况下 $2L+1 > \textit{maxWidth}$，又由于 $L \le \textit{maxWidth}$，所以 $L$ 和 $\textit{maxWidth}$ 是同阶的，此时 $\mathcal{O}(nL) = \mathcal{O}(n\cdot \textit{maxWidth})$。
- 空间复杂度：$\mathcal{O}(nL + \textit{maxWidth})$。最后一行需要 $\mathcal{O}(\textit{maxWidth})$ 的空间，其余行消耗的空间**之和**是 $\mathcal{O}(nL)$。

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
