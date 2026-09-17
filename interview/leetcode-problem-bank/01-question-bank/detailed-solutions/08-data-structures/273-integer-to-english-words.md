# 273. 整数转换英文表示

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/integer-to-english-words/
- 题目 slug：`integer-to-english-words`
- 来源专题：常用数据结构
- 来源分类路径：编程能力强化训练 / Part A
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/integer-to-english-words/solutions/3823589/chai-fen-cheng-ruo-gan-xiao-yu-1000-de-s-okt8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[拆分成若干小于 1000 的数（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/integer-to-english-words/solutions/3823589/chai-fen-cheng-ruo-gan-xiao-yu-1000-de-s-okt8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-fen-cheng-ruo-gan-xiao-yu-1000-de-s-okt8`
- topic id：`3823589`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

例如 $\textit{num} = 1\ 234\ 567\ 811$，可以拆分成如下四段：

- $\texttt{One Billion}$。
- $\texttt{Two Hundred Thirty Four Million}$。
- $\texttt{Five Hundred Sixty Seven Thousand}$。
- $\texttt{Eight Hundred Eleven}$。

每一段都是一个小于 $1000$ 的数 $x$，跟着一个大数单位（$\texttt{Billion}$/$\texttt{Million}$/$\texttt{Thousand}$/空）。

对于每一段：

- 如果 $x=0$，跳过，继续处理下一段。
- 如果 $x\ge 100$，添加百位 $\left\lfloor\dfrac{x}{100}\right\rfloor$ 的对应单词，然后添加 $\texttt{Hundred}$。
- 然后，如果 $x\bmod 100 < 20$，添加 $x\bmod 100$ 对应单词；否则正常拆分成十位 $\left\lfloor\dfrac{x}{10}\right\rfloor\bmod 10$ 和个位 $x\bmod 10$，添加对应单词。

特殊情况：如果 $\textit{num}=0$，返回 $\texttt{Zero}$。

```py [sol-Python3]
ones = ("", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten",
        "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen")
tens = ("", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety")
large_numbers = ("", "Thousand", "Million", "Billion")

class Solution:
    def numberToWords(self, num: int) -> str:
        if num == 0:
            return "Zero"

        ans = []

        # 1_234_567_811
        # One Billion + Two Hundred Thirty Four Million + Five Hundred Sixty Seven Thousand + Eight Hundred Eleven
        # 拆分后，都是小于 1000 的数 + 大数单位（Billion/Million/Thousand/空）
        for i in range(len(large_numbers) - 1, -1, -1):
            x = num // 10 ** (i * 3) % 1000
            if x == 0:
                continue
            # 百位
            if x >= 100:
                ans.append(ones[x // 100])
                ans.append("Hundred")
            # 十位和个位
            if x % 100 < 20:  # 特殊处理小于 20 的数
                ans.append(ones[x % 100])
            else:
                ans.append(tens[x // 10 % 10])
                ans.append(ones[x % 10])
            ans.append(large_numbers[i])  # 大数单位

        return ' '.join(s for s in ans if s)
```

```java [sol-Java]
class Solution {
    private static final String[] ones = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten",
            "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    private static final String[] tens = {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    private static final String[] largeNumbers = {"", "Thousand", "Million", "Billion"};

    public String numberToWords(int num) {
        if (num == 0) {
            return "Zero";
        }

        StringBuilder ans = new StringBuilder();

        // 1_234_567_811
        // One Billion + Two Hundred Thirty Four Million + Five Hundred Sixty Seven Thousand + Eight Hundred Eleven
        // 拆分后，都是小于 1000 的数 + 大数单位（Billion/Million/Thousand/空）
        for (int i = largeNumbers.length - 1; i >= 0; i--) {
            int x = num / (int) Math.pow(10, i * 3) % 1000;
            if (x == 0) {
                continue;
            }
            // 百位
            if (x >= 100) {
                add(ans, ones[x / 100]);
                add(ans, "Hundred");
            }
            // 十位和个位
            if (x % 100 < 20) { // 特殊处理小于 20 的数
                add(ans, ones[x % 100]);
            } else {
                add(ans, tens[x / 10 % 10]);
                add(ans, ones[x % 10]);
            }
            add(ans, largeNumbers[i]); // 大数单位
        }

        return ans.toString();
    }

    private void add(StringBuilder ans, String s) {
        if (s.isEmpty()) {
            return;
        }
        if (!ans.isEmpty()) {
            ans.append(' '); // 相邻单词之间添加空格
        }
        ans.append(s);
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr string ones[20] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten",
                                        "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    static constexpr string tens[10] = {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    static constexpr string large_numbers[4] = {"", "Thousand", "Million", "Billion"};

public:
    string numberToWords(int num) {
        if (num == 0) {
            return "Zero";
        }

        string ans;
        auto add = [&](const string& s) -> void {
            if (s.empty()) {
                return;
            }
            if (!ans.empty()) {
                ans += ' '; // 相邻单词之间添加空格
            }
            ans += s;
        };

        // 1'234'567'811
        // One Billion + Two Hundred Thirty Four Million + Five Hundred Sixty Seven Thousand + Eight Hundred Eleven
        // 拆分后，都是小于 1000 的数 + 大数单位（Billion/Million/Thousand/空）
        for (int i = 3; i >= 0; i--) {
            int x = num / (int) pow(10, i * 3) % 1000;
            if (x == 0) {
                continue;
            }
            // 百位
            if (x >= 100) {
                add(ones[x / 100]);
                add("Hundred");
            }
            // 十位和个位
            if (x % 100 < 20) { // 特殊处理小于 20 的数
                add(ones[x % 100]);
            } else {
                add(tens[x / 10 % 10]);
                add(ones[x % 10]);
            }
            add(large_numbers[i]); // 大数单位
        }

        return ans;
    }
};
```

```go [sol-Go]
var ones = []string{"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten",
	"Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"}
var tens = []string{"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"}
var largeNumbers = []string{"", "Thousand", "Million", "Billion"}

func numberToWords(num int) string {
	if num == 0 {
		return "Zero"
	}

	ans := &strings.Builder{}
	add := func(s string) {
		if s == "" {
			return
		}
		if ans.Len() > 0 {
			ans.WriteByte(' ') // 相邻单词之间添加空格
		}
		ans.WriteString(s)
	}

	// 1_234_567_811
	// One Billion + Two Hundred Thirty Four Million + Five Hundred Sixty Seven Thousand + Eight Hundred Eleven
	// 拆分后，都是小于 1000 的数 + 大数单位（Billion/Million/Thousand/空）
	for i := len(largeNumbers) - 1; i >= 0; i-- {
		x := num / int(math.Pow10(i*3)) % 1000
		if x == 0 {
			continue
		}
		// 百位
		if x >= 100 {
			add(ones[x/100])
			add("Hundred")
		}
		// 十位和个位
		if x%100 < 20 { // 特殊处理小于 20 的数
			add(ones[x%100])
		} else {
			add(tens[x/10%10])
			add(ones[x%10])
		}
		add(largeNumbers[i]) // 大数单位
	}

	return ans.String()
}
```

```js [sol-JavaScript]
var ones = ["", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten",
    "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"];
var tens = ["", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"];
var largeNumbers = ["", "Thousand", "Million", "Billion"];

var numberToWords = function(num) {
    if (num === 0) {
        return "Zero";
    }

    const ans = [];

    // 1_234_567_811
    // One Billion + Two Hundred Thirty Four Million + Five Hundred Sixty Seven Thousand + Eight Hundred Eleven
    // 拆分后，都是小于 1000 的数 + 大数单位（Billion/Million/Thousand/空）
    for (let i = largeNumbers.length - 1; i >= 0; i--) {
        const x = Math.floor(num / Math.pow(10, i * 3)) % 1000;
        if (x === 0) {
            continue;
        }
        // 百位
        if (x >= 100) {
            ans.push(ones[Math.floor(x / 100)], "Hundred");
        }
        // 十位和个位
        if (x % 100 < 20) { // 特殊处理小于 20 的数
            ans.push(ones[x % 100]);
        } else {
            ans.push(tens[Math.floor(x / 10) % 10], ones[x % 10]);
        }
        ans.push(largeNumbers[i]); // 大数单位
    }

    return ans.filter(Boolean).join(' ');
};
```

```rust [sol-Rust]
const ONES: [&str; 20] = ["", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine",
    "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"];
const TENS: [&str; 10] = ["", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"];
const LARGE_NUMBERS: [&str; 4] = ["", "Thousand", "Million", "Billion"];

impl Solution {
    pub fn number_to_words(num: i32) -> String {
        if num == 0 {
            return "Zero".to_string();
        }

        let mut ans = vec![];

        // 1_234_567_811
        // One Billion + Two Hundred Thirty Four Million + Five Hundred Sixty Seven Thousand + Eight Hundred Eleven
        // 拆分后，都是小于 1000 的数 + 大数单位（Billion/Million/Thousand/空）
        for (i, large_num) in LARGE_NUMBERS.iter().enumerate().rev() {
            let x = (num / 10_i32.pow(i as u32 * 3) % 1000) as usize;
            if x == 0 {
                continue;
            }
            // 百位
            if x >= 100 {
                ans.push(ONES[x / 100]);
                ans.push("Hundred");
            }
            // 十位和个位
            if x % 100 < 20 {
                ans.push(ONES[x % 100]);
            } else {
                ans.push(TENS[x / 10 % 10]);
                ans.push(ONES[x % 10]);
            }
            ans.push(large_num);
        }

        ans.into_iter().filter(|s| !s.is_empty()).collect::<Vec<_>>().join(" ")
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。由于循环次数是固定的，可以认为这题是 $\mathcal{O}(1)$ 的。
- 空间复杂度：$\mathcal{O}(\log \textit{num})$ 或 $\mathcal{O}(1)$。

## 相似题目

[12. 整数转罗马数字](https://leetcode.cn/problems/integer-to-roman/)

## 专题训练

见下面数据结构题单的「**专题：比较复杂的题目**」。

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

本题来自 `编程能力强化训练 / Part A`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `编程能力强化训练 / Part A`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
