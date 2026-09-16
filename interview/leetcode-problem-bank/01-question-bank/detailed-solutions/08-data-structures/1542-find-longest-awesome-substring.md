# 1542. 找出最长的超赞子字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-longest-awesome-substring/
- 题目 slug：`find-longest-awesome-substring`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.4 状态压缩前缀和
- 难度分：2222
- 外部题解来源：https://leetcode.cn/problems/find-longest-awesome-substring/solutions/2773468/qian-zhui-yi-huo-he-fu-lei-si-ti-mu-pyth-j8lx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀异或和，附类似题目（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-longest-awesome-substring/solutions/2773468/qian-zhui-yi-huo-he-fu-lei-si-ti-mu-pyth-j8lx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-yi-huo-he-fu-lei-si-ti-mu-pyth-j8lx`
- topic id：`2773468`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识：前缀异或和**。见 [1177. 构建回文串检测](https://leetcode.cn/problems/can-make-palindrome-from-substring/) 的题解 [一步步优化！从前缀和到前缀异或和](https://leetcode.cn/problems/can-make-palindrome-from-substring/solution/yi-bu-bu-you-hua-cong-qian-zhui-he-dao-q-yh5p/)，请先阅读这篇题解。

用一个长为 $10$ 的二进制数 $\textit{mask}$ 记录一个子串中的每个数字的出现次数的奇偶性，例如 $\textit{mask}=0000001101$（从右往左读）表示子串中的数字 $0,2,3$ 出现了奇数次，其余数字出现了偶数次。

定义 $s$ 的异或前缀和 $\textit{pre}[i]$，等于 $s[0]$ 到 $s[i]$ 这个子串（前缀）中的每个数字的出现次数的奇偶性。

按照该定义，从 $s[i+1]$ 到 $s[j]$ 的这个长为 $j-i$ 的子串，其中每个数字的出现次数的奇偶性，等于

$$
\textit{pre}[j]\oplus \textit{pre}[i]
$$

其中 $\oplus$ 表示异或运算。

额外定义 $\textit{pre}[-1]=0$，以兼顾 $i+1=0$ 的情况。

分类讨论：

- 如果从 $s[i]$ 到 $s[j]$ 的子串长度是**偶数**，要使子串可以重排成回文串，每个数字的出现次数都得是偶数，所以 $\textit{pre}[j]\oplus \textit{pre}[i] = 0$，即 $\textit{pre}[i]= \textit{pre}[j]$。
- 如果从 $s[i]$ 到 $s[j]$ 的子串长度是**奇数**，要使子串可以重排成回文串，必须恰好有一个数字的出现次数是奇数，其余数字的出现次数都是偶数，所以 $\textit{pre}[j]\oplus \textit{pre}[i] = 2^k$，即 $\textit{pre}[i]= \textit{pre}[j]\oplus 2^k\ (0\le k\le 9)$。其中 $2^k$ 是只有一个 $1$ 的二进制数，对应恰好一个数字出现次数是奇数的情况。

遍历 $\textit{pre}[j]$。由于子串长度等于 $j-i$，当 $j$ 固定时，$i$ 越小，子串长度越长，所以多个相同异或前缀和，我们只关心最左边的那个。用一个哈希表（或者数组）$\textit{pos}$ 记录每个前缀异或和**首次**出现的下标，例如 $\textit{pos}[1101]=5$ 表示第一个等于 $1101$ 的前缀异或和的下标是 $5$。特别地，$\textit{pos}[0]=-1$，因为 $\textit{pre}[-1] = 0$。

- 对于偶数情况，$\textit{pre}[i]$ 首次出现的下标 $i = \textit{pos}[\textit{pre}[j]]$。用子串长度 $j-i$ 更新答案的最大值。
- 对于奇数情况，枚举 $k$，$\textit{pre}[i]$ 首次出现的下标 $i = \textit{pos}[\textit{pre}[j] \oplus 2^k ]$。用子串长度 $j-i$ 更新答案的最大值。

代码实现时，可以一边计算 $\textit{pre}$，一边遍历 $\textit{pre}$，所以只需要一个变量，而不是数组。

代码中用了一些位运算技巧，见 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class Solution:
    def longestAwesome(self, s: str) -> int:
        D = 10  # s 中的字符种类数
        n = len(s)
        pos = [n] * (1 << D)  # n 表示没有找到异或前缀和
        pos[0] = -1  # pre[-1] = 0
        ans = pre = 0
        for i, x in enumerate(map(int, s)):
            pre ^= 1 << x
            ans = max(ans, i - pos[pre],  # 偶数
                      max(i - pos[pre ^ (1 << d)] for d in range(D)))  # 奇数
            if pos[pre] == n:  # 首次遇到值为 pre 的前缀异或和，记录其下标 i
                pos[pre] = i
        return ans
```

```java [sol-Java]
class Solution {
    private static final int D = 10; // s 中的字符种类数

    public int longestAwesome(String s) {
        int n = s.length();
        int[] pos = new int[1 << D];
        Arrays.fill(pos, n); // n 表示没有找到异或前缀和
        pos[0] = -1; // pre[-1] = 0
        int ans = 0;
        int pre = 0;
        for (int i = 0; i < n; i++) {
            pre ^= 1 << (s.charAt(i) - '0');
            for (int d = 0; d < D; d++) {
                ans = Math.max(ans, i - pos[pre ^ (1 << d)]); // 奇数
            }
            ans = Math.max(ans, i - pos[pre]); // 偶数
            if (pos[pre] == n) { // 首次遇到值为 pre 的前缀异或和，记录其下标 i
                pos[pre] = i;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    const int D = 10; // s 中的字符种类数
public:
    int longestAwesome(string s) {
        int n = s.size();
        vector<int> pos(1 << D, n); // n 表示没有找到异或前缀和
        pos[0] = -1; // pre[-1] = 0
        int ans = 0, pre = 0;
        for (int i = 0; i < n; i++) {
            pre ^= 1 << (s[i] - '0');
            for (int d = 0; d < D; d++) {
                ans = max(ans, i - pos[pre ^ (1 << d)]); // 奇数
            }
            ans = max(ans, i - pos[pre]); // 偶数
            if (pos[pre] == n) { // 首次遇到值为 pre 的前缀异或和，记录其下标 i
                pos[pre] = i;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestAwesome(s string) (ans int) {
    const D = 10 // s 中的字符种类数
    n := len(s)
    pos := [1 << D]int{}
    for i := range pos {
        pos[i] = n // n 表示没有找到异或前缀和
    }
    pos[0] = -1 // pre[-1] = 0
    pre := 0
    for i, c := range s {
        pre ^= 1 << (c - '0')
        for d := 0; d < D; d++ {
            ans = max(ans, i-pos[pre^(1<<d)]) // 奇数
        }
        ans = max(ans, i-pos[pre]) // 偶数
        if pos[pre] == n { // 首次遇到值为 pre 的前缀异或和，记录其下标 i
            pos[pre] = i
        }
    }
    return
}
```

```js [sol-JavaScript]
var longestAwesome = function(s) {
    const D = 10; // s 中的字符种类数
    const n = s.length;
    const pos = Array(1 << D).fill(n); // n 表示没有找到异或前缀和
    pos[0] = -1; // pre[-1] = 0
    let ans = 0;
    let pre = 0;
    for (let i = 0; i < n; i++) {
        pre ^= 1 << (s.charCodeAt(i) - '0'.charCodeAt(0));
        for (let d = 0; d < D; d++) {
            ans = Math.max(ans, i - pos[pre ^ (1 << d)]); // 奇数
        }
        ans = Math.max(ans, i - pos[pre]); // 偶数
        if (pos[pre] === n) { // 首次遇到值为 pre 的前缀异或和，记录其下标 i
            pos[pre] = i;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn longest_awesome(s: String) -> i32 {
        const D: usize = 10;
        let n = s.len() as i32;
        let mut pos = vec![n; 1 << D]; // n 表示没有找到异或前缀和
        pos[0] = -1; // pre[-1] = 0
        let mut ans = 0;
        let mut pre = 0;
        for (i, c) in s.bytes().enumerate() {
            let i = i as i32;
            pre ^= 1 << (c - b'0');
            for d in 0..D {
                ans = ans.max(i - pos[pre ^ (1 << d)]); // 奇数
            }
            ans = ans.max(i - pos[pre]); // 偶数
            if pos[pre] == n { // 首次遇到值为 pre 的前缀异或和，记录其下标 i
                pos[pre] = i;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nD)$，其中 $n$ 为 $s$ 的长度，$D=10$。
- 空间复杂度：$\mathcal{O}(2^D)$。

## 专题：前缀异或和

- [1310. 子数组异或查询](https://leetcode.cn/problems/xor-queries-of-a-subarray/) 1460
- [1177. 构建回文串检测](https://leetcode.cn/problems/can-make-palindrome-from-substring/) 1848
- [1371. 每个元音包含偶数次的最长子字符串](https://leetcode.cn/problems/find-the-longest-substring-containing-vowels-in-even-counts/) 2041
- [1542. 找出最长的超赞子字符串](https://leetcode.cn/problems/find-longest-awesome-substring/) 2222
- [1915. 最美子字符串的数目](https://leetcode.cn/problems/number-of-wonderful-substrings/) 2235
- [2791. 树中可以形成回文的路径数](https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/) 2677

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.4 状态压缩前缀和`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.4 状态压缩前缀和`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
