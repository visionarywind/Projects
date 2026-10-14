# 392. 判断子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/is-subsequence/
- 题目 slug：`is-subsequence`
- 来源专题：滑动窗口与双指针
- 来源分类路径：四、双序列双指针 / §4.2 判断子序列
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/is-subsequence/solutions/2813031/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-mz22/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法，附进阶问题解答（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/is-subsequence/solutions/2813031/jian-ji-xie-fa-pythonjavaccgojsrust-by-e-mz22/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavaccgojsrust-by-e-mz22`
- topic id：`2813031`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先，如果 $s$ 是空串，直接返回 $\texttt{true}$，因为空串是任何字符串的子序列。

如果 $s$ 不是空串，我们可以遍历 $t$，在遍历的过程中看能否匹配 $s$ 的每个字母。**按照子序列的定义**，在遍历 $t$ 的过程中，**把没匹配到的字母删除，剩下的就是匹配的字母**，即字符串 $s$，这就说明 $s$ 是 $t$ 的子序列。

如示例 1，$s=\texttt{abc},\ t=\texttt{ahbgdc}$，在遍历 $t$ 的过程中，我们可以依次匹配字母 $\texttt{a},\texttt{b},\texttt{c}$，说明 $s$ 是 $t$ 的子序列。

具体算法如下：

1. 初始化 $i=0$。
2. 遍历字符串 $t$ 中的字符 $c=t[j]$，看其是否与 $s[i]$ 匹配，如果匹配就把 $i$ 加一。加一后，如果 $i$ 等于 $s$ 的长度，说明 $s$ 的所有字符匹配完毕，$s$ 是 $t$ 的子序列，返回 $\texttt{true}$。
3. 如果遍历中没有返回，说明 $s$ 不是 $t$ 的子序列，返回 $\texttt{false}$。

```py [sol-Python3]
class Solution:
    def isSubsequence(self, s: str, t: str) -> bool:
        if not s:
            return True
        i = 0
        for c in t:
            if s[i] == c:
                i += 1
                if i == len(s):  # 所有字符匹配完毕
                    return True  # s 是 t 的子序列
        return False
```

```py [sol-Python3 iter]
class Solution:
    def isSubsequence(self, s: str, t: str) -> bool:
        it = iter(t)
        return all(c in it for c in s)  # in 会消耗迭代器
```

```java [sol-Java]
class Solution {
    public boolean isSubsequence(String s, String t) {
        if (s.isEmpty()) {
            return true;
        }
        int i = 0;
        for (char c : t.toCharArray()) {
            if (s.charAt(i) == c && ++i == s.length()) { // 所有字符匹配完毕
                return true; // s 是 t 的子序列
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isSubsequence(string s, string t) {
        if (s.empty()) {
            return true;
        }
        int i = 0;
        for (char c : t) {
            if (s[i] == c && ++i == s.length()) { // 所有字符匹配完毕
                return true; // s 是 t 的子序列
            }
        }
        return false;
    }
};
```

```c [sol-C]
bool isSubsequence(char* s, char* t) {
    if (!s[0]) { // s 是空串
        return true;
    }
    int i = 0;
    for (int j = 0; t[j]; j++) {
        if (s[i] == t[j] && !s[++i]) { // 所有字符匹配完毕
            return true; // s 是 t 的子序列
        }
    }
    return false;
}
```

```go [sol-Go]
func isSubsequence(s, t string) bool {
    if s == "" {
        return true
    }
    i := 0
    for _, c := range t {
        if s[i] == byte(c) {
            i++
            if i == len(s) { // 所有字符匹配完毕
                return true // s 是 t 的子序列
            }
        }
    }
    return false
}
```

```js [sol-JS]
var isSubsequence = function(s, t) {
    if (s.length === 0) {
        return true;
    }
    let i = 0;
    for (const c of t) {
        if (s[i] === c && ++i === s.length) { // 所有字符匹配完毕
            return true; // s 是 t 的子序列
        }
    }
    return false;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_subsequence(s: String, t: String) -> bool {
        if s.is_empty() {
            return true;
        }
        let s = s.as_bytes();
        let mut i = 0;
        for c in t.bytes() {
            if s[i] == c {
                i += 1;
                if i == s.len() { // 所有字符匹配完毕
                    return true; // s 是 t 的子序列
                }
            }
        }
        false
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $t$ 的长度。代码只有一个循环，至多循环 $\mathcal{O}(n)$ 次。
- 空间复杂度：$\mathcal{O}(1)$。

## 进阶问题

如果有 $k$ 个字符串 $s_1,s_2,\ldots,s_k$，每个字符串都需要遍历 $t$，都需要花费 $\mathcal{O}(n)$ 的时间，太慢了。

下面介绍一个更快的做法，在预处理后，每个字符串都只需要 $\mathcal{O}(m)$ 的时间，其中 $m$ 为 $s_i$ 的长度。

首先，上面的方法慢在哪？如果 $s$ 中的字母都集中在 $t$ 的末尾（或者相隔很远），那么我们会浪费大量时间在遍历 $t$ 上。

能否「一步到位」呢？比如 $s = \texttt{abc}$，$t = \texttt{ahbgdc}$，能否 $\mathcal{O}(1)$ 算出下一个字母 $\texttt{a}$ 的位置，下一个字母 $\texttt{b}$ 的位置，下一个字母 $\texttt{c}$ 的位置？

可以。定义 $\textit{nxt}[i][c]$ 表示 $t$ 中下标 $\ge i$ 的最近字母 $c$ 的下标。如果 $c$ 不存在，则规定 $\textit{nxt}[i][c]=n$，用 $n$ 表示没找到。

以 $s = \texttt{abc}$，$t = \texttt{ahbgdc}$ 为例：

1. 预处理字符串 $t$，得到 $\textit{nxt}$ 数组，做法见后文。
2. 初始化 $i=-1$。
3. 遍历字符串 $s$。
4. 找 $i$ 右边最近字母 $s[0]=\texttt{a}$ 的下标，直接看 $\textit{nxt}[i+1][\texttt{a}]$ 是多少，即 $0$，更新 $i=0$。
5. 找 $i$ 右边最近字母 $s[1]=\texttt{b}$ 的下标，直接看 $\textit{nxt}[i+1][\texttt{b}]$ 是多少，即 $2$，更新 $i=2$。
6. 找 $i$ 右边最近字母 $s[2]=\texttt{c}$ 的下标，直接看 $\textit{nxt}[i+1][\texttt{c}]$ 是多少，即 $5$，更新 $i=5$。
7. 字符串 $s$ 正常遍历完毕，说明 $s$ 是 $t$ 的子序列。如果遍历中途 $i=n$，则 $s$ 不是 $t$ 的子序列。

那么，如何预处理 $\textit{nxt}[i][c]$ 呢？这可以用**动态规划**：

- 如果 $t[i]=c$，根据定义，$\textit{nxt}[i][c]=i$。
- 如果 $t[i]\ne c$，问题变成 $t$ 中下标 $\ge i+1$ 的最近字母 $c$ 的下标，即 $\textit{nxt}[i][c]=\textit{nxt}[i+1][c]$。

初始值 $t[n][c] = n$。

```py [sol-Python3]
class Solution:
    def isSubsequence(self, s: str, t: str) -> bool:
        n = len(t)
        nxt = [[n] * 26 for _ in range(n + 1)]
        for i in range(n - 1, -1, -1):
            nxt[i][:] = nxt[i + 1]
            nxt[i][ord(t[i]) - ord('a')] = i

        # 这个写法无论 s 为空还是 t 为空，都能算出正确答案
        i = -1
        for c in s:
            i = nxt[i + 1][ord(c) - ord('a')]
            if i == n:  # c 不在 t 中，说明 s 不是 t 的子序列
                return False
        return True  # s 是 t 的子序列
```

```java [sol-Java]
class Solution {
    public boolean isSubsequence(String s, String t) {
        int n = t.length();
        int[][] nxt = new int[n + 1][26];
        Arrays.fill(nxt[n], n);
        for (int i = n - 1; i >= 0; i--) {
            nxt[i] = nxt[i + 1].clone();
            nxt[i][t.charAt(i) - 'a'] = i;
        }

        // 这个写法无论 s 为空还是 t 为空，都能算出正确答案
        int i = -1;
        for (char c : s.toCharArray()) {
            i = nxt[i + 1][c - 'a'];
            if (i == n) { // c 不在 t 中，说明 s 不是 t 的子序列
                return false;
            }
        }
        return true; // s 是 t 的子序列
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isSubsequence(string s, string t) {
        int n = t.size();
        vector<array<int, 26>> nxt(n + 1);
        ranges::fill(nxt[n], n);
        for (int i = n - 1; i >= 0; i--) {
            nxt[i] = nxt[i + 1];
            nxt[i][t[i] - 'a'] = i;
        }

        // 这个写法无论 s 为空还是 t 为空，都能算出正确答案
        int i = -1;
        for (char c : s) {
            i = nxt[i + 1][c - 'a'];
            if (i == n) { // c 不在 t 中，说明 s 不是 t 的子序列
                return false;
            }
        }
        return true; // s 是 t 的子序列
    }
};
```

```c [sol-C]
#define SIGMA 26

bool isSubsequence(char* s, char* t) {
    int n = strlen(t);
    int (*nxt)[SIGMA] = malloc((n + 1) * sizeof(int[SIGMA]));
    for (int j = 0; j < SIGMA; j++) {
        nxt[n][j] = n;
    }
    for (int i = n - 1; i >= 0; i--) {
        memcpy(nxt[i], nxt[i + 1], SIGMA * sizeof(int));
        nxt[i][t[i] - 'a'] = i;
    }

    // 这个写法无论 s 为空还是 t 为空，都能算出正确答案
    int i = -1;
    for (int j = 0; s[j] && i < n; j++) {
        i = nxt[i + 1][s[j] - 'a'];
    }
    free(nxt);
    return i < n;
}
```

```go [sol-Go]
func isSubsequence(s, t string) bool {
    n := len(t)
    nxt := make([][26]int, n+1)
    for j := range nxt[n] {
        nxt[n][j] = n
    }
    for i := n - 1; i >= 0; i-- {
        nxt[i] = nxt[i+1]
        nxt[i][t[i]-'a'] = i
    }

    // 这个写法无论 s 为空还是 t 为空，都能算出正确答案
    i := -1
    for _, b := range s {
        i = nxt[i+1][b-'a']
        if i == n { // b 不在 t 中，说明 s 不是 t 的子序列
            return false
        }
    }
    return true // s 是 t 的子序列
}
```

```js [sol-JavaScript]
var isSubsequence = function(s, t) {
    const ordA = 'a'.charCodeAt(0);
    const n = t.length;
    const nxt = Array(n + 1);
    nxt[n] = Array(26).fill(n);
    for (let i = n - 1; i >= 0; i--) {
        nxt[i] = nxt[i + 1].slice();
        nxt[i][t.charCodeAt(i) - ordA] = i;
    }

    // 这个写法无论 s 为空还是 t 为空，都能算出正确答案
    let i = -1;
    for (const b of s) {
        i = nxt[i + 1][b.charCodeAt(0) - ordA];
        if (i === n) { // b 不在 t 中，说明 s 不是 t 的子序列
            return false;
        }
    }
    return true; // s 是 t 的子序列
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_subsequence(s: String, t: String) -> bool {
        let n = t.len();
        let mut nxt = vec![[n; 26]; n + 1];
        for (i, c) in t.bytes().enumerate().rev() {
            nxt[i] = nxt[i + 1];
            nxt[i][(c - b'a') as usize] = i;
        }

        // 这个写法无论 s 为空还是 t 为空，都能算出正确答案
        let mut i = 0; // 为避免 usize 类型转来转去，这里初始化成 0
        for c in s.bytes() {
            i = nxt[i][(c - b'a') as usize];
            if i == n { // c 不在 t 中，说明 s 不是 t 的子序列
                return false;
            }
            i += 1;
        }
        true // s 是 t 的子序列
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n|\Sigma| + km)$，其中 $n$ 是 $t$ 的长度，$|\Sigma|=26$ 是字符集合的大小，$m$ 是 $s_i$ 的平均长度，$k\ge 10^9$（十亿）是 $s$ 的个数。
- 空间复杂度：$\mathcal{O}(n|\Sigma|)$。

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
