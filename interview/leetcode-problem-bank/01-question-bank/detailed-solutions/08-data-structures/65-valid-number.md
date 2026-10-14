# 65. 有效数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-number/
- 题目 slug：`valid-number`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.5 表达式解析
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/valid-number/solutions/3825891/jing-que-zong-jie-ti-mu-gui-ze-jian-dan-iimho/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[精确总结题目规则，简单实现（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/valid-number/solutions/3825891/jing-que-zong-jie-ti-mu-gui-ze-jian-dan-iimho/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jing-que-zong-jie-ti-mu-gui-ze-jian-dan-iimho`
- topic id：`3825891`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题相当于实现半个 $\texttt{atof}$ 函数（把字符串转成浮点数）。说「半个」是因为本题只需判断字符串是否合法。

这可以分成两个大步骤：

1. 指数符号之前的内容是否合法。
2. 指数符号之后的内容是否合法。

## 指数符号之前

去掉开头的正负号（如果有）后，在指数符号之前的内容必须满足：

1. 至多有一个小数点。
2. 至少有一个数字。
3. 除了小数点和数字以外，不能有其余字符。

## 指数符号之后

去掉开头的正负号（如果有）后，在指数符号之后的内容必须满足：

1. 至少有一个数字。
2. 除了数字以外，不能有其余字符。

```py [sol-Python3]
class Solution:
    def isNumber(self, s: str) -> bool:
        n = len(s)

        # 正负号
        i = 0
        if s[i] in "+-":
            i += 1

        # 指数符号之前，至多一个小数点，其余必须全是数字
        has_dot = has_digit = False
        while i < n and s[i] not in "eE":
            if s[i] == '.':
                if has_dot:  # 不能有两个小数点
                    return False
                has_dot = True
            elif '0' <= s[i] <= '9':
                has_digit = True
            else:
                return False
            i += 1

        # 必须有数字
        if not has_digit:
            return False

        # 指数符号之后，必须是整数
        if i < n and s[i] in "eE":
            i += 1

            # 正负号
            if i < n and s[i] in "+-":
                i += 1

            # 必须有数字
            if i == n:
                return False

            # 剩下的必须全是数字
            while i < n and '0' <= s[i] <= '9':
                i += 1

        # 如果 i < n 说明有非法字符，不是有效数字
        return i == n
```

```java [sol-Java]
class Solution {
    public boolean isNumber(String S) {
        char[] s = S.toCharArray();
        int n = s.length;

        // 正负号
        int i = 0;
        if (s[i] == '+' || s[i] == '-') {
            i++;
        }

        // 指数符号之前，至多一个小数点，其余必须全是数字
        boolean hasDot = false;
        boolean hasDigit = false;
        for (; i < n && s[i] != 'e' && s[i] != 'E'; i++) {
            if (s[i] == '.') {
                if (hasDot) { // 不能有两个小数点
                    return false;
                }
                hasDot = true;
            } else if ('0' <= s[i] && s[i] <= '9') {
                hasDigit = true;
            } else {
                return false;
            }
        }

        // 必须有数字
        if (!hasDigit) {
            return false;
        }

        // 指数符号之后，必须是整数
        if (i < n && (s[i] == 'e' || s[i] == 'E')) {
            i++;

            // 正负号
            if (i < n && (s[i] == '+' || s[i] == '-')) {
                i++;
            }

            // 必须有数字
            if (i == n) {
                return false;
            }

            // 剩下的必须全是数字
            while (i < n && '0' <= s[i] && s[i] <= '9') {
                i++;
            }
        }

        // 如果 i < n 说明有非法字符，不是有效数字
        return i == n;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isNumber(string s) {
        int n = s.size();

        // 正负号
        int i = 0;
        if (s[i] == '+' || s[i] == '-') {
            i++;
        }

        // 指数符号之前，至多一个小数点，其余必须全是数字
        bool has_dot = false;
        bool has_digit = false;
        for (; i < n && s[i] != 'e' && s[i] != 'E'; i++) {
            if (s[i] == '.') {
                if (has_dot) { // 不能有两个小数点
                    return false;
                }
                has_dot = true;
            } else if ('0' <= s[i] && s[i] <= '9') {
                has_digit = true;
            } else {
                return false;
            }
        }

        // 必须有数字
        if (!has_digit) {
            return false;
        }

        // 指数符号之后，必须是整数
        if (i < n && (s[i] == 'e' || s[i] == 'E')) {
            i++;

            // 正负号
            if (i < n && (s[i] == '+' || s[i] == '-')) {
                i++;
            }

            // 必须有数字
            if (i == n) {
                return false;
            }

            // 剩下的必须全是数字
            while (i < n && '0' <= s[i] && s[i] <= '9') {
                i++;
            }
        }

        // 如果 i < n 说明有非法字符，不是有效数字
        return i == n;
    }
};
```

```c [sol-C]
bool isNumber(char* s) {
    // 正负号
    int i = 0;
    if (s[i] == '+' || s[i] == '-') {
        i++;
    }

    // 指数符号之前，至多一个小数点，其余必须全是数字
    bool has_dot = false;
    bool has_digit = false;
    for (; s[i] && s[i] != 'e' && s[i] != 'E'; i++) {
        if (s[i] == '.') {
            if (has_dot) { // 不能有两个小数点
                return false;
            }
            has_dot = true;
        } else if ('0' <= s[i] && s[i] <= '9') {
            has_digit = true;
        } else {
            return false;
        }
    }

    // 必须有数字
    if (!has_digit) {
        return false;
    }

    // 指数符号之后，必须是整数
    if (s[i] == 'e' || s[i] == 'E') {
        i++;

        // 正负号
        if (s[i] == '+' || s[i] == '-') {
            i++;
        }

        // 必须有数字
        if (s[i] == '\0') {
            return false;
        }

        // 剩下的必须全是数字
        while ('0' <= s[i] && s[i] <= '9') {
            i++;
        }
    }

    // 如果 i < strlen(s) 说明有非法字符，不是有效数字
    return s[i] == '\0';
}
```

```go [sol-Go]
func isNumber(s string) bool {
    n := len(s)

    // 正负号
    i := 0
    if s[i] == '+' || s[i] == '-' {
        i++
    }

    // 指数符号之前，至多一个小数点，其余必须全是数字
    hasDot := false
    hasDigit := false
    for ; i < n && s[i] != 'e' && s[i] != 'E'; i++ {
        if s[i] == '.' {
            if hasDot { // 不能有两个小数点
                return false
            }
            hasDot = true
        } else if '0' <= s[i] && s[i] <= '9' {
            hasDigit = true
        } else {
            return false
        }
    }

    // 必须有数字
    if !hasDigit {
        return false
    }

    // 指数符号之后，必须是整数
    if i < n && (s[i] == 'e' || s[i] == 'E') {
        i++

        // 正负号
        if i < n && (s[i] == '+' || s[i] == '-') {
            i++
        }

        // 必须有数字
        if i == n {
            return false
        }

        // 剩下的必须全是数字
        for i < n && '0' <= s[i] && s[i] <= '9' {
            i++
        }
    }

    // 如果 i < n 说明有非法字符，不是有效数字
    return i == n
}
```

```js [sol-JavaScript]
var isNumber = function(s) {
    const n = s.length;

    // 正负号
    let i = 0;
    if (s[i] === '+' || s[i] === '-') {
        i++;
    }

    // 指数符号之前，至多一个小数点，其余必须全是数字
    let hasDot = false;
    let hasDigit = false;
    for (; i < n && s[i] !== 'e' && s[i] !== 'E'; i++) {
        if (s[i] === '.') {
            if (hasDot) { // 不能有两个小数点
                return false;
            }
            hasDot = true;
        } else if (s[i] >= '0' && s[i] <= '9') {
            hasDigit = true;
        } else {
            return false;
        }
    }

    // 必须有数字
    if (!hasDigit) {
        return false;
    }

    // 指数符号之后，必须是整数
    if (i < n && (s[i] === 'e' || s[i] === 'E')) {
        i++;

        // 正负号
        if (i < n && (s[i] === '+' || s[i] === '-')) {
            i++;
        }

        // 必须有数字
        if (i === n) {
            return false;
        }

        // 剩下的必须全是数字
        while (i < n && s[i] >= '0' && s[i] <= '9') {
            i++;
        }
    }

    // 如果 i < n 说明有非法字符，不是有效数字
    return i === n;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_number(s: String) -> bool {
        let s = s.as_bytes();
        let n = s.len();

        // 正负号
        let mut i = 0;
        if i < n && (s[i] == b'+' || s[i] == b'-') {
            i += 1;
        }

        // 指数符号之前，至多一个小数点，其余必须全是数字
        let mut has_dot = false;
        let mut has_digit = false;
        while i < n && s[i] != b'e' && s[i] != b'E' {
            if s[i] == b'.' {
                if has_dot { // 不能有两个小数点
                    return false;
                }
                has_dot = true;
            } else if b'0' <= s[i] && s[i] <= b'9' {
                has_digit = true;
            } else {
                return false;
            }
            i += 1;
        }

        // 必须有数字
        if !has_digit {
            return false;
        }

        // 指数符号之后，必须是整数
        if i < n && (s[i] == b'e' || s[i] == b'E') {
            i += 1;

            // 正负号
            if i < n && (s[i] == b'+' || s[i] == b'-') {
                i += 1;
            }

            // 必须有数字
            if i == n {
                return false;
            }

            // 剩下的必须全是数字
            while i < n && b'0' <= s[i] && s[i] <= b'9' {
                i += 1;
            }
        }

        // 如果 i < n 说明有非法字符，不是有效数字
        i == n
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[8. 字符串转换整数 (atoi)](https://leetcode.cn/problems/string-to-integer-atoi/)

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

## 指数符号之前

去掉开头的正负号（如果有）后，在指数符号之前的内容必须满足：

1. 至多有一个小数点。
2. 至少有一个数字。
3. 除了小数点和数字以外，不能有其余字符。

## 指数符号之后

去掉开头的正负号（如果有）后，在指数符号之后的内容必须满足：

1. 至少有一个数字。
2. 除了数字以外，不能有其余字符。

```py [sol-Python3]
class Solution:
    def isNumber(self, s: str) -> bool:
        n = len(s)

        # 正负号
        i = 0
        if s[i] in "+-":
            i += 1

        # 指数符号之前，至多一个小数点，其余必须全是数字
        has_dot = has_digit = False
        while i < n and s[i] not in "eE":
            if s[i] == '.':
                if has_dot:  # 不能有两个小数点
                    return False
                has_dot = True
            elif '0' <= s[i] <= '9':
                has_digit = True
            else:
                return False
            i += 1

        # 必须有数字
        if not has_digit:
            return False

        # 指数符号之后，必须是整数
        if i < n and s[i] in "eE":
            i += 1

            # 正负号
            if i < n and s[i] in "+-":
                i += 1

            # 必须有数字
            if i == n:
                return False

            # 剩下的必须全是数字
            while i < n and '0' <= s[i] <= '9':
                i += 1

        # 如果 i < n 说明有非法字符，不是有效数字
        return i == n
```

```java [sol-Java]
class Solution {
    public boolean isNumber(String S) {
        char[] s = S.toCharArray();
        int n = s.length;

        // 正负号
        int i = 0;
        if (s[i] == '+' || s[i] == '-') {
            i++;
        }

        // 指数符号之前，至多一个小数点，其余必须全是数字
        boolean hasDot = false;
        boolean hasDigit = false;
        for (; i < n && s[i] != 'e' && s[i] != 'E'; i++) {
            if (s[i] == '.') {
                if (hasDot) { // 不能有两个小数点
                    return false;
                }
                hasDot = true;
            } else if ('0' <= s[i] && s[i] <= '9') {
                hasDigit = true;
            } else {
                return false;
            }
        }

        // 必须有数字
        if (!hasDigit) {
            return false;
        }

        // 指数符号之后，必须是整数
        if (i < n && (s[i] == 'e' || s[i] == 'E')) {
            i++;

            // 正负号
            if (i < n && (s[i] == '+' || s[i] == '-')) {
                i++;
            }

            // 必须有数字
            if (i == n) {
                return false;
            }

            // 剩下的必须全是数字
            while (i < n && '0' <= s[i] && s[i] <= '9') {
                i++;
            }
        }

        // 如果 i < n 说明有非法字符，不是有效数字
        return i == n;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isNumber(string s) {
        int n = s.size();

        // 正负号
        int i = 0;
        if (s[i] == '+' || s[i] == '-') {
            i++;
        }

        // 指数符号之前，至多一个小数点，其余必须全是数字
        bool has_dot = false;
        bool has_digit = false;
        for (; i < n && s[i] != 'e' && s[i] != 'E'; i++) {
            if (s[i] == '.') {
                if (has_dot) { // 不能有两个小数点
                    return false;
                }
                has_dot = true;
            } else if ('0' <= s[i] && s[i] <= '9') {
                has_digit = true;
            } else {
                return false;
            }
        }

        // 必须有数字
        if (!has_digit) {
            return false;
        }

        // 指数符号之后，必须是整数
        if (i < n && (s[i] == 'e' || s[i] == 'E')) {
            i++;

            // 正负号
            if (i < n && (s[i] == '+' || s[i] == '-')) {
                i++;
            }

            // 必须有数字
            if (i == n) {
                return false;
            }

            // 剩下的必须全是数字
            while (i < n && '0' <= s[i] && s[i] <= '9') {
                i++;
            }
        }

        // 如果 i < n 说明有非法字符，不是有效数字
        return i == n;
    }
};
```

```c [sol-C]
bool isNumber(char* s) {
    // 正负号
    int i = 0;
    if (s[i] == '+' || s[i] == '-') {
        i++;
    }

    // 指数符号之前，至多一个小数点，其余必须全是数字
    bool has_dot = false;
    bool has_digit = false;
    for (; s[i] && s[i] != 'e' && s[i] != 'E'; i++) {
        if (s[i] == '.') {
            if (has_dot) { // 不能有两个小数点
                return false;
            }
            has_dot = true;
        } else if ('0' <= s[i] && s[i] <= '9') {
            has_digit = true;
        } else {
            return false;
        }
    }

    // 必须有数字
    if (!has_digit) {
        return false;
    }

    // 指数符号之后，必须是整数
    if (s[i] == 'e' || s[i] == 'E') {
        i++;

        // 正负号
        if (s[i] == '+' || s[i] == '-') {
            i++;
        }

        // 必须有数字
        if (s[i] == '\0') {
            return false;
        }

        // 剩下的必须全是数字
        while ('0' <= s[i] && s[i] <= '9') {
            i++;
        }
    }

    // 如果 i < strlen(s) 说明有非法字符，不是有效数字
    return s[i] == '\0';
}
```

```go [sol-Go]
func isNumber(s string) bool {
    n := len(s)

    // 正负号
    i := 0
    if s[i] == '+' || s[i] == '-' {
        i++
    }

    // 指数符号之前，至多一个小数点，其余必须全是数字
    hasDot := false
    hasDigit := false
    for ; i < n && s[i] != 'e' && s[i] != 'E'; i++ {
        if s[i] == '.' {
            if hasDot { // 不能有两个小数点
                return false
            }
            hasDot = true
        } else if '0' <= s[i] && s[i] <= '9' {
            hasDigit = true
        } else {
            return false
        }
    }

    // 必须有数字
    if !hasDigit {
        return false
    }

    // 指数符号之后，必须是整数
    if i < n && (s[i] == 'e' || s[i] == 'E') {
        i++

        // 正负号
        if i < n && (s[i] == '+' || s[i] == '-') {
            i++
        }

        // 必须有数字
        if i == n {
            return false
        }

        // 剩下的必须全是数字
        for i < n && '0' <= s[i] && s[i] <= '9' {
            i++
        }
    }

    // 如果 i < n 说明有非法字符，不是有效数字
    return i == n
}
```

```js [sol-JavaScript]
var isNumber = function(s) {
    const n = s.length;

    // 正负号
    let i = 0;
    if (s[i] === '+' || s[i] === '-') {
        i++;
    }

    // 指数符号之前，至多一个小数点，其余必须全是数字
    let hasDot = false;
    let hasDigit = false;
    for (; i < n && s[i] !== 'e' && s[i] !== 'E'; i++) {
        if (s[i] === '.') {
            if (hasDot) { // 不能有两个小数点
                return false;
            }
            hasDot = true;
        } else if (s[i] >= '0' && s[i] <= '9') {
            hasDigit = true;
        } else {
            return false;
        }
    }

    // 必须有数字
    if (!hasDigit) {
        return false;
    }

    // 指数符号之后，必须是整数
    if (i < n && (s[i] === 'e' || s[i] === 'E')) {
        i++;

        // 正负号
        if (i < n && (s[i] === '+' || s[i] === '-')) {
            i++;
        }

        // 必须有数字
        if (i === n) {
            return false;
        }

        // 剩下的必须全是数字
        while (i < n && s[i] >= '0' && s[i] <= '9') {
            i++;
        }
    }

    // 如果 i < n 说明有非法字符，不是有效数字
    return i === n;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_number(s: String) -> bool {
        let s = s.as_bytes();
        let n = s.len();

        // 正负号
        let mut i = 0;
        if i < n && (s[i] == b'+' || s[i] == b'-') {
            i += 1;
        }

        // 指数符号之前，至多一个小数点，其余必须全是数字
        let mut has_dot = false;
        let mut has_digit = false;
        while i < n && s[i] != b'e' && s[i] != b'E' {
            if s[i] == b'.' {
                if has_dot { // 不能有两个小数点
                    return false;
                }
                has_dot = true;
            } else if b'0' <= s[i] && s[i] <= b'9' {
                has_digit = true;
            } else {
                return false;
            }
            i += 1;
        }

        // 必须有数字
        if !has_digit {
            return false;
        }

        // 指数符号之后，必须是整数
        if i < n && (s[i] == b'e' || s[i] == b'E') {
            i += 1;

            // 正负号
            if i < n && (s[i] == b'+' || s[i] == b'-') {
                i += 1;
            }

            // 必须有数字
            if i == n {
                return false;
            }

            // 剩下的必须全是数字
            while i < n && b'0' <= s[i] && s[i] <= b'9' {
                i += 1;
            }
        }

        // 如果 i < n 说明有非法字符，不是有效数字
        i == n
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[8. 字符串转换整数 (atoi)](https://leetcode.cn/problems/string-to-integer-atoi/)

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
