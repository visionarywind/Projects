# 1657. 确定两个字符串是否接近

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/determine-if-two-strings-are-close/
- 题目 slug：`determine-if-two-strings-are-close`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.3 等价转化
- 难度分：1530
- 外部题解来源：https://leetcode.cn/problems/determine-if-two-strings-are-close/solutions/2547579/li-jie-cao-zuo-ben-zhi-jian-ji-xie-fa-py-b18i/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[理解操作本质+简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/determine-if-two-strings-are-close/solutions/2547579/li-jie-cao-zuo-ben-zhi-jian-ji-xie-fa-py-b18i/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-jie-cao-zuo-ben-zhi-jian-ji-xie-fa-py-b18i`
- topic id：`2547579`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

为方便描述，下文把 $\textit{word}_1$ 记作 $s$，把 $\textit{word}_2$ 记作 $t$。

### 操作 1 的本质：字符可以任意排列

把 $s$ 看成是一叠扑克牌，我们可以随意洗牌。

统计 $s$ 和 $t$ 中字符的出现次数，如果字符及其出现次数都一样，只用操作 1 就可以把 $s$ 变成 $t$（把一叠扑克洗成另一叠扑克）。

如果字符一样，但对应的出现次数不一样呢？这就需要用到操作 2 了。

### 操作 2 的本质：出现次数是可以交换的

以示例 3 为例。统计 $s=\texttt{cabbba}$ 的字符出现次数：

- $\texttt{a}$ 出现 $2$ 次。
- $\texttt{b}$ 出现 $3$ 次。
- $\texttt{c}$ 出现 $1$ 次。

我们可以把 $\texttt{a}$ 都变成 $\texttt{b}$，同时把 $\texttt{b}$ 都变成 $\texttt{a}$。

这相当于交换 $\texttt{a}$ 和 $\texttt{b}$ 的出现次数，得到：

- $\texttt{a}$ 出现 $3$ 次。
- $\texttt{b}$ 出现 $2$ 次。
- $\texttt{c}$ 出现 $1$ 次。

然后交换 $\texttt{a}$ 和 $\texttt{c}$ 的出现次数，得到：

- $\texttt{a}$ 出现 $1$ 次。
- $\texttt{b}$ 出现 $2$ 次。
- $\texttt{c}$ 出现 $3$ 次。

这便是字符串 $t=\texttt{abbccc}$ 的字符出现次数。

所以「出现次数」也像操作 1 那样，是可以任意排列的。

如果 $s$ 和 $t$ 的字符一样，并且字符出现次数的集合是相同的（比如上面这个例子都是集合 $\{1,2,3\}$），那么可以结合操作 1 和操作 2，把 $s$ 变成 $t$。

### 算法

1. 判断 $s$ 和 $t$ 的长度是否一样，如果不一样直接返回 `false`。
2. 判断 $s$ 和 $t$ 的字符集合是否一样，如果不一样直接返回 `false`。例如 $s$ 中有字符 $\texttt{abc}$，$t$ 中有字符 $\texttt{def}$，我们无论如何都不能把 $s$ 变成 $t$。
3. 判断 $s$ 的字符出现次数的集合，是否等于 $t$ 的字符出现次数的集合，等于返回 `true`，不等于返回 `false`。注意集合可以有相同元素，比如 $\texttt{aabbbccc}$ 对应的集合就是 $\{2,3,3\}$。

### 实现细节

判断字符集合是否一样，可以用位运算实现，也就是用二进制数（从低到高）第 $i$ 位来存储是否有第 $i$ 个小写英文字母，这样只需要判断两个二进制数是否一样即可。具体请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

判断字符出现次数的集合是否一样，可以用两个长为 $26$ 的数组统计 $s$ 和 $t$ 中每个字母的出现次数，分别记作 $\textit{sCnt}$ 和 $\textit{tCnt}$。如果这两个数组排序后是一样的，就说明 $s$ 的字符出现次数的集合，等于 $t$ 的字符出现次数的集合。

> 注：Python 的 `set` 和 `Counter` 写起来更简单，故没有用位运算实现。

```py [sol-Python3 写法一]
class Solution:
    def closeStrings(self, s: str, t: str) -> bool:
        return len(s) == len(t) and \
               set(s) == set(t) and \
               Counter(Counter(s).values()) == Counter(Counter(t).values())
```

```py [sol-Python3 写法二]
class Solution:
    def closeStrings(self, s: str, t: str) -> bool:
        if len(s) != len(t):
            return False
        cs, ct = Counter(s), Counter(t)
        return cs.keys() == ct.keys() and Counter(cs.values()) == Counter(ct.values())
```

```java [sol-Java]
import static java.nio.charset.StandardCharsets.ISO_8859_1;

class Solution {
    public boolean closeStrings(String s, String t) {
        if (s.length() != t.length()) {
            return false;
        }

        int sMask = 0;
        int tMask = 0;
        int[] sCnt = new int[26];
        int[] tCnt = new int[26];
        for (byte c : s.getBytes(ISO_8859_1)) { // 比 toCharArray 更快
            sMask |= 1 << (c - 'a'); // 记录 s 中有字符 c
            sCnt[c - 'a']++;
        }
        for (byte c : t.getBytes(ISO_8859_1)) {
            tMask |= 1 << (c - 'a'); // 记录 t 中有字符 c
            tCnt[c - 'a']++;
        }

        Arrays.sort(sCnt);
        Arrays.sort(tCnt);
        return sMask == tMask && Arrays.equals(sCnt, tCnt);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool closeStrings(string s, string t) {
        if (s.length() != t.length()) {
            return false;
        }

        int s_mask = 0, t_mask = 0;
        vector<int> s_cnt(26), t_cnt(26);
        for (char c : s) {
            s_mask |= 1 << (c - 'a'); // 记录 s 中有哪些字母
            s_cnt[c - 'a']++;
        }
        for (char c : t) {
            t_mask |= 1 << (c - 'a'); // 记录 t 中有哪些字母
            t_cnt[c - 'a']++;
        }

        ranges::sort(s_cnt);
        ranges::sort(t_cnt);
        return s_mask == t_mask && s_cnt == t_cnt;
    }
};
```

```go [sol-Go]
func closeStrings(s, t string) bool {
    if len(s) != len(t) {
        return false
    }

    var sMask, tMask int
    var sCnt, tCnt [26]int
    for _, c := range s {
        sMask |= 1 << (c - 'a') // 记录 s 中有字符 c
        sCnt[c-'a']++
    }
    for _, c := range t {
        tMask |= 1 << (c - 'a') // 记录 t 中有字符 c
        tCnt[c-'a']++
    }

    slices.Sort(sCnt[:])
    slices.Sort(tCnt[:])
    return sMask == tMask && slices.Equal(sCnt[:], tCnt[:])
}
```

```js [sol-JavaScript]
var closeStrings = function(s, t) {
    if (s.length !== t.length) {
        return false;
    }

    const ASCII_A = 'a'.charCodeAt(0);
    let sMask = 0, tMask = 0;
    const sCnt = Array(26).fill(0), tCnt = Array(26).fill(0);
    for (const c of s) {
        sMask |= 1 << (c.charCodeAt(0) - ASCII_A); // 记录 s 中有字符 c
        sCnt[c.charCodeAt(0) - ASCII_A]++;
    }
    for (const c of t) {
        tMask |= 1 << (c.charCodeAt(0) - ASCII_A); // 记录 t 中有字符 c
        tCnt[c.charCodeAt(0) - ASCII_A]++;
    }

    sCnt.sort((a, b) => a - b);
    tCnt.sort((a, b) => a - b);
    return sMask === tMask && _.isEqual(sCnt, tCnt);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn close_strings(s: String, t: String) -> bool {
        if s.len() != t.len() {
            return false;
        }

        let mut s_mask = 0;
        let mut s_cnt = [0; 26];
        for c in s.as_bytes() {
            let c = (c - b'a') as usize;
            s_mask |= 1 << c; // 记录 s 中有字符 c
            s_cnt[c] += 1;
        }

        let mut t_mask = 0;
        let mut t_cnt = [0; 26];
        for c in t.as_bytes() {
            let c = (c - b'a') as usize;
            t_mask |= 1 << c; // 记录 t 中有字符 c
            t_cnt[c] += 1;
        }

        s_cnt.sort_unstable();
        t_cnt.sort_unstable();
        s_mask == t_mask && s_cnt == t_cnt
    }
}
```

不用位运算的写法：

```java [sol-Java]
import static java.nio.charset.StandardCharsets.ISO_8859_1;

class Solution {
    public boolean closeStrings(String s, String t) {
        if (s.length() != t.length()) {
            return false;
        }

        int[] sCnt = new int[26];
        for (byte c : s.getBytes(ISO_8859_1)) {
            sCnt[c - 'a']++;
        }

        int[] tCnt = new int[26];
        for (byte c : t.getBytes(ISO_8859_1)) {
            tCnt[c - 'a']++;
        }

        for (int i = 0; i < 26; i++) {
            if ((sCnt[i] == 0) != (tCnt[i] == 0)) {
                return false;
            }
        }

        Arrays.sort(sCnt);
        Arrays.sort(tCnt);
        return Arrays.equals(sCnt, tCnt);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool closeStrings(string s, string t) {
        if (s.length() != t.length()) {
            return false;
        }

        vector<int> s_cnt(26);
        for (char c : s) {
            s_cnt[c - 'a']++;
        }

        vector<int> t_cnt(26);
        for (char c : t) {
            t_cnt[c - 'a']++;
        }

        for (int i = 0; i < 26; i++) {
            if ((s_cnt[i] == 0) != (t_cnt[i] == 0)) {
                return false;
            }
        }

        ranges::sort(s_cnt);
        ranges::sort(t_cnt);
        return s_cnt == t_cnt;
    }
};
```

```go [sol-Go]
func closeStrings(s, t string) bool {
    if len(s) != len(t) {
        return false
    }

    var sCnt, tCnt [26]int
    for _, c := range s {
        sCnt[c-'a']++
    }
    for _, c := range t {
        tCnt[c-'a']++
    }

    for i := range 26 {
        if (sCnt[i] == 0) != (tCnt[i] == 0) {
            return false
        }
    }

    slices.Sort(sCnt[:])
    slices.Sort(tCnt[:])
    return sCnt == tCnt
}
```

```js [sol-JavaScript]
var closeStrings = function(s, t) {
    if (s.length !== t.length) {
        return false;
    }

    const ASCII_A = 'a'.charCodeAt(0);
    const sCnt = Array(26).fill(0);
    for (const c of s) {
        sCnt[c.charCodeAt(0) - ASCII_A]++;
    }

    const tCnt = Array(26).fill(0);
    for (const c of t) {
        tCnt[c.charCodeAt(0) - ASCII_A]++;
    }

    for (let i = 0; i < 26; i++) {
        if ((sCnt[i] === 0) !== (tCnt[i] === 0)) {
            return false;
        }
    }

    sCnt.sort((a, b) => a - b);
    tCnt.sort((a, b) => a - b);
    return _.isEqual(sCnt, tCnt);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn close_strings(s: String, t: String) -> bool {
        if s.len() != t.len() {
            return false;
        }

        let mut s_cnt = [0; 26];
        for c in s.as_bytes() {
            s_cnt[(c - b'a') as usize] += 1;
        }

        let mut t_cnt = [0; 26];
        for c in t.as_bytes() {
            t_cnt[(c - b'a') as usize] += 1;
        }

        for i in 0..26 {
            if (s_cnt[i] == 0) != (t_cnt[i] == 0) {
                return false;
            }
        }

        s_cnt.sort_unstable();
        t_cnt.sort_unstable();
        s_cnt == t_cnt
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 为 $s$ 的长度，$m$ 为 $t$ 的长度。注：如果考虑上排序的时间，就是 $\mathcal{O}(n+m+|\Sigma|\log |\Sigma|)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$，其中 $|\Sigma|$ 为字符集合的大小，本题中字符均为小写英文字母，所以 $|\Sigma|=26$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.3 等价转化`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.3 等价转化`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
