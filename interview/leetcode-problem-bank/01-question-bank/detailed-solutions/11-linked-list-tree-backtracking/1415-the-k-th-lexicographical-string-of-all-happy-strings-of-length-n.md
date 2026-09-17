# 1415. 长度为 n 的开心字符串中字典序第 k 小的字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/the-k-th-lexicographical-string-of-all-happy-strings-of-length-n/
- 题目 slug：`the-k-th-lexicographical-string-of-all-happy-strings-of-length-n`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.7 搜索
- 难度分：1576
- 外部题解来源：https://leetcode.cn/problems/the-k-th-lexicographical-string-of-all-happy-strings-of-length-n/solutions/3913970/on-jian-ji-xie-fa-pythonjavaccgojsrust-b-vxxs/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/the-k-th-lexicographical-string-of-all-happy-strings-of-length-n/solutions/3913970/on-jian-ji-xie-fa-pythonjavaccgojsrust-b-vxxs/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-jian-ji-xie-fa-pythonjavaccgojsrust-b-vxxs`
- topic id：`3913970`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

首先计算有多少个长为 $n$ 的开心字符串。 

这是一个计数问题。有 $n$ 个空位，第一个位置可以填 $3$ 种字母，后面的每个位置，都不能和前一个位置的字母相同，所以都只有 $2$ 种填法，因此方案数为

$$
3\cdot 2^{n-1}
$$

如果 $k > 3\cdot 2^{n-1}$，返回空串。否则答案是存在的。

---

为方便计算，我们先把 $k$ **减一**，改成从 $0$ 开始。

以 $n=4$，$k=12=1100_{(2)}$（**减一后**）为例说明。

答案的第一个字母是什么？

如果第一个字母是 $\texttt{a}$，那么后面的 $n-1=3$ 个位置有 $2^{n-1}=8$ 种填法，不够。

如果第一个字母是 $\texttt{b}$，同样地，后面的 $n-1=3$ 个位置有 $2^{n-1}=8$ 种填法，这就够了，所以答案的第一个字母填 $\texttt{b}$。

现在答案为 $\texttt{b\_\_\_}$，剩余的三个位置填什么字母？

由于每个位置都有两种填法，这和 $k$ 的二进制可以完美对应，如下表（注意相邻字母不同的要求）：

| $k$ 的低三位  | 对应填法 |
|---|---|
| $000$ | $\texttt{aba}$ |
| $001$ | $\texttt{abc}$ |
| $010$ | $\texttt{aca}$ |
| $011$ | $\texttt{acb}$ |
| $100$ | $\texttt{cab}$ |
| $101$ | $\texttt{cac}$ |
| $110$ | $\texttt{cba}$ |
| $111$ | $\texttt{cbc}$ |

对于 $k=1100_{(2)}$（**减一后**）这个例子，答案是这样填的：

- 答案的第二个字母不能和前一个字母 $\textit{b}$ 相同，只能填 $\texttt{a}$ 或者 $\texttt{c}$，由于 $k$ 这一位是 $1$，所以填 $\texttt{c}$。
- 答案的第三个字母不能和前一个字母 $\textit{c}$ 相同，只能填 $\texttt{a}$ 或者 $\texttt{b}$，由于 $k$ 这一位是 $0$，所以填 $\texttt{a}$。
- 答案的第四个字母不能和前一个字母 $\textit{a}$ 相同，只能填 $\texttt{b}$ 或者 $\texttt{c}$，由于 $k$ 这一位是 $0$，所以填 $\texttt{b}$。

所以答案为 $\texttt{bcab}$。

一般地，答案的第一个字母是第 $\left\lfloor\dfrac{k}{2^{n-1}}\right\rfloor$ 个小写字母，随后的字母可以根据 $k\bmod 2^{n-1}$ 二进制从高到低是 $0$ 还是 $1$，填入相应的字母：

- 首先，如果二进制这一位是 $0$，那么填入 $\texttt{a}$，否则填入 $\texttt{b}$。
- 然后修正：看填入的字母是否大于等于左侧相邻字母，如果大于等于，那么把填入的字母加一。比如左侧相邻字母是 $\texttt{a}$，那么当前这一位如果填的是 $\texttt{a}$，要变成 $\texttt{b}$；如果填的是 $\texttt{b}$，要变成 $\texttt{c}$。

> **注**：这个「加一」的技巧可用于生成两个不同的随机整数，见 [961. 在长度 2N 的数组中找出重复 N 次的元素](https://leetcode.cn/problems/n-repeated-element-in-size-2n-array/) 我的题解的方法四。

```py [sol-Python3]
class Solution:
    def getHappyString(self, n: int, k: int) -> str:
        if k > 3 << (n - 1):
            return ""
        k -= 1  # 改成从 0 开始，方便计算
        ans = [ord('a')] * n
        ans[0] += k >> (n - 1)
        for i in range(1, n):
            ans[i] += k >> (n - 1 - i) & 1
            if ans[i] >= ans[i - 1]:
                ans[i] += 1
        return ''.join(map(chr, ans))
```

```java [sol-Java]
class Solution {
    public String getHappyString(int n, int k) {
        if (k > 3 << (n - 1)) {
            return "";
        }
        k--; // 改成从 0 开始，方便计算
        char[] ans = new char[n];
        ans[0] = (char) ('a' + (k >> (n - 1)));
        for (int i = 1; i < n; i++) {
            ans[i] = (char) ('a' + (k >> (n - 1 - i) & 1));
            if (ans[i] >= ans[i - 1]) {
                ans[i]++;
            }
        }
        return new String(ans);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string getHappyString(int n, int k) {
        if (k > 3 << (n - 1)) {
            return "";
        }
        k--; // 改成从 0 开始，方便计算
        string ans(n, 'a');
        ans[0] += k >> (n - 1);
        for (int i = 1; i < n; i++) {
            ans[i] += k >> (n - 1 - i) & 1;
            if (ans[i] >= ans[i - 1]) {
                ans[i]++;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
char* getHappyString(int n, int k) {
    if (k > 3 << (n - 1)) {
        return "";
    }
    k--; // 改成从 0 开始，方便计算
    char* ans = malloc((n + 1) * sizeof(char));
    ans[0] = 'a' + (k >> (n - 1));
    for (int i = 1; i < n; i++) {
        ans[i] = 'a' + (k >> (n - 1 - i) & 1);
        if (ans[i] >= ans[i - 1]) {
            ans[i]++;
        }
    }
    ans[n] = '\0';
    return ans;
}
```

```go [sol-Go]
func getHappyString(n, k int) string {
	if k > 3<<(n-1) {
		return ""
	}
	k-- // 改成从 0 开始，方便计算
	ans := make([]byte, n)
	ans[0] = 'a' + byte(k>>(n-1))
	for i := 1; i < n; i++ {
		ans[i] = 'a' + byte(k>>(n-1-i)&1)
		if ans[i] >= ans[i-1] {
			ans[i]++
		}
	}
	return string(ans)
}
```

```js [sol-JavaScript]
var getHappyString = function(n, k) {
    if (k > 3 << (n - 1)) {
        return "";
    }
    k--; // 改成从 0 开始，方便计算
    const ans = Array(n).fill('a'.charCodeAt(0));
    ans[0] += k >> (n - 1);
    for (let i = 1; i < n; i++) {
        ans[i] += k >> (n - 1 - i) & 1;
        if (ans[i] >= ans[i - 1]) {
            ans[i]++;
        }
    }
    return String.fromCharCode(...ans);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn get_happy_string(n: i32, mut k: i32) -> String {
        if k > 3 << (n - 1) {
            return String::new();
        }
        k -= 1; // 改成从 0 开始，方便计算
        let n = n as usize;
        let mut ans = vec![0; n];
        ans[0] = b'a' + (k >> (n - 1)) as u8;
        for i in 1..n {
            ans[i] = b'a' + (k >> (n - 1 - i) & 1) as u8;
            if ans[i] >= ans[i - 1] {
                ans[i] += 1;
            }
        }
        unsafe { String::from_utf8_unchecked(ans) }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 相似题目

[60. 排列序列](https://leetcode.cn/problems/permutation-sequence/)

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

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.7 搜索`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.7 搜索`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
