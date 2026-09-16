# 20. 有效的括号

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-parentheses/
- 题目 slug：`valid-parentheses`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.4 合法括号字符串（RBS）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/valid-parentheses/solutions/2809539/gua-hao-xiao-xiao-le-san-chong-li-yong-z-2xb3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[括号消消乐：三种利用栈的写法，附题单！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/valid-parentheses/solutions/2809539/gua-hao-xiao-xiao-le-san-chong-li-yong-z-2xb3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gua-hao-xiao-xiao-le-san-chong-li-yong-z-2xb3`
- topic id：`2809539`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意解读

#### 什么情况下是有效字符串？

可以从消消乐的角度理解，每次可以消除一对**相邻**的匹配括号，不断消除，如果可以把 $s$ 变成空字符串，则 $s$ 是有效字符串。

比如 $\texttt{()},\ \texttt{(())},\ \texttt{[()]},\ \texttt{[()]\{\}}$ 等等，都可以通过消除，把 $s$ 变成空字符串。例如

$$
\texttt{[()]} \rightarrow \texttt{[]} \rightarrow 空串
$$

#### 什么情况下是无效字符串？

1. 左括号没有对应的右括号。例如 $\texttt{(()}$，缺失了一个右括号。
2. 右括号没有对应的左括号。例如 $\texttt{())}$，缺失了一个左括号。
3. 括号类型不匹配。例如 $\texttt{[()\}}$，其中 $\texttt{[}$ 要和 $\texttt{\}}$ 组成一对括号，但是括号类型不同。

## 思路

本题是「**邻项消除**」问题（见文末的题单），这类问题都可以用**栈**解决。

以 $s=\texttt{\{[()]\}}$ 为例说明：

1. 创建一个空栈。
2. 从左到右遍历 $s$。
3. $s[0]=\texttt{\{}$，这是一个左括号，入栈。
4. $s[1]=\texttt{[}$，这是一个左括号，入栈。
5. $s[2]=\texttt{(}$，这是一个左括号，入栈。
6. $s[3]=\texttt{)}$，这是一个右括号，它必须和栈顶的 $\texttt{(}$ 组成一对（消除），弹出栈顶。
7. $s[4]=\texttt{]}$，这是一个右括号，它必须和栈顶的 $\texttt{[}$ 组成一对（消除），弹出栈顶。
8. $s[5]=\texttt{\}}$，这是一个右括号，它必须和栈顶的 $\texttt{\{}$ 组成一对（消除），弹出栈顶。
9. 遍历结束，由于栈为空，说明所有括号均已匹配完毕，返回 $\texttt{true}$。反之，如果在遍历的过程中，发现栈为空，或者括号类型不匹配的情况，返回 $\texttt{false}$。此外，如果遍历结束栈不为空，说明还有没匹配的左括号，返回 $\texttt{false}$。

#### 细节

1. 由于括号两两一对，所以 $s$ 的长度必须是偶数。如果 $s$ 的长度是奇数，可以直接返回 $\texttt{false}$。
2. 我们可以创建一个哈希表（或者数组），保存每个右括号对应的左括号，这样可以直接判断栈顶的左括号是否与右括号为同一类型，从而省去大量 if-else 判断。

## 写法一

```py [sol-Python3]
class Solution:
    def isValid(self, s: str) -> bool:
        if len(s) % 2:  # s 长度必须是偶数
            return False
        mp = {')': '(', ']': '[', '}': '{'}
        st = []
        for c in s:
            if c not in mp:  # c 是左括号
                st.append(c)  # 入栈
            elif not st or st.pop() != mp[c]:  # c 是右括号
                return False  # 没有左括号，或者左括号类型不对
        return not st  # 所有左括号必须匹配完毕
```

```java [sol-Java]
class Solution {
    public boolean isValid(String s) {
        if (s.length() % 2 != 0) { // s 长度必须是偶数
            return false;
        }
        Map<Character, Character> mp = new HashMap<>() {{
            put(')', '(');
            put(']', '[');
            put('}', '{');
        }};
        Deque<Character> st = new ArrayDeque<>();
        for (char c : s.toCharArray()) {
            if (!mp.containsKey(c)) { // c 是左括号
                st.push(c); // 入栈
            } else if (st.isEmpty() || st.pop() != mp.get(c)) { // c 是右括号
                return false; // 没有左括号，或者左括号类型不对
            }
        }
        return st.isEmpty(); // 所有左括号必须匹配完毕
    }
}
```

```cpp [sol-C++]
class Solution {
    unordered_map<char, char> mp = {{')', '('}, {']', '['}, {'}', '{'}};
public:
    bool isValid(string s) {
        if (s.length() % 2) { // s 长度必须是偶数
            return false;
        }
        stack<char> st;
        for (char c : s) {
            // mp.contains(c) 用来判断 c 是不是 mp 的一个 key
            if (!mp.contains(c)) { // c 是左括号
                st.push(c); // 入栈
            } else { // c 是右括号
                if (st.empty() || st.top() != mp[c]) {
                    return false; // 没有左括号，或者左括号类型不对
                }
                st.pop(); // 出栈
            }
        }
        return st.empty(); // 所有左括号必须匹配完毕
    }
};
```

```c [sol-C]
bool isValid(char* s) {
    char mp[128] = {};
    mp[')'] = '(';
    mp[']'] = '[';
    mp['}'] = '{';

    int top = 0; // 直接把 s 当作栈
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (mp[c] == 0) { // c 是左括号
            s[top++] = c; // 入栈
        } else if (top == 0 || s[--top] != mp[c]) { // c 是右括号
            return false; // 没有左括号，或者左括号类型不对
        }
    }
    return top == 0; // 所有左括号必须匹配完毕
}
```

```go [sol-Go]
func isValid(s string) bool {
    if len(s)%2 != 0 { // s 长度必须是偶数
        return false
    }
    mp := map[rune]rune{')': '(', ']': '[', '}': '{'}
    st := []rune{}
    for _, c := range s {
        if mp[c] == 0 { // c 是左括号
            st = append(st, c) // 入栈
        } else { // c 是右括号
            if len(st) == 0 || st[len(st)-1] != mp[c] {
                return false // 没有左括号，或者左括号类型不对
            }
            st = st[:len(st)-1] // 出栈
        }
    }
    return len(st) == 0 // 所有左括号必须匹配完毕
}
```

```js [sol-JavaScript]
var isValid = function(s) {
    if (s.length % 2) { // s 长度必须是偶数
        return false;
    }
    const mp = {')': '(', ']': '[', '}': '{'};
    const st = [];
    for (const c of s) {
        if (!mp.hasOwnProperty(c)) { // c 是左括号
            st.push(c); // 入栈
        } else if (st.length === 0 || st.pop() !== mp[c]) { // c 是右括号
            return false; // 没有左括号，或者左括号类型不对
        }
    }
    return st.length === 0; // 所有左括号必须匹配完毕
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn is_valid(s: String) -> bool {
        if s.len() % 2 != 0 { // s 长度必须是偶数
            return false;
        }
        let mp = [(b')', b'('), (b']', b'['), (b'}', b'{')].iter().cloned().collect::<HashMap<_, _>>();
        let mut st = vec![];
        for c in s.bytes() {
            if !mp.contains_key(&c) { // c 是左括号
                st.push(c); // 入栈
            } else if st.is_empty() || st.pop().unwrap() != *mp.get(&c).unwrap() { // c 是右括号
                return false; // 没有左括号，或者左括号类型不对
            }
        }
        st.is_empty() // 所有左括号必须匹配完毕
    }
}
```

## 写法二

也可以在哈希表/数组中保存每个左括号对应的右括号。在遍历到左括号时，把对应的右括号入栈。这样遍历到右括号时，只需看栈顶括号是否一样即可。

```py [sol-Python3]
class Solution:
    def isValid(self, s: str) -> bool:
        if len(s) % 2:  # s 长度必须是偶数
            return False
        mp = {'(': ')', '[': ']', '{': '}'}
        st = []
        for c in s:
            if c in mp:  # c 是左括号
                st.append(mp[c])  # 入栈对应的右括号
            elif not st or st.pop() != c:  # c 是右括号
                return False  # 没有左括号，或者左括号类型不对
        return not st  # 所有左括号必须匹配完毕
```

```java [sol-Java]
class Solution {
    public boolean isValid(String s) {
        if (s.length() % 2 != 0) { // s 长度必须是偶数
            return false;
        }
        Map<Character, Character> mp = new HashMap<>() {{
            put('(', ')');
            put('[', ']');
            put('{', '}');
        }};
        Deque<Character> st = new ArrayDeque<>();
        for (char c : s.toCharArray()) {
            if (mp.containsKey(c)) { // c 是左括号
                st.push(mp.get(c)); // 入栈
            } else if (st.isEmpty() || st.pop() != c) { // c 是右括号
                return false; // 没有左括号，或者左括号类型不对
            }
        }
        return st.isEmpty(); // 所有左括号必须匹配完毕
    }
}
```

```cpp [sol-C++]
class Solution {
    unordered_map<char, char> mp = {{'(', ')'}, {'[', ']'}, {'{', '}'}};
public:
    bool isValid(string s) {
        if (s.length() % 2) { // s 长度必须是偶数
            return false;
        }
        stack<char> st;
        for (char c : s) {
            if (mp.contains(c)) { // c 是左括号
                st.push(mp[c]); // 入栈对应的右括号
            } else { // c 是右括号
                if (st.empty() || st.top() != c) {
                    return false; // 没有左括号，或者左括号类型不对
                }
                st.pop(); // 出栈
            }
        }
        return st.empty(); // 所有左括号必须匹配完毕
    }
};
```

```c [sol-C]
bool isValid(char* s) {
    char mp[128] = {};
    mp['('] = ')';
    mp['['] = ']';
    mp['{'] = '}';

    int top = 0; // 直接把 s 当作栈
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (mp[c]) { // c 是左括号
            s[top++] = mp[c]; // 入栈
        } else if (top == 0 || s[--top] != c) { // c 是右括号
            return false; // 没有左括号，或者左括号类型不对
        }
    }
    return top == 0; // 所有左括号必须匹配完毕
}
```

```go [sol-Go]
func isValid(s string) bool {
    if len(s)%2 != 0 { // s 长度必须是偶数
        return false
    }
    mp := map[rune]rune{'(': ')', '[': ']', '{': '}'}
    st := []rune{}
    for _, c := range s {
        if v := mp[c]; v > 0 { // c 是左括号
            st = append(st, v) // 入栈
        } else { // c 是右括号
            if len(st) == 0 || st[len(st)-1] != c {
                return false // 没有左括号，或者左括号类型不对
            }
            st = st[:len(st)-1] // 出栈
        }
    }
    return len(st) == 0 // 所有左括号必须匹配完毕
}
```

```js [sol-JavaScript]
var isValid = function(s) {
    if (s.length % 2) { // s 长度必须是偶数
        return false;
    }
    const mp = {'(': ')', '[': ']', '{': '}'};
    const st = [];
    for (const c of s) {
        if (mp[c]) { // c 是左括号
            st.push(mp[c]); // 入栈
        } else if (st.length === 0 || st.pop() !== c) { // c 是右括号
            return false; // 没有左括号，或者左括号类型不对
        }
    }
    return st.length === 0; // 所有左括号必须匹配完毕
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn is_valid(s: String) -> bool {
        if s.len() % 2 != 0 { // s 长度必须是偶数
            return false;
        }
        let mp = [(b'(', b')'), (b'[', b']'), (b'{', b'}')].iter().cloned().collect::<HashMap<_, _>>();
        let mut st = vec![];
        for c in s.bytes() {
            if let Some(&v) = mp.get(&c) { // c 是左括号
                st.push(v); // 入栈对应的右括号
            } else if st.pop() != Some(c) { // c 是右括号
                return false; // 没有左括号，或者左括号类型不对
            }
        }
        st.is_empty() // 所有左括号必须匹配完毕
    }
}
```

## 写法三

用 if-else 代替 `mp`。 

```py [sol-Python3]
class Solution:
    def isValid(self, s: str) -> bool:
        if len(s) % 2:  # s 长度必须是偶数
            return False
        st = []
        for c in s:
            if c == '(':
                st.append(')')  # 入栈对应的右括号
            elif c == '[':
                st.append(']')
            elif c == '{':
                st.append('}')
            elif not st or st.pop() != c:  # c 是右括号
                return False  # 没有左括号，或者左括号类型不对
        return not st  # 所有左括号必须匹配完毕
```

```java [sol-Java]
class Solution {
    public boolean isValid(String s) {
        if (s.length() % 2 != 0) { // s 长度必须是偶数
            return false;
        }
        Deque<Character> st = new ArrayDeque<>();
        for (char c : s.toCharArray()) {
            if (c == '(') {
                st.push(')'); // 入栈对应的右括号
            } else if (c == '[') {
                st.push(']');
            } else if (c == '{') {
                st.push('}');
            } else if (st.isEmpty() || st.pop() != c) { // c 是右括号
                return false; // 没有左括号，或者左括号类型不对
            }
        }
        return st.isEmpty(); // 所有左括号必须匹配完毕
    }
}
```

```java [sol-Java]
class Solution {
    public boolean isValid(String S) {
        if (S.length() % 2 != 0) { // s 长度必须是偶数
            return false;
        }
        char[] s = S.toCharArray();
        int top = 0; // 直接把 s 当作栈
        for (char c : s) {
            if (c == '(') {
                s[top++] = ')'; // 入栈对应的右括号
            } else if (c == '[') {
                s[top++] = ']';
            } else if (c == '{') {
                s[top++] = '}';
            } else if (top == 0 || s[--top] != c) { // c 是右括号
                return false; // 没有左括号，或者左括号类型不对
            }
        }
        return top == 0; // 所有左括号必须匹配完毕
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isValid(string s) {
        if (s.length() % 2) { // s 长度必须是偶数
            return false;
        }
        stack<char> st;
        for (char c : s) {
            if (c == '(') {
                st.push(')'); // 入栈对应的右括号
            } else if (c == '[') {
                st.push(']');
            } else if (c == '{') {
                st.push('}');
            } else { // c 是右括号
                if (st.empty() || st.top() != c) {
                    return false; // 没有左括号，或者左括号类型不对
                }
                st.pop(); // 出栈
            }
        }
        return st.empty(); // 所有左括号必须匹配完毕
    }
};
```

```c [sol-C]
bool isValid(char* s) {
    int top = 0; // 直接把 s 当作栈
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (c == '(') {
            s[top++] = ')'; // 入栈对应的右括号
        } else if (c == '[') {
            s[top++] = ']';
        } else if (c == '{') {
            s[top++] = '}';
        } else if (top == 0 || s[--top] != c) { // c 是右括号
            return false; // 没有左括号，或者左括号类型不对
        }
    }
    return top == 0; // 所有左括号必须匹配完毕
}
```

```go [sol-Go]
func isValid(s string) bool {
    if len(s)%2 != 0 { // s 长度必须是偶数
        return false
    }
    st := []rune{}
    for _, c := range s {
        switch c {
        case '(':
            st = append(st, ')') // 入栈对应的右括号
        case '[':
            st = append(st, ']')
        case '{':
            st = append(st, '}')
        default: // c 是右括号
            if len(st) == 0 || st[len(st)-1] != c {
                return false // 没有左括号，或者左括号类型不对
            }
            st = st[:len(st)-1] // 出栈
        }
    }
    return len(st) == 0 // 所有左括号必须匹配完毕
}
```

```js [sol-JavaScript]
var isValid = function(s) {
    if (s.length % 2) { // s 长度必须是偶数
        return false;
    }
    const st = [];
    for (const c of s) {
        if (c === '(') {
            st.push(')'); // 入栈对应的右括号
        } else if (c === '[') {
            st.push(']');
        } else if (c === '{') {
            st.push('}');
        } else if (st.length === 0 || st.pop() !== c) { // c 是右括号
            return false; // 没有左括号，或者左括号类型不对
        }
    }
    return st.length === 0; // 所有左括号必须匹配完毕
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_valid(s: String) -> bool {
        if s.len() % 2 != 0 { // s 长度必须是偶数
            return false;
        }
        let mut st = vec![];
        for c in s.bytes() {
            match c {
                b'(' => st.push(b')'), // 入栈对应的右括号
                b'[' => st.push(b']'),
                b'{' => st.push(b'}'),
                _ => { // c 是右括号
                    if st.pop() != Some(c) {
                        return false; // 没有左括号，或者左括号类型不对
                    }
                }
            }
        }
        st.is_empty() // 所有左括号必须匹配完毕
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。如果能修改 $s$，那么直接把 $s$ 当作栈，可以做到 $\mathcal{O}(1)$ 额外空间。

## 相似题目

见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/) 中的【**§3.3 邻项消除**】。

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

本题来自 `三、栈 / §3.4 合法括号字符串（RBS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.4 合法括号字符串（RBS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
