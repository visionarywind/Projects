# 696. 计数二进制子串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-binary-substrings/
- 题目 slug：`count-binary-substrings`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-binary-substrings/solutions/3898025/yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-0mdk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次遍历，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/count-binary-substrings/solutions/3898025/yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-0mdk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-ci-bian-li-jian-ji-xie-fa-pythonjavac-0mdk`
- topic id：`3898025`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**题意**：子串必须形如 $\underbrace{\texttt{0}\cdots \texttt{0}}_{k\ 个\ \texttt{0}}\underbrace{\texttt{1}\cdots \texttt{1}}_{k\ 个\ \texttt{1}}$ 或者 $\underbrace{\texttt{1}\cdots \texttt{1}}_{k\ 个\ \texttt{1}}\underbrace{\texttt{0}\cdots \texttt{0}}_{k\ 个\ \texttt{0}}$。只能有一段 $\texttt{0}$ 和一段 $\texttt{1}$，不能是 $\texttt{00111}$（两段长度不等）或者 $\texttt{010}$（超过两段）等。

例如 $s = \texttt{001110000}$，按照连续相同字符，分成三组 $\texttt{00},\texttt{111},\texttt{0000}$。

- 在前两组中，我们可以得到 $2$ 个合法子串：$\texttt{0011}$ 和 $\texttt{01}$。
- 在后两组中，我们可以得到 $3$ 个合法子串：$\texttt{111000}$、$\texttt{1100}$ 和 $\texttt{10}$。

一般地，遍历 $s$，按照连续相同字符分组，计算每一组的长度。设当前这组的长度为 $\textit{cur}$，上一组的长度为 $\textit{pre}$，那么当前这组和上一组，能得到 $\min(\textit{pre},\textit{cur})$ 个合法子串，加到答案中。

```py [sol-Python3]
class Solution:
    def countBinarySubstrings(self, s: str) -> int:
        n = len(s)
        pre = cur = ans = 0
        for i in range(n):
            cur += 1
            if i == n - 1 or s[i] != s[i + 1]:
                # 遍历到了这一组的末尾
                ans += min(pre, cur)
                pre = cur
                cur = 0
        return ans
```

```java [sol-Java]
class Solution {
    public int countBinarySubstrings(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int pre = 0;
        int cur = 0;
        int ans = 0;
        for (int i = 0; i < n; i++) {
            cur++;
            if (i == n - 1 || s[i] != s[i + 1]) {
                // 遍历到了这一组的末尾
                ans += Math.min(pre, cur);
                pre = cur;
                cur = 0;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countBinarySubstrings(string s) {
        int n = s.size();
        int pre = 0, cur = 0, ans = 0;
        for (int i = 0; i < n; i++) {
            cur++;
            if (i == n - 1 || s[i] != s[i + 1]) {
                // 遍历到了这一组的末尾
                ans += min(pre, cur);
                pre = cur;
                cur = 0;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int countBinarySubstrings(char* s) {
    int pre = 0, cur = 0, ans = 0;
    for (int i = 0; s[i]; i++) {
        cur++;
        if (s[i] != s[i + 1]) {
            // 遍历到了这一组的末尾
            ans += MIN(pre, cur);
            pre = cur;
            cur = 0;
        }
    }
    return ans;
}
```

```go [sol-Go]
func countBinarySubstrings(s string) (ans int) {
	n := len(s)
	pre, cur := 0, 0
	for i := range n {
		cur++
		if i == n-1 || s[i] != s[i+1] {
			// 遍历到了这一组的末尾
			ans += min(pre, cur)
			pre = cur
			cur = 0
		}
	}
	return
}
```

```js [sol-JavaScript]
var countBinarySubstrings = function(s) {
    const n = s.length;
    let pre = 0, cur = 0, ans = 0;
    for (let i = 0; i < n; i++) {
        cur++;
        if (i === n - 1 || s[i] !== s[i + 1]) {
            // 遍历到了这一组的末尾
            ans += Math.min(pre, cur);
            pre = cur;
            cur = 0;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn count_binary_substrings(s: String) -> i32 {
        let s = s.as_bytes();
        let n = s.len();
        let mut pre = 0;
        let mut cur = 0;
        let mut ans = 0;
        for i in 0..n {
            cur += 1;
            if i == n - 1 || s[i] != s[i + 1] {
                // 遍历到了这一组的末尾
                ans += pre.min(cur);
                pre = cur;
                cur = 0;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面双指针题单的「**六、分组循环**」。

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
