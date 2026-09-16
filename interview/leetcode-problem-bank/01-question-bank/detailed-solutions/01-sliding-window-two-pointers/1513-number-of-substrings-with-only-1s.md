# 1513. 仅含 1 的子串数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-substrings-with-only-1s/
- 题目 slug：`number-of-substrings-with-only-1s`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1351
- 外部题解来源：https://leetcode.cn/problems/number-of-substrings-with-only-1s/solutions/3820453/mei-ju-zi-chuan-you-duan-dian-jian-ji-xi-f5ry/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举子串右端点，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/number-of-substrings-with-only-1s/solutions/3820453/mei-ju-zi-chuan-you-duan-dian-jian-ji-xi-f5ry/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-zi-chuan-you-duan-dian-jian-ji-xi-f5ry`
- topic id：`3820453`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

遍历 $s$ 的过程中，记录上一个 $0$ 的位置 $\textit{last}_0$。

如果 $s[i]=1$，那么对于右端点为 $i$ 的全 $1$ 子串，左端点可以是 

$$
\textit{last}_0+1,\textit{last}_0+2,\ldots,i-1,i
$$

一共有 $i - \textit{last}_0$ 个，加到答案中。

比如 $\textit{last}_0=2$，现在 $s[5]=1$，那么子串 $[3,5],[4,5],[5,5]$ 都只包含 $1$，有 $5-2=3$ 个以 $5$ 为右端点的全 $1$ 子串。

可能一开始没有 $0$，为了让公式兼容这种情况，初始化 $\textit{last}_0=-1$。

注意答案不超过 $64$ 位整数的最大值，可以在最后返回时再取模。

```py [sol-Python3]
class Solution:
    def numSub(self, s: str) -> int:
        MOD = 1_000_000_007
        ans = 0
        last0 = -1
        for i, ch in enumerate(s):
            if ch == '0':
                last0 = i  # 记录上个 0 的位置
            else:
                ans += i - last0  # 右端点为 i 的全 1 子串个数
        return ans % MOD
```

```java [sol-Java]
class Solution {
    public int numSub(String s) {
        final int MOD = 1_000_000_007;
        long ans = 0;
        int last0 = -1;
        for (int i = 0; i < s.length(); i++) {
            char ch = s.charAt(i);
            if (ch == '0') {
                last0 = i; // 记录上个 0 的位置
            } else {
                ans += i - last0; // 右端点为 i 的全 1 子串个数
            }
        }
        return (int) (ans % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numSub(string s) {
        constexpr int MOD = 1'000'000'007;
        long long ans = 0;
        int last0 = -1;
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '0') {
                last0 = i; // 记录上个 0 的位置
            } else {
                ans += i - last0; // 右端点为 i 的全 1 子串个数
            }
        }
        return ans % MOD;
    }
};
```

```c [sol-C]
#define MOD 1000000007

int numSub(char* s) {
    long long ans = 0;
    int last0 = -1;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '0') {
            last0 = i; // 记录上个 0 的位置
        } else {
            ans += i - last0; // 右端点为 i 的全 1 子串个数
        }
    }
    return ans % MOD;
}
```

```go [sol-Go]
func numSub(s string) (ans int) {
	const mod = 1_000_000_007
	last0 := -1
	for i, ch := range s {
		if ch == '0' {
			last0 = i // 记录上个 0 的位置
		} else {
			ans += i - last0 // 右端点为 i 的全 1 子串个数
		}
	}
	return ans % mod
}
```

```js [sol-JavaScript]
var numSub = function(s) {
    const MOD = 1_000_000_007;
    let ans = 0, last0 = -1;
    for (let i = 0; i < s.length; i++) {
        if (s[i] === '0') {
            last0 = i; // 记录上个 0 的位置
        } else {
            ans += i - last0; // 右端点为 i 的全 1 子串个数
        }
    }
    return ans % MOD;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn num_sub(s: String) -> i32 {
        const MOD: i64 = 1_000_000_007;
        let mut ans = 0;
        let mut last0 = -1;
        for (i, ch) in s.bytes().enumerate() {
            if ch == b'0' {
                last0 = i as i32; // 记录上个 0 的位置
            } else {
                ans += (i as i32 - last0) as i64; // 右端点为 i 的全 1 子串个数
            }
        }
        (ans % MOD) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[2348. 全 0 子数组的数目](https://leetcode.cn/problems/number-of-zero-filled-subarrays/)

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

## 本地原创解析

### 1. 题意重述

本题来自 `六、分组循环`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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
    int maxVowels(string s, int k) {
        auto isVowel = [](char c) {
            return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        };
        int cur = 0, ans = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            cur += isVowel(s[i]);
            if (i >= k) cur -= isVowel(s[i - k]);
            if (i + 1 >= k) ans = max(ans, cur);
        }
        return ans;
    }
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

回到来源分类 `六、分组循环`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
