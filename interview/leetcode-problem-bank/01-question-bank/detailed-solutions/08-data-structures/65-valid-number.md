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

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.5 表达式解析`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.5 表达式解析`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
