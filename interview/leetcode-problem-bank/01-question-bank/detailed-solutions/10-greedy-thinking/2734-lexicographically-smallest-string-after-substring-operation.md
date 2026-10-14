# 2734. 执行子串操作后的字典序最小字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lexicographically-smallest-string-after-substring-operation/
- 题目 slug：`lexicographically-smallest-string-after-substring-operation`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1405
- 外部题解来源：https://leetcode.cn/problems/lexicographically-smallest-string-after-substring-operation/solutions/2304936/tan-xin-pythonjavacgo-by-endlesscheng-gm1d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/lexicographically-smallest-string-after-substring-operation/solutions/2304936/tan-xin-pythonjavacgo-by-endlesscheng-gm1d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-gm1d`
- topic id：`2304936`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

根据题意，把 $\texttt{a}$ 替换成 $\texttt{z}$ 会让字典序变大，所以被替换的子串不能包含 $\texttt{a}$。反过来，如果子串不含 $\texttt{a}$，那么对其操作可以让 $s$ 字典序变小。

从左到右找到第一个**不等于** $\texttt{a}$ 的字符 $s[i]$，然后从 $i$ 开始，把每个字符都减一，直到遍历结束或者遇到了 $\texttt{a}$。例如 $\texttt{abca}$ 操作中间的子串 $\texttt{bc}$，得到答案 $\texttt{aaba}$。

**细节**：如果 $s$ 全为 $\texttt{a}$，由于题目要求**必须操作一次**，可以把最后一个 $\texttt{a}$ 改成 $\texttt{z}$。

[本题视频讲解](https://www.bilibili.com/video/BV15V4y1m7Sb/) 第二题。

```py [sol-Python3]
class Solution:
    def smallestString(self, s: str) -> str:
        t = list(s)
        for i, c in enumerate(t):
            if c == 'a':
                continue
            # 继续向后遍历
            for j in range(i, len(t)):
                if t[j] == 'a':
                    break
                t[j] = chr(ord(t[j]) - 1)
            return ''.join(t)
        # 所有字母均为 a
        t[-1] = 'z'
        return ''.join(t)
```

```java [sol-Java]
class Solution {
    public String smallestString(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        for (int i = 0; i < n; i++) {
            if (s[i] > 'a') {
                // 继续向后遍历
                for (; i < n && s[i] > 'a'; i++) {
                    s[i]--;
                }
                return new String(s);
            }
        }
        // 所有字母均为 a
        s[n - 1] = 'z';
        return new String(s);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string smallestString(string s) {
        int n = s.length();
        for (int i = 0; i < n; i++) {
            if (s[i] > 'a') {
                // 继续向后遍历
                for (; i < n && s[i] > 'a'; i++) {
                    s[i]--;
                }
                return s;
            }
        }
        // 所有字母均为 a
        s.back() = 'z';
        return s;
    }
};
```

```c [sol-C]
char* smallestString(char* s) {
    int i = 0;
    for (; s[i]; i++) {
        if (s[i] > 'a') {
            // 继续向后遍历
            for (; s[i] > 'a'; i++) {
                s[i]--;
            }
            return s;
        }
    }
    // 所有字母均为 a
    s[i - 1] = 'z';
    return s;
}
```

```go [sol-Go]
func smallestString(s string) string {
    t := []byte(s)
    for i, c := range t {
        if c > 'a' {
            // 继续向后遍历
            for ; i < len(t) && t[i] > 'a'; i++ {
                t[i]--
            }
            return string(t)
        }
    }
    // 所有字母均为 a
    t[len(t)-1] = 'z'
    return string(t)
}
```

```js [sol-JavaScript]
var smallestString = function(S) {
    const s = S.split('');
    const n = s.length;
    for (let i = 0; i < n; i++) {
        if (s[i] > 'a') {
            // 继续向后遍历
            for (; i < n && s[i] > 'a'; i++) {
                s[i] = String.fromCharCode(s[i].charCodeAt(0) - 1);
            }
            return s.join('');
        }
    }
    // 所有字母均为 a
    s[n - 1] = 'z';
    return s.join('');
}
```

```rust [sol-Rust]
impl Solution {
    pub fn smallest_string(S: String) -> String {
        let mut s = S.into_bytes();
        let n = s.len();
        for i in 0..n {
            if s[i] > b'a' {
                // 继续向后遍历
                for j in i..n {
                    if s[j] == b'a' {
                        break;
                    }
                    s[j] -= 1;
                }
                return unsafe { String::from_utf8_unchecked(s) };
            }
        }
        // 所有字母均为 a
        s[n - 1] = b'z';
        unsafe { String::from_utf8_unchecked(s) }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。如果可以直接修改 $s$ 则为 $\mathcal{O}(1)$ 额外空间（C/C++）。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
