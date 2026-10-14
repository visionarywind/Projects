# 13. 罗马数字转整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/roman-to-integer/
- 题目 slug：`roman-to-integer`
- 来源专题：常用数据结构
- 来源分类路径：编程能力强化训练 / Part A
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/roman-to-integer/solutions/2928945/jian-dan-ti-jiu-you-jian-dan-xie-fa-pyth-egyn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题就有简单写法！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/roman-to-integer/solutions/2928945/jian-dan-ti-jiu-you-jian-dan-xie-fa-pyth-egyn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jiu-you-jian-dan-xie-fa-pyth-egyn`
- topic id：`2928945`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

本题的难点在于处理六种特殊规则，但这六种特殊规则其实可以统一起来：

- 设 $x=s[i-1],\ y=s[i]$，这是两个**相邻**的罗马数字。
- 如果 $x$ 的数值小于 $y$ 的数值，那么 $x$ 的数值要取相反数。例如 $\texttt{IV}$ 中的 $\texttt{I}$ 相当于 $-1$，$\texttt{CM}$ 中的 $\texttt{C}$ 相当于 $-100$。

把所有数值相加，即为答案。

代码实现时，可以创建一个哈希表（或者数组），把字符作为 key，对应的数值作为 value，从而避免写一堆 $\texttt{if-else}$。

```py [sol-Python3]
# 单个罗马数字到整数的映射
ROMAN = {
    'I': 1,
    'V': 5,
    'X': 10,
    'L': 50,
    'C': 100,
    'D': 500,
    'M': 1000,
}

class Solution:
    def romanToInt(self, s: str) -> int:
        ans = 0
        for x, y in pairwise(s):  # 遍历 s 中的相邻字符
            x, y = ROMAN[x], ROMAN[y]
            # 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
            ans += x if x >= y else -x
        return ans + ROMAN[s[-1]]  # 加上最后一个罗马数字
```

```java [sol-Java]
class Solution {
    // 单个罗马数字到整数的映射
    private static final Map<Character, Integer> ROMAN = Map.of(
        'I', 1,
        'V', 5,
        'X', 10,
        'L', 50,
        'C', 100,
        'D', 500,
        'M', 1000
    );

    public int romanToInt(String S) {
        char[] s = S.toCharArray(); // 也可以下面用 charAt，从而保证空间复杂度是 O(1)
        int n = s.length;
        int ans = 0;
        for (int i = 0; i < n - 1; i++) { // 遍历 s
            int x = ROMAN.get(s[i]);
            int y = ROMAN.get(s[i + 1]);
            ans += x < y ? -x : x; // 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
        }
        return ans + ROMAN.get(s[n - 1]); // 加上最后一个罗马数字
    }
}
```

```cpp [sol-C++]
// 单个罗马数字到整数的映射
unordered_map<char, int> ROMAN = {
    {'I', 1},
    {'V', 5},
    {'X', 10},
    {'L', 50},
    {'C', 100},
    {'D', 500},
    {'M', 1000},
};

class Solution {
public:
    int romanToInt(string s) {
        int ans = 0;
        for (int i = 0; i + 1 < s.size(); i++) { // 遍历 s
            int x = ROMAN[s[i]], y = ROMAN[s[i + 1]];
            ans += x < y ? -x : x; // 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
        }
        return ans + ROMAN[s.back()]; // 加上最后一个罗马数字
    }
};
```

```c [sol-C]
int romanToInt(char* s) {
    int roman[128]; // 保证可以容纳所有 ASCII 字符
    // 单个罗马数字到整数的映射
    roman['I'] = 1;
    roman['V'] = 5;
    roman['X'] = 10;
    roman['L'] = 50;
    roman['C'] = 100;
    roman['D'] = 500;
    roman['M'] = 1000;

    int ans = 0, i = 1;
    for (; s[i]; i++) { // 遍历 s
        int x = roman[s[i - 1]], y = roman[s[i]];
        // 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
        ans += x < y ? -x : x;
    }
    return ans + roman[s[i - 1]]; // 加上最后一个罗马数字
}
```

```go [sol-Go]
// 单个罗马数字到整数的映射
var ROMAN = map[byte]int{
    'I': 1,
    'V': 5,
    'X': 10,
    'L': 50,
    'C': 100,
    'D': 500,
    'M': 1000,
}

func romanToInt(s string) int {
    n := len(s)
    ans := 0
    for i := range n - 1 { // 遍历 s
        x, y := ROMAN[s[i]], ROMAN[s[i+1]]
        // 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
        if x < y {
            ans -= x
        } else {
            ans += x
        }
    }
    return ans + ROMAN[s[n-1]] // 加上最后一个罗马数字
}
```

```js [sol-JavaScript]
// 单个罗马数字到整数的映射
const ROMAN = {
    'I': 1,
    'V': 5,
    'X': 10,
    'L': 50,
    'C': 100,
    'D': 500,
    'M': 1000,
};

var romanToInt = function(s) {
    let ans = 0;
    for (let i = 0; i < s.length - 1; i++) { // 遍历 s
        const x = ROMAN[s[i]], y = ROMAN[s[i + 1]];
        ans += x < y ? -x : x; // 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
    }
    return ans + ROMAN[s[s.length - 1]]; // 加上最后一个罗马数字
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn roman_to_int(s: String) -> i32 {
        // 单个罗马数字到整数的映射
        let roman = HashMap::from([
            (b'I', 1),
            (b'V', 5),
            (b'X', 10),
            (b'L', 50),
            (b'C', 100),
            (b'D', 500),
            (b'M', 1000),
        ]);

        let mut ans = 0;
        let s = s.as_bytes();
        for i in 0..s.len() - 1 { // 遍历 s
            let x = roman[&s[i]];
            let y = roman[&s[i + 1]];
            // 累加 x 或者 -x，这里 y 只是用来辅助判断 x 的正负
            if x < y {
                ans -= x;
            } else {
                ans += x;
            }
        }
        ans + roman[&s[s.len() - 1]] // 加上最后一个罗马数字
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
