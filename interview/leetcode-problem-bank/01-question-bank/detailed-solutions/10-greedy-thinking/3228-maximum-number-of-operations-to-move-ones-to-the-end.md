# 3228. 将 1 移动到末尾的最大操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-operations-to-move-ones-to-the-end/
- 题目 slug：`maximum-number-of-operations-to-move-ones-to-the-end`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1593
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-operations-to-move-ones-to-the-end/solutions/2851730/du-che-pythonjavacgo-by-endlesscheng-tllv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[堵车模型（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-operations-to-move-ones-to-the-end/solutions/2851730/du-che-pythonjavacgo-by-endlesscheng-tllv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`du-che-pythonjavacgo-by-endlesscheng-tllv`
- topic id：`2851730`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

把 $1$ 当作**车**，想象有一条长为 $n$ 的道路上有一些车。

**题意**：把所有的车都开到最右边。例如 $011010$ 最终要变成 $000111$。

如果优先操作右边的（能移动的）车，那么这些车都只需操作一次：

$$
\begin{aligned}
      & 011010        \\
\to{} & 011001        \\
\to{} & 010011        \\
\to{} & 000111        \\
\end{aligned}
$$

一共需要操作 $3$ 次（注意一次操作可以让一辆车移动多次）。

而如果优先操作左边的（能移动的）车，这会制造大量的「**堵车**」，每辆车的操作次数会更多。

$$
\begin{aligned}
      & 011010        \\
\to{} & 010110        \\
\to{} & 001110        \\
\to{} & 001101        \\
\to{} & 001011        \\
\to{} & 000111        \\
\end{aligned}
$$

一共需要操作 $5$ 次。

**算法**：

1. 从左到右遍历 $s$，同时用一个变量 $\textit{cnt}_1$ 维护遍历到的 $1$ 的个数。
2. 如果 $s[i]$ 是 $1$，把 $\textit{cnt}_1$ 增加 $1$。
3. 如果 $s[i]$ 是 $0$ 且 $s[i-1]$ 是 $1$，意味着我们找到了一段道路，可以让 $i$ **左边的每辆车都操作一次**，把答案增加 $\textit{cnt}_1$。
4. 遍历结束，返回答案。

[本题视频讲解](https://www.bilibili.com/video/BV16Z421N7P2/?t=7m56s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxOperations(self, s: str) -> int:
        ans = cnt1 = 0
        for i, c in enumerate(s):
            if c == '1':
                cnt1 += 1
            elif i > 0 and s[i - 1] == '1':
                ans += cnt1
        return ans
```

```java [sol-Java]
class Solution {
    public int maxOperations(String S) {
        char[] s = S.toCharArray();
        int ans = 0;
        int cnt1 = 0;
        for (int i = 0; i < s.length; i++) {
            if (s[i] == '1') {
                cnt1++;
            } else if (i > 0 && s[i - 1] == '1') {
                ans += cnt1;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxOperations(string s) {
        int ans = 0, cnt1 = 0;
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '1') {
                cnt1++;
            } else if (i > 0 && s[i - 1] == '1') {
                ans += cnt1;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int maxOperations(char* s) {
    int ans = 0, cnt1 = 0;
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        if (c == '1') {
            cnt1++;
        } else if (i > 0 && s[i - 1] == '1') {
            ans += cnt1;
        }
    }
    return ans;
}
```

```go [sol-Go]
func maxOperations(s string) (ans int) {
	cnt1 := 0
	for i, c := range s {
		if c == '1' {
			cnt1++
		} else if i > 0 && s[i-1] == '1' {
			ans += cnt1
		}
	}
	return
}
```

```js [sol-JavaScript]
var maxOperations = function(s) {
    let ans = 0, cnt1 = 0;
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        if (c === '1') {
            cnt1++;
        } else if (i > 0 && s[i - 1] === '1') {
            ans += cnt1;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_operations(s: String) -> i32 {
        let s = s.as_bytes();
        let mut ans = 0;
        let mut cnt1 = 0;
        for (i, &c) in s.iter().enumerate() {
            if c == b'1' {
                cnt1 += 1;
            } else if i > 0 && s[i - 1] == b'1' {
                ans += cnt1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

构造一个 $s$，让返回值尽量大。

如果 $n=10^5$，答案最大能是多少？会不会超过 $\texttt{int}$ 最大值？

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

本题来自 `一、贪心策略 / §1.4 从最左/最右开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.4 从最左/最右开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
