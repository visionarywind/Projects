# 541. 反转字符串 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-string-ii/
- 题目 slug：`reverse-string-ii`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.1 反转字符串
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/reverse-string-ii/solutions/3056217/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-ig15/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/reverse-string-ii/solutions/3056217/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-ig15/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-ig15`
- topic id：`3056217`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

题目让我们每隔 $2k$ 切一刀，把 $s$ 分割成 $[0,2k),[2k,4k),[4k,6k),\ldots$ 对于每一段，反转前 $k$ 个字母。

所以我们要反转的下标区间为

$$
[0,k),[2k,3k),[4k,5k),\ldots
$$

注意最后一个区间的右开端点要和 $n$ 取最小值，防止下标越界。

如何反转字符串见 [344 题的题解](https://leetcode.cn/problems/reverse-string/solutions/2376290/ji-chong-bu-tong-de-xie-fa-pythonjavacgo-9trb/)。

```py [sol-Python3]
class Solution:
    def reverseStr(self, s: str, k: int) -> str:
        s = list(s)
        for i in range(0, len(s), k * 2):
            s[i: i + k] = s[i: i + k][::-1]  # 右端点自动和 n 取最小值
        return ''.join(s)
```

```java [sol-Java]
class Solution {
    public String reverseStr(String S, int k) {
        char[] s = S.toCharArray();
        int n = s.length;
        for (int i = 0; i < n; i += k * 2) {
            reverse(s, i, Math.min(i + k, n) - 1);
        }
        return new String(s);
    }

    private void reverse(char[] s, int left, int right) {
        while (left < right) {
            char tmp = s[left];
            s[left++] = s[right];
            s[right--] = tmp;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string reverseStr(string s, int k) {
        int n = s.size();
        for (int i = 0; i < n; i += k * 2) {
            reverse(s.begin() + i, s.begin() + min(i + k, n));
        }
        return s;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

void reverse(char* s, int left, int right) {
    while (left < right) {
        char tmp = s[left];
        s[left++] = s[right];
        s[right--] = tmp;
    }
}

char* reverseStr(char* s, int k) {
    int n = strlen(s);
    for (int i = 0; i < n; i += k * 2) {
        reverse(s, i, MIN(i + k, n) - 1);
    }
    return s;
}
```

```go [sol-Go]
func reverseStr(S string, k int) string {
    s := []byte(S)
    n := len(s)
    for i := 0; i < n; i += k * 2 {
        slices.Reverse(s[i:min(i+k, n)])
    }
    return string(s)
}
```

```js [sol-JavaScript]
var reverse = function(s, left, right) {
    while (left < right) {
        const tmp = s[left];
        s[left++] = s[right];
        s[right--] = tmp;
    }
};

var reverseStr = function(s, k) {
    s = s.split('');
    const n = s.length;
    for (let i = 0; i < n; i += k * 2) {
        reverse(s, i, Math.min(i + k, n) - 1);
    }
    return s.join('');
};
```

```rust [sol-Rust]
impl Solution {
    pub fn reverse_str(mut S: String, k: i32) -> String {
        let mut s = unsafe { S.as_bytes_mut() };
        let k = k as usize;
        let n = s.len();
        for i in (0..n).step_by(k * 2) {
            s[i..n.min(i + k)].reverse();
        }
        S
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。如果可以原地修改字符串，则空间复杂度为 $\mathcal{O}(1)$。

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
