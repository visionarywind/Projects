# 12. 整数转罗马数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/integer-to-roman/
- 题目 slug：`integer-to-roman`
- 来源专题：常用数据结构
- 来源分类路径：编程能力强化训练 / Part A
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/integer-to-roman/solutions/2848775/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-kmp6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/integer-to-roman/solutions/2848775/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-kmp6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavaccgojsrust-by-e-kmp6`
- topic id：`2848775`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

把 $\textit{num}$ 拆分成千位数、百位数、十位数和个位数，分别用罗马数字表示。

例如示例 1 的 $\textit{num}=3749$，拆分后对应的罗马数字分别为 $\texttt{MMM},\texttt{DCC},\texttt{XL},\texttt{IX}$。

根据题意，从千位数到个位数，阿拉伯数字与罗马数字的转换关系为：

- **千位**从 $1$ 到 $3$ 依次为 $\texttt{M},\texttt{MM},\texttt{MMM}$。
- **百位**从 $1$ 到 $9$ 依次为 $\texttt{C},\texttt{CC},\texttt{CCC},\texttt{CD},\texttt{D},\texttt{DC},\texttt{DCC},\texttt{DCCC},\texttt{CM}$。
- **十位**从 $1$ 到 $9$ 依次为 $\texttt{X},\texttt{XX},\texttt{XXX},\texttt{XL},\texttt{L},\texttt{LX},\texttt{LXX},\texttt{LXXX},\texttt{XC}$。
- **个位**从 $1$ 到 $9$ 依次为 $\texttt{I},\texttt{II},\texttt{III},\texttt{IV},\texttt{V},\texttt{VI},\texttt{VII},\texttt{VIII},\texttt{IX}$。

把 $\textit{num}$ 拆分成各个数位的公式为：

- **千位**：$\left\lfloor\dfrac{\textit{num}}{1000}\right\rfloor$。例如 $\left\lfloor\dfrac{3749}{1000}\right\rfloor = 3$。
- **百位**：$\left\lfloor\dfrac{\textit{num}}{100}\right\rfloor\bmod 10$。例如 $\left\lfloor\dfrac{3749}{100}\right\rfloor\bmod 10 = 37\bmod 10 = 7$。
- **十位**：$\left\lfloor\dfrac{\textit{num}}{10}\right\rfloor\bmod 10$。例如 $\left\lfloor\dfrac{3749}{10}\right\rfloor\bmod 10 = 374\bmod 10 = 4$。
- **个位**：$\textit{num} \bmod 10$。例如 $3749 \bmod 10 = 9$。

用**字符串数组**存储罗马数字，用上述公式计算出的数位当作数组下标，取出对应的罗马数字（字符串）。把这些字符串拼接起来，得到答案。

```py [sol-Python3]
R = (
    ("", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"),  # 个位
    ("", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"),  # 十位
    ("", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"),  # 百位
    ("", "M", "MM", "MMM"),  # 千位
)

class Solution:
    def intToRoman(self, num: int) -> str:
        return R[3][num // 1000] + R[2][num // 100 % 10] + R[1][num // 10 % 10] + R[0][num % 10]
```

```java [sol-Java]
class Solution {
    private static final String[][] R = new String[][]{
        {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"}, // 个位
        {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"}, // 十位
        {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"}, // 百位
        {"", "M", "MM", "MMM"}, // 千位
    };

    public String intToRoman(int num) {
        return R[3][num / 1000] + R[2][num / 100 % 10] + R[1][num / 10 % 10] + R[0][num % 10];
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr string R[4][10] = {
        {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"}, // 个位
        {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"}, // 十位
        {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"}, // 百位
        {"", "M", "MM", "MMM"}, // 千位
    };

public:
    string intToRoman(int num) {
        return R[3][num / 1000] + R[2][num / 100 % 10] + R[1][num / 10 % 10] + R[0][num % 10];
    }
};
```

```c [sol-C]
const char* R[4][10] = {
    {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"}, // 个位
    {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"}, // 十位
    {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"}, // 百位
    {"", "M", "MM", "MMM"}, // 千位
};

char* intToRoman(int num) {
    char* ans = malloc(16);
    strcpy(ans, R[3][num / 1000]);
    strcat(ans, R[2][num / 100 % 10]);
    strcat(ans, R[1][num / 10 % 10]);
    strcat(ans, R[0][num % 10]);
    return ans;
}
```

```go [sol-Go]
var R = [4][10]string{
    {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"}, // 个位
    {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"}, // 十位
    {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"}, // 百位
    {"", "M", "MM", "MMM"}, // 千位
}

func intToRoman(num int) string {
    return R[3][num/1000] + R[2][num/100%10] + R[1][num/10%10] + R[0][num%10]
}
```

```js [sol-JavaScript]
const R = [
    ["", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"], // 个位
    ["", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"], // 十位
    ["", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"], // 百位
    ["", "M", "MM", "MMM"], // 千位
];

var intToRoman = function(num) {
    return R[3][Math.floor(num / 1000)] + R[2][Math.floor(num / 100) % 10] + R[1][Math.floor(num / 10) % 10] + R[0][num % 10];
};
```

```rust [sol-Rust]
const ONES: [&str; 10] = ["", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"]; // 个位
const TENS: [&str; 10] = ["", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"]; // 十位
const HUNDREDS: [&str; 10] = ["", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"]; // 百位
const THOUSANDS: [&str; 4] = ["", "M", "MM", "MMM"]; // 千位

impl Solution {
    pub fn int_to_roman(num: i32) -> String {
        let n = num as usize;
        format!("{}{}{}{}", THOUSANDS[n / 1000], HUNDREDS[n / 100 % 10], TENS[n / 10 % 10], ONES[n % 10])
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

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
