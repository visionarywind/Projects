# 2516. 每种字符至少取 K 个

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/take-k-of-each-character-from-left-and-right/
- 题目 slug：`take-k-of-each-character-from-left-and-right`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）
- 难度分：1948
- 外部题解来源：https://leetcode.cn/problems/take-k-of-each-character-from-left-and-right/solutions/2031995/on-shuang-zhi-zhen-by-endlesscheng-4g9p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/take-k-of-each-character-from-left-and-right/solutions/2031995/on-shuang-zhi-zhen-by-endlesscheng-4g9p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-shuang-zhi-zhen-by-endlesscheng-4g9p`
- topic id：`2031995`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

比如 $s$ 中有 $3$ 个 $\texttt{a}$，$4$ 个 $\texttt{b}$，$5$ 个 $\texttt{c}$，$k=2$，每种字母至少取走 $2$ 个，等价于**剩下的字母**至多有 $1$ 个 $\texttt{a}$，$2$ 个 $\texttt{b}$ 和 $3$ 个 $\texttt{c}$。

由于只能从 $s$ 最左侧和最右侧取走字母，所以剩下的字母是 $s$ 的**子串**。

设 $s$ 中的 $\texttt{a},\texttt{b},\texttt{c}$ 的个数分别为 $x,y,z$，现在问题变成：

- 计算 $s$ 的最长子串长度，该子串满足 $\texttt{a},\texttt{b},\texttt{c}$ 的个数分别**至多**为 $x-k,y-k,z-k$。

由于子串越短越能满足要求，越长越不能满足要求，有单调性，可以用**滑动窗口**解决。如果你不了解滑动窗口，可以看视频[【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

## 实现技巧

与其维护窗口内的字母个数，不如**直接维护窗口外的字母个数**，这也是我们取走的字母个数。

- 一开始，假设我们取走了所有的字母。或者说，初始窗口是空的，窗口外的字母个数就是 $s$ 的每个字母的出现次数。
- 右端点字母进入窗口后，该字母取走的个数减一。
- 如果减一后，窗口外该字母的个数小于 $k$，说明子串太长了，或者取走的字母个数太少了，那么就不断右移左端点，把左端点字母移出窗口，相当于我们取走移出窗口的字母，直到该字母个数等于 $k$，退出内层循环。
- 内层循环结束后，用窗口长度 $\textit{right}-\textit{left}+1$ 更新子串长度的最大值。

最后，原问题的答案为 $n$ 减去子串长度的最大值。

特别地，如果 $s$ 中某个字母的个数不足 $k$，那么无法满足题目要求，返回 $-1$。

```py [sol-Python3]
class Solution:
    def takeCharacters(self, s: str, k: int) -> int:
        cnt = Counter(s)  # 一开始，把所有字母都取走
        if any(cnt[c] < k for c in "abc"):
            return -1  # 字母个数不足 k

        mx = left = 0
        for right, c in enumerate(s):
            cnt[c] -= 1  # 移入窗口，相当于不取走 c
            while cnt[c] < k:  # 窗口之外的 c 不足 k
                cnt[s[left]] += 1  # 移出窗口，相当于取走 s[left]
                left += 1
            mx = max(mx, right - left + 1)
        return len(s) - mx
```

```java [sol-Java]
class Solution {
    public int takeCharacters(String S, int k) {
        char[] s = S.toCharArray();
        int[] cnt = new int[3];
        for (char c : s) {
            cnt[c - 'a']++; // 一开始，把所有字母都取走
        }
        if (cnt[0] < k || cnt[1] < k || cnt[2] < k) {
            return -1; // 字母个数不足 k
        }

        int mx = 0; // 子串最大长度
        int left = 0;
        for (int right = 0; right < s.length; right++) {
            int c = s[right] - 'a';
            cnt[c]--; // 移入窗口，相当于不取走 c
            while (cnt[c] < k) { // 窗口之外的 c 不足 k
                cnt[s[left] - 'a']++; // 移出窗口，相当于取走 s[left]
                left++;
            }
            mx = Math.max(mx, right - left + 1);
        }
        return s.length - mx;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int takeCharacters(string s, int k) {
        int cnt[3]{};
        for (char c : s) {
            cnt[c - 'a']++; // 一开始，把所有字母都取走
        }
        if (cnt[0] < k || cnt[1] < k || cnt[2] < k) {
            return -1; // 字母个数不足 k
        }

        int mx = 0, left = 0;
        for (int right = 0; right < s.length(); right++) {
            int c = s[right] - 'a';
            cnt[c]--; // 移入窗口，相当于不取走 c
            while (cnt[c] < k) { // 窗口之外的 c 不足 k
                cnt[s[left] - 'a']++; // 移出窗口，相当于取走 s[left]
                left++;
            }
            mx = max(mx, right - left + 1);
        }
        return s.length() - mx;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int takeCharacters(char* s, int k) {
    int cnt[3] = {};
    for (int i = 0; s[i]; i++) {
        cnt[s[i] - 'a']++; // 一开始，把所有字母都取走
    }
    if (cnt[0] < k || cnt[1] < k || cnt[2] < k) {
        return -1; // 字母个数不足 k
    }

    int mx = 0, left = 0, right = 0;
    for (; s[right]; right++) {
        int c = s[right] - 'a';
        cnt[c]--; // 移入窗口，相当于不取走 c
        while (cnt[c] < k) { // 窗口之外的 c 不足 k
            cnt[s[left] - 'a']++; // 移出窗口，相当于取走 s[left]
            left++;
        }
        mx = MAX(mx, right - left + 1);
    }
    return right - mx;
}
```

```go [sol-Go]
func takeCharacters(s string, k int) int {
	cnt := [3]int{}
	for _, c := range s {
		cnt[c-'a']++ // 一开始，把所有字母都取走
	}
	if cnt[0] < k || cnt[1] < k || cnt[2] < k {
		return -1 // 字母个数不足 k
	}

	mx, left := 0, 0
	for right, c := range s {
		c -= 'a'
		cnt[c]-- // 移入窗口，相当于不取走 c
		for cnt[c] < k { // 窗口之外的 c 不足 k
			cnt[s[left]-'a']++ // 移出窗口，相当于取走 s[left]
			left++
		}
		mx = max(mx, right-left+1)
	}
	return len(s) - mx
}
```

```js [sol-JavaScript]
var takeCharacters = function(s, k) {
    const ordA = 'a'.charCodeAt(0);
    const cnt = [0, 0, 0];
    for (const c of s) {
        cnt[c.charCodeAt(0) - ordA]++; // 一开始，把所有字母都取走
    }
    if (cnt[0] < k || cnt[1] < k || cnt[2] < k) {
        return -1; // 字母个数不足 k
    }

    let mx = 0, left = 0;
    for (let right = 0; right < s.length; right++) {
        let c = s[right].charCodeAt(0) - ordA;
        cnt[c]--; // 移入窗口，相当于不取走 c
        while (cnt[c] < k) { // 窗口之外的 c 不足 k
            cnt[s[left].charCodeAt(0) - ordA]++; // 移出窗口，相当于取走 s[left]
            left++;
        }
        mx = Math.max(mx, right - left + 1);
    }
    return s.length - mx;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn take_characters(s: String, k: i32) -> i32 {
        let mut cnt = [0; 3];
        for c in s.bytes() {
            cnt[(c - b'a') as usize] += 1; // 一开始，把所有字母都取走
        }
        if cnt[0] < k || cnt[1] < k || cnt[2] < k {
            return -1; // 字母个数不足 k
        }

        let mut mx = 0;
        let mut left = 0;
        let s = s.as_bytes();
        for (right, &c) in s.iter().enumerate() {
            let c = (c - b'a') as usize;
            cnt[c] -= 1; // 移入窗口，相当于不取走 c
            while cnt[c] < k { // 窗口之外的 c 不足 k
                cnt[(s[left] - b'a') as usize] += 1; // 移出窗口，相当于取走 s[left]
                left += 1;
            }
            mx = mx.max(right - left + 1);
        }
        (s.len() - mx) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|)$，其中 $n$ 为 $s$ 的长度，$|\Sigma|=3$ 为字符集合的大小。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $n$ 次，所以二重循环的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

更多类似题目，见下面的滑动窗口题单。

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

## 本地原创解析

### 1. 题意重述

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
