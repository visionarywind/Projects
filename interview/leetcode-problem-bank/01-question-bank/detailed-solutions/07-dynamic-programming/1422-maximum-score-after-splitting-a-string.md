# 1422. 分割字符串的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-after-splitting-a-string/
- 题目 slug：`maximum-score-after-splitting-a-string`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-score-after-splitting-a-string/solutions/3007878/mei-ju-fen-ge-xian-on-qian-hou-zhui-fen-a3pio/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举分割线，O(n) 前后缀分解 / 一次遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-score-after-splitting-a-string/solutions/3007878/mei-ju-fen-ge-xian-on-qian-hou-zhui-fen-a3pio/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-fen-ge-xian-on-qian-hou-zhui-fen-a3pio`
- topic id：`3007878`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**核心思想**：枚举分割线的位置，维护 $0$ 和 $1$ 的个数。

设前缀（左子字符串）中 $0$ 的个数为 $\textit{left}_0$，后缀（右子字符串）中 $1$ 的个数为 $\textit{right}_1$。

设分割线在 $s[i]$ 和 $s[i+1]$ 之间，$s=\texttt{011101}$ 的计算过程如下：

| $i$ | 分割线  | $\textit{left}_0$  |  $\textit{right}_1$ | 得分 |
|---|---|---|---|---|
| $0$  |  $\texttt{0}$\|$\texttt{11101}$ | $1$  | $4$  | $5$ |
| $1$  |  $\texttt{01}$\|$\texttt{1101}$ | $1$ | $3$  | $4$ |
| $2$  |  $\texttt{011}$\|$\texttt{101}$ | $1$ | $2$  | $3$ |
| $3$  |  $\texttt{0111}$\|$\texttt{01}$ | $1$ | $1$  | $2$ |
| $4$  |  $\texttt{01110}$\|$\texttt{1}$ | $2$ | $1$  | $3$ |

> 注意至多计算到 $i=n-2$，因为题目要求前缀以及后缀必须是**非空**的。

$\textit{left}_0$ 可以在遍历 $s$ 的过程中计算。

$\textit{right}_1$ 呢？我们可以在枚举分割线之前，先统计整个 $s$ 中的 $\texttt{1}$ 的个数。

然后再枚举分割线：

- 如果 $s[i] = \texttt{0}$，那么把 $\textit{left}_0$ 增加 $1$。
- 否则 $s[i] = \texttt{1}$，那么把 $\textit{right}_1$ 减少 $1$。因为 $s[i]$ 之前在分割线右侧，现在属于分割线左侧了。

## 写法一：前后缀分解

```py [sol-Python3]
class Solution:
    def maxScore(self, s: str) -> int:
        right1 = s.count('1')
        ans = left0 = 0
        for c in s[:-1]:  # 移动分割线
            if c == '0':
                left0 += 1
            else:
                right1 -= 1
            ans = max(ans, left0 + right1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxScore(String S) {
        char[] s = S.toCharArray();
        int right1 = 0;
        for (char c : s) {
            right1 += c - '0';
        }

        int ans = 0;
        int left0 = 0;
        for (int i = 0; i < s.length - 1; i++) { // 移动分割线
            if (s[i] == '0') {
                left0++;
            } else {
                right1--;
            }
            ans = Math.max(ans, left0 + right1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxScore(string s) {
        int right1 = ranges::count(s, '1');
        int ans = 0, left0 = 0;
        for (int i = 0; i + 1 < s.length(); i++) { // 移动分割线
            if (s[i] == '0') {
                left0++;
            } else {
                right1--;
            }
            ans = max(ans, left0 + right1);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxScore(char* s) {
    int right1 = 0;
    for (int i = 0; s[i]; i++) {
        right1 += s[i] - '0';
    }

    int ans = 0, left0 = 0;
    for (int i = 0; s[i + 1]; i++) { // 移动分割线
        if (s[i] == '0') {
            left0++;
        } else {
            right1--;
        }
        ans = MAX(ans, left0 + right1);
    }
    return ans;
}
```

```go [sol-Go]
func maxScore(s string) (ans int) {
    right1 := strings.Count(s, "1")
    left0 := 0
    for _, c := range s[:len(s)-1] { // 移动分割线
        if c == '0' {
            left0++
        } else {
            right1--
        }
        ans = max(ans, left0+right1)
    }
    return
}
```

```js [sol-JavaScript]
var maxScore = function(s) {
    let right1 = _.countBy(s)['1'] ?? 0;
    let ans = 0, left0 = 0;
    for (let i = 0; i < s.length - 1; i++) { // 移动分割线
        if (s[i] === '0') {
            left0++;
        } else {
            right1--;
        }
        ans = Math.max(ans, left0 + right1);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_score(s: String) -> i32 {
        let mut right1 = s.bytes().filter(|&c| c == b'1').count() as i32;
        let mut ans = 0;
        let mut left0 = 0;
        let s = s.as_bytes();
        for &c in &s[..s.len() - 1] { // 移动分割线
            if c == b'0' {
                left0 += 1;
            } else {
                right1 -= 1;
            }
            ans = ans.max(left0 + right1);
        }
        ans
    }
}
```

## 写法二：合并变量

把 $\textit{left}_0$ 和 $\textit{right}_1$ 合并成一个变量 $\textit{score}$。

一开始，分割线在最左边，$\textit{score}$ 初始化成 $s$ 中的 $\texttt{1}$ 的个数。

不断向右移动分割线，或者说，把 $s[i]$ 从分割线右边移动到分割线左边。

- 如果 $s[i] = \texttt{0}$，那么左边多了个 $\texttt{0}$，把 $\textit{score}$ 增加 $1$。
- 否则 $s[i] = \texttt{1}$，那么右边少了个 $\texttt{1}$，把 $\textit{score}$ 减少 $1$。

```py [sol-Python3]
class Solution:
    def maxScore(self, s: str) -> int:
        score = s.count('1')
        ans = 0
        for c in s[:-1]:
            score += 1 if c == '0' else -1
            ans = max(ans, score)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxScore(String S) {
        char[] s = S.toCharArray();
        int score = 0;
        for (char c : s) {
            score += c - '0';
        }

        int ans = 0;
        for (int i = 0; i < s.length - 1; i++) {
            score += s[i] == '0' ? 1 : -1;
            ans = Math.max(ans, score);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxScore(string s) {
        int score = ranges::count(s, '1');
        int ans = 0;
        for (int i = 0; i + 1 < s.length(); i++) {
            score += s[i] == '0' ? 1 : -1;
            ans = max(ans, score);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxScore(char* s) {
    int score = 0;
    for (int i = 0; s[i]; i++) {
        score += s[i] - '0';
    }

    int ans = 0;
    for (int i = 0; s[i + 1]; i++) {
        score += s[i] == '0' ? 1 : -1;
        ans = MAX(ans, score);
    }
    return ans;
}
```

```go [sol-Go]
func maxScore(s string) (ans int) {
    score := strings.Count(s, "1")
    for _, c := range s[:len(s)-1] {
        if c == '0' {
            score++
        } else {
            score--
        }
        ans = max(ans, score)
    }
    return
}
```

```go [sol-Go 写法二]
func maxScore(s string) (ans int) {
    score := strings.Count(s, "1")
    for _, c := range s[:len(s)-1] {
        score += 1 - int(c-'0')*2
        ans = max(ans, score)
    }
    return
}
```

```js [sol-JavaScript]
var maxScore = function(s) {
    let score = _.countBy(s)['1'] ?? 0;
    let ans = 0;
    for (let i = 0; i < s.length - 1; i++) {
        score += s[i] === '0' ? 1 : -1;
        ans = Math.max(ans, score);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_score(s: String) -> i32 {
        let mut score = s.bytes().filter(|&c| c == b'1').count() as i32;
        let mut ans = 0;
        let s = s.as_bytes();
        for &c in &s[..s.len() - 1] {
            score += if c == b'0' { 1 } else { -1 };
            ans = ans.max(score);
        }
        ans
    }
}
```

## 写法三：拆分变量，一次遍历

回顾写法二的算法，我们做了两件事情：

1. 统计 $s$ 中的 $\texttt{1}$ 的总数，记作 $\textit{total}_1$。
2. 在 $\textit{total}_1$ 的基础上，做了一些 $\pm 1$。我们可以单独计算 $\pm 1$ 带来的**最大增量**，记作 $\textit{maxExtra}$。

最后答案为 $\textit{total}_1 + \textit{maxExtra}$。

```py [sol-Python3]
class Solution:
    def maxScore(self, s: str) -> int:
        total1 = extra = 0
        max_extra = -1
        for c in s[:-1]:
            total1 += 1 if c == '1' else 0
            extra += 1 if c == '0' else -1
            max_extra = max(max_extra, extra)
        total1 += int(s[-1])
        return total1 + max_extra
```

```java [sol-Java]
class Solution {
    public int maxScore(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int total1 = 0;
        int extra = 0;
        int maxExtra = -1;
        for (int i = 0; i < n - 1; i++) {
            total1 += s[i] - '0';
            extra += s[i] == '0' ? 1 : -1;
            maxExtra = Math.max(maxExtra, extra);
        }
        total1 += s[n - 1] - '0';
        return total1 + maxExtra;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxScore(string s) {
        int n = s.size();
        int total1 = 0;
        int extra = 0;
        int max_extra = -1;
        for (int i = 0; i + 1 < n; i++) {
            total1 += s[i] - '0';
            extra += s[i] == '0' ? 1 : -1;
            max_extra = max(max_extra, extra);
        }
        total1 += s[n - 1] - '0';
        return total1 + max_extra;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxScore(char* s) {
    int total1 = 0;
    int extra = 0;
    int max_extra = -1;
    int i = 0;
    for (; s[i + 1]; i++) {
        total1 += s[i] - '0';
        extra += s[i] == '0' ? 1 : -1;
        max_extra = MAX(max_extra, extra);
    }
    total1 += s[i] - '0';
    return total1 + max_extra;
}
```

```go [sol-Go]
func maxScore(s string) int {
	n := len(s)
	total1, extra := 0, 0
	maxExtra := -1
	for _, c := range s[:n-1] {
		if c == '0' {
			extra++
		} else {
			total1++
			extra--
		}
		maxExtra = max(maxExtra, extra)
	}
	total1 += int(s[n-1] - '0')
	return total1 + maxExtra
}
```

```js [sol-JavaScript]
var maxScore = function(s) {
    const n = s.length;
    let total1 = 0;
    let extra = 0;
    let maxExtra = -1;
    for (let i = 0; i < n - 1; i++) {
        total1 += s[i] === '1' ? 1 : 0;
        extra += s[i] === '0' ? 1 : -1;
        maxExtra = Math.max(maxExtra, extra);
    }
    total1 += (s[n - 1] === '1' ? 1 : 0);
    return total1 + maxExtra;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_score(s: String) -> i32 {
        let s = s.as_bytes();
        let n = s.len();
        let mut total1 = 0;
        let mut extra = 0;
        let mut max_extra = -1;
        for &c in &s[..n - 1] {
            total1 += (c - b'0') as i32;
            extra += if c == b'0' { 1 } else { -1 };
            max_extra = max_extra.max(extra);
        }
        total1 += (s[n - 1] - b'0') as i32;
        total1 + max_extra
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面动态规划题单的「**专题：前后缀分解**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
