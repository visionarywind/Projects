# 3403. 从盒子中找出字典序最大的字符串 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-lexicographically-largest-string-from-the-box-i/
- 题目 slug：`find-the-lexicographically-largest-string-from-the-box-i`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1762
- 外部题解来源：https://leetcode.cn/problems/find-the-lexicographically-largest-string-from-the-box-i/solutions/3033286/mei-ju-zuo-duan-dian-tan-xin-pythonjavac-y2em/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举左端点/最大表示法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-the-lexicographically-largest-string-from-the-box-i/solutions/3033286/mei-ju-zuo-duan-dian-tan-xin-pythonjavac-y2em/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-zuo-duan-dian-tan-xin-pythonjavac-y2em`
- topic id：`3033286`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

为方便描述，下文把 $\textit{word}$ 简称为 $s$，把 $\textit{numFriends}$ 简称为 $k$。

## 题意

把 $s$ 分割为 $k$ 个非空子串，返回其中字典序最大的子串。

## 方法一：枚举子串左端点

如果固定子串的左端点，那么**子串越长，字典序越大**。

所以核心思路是：枚举子串的左端点，计算最大子串。

单个子串的长度不能超过多少？

由于其余 $k-1$ 个子串必须是非空的，取长度为 $1$，其余子串的长度之和**至少**为 $k-1$。

所以我们枚举的子串，长度**至多**为 $n-(k-1)$。

注意特判 $k=1$ 的情况，此时无法分割，子串左端点只能是 $0$，答案是 $s$。

```py [sol-Python3]
class Solution:
    def answerString(self, s: str, k: int) -> str:
        if k == 1:
            return s
        n = len(s)
        return max(s[i: i + n - k + 1] for i in range(n))
```

```java [sol-Java]
class Solution {
    public String answerString(String s, int k) {
        if (k == 1) {
            return s;
        }
        int n = s.length();
        String ans = "";
        for (int i = 0; i < n; i++) {
            String sub = s.substring(i, Math.min(i + n - k + 1, n));
            if (sub.compareTo(ans) > 0) {
                ans = sub;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string answerString(string s, int k) {
        if (k == 1) {
            return s;
        }
        int n = s.size();
        string ans;
        for (int i = 0; i < n; i++) {
            ans = max(ans, s.substr(i, n - max(k - 1, i)));
        }
        return ans;
    }
};
```

```go [sol-Go]
func answerString(s string, k int) (ans string) {
	if k == 1 {
		return s
	}
	n := len(s)
	for i := range n {
		ans = max(ans, s[i:min(i+n-k+1, n)])
	}
	return
}
```

```js [sol-JavaScript]
var answerString = function(s, k) {
    if (k === 1) {
        return s;
    }
    const n = s.length;
    let ans = "";
    for (let i = 0; i < n; i++) {
        const sub = s.substring(i, Math.min(i + n - k + 1, n));
        if (sub > ans) {
            ans = sub;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn answer_string(s: String, k: i32) -> String {
        if k == 1 {
            return s;
        }
        let n = s.len();
        (0..n).map(|i| &s[i..n.min(i + n - k as usize + 1)])
              .max()
              .unwrap()
              .to_string()
    }
}
```

```rust [sol-Rust 写法二]
impl Solution {
    pub fn answer_string(s: String, k: i32) -> String {
        if k == 1 {
            return s;
        }
        let n = s.len();
        let mut ans = "";
        for i in 0..n {
            let end = n.min(i + n - k as usize + 1);
            ans = ans.max(&s[i..end]);
        }
        ans.to_string()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(n-k))$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n-k)$ 或 $\mathcal{O}(1)$。Go 的切片不会发生拷贝，只需要 $\mathcal{O}(1)$ 额外空间。

## 方法二：计算字典序最大的后缀

也可以先把字典序最大的后缀算出来，然后取它的长度至多为 $n-k+1$ 的前缀，作为答案。

正确性证明。在比较字典序大小的过程中，如果当前的后缀比前面的某个后缀的字典序更大，那么：

- 如果两个后缀的首个不同字母的位置小于 $n-k+1$，那么应当更新答案的最大值。
- 如果两个后缀的首个不同字母的位置不小于 $n-k+1$，那么这两个后缀（只看长为 $n-k+1$ 的前缀）是一样的，即使更新答案，也不影响最终结果。

如何计算字典序最大的后缀，见 [1163. 按字典序排在最后的子串](https://leetcode.cn/problems/last-substring-in-lexicographical-order/)。

```py [sol-Python3]
class Solution:
    def answerString(self, s: str, numFriends: int) -> str:
        if numFriends == 1:
            return s
        n = len(s)
        i, j = 0, 1
        while j < n:
            k = 0
            while j + k < n and s[i + k] == s[j + k]:
                k += 1
            if j + k < n and s[i + k] < s[j + k]:
                i, j = j, max(j + 1, i + k + 1)
            else:
                j += k + 1
        return s[i: i + n - numFriends + 1]
```

```java [sol-Java]
class Solution {
    public String answerString(String s, int numFriends) {
        if (numFriends == 1) {
            return s;
        }
        int n = s.length();
        int i = 0;
        int j = 1;
        while (j < n) {
            int k = 0;
            while (j + k < n && s.charAt(i + k) == s.charAt(j + k)) {
                k++;
            }
            if (j + k < n && s.charAt(i + k) < s.charAt(j + k)) {
                int t = i;
                i = j;
                j = Math.max(j + 1, t + k + 1);
            } else {
                j += k + 1;
            }
        }
        return s.substring(i, Math.min(i + n - numFriends + 1, n));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string answerString(string s, int k) {
        if (k == 1) {
            return s;
        }
        int n = s.size();
        int i = 0, j = 1;
        while (j < n) {
            int k = 0;
            while (j + k < n && s[i + k] == s[j + k]) {
                k++;
            }
            if (j + k < n && s[i + k] < s[j + k]) {
                int t = i;
                i = j;
                j = max(j + 1, t + k + 1);
            } else {
                j += k + 1;
            }
        }
        return s.substr(i, n - max(k - 1, i));
    }
};
```

```go [sol-Go]
func answerString(s string, k int) string {
	if k == 1 {
		return s
	}
	n := len(s)
	i, j := 0, 1
	for j < n {
		k := 0
		for j+k < n && s[i+k] == s[j+k] {
			k++
		}
		if j+k < n && s[i+k] < s[j+k] {
			i, j = j, max(j+1, i+k+1)
		} else {
			j += k + 1
		}
	}
	return s[i:min(i+n-k+1, n)]
}
```

```go [sol-Go 后缀数组]
func answerString(s string, k int) string {
	if k == 1 {
		return s
	}
	sa := (*struct{_[]byte;sa[]int32})(unsafe.Pointer(suffixarray.New([]byte(s)))).sa
	n := len(s)
	i := int(sa[n-1])
	return s[i:min(i+n-k+1, n)]
}
```

```js [sol-JavaScript]
var answerString = function(s, numFriends) {
    if (numFriends === 1) {
        return s;
    }
    const n = s.length;
    let i = 0, j = 1;
    while (j < n) {
        let k = 0;
        while (j + k < n && s[i + k] === s[j + k]) {
            k++;
        }
        if (j + k < n && s[i + k] < s[j + k]) {
            const t = i;
            i = j;
            j = Math.max(j + 1, t + k + 1);
        } else {
            j += k + 1;
        }
    }
    return s.substring(i, Math.min(i + n - numFriends + 1, n));
};
```

```rust [sol-Rust]
impl Solution {
    pub fn answer_string(word: String, k: i32) -> String {
        if k == 1 {
            return word;
        }
        let n = word.len();
        let s = word.as_bytes();
        let mut i = 0;
        let mut j = 1;
        while j < n {
            let mut k = 0;
            while j + k < n && s[i + k] == s[j + k] {
                k += 1;
            }
            if j + k < n && s[i + k] < s[j + k] {
                let t = i;
                i = j;
                j = (j + 1).max(t + k + 1);
            } else {
                j += k + 1;
            }
        }
        let end = n.min(i + n - k as usize + 1);
        word[i..end].to_string()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

更多相似题目，见下面贪心题单中的「**§3.1 字典序最小/最大**」。

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

本题来自 `三、字符串贪心 / §3.1 字典序最小/最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、字符串贪心 / §3.1 字典序最小/最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
