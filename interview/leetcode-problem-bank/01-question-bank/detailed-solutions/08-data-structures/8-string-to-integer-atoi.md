# 8. 字符串转换整数 (atoi)

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/string-to-integer-atoi/
- 题目 slug：`string-to-integer-atoi`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.5 表达式解析
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/string-to-integer-atoi/solutions/3825708/zhi-shi-yong-32-wei-zheng-shu-de-xie-fa-fjmbn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[只使用 32 位整数的写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/string-to-integer-atoi/solutions/3825708/zhi-shi-yong-32-wei-zheng-shu-de-xie-fa-fjmbn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhi-shi-yong-32-wei-zheng-shu-de-xie-fa-fjmbn`
- topic id：`3825708`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、跳过前导空格

从 $i=0$ 开始循环，如果 $i<n$ 且 $s[i]$ 是空格，那么把 $i$ 加一，表示跳过前导空格。

## 二、处理正负号

如果 $i<n$ 且 $s[i]$ 是正号或者负号，那么记录 $\textit{sign}=1$ 或者 $\textit{sign}=-1$，然后把 $i$ 加一。

## 三、处理数字

比如把字符串 $\texttt{123}$ 转换成数字 $123$，我们可以：

1. 初始化 $\textit{num} =0$。
2. 读取字符 $\texttt{1}$，把 $\textit{num}$ 更新成 $\textit{num}\cdot 10 + 1 = 1$。
3. 读取字符 $\texttt{2}$，把 $\textit{num}$ 更新成 $\textit{num}\cdot 10 + 2 = 10+2 = 12$。
4. 读取字符 $\texttt{3}$，把 $\textit{num}$ 更新成 $\textit{num}\cdot 10 + 3 = 120 +3 = 123$。

## 四、处理截断

设 $\textit{mx} = 2^{31}-1$，$d$ 是 $s[i]$ 对应的数字。

在处理数字的过程中，如果发现

$$
\textit{num}\cdot 10 + d > \textit{mx}
$$

则可以提前返回答案。如果 $\textit{sign}=1$，返回 $2^{31}-1$，否则返回 $-2^{31}$。

> 注意：如果 $\textit{sign}=-1$，且 $\textit{num}$ 更新后是 $2^{31}$，虽然没有发生截断，但无论 $s$ 剩余字符是什么，最终答案都是 $-2^{31}$，所以也可以提前返回。

为避免乘法和加法溢出，把不等式拆分成两个：

1. $\textit{num}\cdot 10 > \textit{mx}$，这等价于 $\textit{num} > \left\lfloor\dfrac{\textit{mx}}{10}\right\rfloor$。
2. $\textit{num}\cdot 10 + d > \textit{mx}$，这等价于 $\textit{num} \cdot 10 > \textit{mx} - d$。

如果正常循环结束，那么返回 $\textit{sign}\cdot \textit{num}$。

```py [sol-Python3]
class Solution:
    def myAtoi(self, s: str) -> int:
        n = len(s)

        # 跳过前导空格
        i = 0
        while i < n and s[i] == ' ':
            i += 1

        # 处理正负号
        sign = 1
        if i < n and s[i] in "+-":
            sign = 1 if s[i] == '+' else -1
            i += 1

        # 处理数字
        MX = (1 << 31) - 1
        num = 0
        while i < n and '0' <= s[i] <= '9':
            num = num * 10 + int(s[i])
            if num > MX:  # 最终答案已确定，提前返回
                return MX if sign > 0 else -(1 << 31)
            i += 1

        return sign * num
```

```java [sol-Java]
class Solution {
    public int myAtoi(String s) {
        int n = s.length();

        // 跳过前导空格
        int i = 0;
        while (i < n && s.charAt(i) == ' ') {
            i++;
        }

        // 处理正负号
        int sign = 1;
        if (i < n && (s.charAt(i) == '+' || s.charAt(i) == '-')) {
            sign = s.charAt(i) == '+' ? 1 : -1;
            i++;
        }

        // 处理数字
        int num = 0;
        for (; i < n && Character.isDigit(s.charAt(i)); i++) {
            int d = s.charAt(i) - '0';
            // 如果 num*10+d > MAX_VALUE，则最终答案已确定，提前返回
            if (num > Integer.MAX_VALUE / 10 || num * 10 > Integer.MAX_VALUE - d) { // 避免溢出
                return sign > 0 ? Integer.MAX_VALUE : Integer.MIN_VALUE;
            }
            num = num * 10 + d;
        }

        return sign * num;
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public int myAtoi(String S) {
        char[] s = S.toCharArray();
        int n = s.length;

        // 跳过前导空格
        int i = 0;
        while (i < n && s[i] == ' ') {
            i++;
        }

        // 处理正负号
        int sign = 1;
        if (i < n && (s[i] == '+' || s[i] == '-')) {
            sign = s[i] == '+' ? 1 : -1;
            i++;
        }

        // 处理数字
        int num = 0;
        for (; i < n && '0' <= s[i] && s[i] <= '9'; i++) {
            int d = s[i] - '0';
            // 如果 num*10+d > MAX_VALUE，则最终答案已确定，提前返回
            if (num > Integer.MAX_VALUE / 10 || num * 10 > Integer.MAX_VALUE - d) { // 避免溢出
                return sign > 0 ? Integer.MAX_VALUE : Integer.MIN_VALUE;
            }
            num = num * 10 + d;
        }

        return sign * num;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int myAtoi(string s) {
        int n = s.size();

        // 跳过前导空格
        int i = 0;
        while (i < n && s[i] == ' ') {
            i++;
        }

        // 处理正负号
        int sign = 1;
        if (i < n && (s[i] == '+' || s[i] == '-')) {
            sign = s[i] == '+' ? 1 : -1;
            i++;
        }

        // 处理数字
        int num = 0;
        for (; i < n && '0' <= s[i] && s[i] <= '9'; i++) {
            int d = s[i] - '0';
            // 如果 num*10+d > INT_MAX，则最终答案已确定，提前返回
            if (num > INT_MAX / 10 || num * 10 > INT_MAX - d) { // 避免溢出
                return sign > 0 ? INT_MAX : INT_MIN;
            }
            num = num * 10 + d;
        }

        return sign * num;
    }
};
```

```c [sol-C]
int myAtoi(char* s){
    // 跳过前导空格
    int i = 0;
    while (s[i] == ' ') {
        i++;
    }

    // 处理正负号
    int sign = 1;
    if (s[i] == '+' || s[i] == '-') {
        sign = s[i] == '+' ? 1 : -1;
        i++;
    }

    // 处理数字
    int num = 0;
    for (; '0' <= s[i] && s[i] <= '9'; i++) {
        int d = s[i] - '0';
        // 如果 num*10+d > INT_MAX，则最终答案已确定，提前返回
        if (num > INT_MAX / 10 || num * 10 > INT_MAX - d) {
            return sign > 0 ? INT_MAX : INT_MIN;
        }
        num = num * 10 + d;
    }

    return sign * num;
}
```

```go [sol-Go]
func myAtoi(s string) int {
    n := len(s)

    // 跳过前导空格
    i := 0
    for i < n && s[i] == ' ' {
        i++
    }

    // 处理正负号
    sign := 1
    if i < n && (s[i] == '+' || s[i] == '-') {
        if s[i] == '-' {
            sign = -1
        }
        i++
    }

    // 处理数字
    num := 0
    for ; i < n && '0' <= s[i] && s[i] <= '9'; i++ {
        d := int(s[i] - '0')
        // 如果 num*10+d > MaxInt32，则最终答案已确定，提前返回
        if num > math.MaxInt32/10 || num*10 > math.MaxInt32-d { // 避免溢出
            if sign > 0 {
                return math.MaxInt32
            }
            return math.MinInt32
        }
        num = num*10 + d
    }

    return sign * num
}
```

```js [sol-JavaScript]
var myAtoi = function(s) {
    const n = s.length;

    // 跳过前导空格
    let i = 0;
    while (i < n && s[i] === ' ') {
        i++;
    }

    // 处理正负号
    let sign = 1;
    if (i < n && (s[i] === '+' || s[i] === '-')) {
        sign = s[i] === '+' ? 1 : -1;
        i++;
    }

    // 处理数字
    const MX = 2 ** 31 - 1;
    let num = 0;
    for (; i < n && '0' <= s[i] && s[i] <= '9'; i++) {
        num = num * 10 + s[i].charCodeAt(0) - '0'.charCodeAt(0);
        if (num > MX) { // 最终答案已确定，提前返回
            return sign > 0 ? MX : -MX - 1;
        }
    }

    return sign * num;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn my_atoi(s: String) -> i32 {
        let s = s.as_bytes();
        let n = s.len();

        // 跳过前导空格
        let mut i = 0;
        while i < n && s[i] == b' ' {
            i += 1;
        }

        // 处理正负号
        let mut sign = 1;
        if i < n && (s[i] == b'+' || s[i] == b'-') {
            if s[i] == b'-' {
                sign = -1;
            }
            i += 1;
        }

        // 处理数字
        let mut num = 0;
        while i < n && (b'0' <= s[i] && s[i] <= b'9') {
            let d = (s[i] - b'0') as i32;
            // 如果 num*10+d > i32::MAX，则最终答案已确定，提前返回
            if num > i32::MAX / 10 || num * 10 > i32::MAX - d { // 避免溢出
                return if sign > 0 { i32::MAX } else { i32::MIN };
            }
            num = num * 10 + d;
            i += 1;
        }

        sign * num
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [65. 有效数字](https://leetcode.cn/problems/valid-number/)

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

## 一、跳过前导空格

从 $i=0$ 开始循环，如果 $i<n$ 且 $s[i]$ 是空格，那么把 $i$ 加一，表示跳过前导空格。

## 二、处理正负号

如果 $i<n$ 且 $s[i]$ 是正号或者负号，那么记录 $\textit{sign}=1$ 或者 $\textit{sign}=-1$，然后把 $i$ 加一。

## 三、处理数字

比如把字符串 $\texttt{123}$ 转换成数字 $123$，我们可以：

1. 初始化 $\textit{num} =0$。
2. 读取字符 $\texttt{1}$，把 $\textit{num}$ 更新成 $\textit{num}\cdot 10 + 1 = 1$。
3. 读取字符 $\texttt{2}$，把 $\textit{num}$ 更新成 $\textit{num}\cdot 10 + 2 = 10+2 = 12$。
4. 读取字符 $\texttt{3}$，把 $\textit{num}$ 更新成 $\textit{num}\cdot 10 + 3 = 120 +3 = 123$。

## 四、处理截断

设 $\textit{mx} = 2^{31}-1$，$d$ 是 $s[i]$ 对应的数字。

在处理数字的过程中，如果发现

$$
\textit{num}\cdot 10 + d > \textit{mx}
$$

则可以提前返回答案。如果 $\textit{sign}=1$，返回 $2^{31}-1$，否则返回 $-2^{31}$。

> 注意：如果 $\textit{sign}=-1$，且 $\textit{num}$ 更新后是 $2^{31}$，虽然没有发生截断，但无论 $s$ 剩余字符是什么，最终答案都是 $-2^{31}$，所以也可以提前返回。

为避免乘法和加法溢出，把不等式拆分成两个：

1. $\textit{num}\cdot 10 > \textit{mx}$，这等价于 $\textit{num} > \left\lfloor\dfrac{\textit{mx}}{10}\right\rfloor$。
2. $\textit{num}\cdot 10 + d > \textit{mx}$，这等价于 $\textit{num} \cdot 10 > \textit{mx} - d$。

如果正常循环结束，那么返回 $\textit{sign}\cdot \textit{num}$。

```py [sol-Python3]
class Solution:
    def myAtoi(self, s: str) -> int:
        n = len(s)

        # 跳过前导空格
        i = 0
        while i < n and s[i] == ' ':
            i += 1

        # 处理正负号
        sign = 1
        if i < n and s[i] in "+-":
            sign = 1 if s[i] == '+' else -1
            i += 1

        # 处理数字
        MX = (1 << 31) - 1
        num = 0
        while i < n and '0' <= s[i] <= '9':
            num = num * 10 + int(s[i])
            if num > MX:  # 最终答案已确定，提前返回
                return MX if sign > 0 else -(1 << 31)
            i += 1

        return sign * num
```

```java [sol-Java]
class Solution {
    public int myAtoi(String s) {
        int n = s.length();

        // 跳过前导空格
        int i = 0;
        while (i < n && s.charAt(i) == ' ') {
            i++;
        }

        // 处理正负号
        int sign = 1;
        if (i < n && (s.charAt(i) == '+' || s.charAt(i) == '-')) {
            sign = s.charAt(i) == '+' ? 1 : -1;
            i++;
        }

        // 处理数字
        int num = 0;
        for (; i < n && Character.isDigit(s.charAt(i)); i++) {
            int d = s.charAt(i) - '0';
            // 如果 num*10+d > MAX_VALUE，则最终答案已确定，提前返回
            if (num > Integer.MAX_VALUE / 10 || num * 10 > Integer.MAX_VALUE - d) { // 避免溢出
                return sign > 0 ? Integer.MAX_VALUE : Integer.MIN_VALUE;
            }
            num = num * 10 + d;
        }

        return sign * num;
    }
}
```

```java [sol-Java 写法二]
class Solution {
    public int myAtoi(String S) {
        char[] s = S.toCharArray();
        int n = s.length;

        // 跳过前导空格
        int i = 0;
        while (i < n && s[i] == ' ') {
            i++;
        }

        // 处理正负号
        int sign = 1;
        if (i < n && (s[i] == '+' || s[i] == '-')) {
            sign = s[i] == '+' ? 1 : -1;
            i++;
        }

        // 处理数字
        int num = 0;
        for (; i < n && '0' <= s[i] && s[i] <= '9'; i++) {
            int d = s[i] - '0';
            // 如果 num*10+d > MAX_VALUE，则最终答案已确定，提前返回
            if (num > Integer.MAX_VALUE / 10 || num * 10 > Integer.MAX_VALUE - d) { // 避免溢出
                return sign > 0 ? Integer.MAX_VALUE : Integer.MIN_VALUE;
            }
            num = num * 10 + d;
        }

        return sign * num;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int myAtoi(string s) {
        int n = s.size();

        // 跳过前导空格
        int i = 0;
        while (i < n && s[i] == ' ') {
            i++;
        }

        // 处理正负号
        int sign = 1;
        if (i < n && (s[i] == '+' || s[i] == '-')) {
            sign = s[i] == '+' ? 1 : -1;
            i++;
        }

        // 处理数字
        int num = 0;
        for (; i < n && '0' <= s[i] && s[i] <= '9'; i++) {
            int d = s[i] - '0';
            // 如果 num*10+d > INT_MAX，则最终答案已确定，提前返回
            if (num > INT_MAX / 10 || num * 10 > INT_MAX - d) { // 避免溢出
                return sign > 0 ? INT_MAX : INT_MIN;
            }
            num = num * 10 + d;
        }

        return sign * num;
    }
};
```

```c [sol-C]
int myAtoi(char* s){
    // 跳过前导空格
    int i = 0;
    while (s[i] == ' ') {
        i++;
    }

    // 处理正负号
    int sign = 1;
    if (s[i] == '+' || s[i] == '-') {
        sign = s[i] == '+' ? 1 : -1;
        i++;
    }

    // 处理数字
    int num = 0;
    for (; '0' <= s[i] && s[i] <= '9'; i++) {
        int d = s[i] - '0';
        // 如果 num*10+d > INT_MAX，则最终答案已确定，提前返回
        if (num > INT_MAX / 10 || num * 10 > INT_MAX - d) {
            return sign > 0 ? INT_MAX : INT_MIN;
        }
        num = num * 10 + d;
    }

    return sign * num;
}
```

```go [sol-Go]
func myAtoi(s string) int {
    n := len(s)

    // 跳过前导空格
    i := 0
    for i < n && s[i] == ' ' {
        i++
    }

    // 处理正负号
    sign := 1
    if i < n && (s[i] == '+' || s[i] == '-') {
        if s[i] == '-' {
            sign = -1
        }
        i++
    }

    // 处理数字
    num := 0
    for ; i < n && '0' <= s[i] && s[i] <= '9'; i++ {
        d := int(s[i] - '0')
        // 如果 num*10+d > MaxInt32，则最终答案已确定，提前返回
        if num > math.MaxInt32/10 || num*10 > math.MaxInt32-d { // 避免溢出
            if sign > 0 {
                return math.MaxInt32
            }
            return math.MinInt32
        }
        num = num*10 + d
    }

    return sign * num
}
```

```js [sol-JavaScript]
var myAtoi = function(s) {
    const n = s.length;

    // 跳过前导空格
    let i = 0;
    while (i < n && s[i] === ' ') {
        i++;
    }

    // 处理正负号
    let sign = 1;
    if (i < n && (s[i] === '+' || s[i] === '-')) {
        sign = s[i] === '+' ? 1 : -1;
        i++;
    }

    // 处理数字
    const MX = 2 ** 31 - 1;
    let num = 0;
    for (; i < n && '0' <= s[i] && s[i] <= '9'; i++) {
        num = num * 10 + s[i].charCodeAt(0) - '0'.charCodeAt(0);
        if (num > MX) { // 最终答案已确定，提前返回
            return sign > 0 ? MX : -MX - 1;
        }
    }

    return sign * num;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn my_atoi(s: String) -> i32 {
        let s = s.as_bytes();
        let n = s.len();

        // 跳过前导空格
        let mut i = 0;
        while i < n && s[i] == b' ' {
            i += 1;
        }

        // 处理正负号
        let mut sign = 1;
        if i < n && (s[i] == b'+' || s[i] == b'-') {
            if s[i] == b'-' {
                sign = -1;
            }
            i += 1;
        }

        // 处理数字
        let mut num = 0;
        while i < n && (b'0' <= s[i] && s[i] <= b'9') {
            let d = (s[i] - b'0') as i32;
            // 如果 num*10+d > i32::MAX，则最终答案已确定，提前返回
            if num > i32::MAX / 10 || num * 10 > i32::MAX - d { // 避免溢出
                return if sign > 0 { i32::MAX } else { i32::MIN };
            }
            num = num * 10 + d;
            i += 1;
        }

        sign * num
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [65. 有效数字](https://leetcode.cn/problems/valid-number/)

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
