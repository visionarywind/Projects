# 1963. 使字符串平衡的最小交换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-string-balanced/
- 题目 slug：`minimum-number-of-swaps-to-make-the-string-balanced`
- 来源专题：常用数据结构
- 来源分类路径：三、栈 / §3.4 合法括号字符串（RBS）
- 难度分：1689
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-string-balanced/solutions/922728/go-tan-xin-by-endlesscheng-7h9n/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心，三种写法，一步步优化（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-string-balanced/solutions/922728/go-tan-xin-by-endlesscheng-7h9n/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-tan-xin-by-endlesscheng-7h9n`
- topic id：`922728`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

考察平衡字符串的性质。

在平衡字符串的任意一个**前缀**中，左括号的数量都大于等于右括号的数量。例如平衡字符串 $\texttt{[[]][]}$，它的前缀有 $\texttt{[[]}$，$\texttt{[[]][}$ 等，都满足这一性质。为什么？因为对于**前缀**来说，每个右括号的左边，必然有与之匹配的左括号，但左括号不一定有与之匹配的右括号。

根据这一性质，从左到右遍历字符串 $s$，统计未匹配的左括号的个数 $c$：遇到左括号就把 $c$ 加一，遇到右括号就把 $c$ 减一。如果任何时刻 $c$ 都不为负数，那么 $s$ 就是平衡字符串。（注意题目保证左右括号个数相等，所以最终 $c$ 一定为 $0$。）

反之，如果遍历到右括号，且此时 $c=0$，那么减一后 $c$ 是负数，说明右括号比左括号多，必然存在一个右括号，没有相匹配的左括号，无论后面的字符是什么样的，$s$ 都不可能是平衡字符串。例如 $s=\texttt{[]][}$。

这时就需要把这个右括号换走了。和另一个右括号交换是没有意义的（$s$ 不变），所以一定要和左括号交换。如何交换最优呢？

根据 $c$ 的计算规则，把左括号视作 $+1$，右括号视作 $-1$。为了让 $c$ 不是负数，**执行 $-1$ 的时刻越晚（越靠后），$c$ 就越不可能变成负数**。

所以这个右括号要换到最后面去，也就是找最右边的左括号交换。

例如 $s=\texttt{][][}$：

- 如果把第一个右括号和第一个左括号交换，得 $\texttt{[]][}$，不是平衡字符串，仍需继续交换。
- 如果把第一个右括号和第二个左括号交换，得 $\texttt{[[]]}$，是平衡字符串。

### 优化前：模拟交换流程

```py [sol-Python3]
class Solution:
    def minSwaps(self, s: str) -> int:
        s = list(s)
        ans = c = 0
        j = len(s) - 1
        for b in s:
            if b == '[':
                c += 1
            elif c > 0:
                c -= 1
            else:  # c == 0
                # 找最右边的左括号交换
                while s[j] == ']':
                    j -= 1
                s[j] = ']'  # s[i] = '[' 可以省略
                ans += 1
                c += 1  # s[i] 变成左括号，c 加一
        return ans
```

```java [sol-Java]
class Solution {
    public int minSwaps(String S) {
        char[] s = S.toCharArray();
        int ans = 0;
        int c = 0;
        int j = s.length - 1;
        for (char b : s) {
            if (b == '[') {
                c++;
            } else if (c > 0) {
                c--;
            } else { // c == 0
                // 找最右边的左括号交换
                while (s[j] == ']') {
                    j--;
                }
                s[j] = ']'; // s[i] = '[' 可以省略
                ans++;
                c++; // s[i] 变成左括号，c 加一
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSwaps(string s) {
        int ans = 0, c = 0;
        int j = s.size() - 1;
        for (char b : s) {
            if (b == '[') {
                c++;
            } else if (c > 0) {
                c--;
            } else { // c == 0
                // 找最右边的左括号交换
                while (s[j] == ']') {
                    j--;
                }
                s[j] = ']'; // s[i] = '[' 可以省略
                ans++;
                c++; // s[i] 变成左括号，c 加一
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int minSwaps(char* s) {
    int n = strlen(s);
    int ans = 0, c = 0;
    int j = n - 1;
    for (int i = 0; i < n; i++) {
        if (s[i] == '[') {
            c++;
        } else if (c > 0) {
            c--;
        } else { // c == 0
            // 找最右边的左括号交换
            while (s[j] == ']') {
                j--;
            }
            s[j] = ']'; // s[i] = '[' 可以省略
            ans++;
            c++; // s[i] 变成左括号，c 加一
        }
    }
    return ans;
}
```

```go [sol-Go]
func minSwaps(S string) (ans int) {
	s := []byte(S)
	c := 0
	j := len(s) - 1
	for _, b := range s {
		if b == '[' {
			c++
		} else if c > 0 {
			c--
		} else { // c == 0
			// 找最右边的左括号交换
			for s[j] == ']' {
				j--
			}
			s[j] = ']' // s[i] = '[' 可以省略
			ans++
			c++ // s[i] 变成左括号，c 加一
		}
	}
	return
}
```

```js [sol-JavaScript]
var minSwaps = function(s) {
    s = s.split('');
    let ans = 0, c = 0;
    let j = s.length - 1;
    for (const b of s) {
        if (b === '[') {
            c++;
        } else if (c > 0) {
            c--;
        } else { // c === 0
            // 找最右边的左括号交换
            while (s[j] === ']') {
                j--;
            }
            s[j] = ']'; // s[i] = '[' 可以省略
            ans++;
            c++; // s[i] 变成左括号，c 加一
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_swaps(mut s: String) -> i32 {
        let mut s = unsafe { s.as_bytes_mut() };
        let mut ans = 0;
        let mut c = 0;
        let mut j = s.len() - 1;
        for i in 0..s.len() {
            if s[i] == b'[' {
                c += 1;
            } else if c > 0 {
                c -= 1;
            } else { // c == 0
                // 找最右边的左括号交换
                while s[j] == b']' {
                    j -= 1;
                }
                s[j] = b']'; // s[i] = '[' 可以省略
                ans += 1;
                c += 1; // s[i] 变成左括号，c 加一
            }
        }
        ans
    }
}
```

### 优化一：不需要真的交换

不需要写交换括号的逻辑。

如果不交换，继续向后遍历，若在下标 $i$ 处遇到了（本该被交换的）左括号，那么在执行了交换的字符串中，$[i,n-1]$ 中的左括号全部被换成了右括号，即此时该字符串已经是平衡的了，继续遍历不会导致 $c<0$，不会继续增大答案，所以**不交换并不会导致计算错误**。

因此，当遍历到右括号且 $c=0$ 时，只需将 $c$ 和答案（交换次数）加一，即视作将后面的一个左括号与该右括号交换。

示例 2 的 $s=\texttt{]]][[[}$，第一、第三个右括号与后两个左括号交换，得 $\texttt{[][[]]}$。但实际上我们只是把第一、第三个右括号视作左括号，没有真的交换，所以看上去遍历的是 $\texttt{[][[[[}$，最终 $c=4$。但这并不会导致计算错误，正如上文所说，如果遍历到了这些（本该被交换的）左括号，那么后面 $c$ 不会再变成负数，我们不会继续增大答案。

```py [sol-Python3]
class Solution:
    def minSwaps(self, s: str) -> int:
        ans = c = 0
        for b in s:
            if b == '[':
                c += 1
            elif c > 0:
                c -= 1
            else:  # c == 0
                ans += 1
                c += 1  # s[i] 变成左括号，c 加一
        return ans
```

```java [sol-Java]
class Solution {
    public int minSwaps(String s) {
        int ans = 0;
        int c = 0;
        for (char b : s.toCharArray()) {
            if (b == '[') {
                c++;
            } else if (c > 0) {
                c--;
            } else { // c == 0
                ans++;
                c++; // s[i] 变成左括号，c 加一
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSwaps(string s) {
        int ans = 0, c = 0;
        for (char b : s) {
            if (b == '[') {
                c++;
            } else if (c > 0) {
                c--;
            } else { // c == 0
                ans++;
                c++; // s[i] 变成左括号，c 加一
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int minSwaps(char* s) {
    int ans = 0, c = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '[') {
            c++;
        } else if (c > 0) {
            c--;
        } else { // c == 0
            ans++;
            c++; // s[i] 变成左括号，c 加一
        }
    }
    return ans;
}
```

```go [sol-Go]
func minSwaps(s string) (ans int) {
	c := 0
	for _, b := range s {
		if b == '[' {
			c++
		} else if c > 0 {
			c--
		} else { // c == 0
			ans++
			c++ // s[i] 变成左括号，c 加一
		}
	}
	return
}
```

```js [sol-JavaScript]
var minSwaps = function(s) {
    let ans = 0, c = 0;
    for (const b of s) {
        if (b === '[') {
            c++;
        } else if (c > 0) {
            c--;
        } else { // c === 0
            ans++;
            c++; // s[i] 变成左括号，c 加一
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_swaps(s: String) -> i32 {
        let mut ans = 0;
        let mut c = 0;
        for b in s.bytes() {
            if b == b'[' {
                c += 1;
            } else if c > 0 {
                c -= 1;
            } else { // c == 0
                ans += 1;
                c += 1; // s[i] 变成左括号，c 加一
            }
        }
        ans
    }
}
```

### 优化二：去掉 ans 变量

在前文的例子中，$\texttt{[][[[[}$ 的后两个括号本应是右括号，本应把 $c$ 执行两次 $-1$，最终得到 $c=0$，但我们反而执行了两次 $+1$，最终得到 $c=4$。换句话说，每次交换都会让最终的 $c$ 增大 $2$（把 $-1$ 改成了 $+1$），所以最终的 $c$ 除以 $2$，便是交换次数。

```py [sol-Python3]
class Solution:
    def minSwaps(self, s: str) -> int:
        c = 0
        for b in s:
            if b == '[' or c == 0:
                c += 1
            else:
                c -= 1
        return c // 2
```

```java [sol-Java]
class Solution {
    public int minSwaps(String s) {
        int c = 0;
        for (char b : s.toCharArray()) {
            if (b == '[' || c == 0) {
                c++;
            } else {
                c--;
            }
        }
        return c / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSwaps(string s) {
        int c = 0;
        for (char b : s) {
            if (b == '[' || c == 0) {
                c++;
            } else {
                c--;
            }
        }
        return c / 2;
    }
};
```

```c [sol-C]
int minSwaps(char* s) {
    int c = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '[' || c == 0) {
            c++;
        } else {
            c--;
        }
    }
    return c / 2;
}
```

```go [sol-Go]
func minSwaps(s string) int {
	c := 0
	for _, b := range s {
		if b == '[' || c == 0 {
			c++
		} else {
			c--
		}
	}
	return c / 2
}
```

```js [sol-JavaScript]
var minSwaps = function(s) {
    let c = 0;
    for (const b of s) {
        if (b === '[' || c === 0) {
            c++;
        } else {
            c--;
        }
    }
    return c / 2;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_swaps(s: String) -> i32 {
        let mut c = 0;
        for b in s.bytes() {
            if b == b'[' || c == 0 {
                c += 1;
            } else {
                c -= 1;
            }
        }
        c / 2
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

更多相似题目，见下面数据结构题单中的「**§3.4 合法括号字符串**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、栈 / §3.4 合法括号字符串（RBS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、栈 / §3.4 合法括号字符串（RBS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
