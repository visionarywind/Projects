# 1432. 改变一个整数能得到的最大差值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/max-difference-you-can-get-from-changing-an-integer/
- 题目 slug：`max-difference-you-can-get-from-changing-an-integer`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1427
- 外部题解来源：https://leetcode.cn/problems/max-difference-you-can-get-from-changing-an-integer/solutions/3690149/tan-xin-pythonjavaccgojsrust-by-endlessc-k8iw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 O(log num) 做法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/max-difference-you-can-get-from-changing-an-integer/solutions/3690149/tan-xin-pythonjavaccgojsrust-by-endlessc-k8iw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavaccgojsrust-by-endlessc-k8iw`
- topic id：`3690149`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

## 分析

要让差值最大，$a$ 和 $b$ 一个要尽量大，另一个要尽量小，所以问题相当于计算替换后的最大值 $\textit{mx}$ 和替换后的最小值 $\textit{mn}$，则答案为 $\textit{mx}-\textit{mn}$。

设 $\textit{num}$ 的十进制字符串为 $s$。

要让替换后的数尽量大，等价于让 $s$ 的字典序尽量大，替换前面的数（高位）相比替换后面的数更好。

比如 $\textit{num}=567$，替换 $5$ 为 $9$ 得到 $967$，替换 $6$ 为 $9$ 得到 $597<967$。

同理，要让替换后的数尽量小，等价于让 $s$ 的字典序尽量小，替换前面的数（高位）相比替换后面的数更好。

## 最大值

替换最高的非 $9$ 数字。

从左到右遍历 $s$，找到第一个不等于 $9$ 的数字 $d=s[i]$，把所有 $d$ 替换成 $9$。

如果没有这样的数字，则 $s$ 全为 $9$，已经最大，无需替换。

## 最小值

题目要求替换后的数不能有前导零，且非零，意思就是 $s[0]$ 不能替换成 $0$。

分类讨论：

- 如果 $s[0]\ne 1$，那么把所有等于 $s[0]$ 的数字都替换成 $1$。注意不能替换成 $0$，否则就有前导零了。
- 否则，从左到右遍历 $s$，找到第一个大于 $1$ 的数字 $d=s[i]$，把所有 $d$ 替换成 $0$。注意这里是大于 $1$，不是大于 $0$，为什么？如果把 $1$ 替换成 $0$，那么就把 $s[0]$ 也替换成 $0$ 了，有前导零，不符合题目要求。
- 如果没有这样的数字，说明 $s$ 只包含 $0$ 和 $1$，比如 $10010$，无法替换成更小的数。注意要替换必须全部替换，不能只把一部分 $1$ 替换成 $0$，比如 $10010$ 不能替换成 $10000$。

> **注**：本题 $\textit{num}\le 10^8$，如果改成 $\textit{num}\le 10^9$，替换成最大值会超过 $32$ 位整数范围，需要用 $64$ 位整数。

```py [sol-Python3]
class Solution:
    def maxDiff(self, num: int) -> int:
        s = str(num)

        mx = num
        for d in s:
            if d != '9':
                mx = int(s.replace(d, '9'))
                break

        mn = num
        if s[0] != '1':
            mn = int(s.replace(s[0], '1'))
        else:
            for d in s:
                if d > '1':  # 不是 0 也不是 1
                    mn = int(s.replace(d, '0'))
                    break

        return mx - mn
```

```java [sol-Java]
class Solution {
    public int maxDiff(int num) {
        String s = String.valueOf(num);
        char[] cs = s.toCharArray();

        int mx = num;
        for (char d : cs) {
            if (d != '9') {
                mx = replace(s, d, '9');
                break;
            }
        }

        int mn = num;
        if (cs[0] != '1') {
            mn = replace(s, cs[0], '1');
        } else {
            for (int i = 1; i < cs.length; i++) {
                if (cs[i] > '1') { // 不是 0 也不是 1
                    mn = replace(s, cs[i], '0');
                    break;
                }
            }
        }

        return mx - mn;
    }

    private int replace(String s, char oldChar, char newChar) {
        String t = s.replace(oldChar, newChar);
        return Integer.parseInt(t);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxDiff(int num) {
        string s = to_string(num);

        auto replace_stoi = [&](char old_char, char new_char) {
            int x = 0;
            for (char d : s) {
                char c = d == old_char ? new_char : d;
                x = x * 10 + (c - '0');
            }
            return x;
        };

        int mx = num;
        for (char d : s) {
            if (d != '9') {
                mx = replace_stoi(d, '9');
                break;
            }
        }

        int mn = num;
        if (s[0] != '1') {
            mn = replace_stoi(s[0], '1');
        } else {
            for (int i = 1; i < s.size(); i++) {
                if (s[i] > '1') { // 不是 0 也不是 1
                    mn = replace_stoi(s[i], '0');
                    break;
                }
            }
        }

        return mx - mn;
    }
};
```

```c [sol-C]
int replace_atoi(char* s, char old_char, char new_char) {
    int x = 0;
    for (int i = 0; s[i]; i++) {
        char c = s[i] == old_char ? new_char : s[i];
        x = x * 10 + (c - '0');
    }
    return x;
}

int maxDiff(int num) {
    char s[10];
    sprintf(s, "%d", num);

    int mx = num;
    for (int i = 0; s[i]; i++) {
        if (s[i] != '9') {
            mx = replace_atoi(s, s[i], '9');
            break;
        }
    }

    int mn = num;
    if (s[0] != '1') {
        mn = replace_atoi(s, s[0], '1');
    } else {
        for (int i = 1; i < s[i]; i++) {
            if (s[i] > '1') { // 不是 0 也不是 1
                mn = replace_atoi(s, s[i], '0');
                break;
            }
        }
    }

    return mx - mn;
}
```

```go [sol-Go]
func maxDiff(num int) int {
    s := strconv.Itoa(num)

    replace := func(old byte, new string) int {
        t := strings.ReplaceAll(s, string(old), new)
        x, _ := strconv.Atoi(t)
        return x
    }

    mx := num
    for _, d := range s {
        if d != '9' {
            mx = replace(byte(d), "9")
            break
        }
    }

    mn := num
    if s[0] != '1' {
        mn = replace(s[0], "1")
    } else {
        for _, d := range s[1:] {
            if d > '1' { // 不是 0 也不是 1
                mn = replace(byte(d), "0")
                break
            }
        }
    }

    return mx - mn
}
```

```js [sol-JavaScript]
var maxDiff = function(num) {
    const s = num.toString();

    function replace(oldChar, newChar) {
        const t = s.split(oldChar).join(newChar);
        return parseInt(t, 10);
    }

    let mx = num;
    for (const d of s) {
        if (d !== '9') {
            mx = replace(d, '9');
            break;
        }
    }

    let mn = num;
    if (s[0] !== '1') {
        mn = replace(s[0], '1');
    } else {
        for (let i = 1; i < s.length; i++) {
            if (s[i] > '1') { // 不是 0 也不是 1
                mn = replace(s[i], '0');
                break;
            }
        }
    }

    return mx - mn;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_diff(num: i32) -> i32 {
        let s = num.to_string();
        let s_bytes = s.as_bytes();

        let replace = |old: u8, new: &str| -> i32 {
            let t = s.replace(old as char, new);
            t.parse::<i32>().unwrap()
        };

        let mut mx = num;
        for &d in s_bytes {
            if d != b'9' {
                mx = replace(d, "9");
                break;
            }
        }

        let mut mn = num;
        if s_bytes[0] != b'1' {
            mn = replace(s_bytes[0], "1");
        } else {
            for &d in &s_bytes[1..] {
                if d > b'1' { // 不是 0 也不是 1
                    mn = replace(d, "0");
                    break;
                }
            }
        }

        mx - mn
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \textit{num})$。
- 空间复杂度：$\mathcal{O}(\log \textit{num})$。

更多相似题目，见下面贪心题单的「**§3.1 字典序最小/最大**」。

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
