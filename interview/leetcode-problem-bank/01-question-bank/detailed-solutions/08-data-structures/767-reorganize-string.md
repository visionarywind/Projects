# 767. 重构字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reorganize-string/
- 题目 slug：`reorganize-string`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.4 重排元素
- 难度分：1681
- 外部题解来源：https://leetcode.cn/problems/reorganize-string/solutions/2779462/tan-xin-gou-zao-pai-xu-bu-pai-xu-liang-c-h9jg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[结论+构造，排序/不排序两种写法，附相似题目（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/reorganize-string/solutions/2779462/tan-xin-gou-zao-pai-xu-bu-pai-xu-liang-c-h9jg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-gou-zao-pai-xu-bu-pai-xu-liang-c-h9jg`
- topic id：`2779462`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

看示例 2，字母 $\text{a}$ 出现了很多次，导致无论怎么排，都会有两个 $\text{a}$ 是相邻的。看上去，如果有一个字母的出现次数特别大，就没法重排。这个出现次数具体要多大呢？

设 $n$ 为 $s$ 的长度，$m$ 为是出现次数最多的字母的出现次数。

**结论**：如果 $m\le n-m+1$，则可以做到，否则无法做到。

[证明+具体操作方案](https://zhuanlan.zhihu.com/p/1945782212176909162)

注意当 $n$ 为奇数时，对于 $s=\texttt{aaabb}$ 这样的字符串，可以重排成 $\texttt{ababa}$，其中 $\texttt{a}$ 的出现次数比其余字母的个数多一。

## 写法一

```py [sol-Python3]
class Solution:
    def reorganizeString(self, s: str) -> str:
        n = len(s)
        a = Counter(s).most_common()  # 按出现次数从大到小排序
        m = a[0][1]
        if m > n - m + 1:
            return ""

        ans = [''] * n
        i = 0
        for ch, cnt in a:
            for _ in range(cnt):
                ans[i] = ch
                i += 2
                if i >= n:
                    i = 1  # 填完偶数填奇数
        return ''.join(ans)
```

```java [sol-Java]
class Solution {
    public String reorganizeString(String s) {
        int n = s.length();
        Map<Character, Integer> count = new HashMap<>();
        for (char c : s.toCharArray()) {
            count.merge(c, 1, Integer::sum);
        }

        List<Map.Entry<Character, Integer>> a = new ArrayList<>(count.entrySet());
        // 按出现次数从大到小排序
        a.sort((p, q) -> q.getValue() - p.getValue());
        int m = a.get(0).getValue();
        if (m > n - m + 1) {
            return "";
        }

        char[] ans = new char[n];
        int i = 0;
        for (Map.Entry<Character, Integer> entry : a) {
            char ch = entry.getKey();
            int cnt = entry.getValue();
            while (cnt-- > 0) {
                ans[i] = ch;
                i += 2;
                if (i >= n) {
                    i = 1; // 填完偶数填奇数
                }
            }
        }
        return new String(ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string reorganizeString(string s) {
        int n = s.length();
        unordered_map<char, int> count;
        for (char ch : s) {
            count[ch]++;
        }

        vector<pair<char, int>> a(count.begin(), count.end());
        // 按出现次数从大到小排序
        ranges::sort(a, [](const auto& p, const auto& q) { return p.second > q.second; });
        int m = a[0].second;
        if (m > n - m + 1) {
            return "";
        }

        string ans(n, 0);
        int i = 0;
        for (auto [ch, cnt] : a) {
            while (cnt--) {
                ans[i] = ch;
                i += 2;
                if (i >= n) {
                    i = 1; // 填完偶数填奇数
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func reorganizeString(s string) string {
    n := len(s)
    count := map[byte]int{}
    for _, ch := range s {
        count[byte(ch)]++
    }

    type pair struct {
        ch  byte
        cnt int
    }
    a := make([]pair, 0, len(count))
    for ch, cnt := range count {
        a = append(a, pair{ch, cnt})
    }
    // 按出现次数从大到小排序
    slices.SortFunc(a, func(a, b pair) int { return b.cnt - a.cnt })
    m := a[0].cnt
    if m > n-m+1 {
        return ""
    }

    ans := make([]byte, n)
    i := 0
    for _, p := range a {
        for cnt := p.cnt; cnt > 0; cnt-- {
            ans[i] = p.ch
            i += 2
            if i >= n {
                i = 1 // 填完偶数填奇数
            }
        }
    }
    return string(ans)
}
```

```js [sol-JavaScript]
var reorganizeString = function(s) {
    const n = s.length;
    const count = new Map();
    for (const ch of s) {
        count.set(ch, (count.get(ch) ?? 0) + 1);
    }

    const a = Array.from(count.entries());
    // 按出现次数从大到小排序
    a.sort((p, q) => q[1] - p[1]);
    const m = a[0][1];
    if (m > n - m + 1) {
        return "";
    }

    const ans = Array(n);
    let i = 0;
    for (let [ch, cnt] of a) {
        while (cnt--) {
            ans[i] = ch;
            i += 2;
            if (i >= n) {
                i = 1; // 填完偶数填奇数
            }
        }
    }
    return ans.join('');
}
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn reorganize_string(s: String) -> String {
        let n = s.len();
        let mut count = HashMap::new();
        for c in s.bytes() {
            *count.entry(c).or_insert(0) += 1;
        }

        let mut a = count.into_iter().collect::<Vec<_>>();
        // 按出现次数从大到小排序
        a.sort_unstable_by(|p, q| q.1.cmp(&p.1));
        let m = a[0].1;
        if m > n - m + 1 {
            return "".to_string();
        }

        let mut ans = vec![b'\0'; n];
        let mut i = 0;
        for (ch, cnt) in a {
            for _ in 0..cnt {
                ans[i] = ch;
                i += 2;
                if i >= n {
                    i = 1; // 填完偶数填奇数
                }
            }
        }
        unsafe { String::from_utf8_unchecked(ans) }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + |\Sigma|\log |\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|$ 为字符集合的大小，本题字符均为小写字母，所以 $|\Sigma|=26$。
- 空间复杂度：$\mathcal{O}(n + |\Sigma|)$。如果不考虑返回值的空间，C++ 为 $\mathcal{O}(|\Sigma|)$。

## 写法二：不排序

只需要先填出现次数最多的字母，其余字母无需排序。

比如 $s=\texttt{aaabb}$：

- 先填 $\texttt{a}$ 再填 $\texttt{b}$，得到 $\texttt{ababa}$。
- 先填 $\texttt{b}$ 再填 $\texttt{a}$，得到 $\texttt{babaa}$，不符合要求。

```py [sol-Python3]
class Solution:
    def reorganizeString(self, s: str) -> str:
        n = len(s)
        count = Counter(s)
        ch, m = count.most_common(1)[0]
        if m > n - m + 1:
            return ""

        ans = [''] * n
        i = m * 2
        ans[:i:2] = [ch] * m  # 先填出现次数最多的字母
        del count[ch]

        # 再填其它字母
        for ch, cnt in count.items():
            for _ in range(cnt):
                if i >= n:
                    i = 1  # 填完偶数填奇数
                ans[i] = ch
                i += 2
        return ''.join(ans)
```

```java [sol-Java]
class Solution {
    public String reorganizeString(String s) {
        int n = s.length();
        int[] count = new int[26];
        int m = 0;
        char mch = 0;
        for (char ch : s.toCharArray()) {
            if (++count[ch - 'a'] > m) {
                m = count[ch - 'a'];
                mch = ch;
            }
        }
        if (m > n - m + 1) {
            return "";
        }

        char[] ans = new char[n];
        int i = 0;
        for (; m-- > 0; i += 2) {
            ans[i] = mch; // 先填出现次数最多的字母
        }
        count[mch - 'a'] = 0;

        // 再填其它字母
        for (int j = 0; j < 26; j++) {
            int cnt = count[j];
            while (cnt-- > 0) {
                if (i >= n) {
                    i = 1; // 填完偶数填奇数
                }
                ans[i] = (char) ('a' + j);
                i += 2;
            }
        }
        return new String(ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string reorganizeString(string s) {
        int n = s.length();
        int count[26]{}, m = 0;
        char mch;
        for (char ch : s) {
            if (++count[ch - 'a'] > m) {
                m = count[ch - 'a'];
                mch = ch;
            }
        }
        if (m > n - m + 1) {
            return "";
        }

        string ans(n, 0);
        int i = 0;
        for (; m--; i += 2) {
            ans[i] = mch; // 先填出现次数最多的字母
        }
        count[mch - 'a'] = 0;

        // 再填其它字母
        for (int j = 0; j < 26; j++) {
            int cnt = count[j];
            while (cnt--) {
                if (i >= n) {
                    i = 1; // 填完偶数填奇数
                }
                ans[i] = 'a' + j;
                i += 2;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func reorganizeString(s string) string {
    n := len(s)
    count := [26]int{}
    m, mch := 0, byte(0)
    for _, ch := range s {
        count[ch-'a']++
        if count[ch-'a'] > m {
            m = count[ch-'a']
            mch = byte(ch)
        }
    }
    if m > n-m+1 {
        return ""
    }

    ans := make([]byte, n)
    i := 0
    for ; m > 0; m-- {
        ans[i] = mch // 先填出现次数最多的字母
        i += 2
    }
    count[mch-'a'] = 0

    // 再填其它字母
    for j, cnt := range count {
        for ; cnt > 0; cnt-- {
            if i >= n {
                i = 1 // 填完偶数填奇数
            }
            ans[i] = byte('a' + j)
            i += 2
        }
    }
    return string(ans)
}
```

```js [sol-JavaScript]
var reorganizeString = function(s) {
    const n = s.length;
    const count = Array(26).fill(0);
    let m = 0;
    let mch;
    for (const ch of s) {
        const idx = ch.charCodeAt(0) - 'a'.charCodeAt(0);
        if (++count[idx] > m) {
            m = count[idx];
            mch = ch;
        }
    }
    if (m > n - m + 1) {
        return "";
    }

    const ans = Array(n);
    let i = 0;
    for (; m--; i += 2) {
        ans[i] = mch; // 先填出现次数最多的字母
    }
    count[mch.charCodeAt(0) - 'a'.charCodeAt(0)] = 0;

    // 再填其它字母
    for (let j = 0; j < 26; j++) {
        let cnt = count[j];
        while (cnt--) {
            if (i >= n) {
                i = 1; // 填完偶数填奇数
            }
            ans[i] = String.fromCharCode('a'.charCodeAt(0) + j);
            i += 2;
        }
    }
    return ans.join('');
}
```

```rust [sol-Rust]
impl Solution {
    pub fn reorganize_string(s: String) -> String {
        let n = s.len();
        let mut count = vec![0; 26];
        let mut m = 0;
        let mut mch = b'a';
        for ch in s.bytes() {
            let idx = (ch - b'a') as usize;
            count[idx] += 1;
            if count[idx] > m {
                m = count[idx];
                mch = ch;
            }
        }
        if m > n - m + 1 {
            return "".to_string();
        }

        let mut ans = vec![b'\0'; n];
        let mut i = 0;
        for _ in 0..m {
            ans[i] = mch; // 先填出现次数最多的字母
            i += 2;
        }
        count[(mch - b'a') as usize] = 0;

        // 再填其它字母
        for (j, &cnt) in count.iter().enumerate() {
            for _ in 0..cnt {
                if i >= n {
                    i = 1; // 填完偶数填奇数
                }
                ans[i] = b'a' + j as u8;
                i += 2;
            }
        }
        unsafe { String::from_utf8_unchecked(ans) }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + |\Sigma|)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|$ 为字符集合的大小，本题字符均为小写字母，所以 $|\Sigma|=26$。
- 空间复杂度：$\mathcal{O}(n + |\Sigma|)$。如果不考虑返回值的空间，C++ 为 $\mathcal{O}(|\Sigma|)$。

## 思考题

如果要求答案的字典序尽量小呢？

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面贪心题单的「**§1.8 相邻不同**」。

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
10. 【本题相关】[贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
