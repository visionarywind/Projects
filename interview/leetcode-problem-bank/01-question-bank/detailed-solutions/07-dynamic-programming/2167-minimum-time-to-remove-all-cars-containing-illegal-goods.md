# 2167. 移除所有载有违禁货物车厢所需的最少时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-time-to-remove-all-cars-containing-illegal-goods/
- 题目 slug：`minimum-time-to-remove-all-cars-containing-illegal-goods`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.1 一维 DP
- 难度分：2219
- 外部题解来源：https://leetcode.cn/problems/minimum-time-to-remove-all-cars-containing-illegal-goods/solutions/1247626/qian-hou-zhui-fen-jie-dp-by-endlesscheng-6u1b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解 + DP（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-time-to-remove-all-cars-containing-illegal-goods/solutions/1247626/qian-hou-zhui-fen-jie-dp-by-endlesscheng-6u1b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-dp-by-endlesscheng-6u1b`
- topic id：`1247626`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 解法一：前后缀分解

#### 提示 1

考虑将列车分成左右两部分，枚举分割线，分别计算这两部分的最少时间。

#### 提示 2

考虑 DP。

![](https://pic.leetcode.cn/1644496698-FtlVnq-2167-1.drawio%20\(1\).png)

考虑左半部分的最少时间。

定义 $\textit{pre}[i]$ 表示移除从 $s[0]$ 到 $s[i]$ 的所有违禁货物车厢所花费的最少时间。

讨论 $s[i]$：

- 当 $s[i]=0$ 时，无需移除车厢，则有 $\textit{pre}[i]=\textit{pre}[i-1]$；
- 当 $s[i]=1$ 时，可以单独移除第 $i$ 节车厢，也可以移除前 $i$ 个车厢，二者取最小值，即 $\textit{pre}[i]=\min(\textit{pre}[i-1]+2,i+1)$。

对于右半部分，同样定义 $\textit{suf}[i]$ 表示移除从 $s[i]$ 到 $s[n-1]$ 的所有违禁货物车厢所花费的最少时间，有

$$
\textit{suf}[i] =
\begin{cases} 
\textit{suf}[i+1],&s[i]=0\\
\min(\textit{suf}[i+1]+2,n-i),&s[i]=1
\end{cases}
$$

然后枚举分割线，计算所有 $\textit{pre}[i]+\textit{suf}[i+1]$ 的最小值，即为答案。

---

代码实现时，有如下三处优化点：

优化 1：可以先计算 $\textit{suf}$，然后在枚举分割线的同时计算 $\textit{pre}$。

优化 2：由于计算 $\textit{pre}$ 的转移时当前状态只和上一个状态有关，因此可以使用滚动数组优化，即用一个变量来表示 $\textit{pre}$。

优化 3：由于 $s[i]=0$ 时，$\textit{pre}[i]$ 和 $\textit{suf}[i]$ 的值均不会变化，因此仅需要考虑 $s[i]=1$ 时的 $\textit{pre}[i]+\textit{suf}[i+1]$ 的最小值。

```py [sol1-Python3]
class Solution:
    def minimumTime(self, s: str) -> int:
        n = len(s)
        suf = [0] * (n + 1)
        for i in range(n - 1, -1, -1):
            suf[i] = suf[i + 1] if s[i] == '0' else min(suf[i + 1] + 2, n - i)
        ans = suf[0]
        pre = 0
        for i, ch in enumerate(s):
            if ch == '1':
                pre = min(pre + 2, i + 1)
                ans = min(ans, pre + suf[i + 1])
        return ans
```

```C++ [sol1-C++]
class Solution {
public:
    int minimumTime(string s) {
        int n = s.length();
        vector<int> suf(n + 1);
        for (int i = n - 1; i >= 0; --i)
            suf[i] = s[i] == '0' ? suf[i + 1] : min(suf[i + 1] + 2, n - i);
        int ans = suf[0], pre = 0;
        for (int i = 0; i < n; ++i)
            if (s[i] == '1') {
                pre = min(pre + 2, i + 1);
                ans = min(ans, pre + suf[i + 1]);
            }
        return ans;
    }
};
```

```java [sol1-Java]
class Solution {
    public int minimumTime(String s) {
        var n = s.length();
        var suf = new int[n + 1];
        for (var i = n - 1; i >= 0; --i)
            suf[i] = s.charAt(i) == '0' ? suf[i + 1] : Math.min(suf[i + 1] + 2, n - i);
        var ans = suf[0];
        var pre = 0;
        for (var i = 0; i < n; ++i)
            if (s.charAt(i) == '1') {
                pre = Math.min(pre + 2, i + 1);
                ans = Math.min(ans, pre + suf[i + 1]);
            }
        return ans;
    }
}
```

```go [sol1-Go]
func minimumTime(s string) int {
	n := len(s)
	suf := make([]int, n+1)
	for i := n - 1; i >= 0; i-- {
		if s[i] == '0' {
			suf[i] = suf[i+1]
		} else {
			suf[i] = min(suf[i+1]+2, n-i)
		}
	}
	ans := suf[0]
	pre := 0
	for i, ch := range s {
		if ch == '1' {
			pre = min(pre+2, i+1)
			ans = min(ans, pre+suf[i+1])
		}
	}
	return ans
}

func min(a, b int) int { if a > b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

### 解法二：进一步优化，一次遍历

由于我们计算的是「移除前缀 + 移除分割线左侧某些车厢 + (分割线) + 移除分割线右侧某些车厢 + 移除后缀」的最少花费，其中「移除分割线左侧某些车厢 + 移除分割线右侧某些车厢」都是在移除中间的某些车厢，因此这是可以合并的，不妨合并到分割线左侧，即计算「移除前缀 + 移除分割线左侧某些车厢 + (分割线) + 移除后缀」的最少花费。

![](https://pic.leetcode.cn/1644496724-UEaqty-2167-2.drawio.png)

合并后，计算 $\textit{pre}$ 的过程不变，而 $\textit{suf}$ 就仅为移除后缀所有车厢的花费了，这可以直接用下标计算出来。因此我们可以省略 $\textit{suf}$ 的计算流程，直接一次遍历计算出答案。

```py [sol2-Python3]
class Solution:
    def minimumTime(self, s: str) -> int:
        ans = n = len(s)
        pre = 0
        for i, ch in enumerate(s):
            if ch == '1':
                pre = min(pre + 2, i + 1)
            ans = min(ans, pre + n - 1 - i)
        return ans
```

```C++ [sol2-C++]
class Solution {
public:
    int minimumTime(string s) {
        int n = s.length(), ans = n, pre = 0;
        for (int i = 0; i < n; ++i) {
            if (s[i] == '1') pre = min(pre + 2, i + 1);
            ans = min(ans, pre + n - 1 - i);
        }
        return ans;
    }
};
```



```java [sol2-Java]
class Solution {
    public int minimumTime(String s) {
        var n = s.length();
        var ans = n;
        var pre = 0;
        for (var i = 0; i < n; ++i) {
            if (s.charAt(i) == '1') pre = Math.min(pre + 2, i + 1);
            ans = Math.min(ans, pre + n - 1 - i);
        }
        return ans;
    }
}
```

```go [sol2-Go]
func minimumTime(s string) int {
	n := len(s)
	ans := n
	pre := 0
	for i, ch := range s {
		if ch == '1' {
			pre = min(pre+2, i+1)
		}
		ans = min(ans, pre+n-1-i)
	}
	return ans
}

func min(a, b int) int { if a > b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 解法一：前后缀分解

#### 提示 1

考虑将列车分成左右两部分，枚举分割线，分别计算这两部分的最少时间。

#### 提示 2

考虑 DP。

![](https://pic.leetcode.cn/1644496698-FtlVnq-2167-1.drawio%20\(1\).png)

考虑左半部分的最少时间。

定义 $\textit{pre}[i]$ 表示移除从 $s[0]$ 到 $s[i]$ 的所有违禁货物车厢所花费的最少时间。

讨论 $s[i]$：

- 当 $s[i]=0$ 时，无需移除车厢，则有 $\textit{pre}[i]=\textit{pre}[i-1]$；
- 当 $s[i]=1$ 时，可以单独移除第 $i$ 节车厢，也可以移除前 $i$ 个车厢，二者取最小值，即 $\textit{pre}[i]=\min(\textit{pre}[i-1]+2,i+1)$。

对于右半部分，同样定义 $\textit{suf}[i]$ 表示移除从 $s[i]$ 到 $s[n-1]$ 的所有违禁货物车厢所花费的最少时间，有

$$
\textit{suf}[i] =
\begin{cases} 
\textit{suf}[i+1],&s[i]=0\\
\min(\textit{suf}[i+1]+2,n-i),&s[i]=1
\end{cases}
$$

然后枚举分割线，计算所有 $\textit{pre}[i]+\textit{suf}[i+1]$ 的最小值，即为答案。

---

代码实现时，有如下三处优化点：

优化 1：可以先计算 $\textit{suf}$，然后在枚举分割线的同时计算 $\textit{pre}$。

优化 2：由于计算 $\textit{pre}$ 的转移时当前状态只和上一个状态有关，因此可以使用滚动数组优化，即用一个变量来表示 $\textit{pre}$。

优化 3：由于 $s[i]=0$ 时，$\textit{pre}[i]$ 和 $\textit{suf}[i]$ 的值均不会变化，因此仅需要考虑 $s[i]=1$ 时的 $\textit{pre}[i]+\textit{suf}[i+1]$ 的最小值。

```py [sol1-Python3]
class Solution:
    def minimumTime(self, s: str) -> int:
        n = len(s)
        suf = [0] * (n + 1)
        for i in range(n - 1, -1, -1):
            suf[i] = suf[i + 1] if s[i] == '0' else min(suf[i + 1] + 2, n - i)
        ans = suf[0]
        pre = 0
        for i, ch in enumerate(s):
            if ch == '1':
                pre = min(pre + 2, i + 1)
                ans = min(ans, pre + suf[i + 1])
        return ans
```

```C++ [sol1-C++]
class Solution {
public:
    int minimumTime(string s) {
        int n = s.length();
        vector<int> suf(n + 1);
        for (int i = n - 1; i >= 0; --i)
            suf[i] = s[i] == '0' ? suf[i + 1] : min(suf[i + 1] + 2, n - i);
        int ans = suf[0], pre = 0;
        for (int i = 0; i < n; ++i)
            if (s[i] == '1') {
                pre = min(pre + 2, i + 1);
                ans = min(ans, pre + suf[i + 1]);
            }
        return ans;
    }
};
```

```java [sol1-Java]
class Solution {
    public int minimumTime(String s) {
        var n = s.length();
        var suf = new int[n + 1];
        for (var i = n - 1; i >= 0; --i)
            suf[i] = s.charAt(i) == '0' ? suf[i + 1] : Math.min(suf[i + 1] + 2, n - i);
        var ans = suf[0];
        var pre = 0;
        for (var i = 0; i < n; ++i)
            if (s.charAt(i) == '1') {
                pre = Math.min(pre + 2, i + 1);
                ans = Math.min(ans, pre + suf[i + 1]);
            }
        return ans;
    }
}
```

```go [sol1-Go]
func minimumTime(s string) int {
	n := len(s)
	suf := make([]int, n+1)
	for i := n - 1; i >= 0; i-- {
		if s[i] == '0' {
			suf[i] = suf[i+1]
		} else {
			suf[i] = min(suf[i+1]+2, n-i)
		}
	}
	ans := suf[0]
	pre := 0
	for i, ch := range s {
		if ch == '1' {
			pre = min(pre+2, i+1)
			ans = min(ans, pre+suf[i+1])
		}
	}
	return ans
}

func min(a, b int) int { if a > b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

### 解法二：进一步优化，一次遍历

由于我们计算的是「移除前缀 + 移除分割线左侧某些车厢 + (分割线) + 移除分割线右侧某些车厢 + 移除后缀」的最少花费，其中「移除分割线左侧某些车厢 + 移除分割线右侧某些车厢」都是在移除中间的某些车厢，因此这是可以合并的，不妨合并到分割线左侧，即计算「移除前缀 + 移除分割线左侧某些车厢 + (分割线) + 移除后缀」的最少花费。

![](https://pic.leetcode.cn/1644496724-UEaqty-2167-2.drawio.png)

合并后，计算 $\textit{pre}$ 的过程不变，而 $\textit{suf}$ 就仅为移除后缀所有车厢的花费了，这可以直接用下标计算出来。因此我们可以省略 $\textit{suf}$ 的计算流程，直接一次遍历计算出答案。

```py [sol2-Python3]
class Solution:
    def minimumTime(self, s: str) -> int:
        ans = n = len(s)
        pre = 0
        for i, ch in enumerate(s):
            if ch == '1':
                pre = min(pre + 2, i + 1)
            ans = min(ans, pre + n - 1 - i)
        return ans
```

```C++ [sol2-C++]
class Solution {
public:
    int minimumTime(string s) {
        int n = s.length(), ans = n, pre = 0;
        for (int i = 0; i < n; ++i) {
            if (s[i] == '1') pre = min(pre + 2, i + 1);
            ans = min(ans, pre + n - 1 - i);
        }
        return ans;
    }
};
```



```java [sol2-Java]
class Solution {
    public int minimumTime(String s) {
        var n = s.length();
        var ans = n;
        var pre = 0;
        for (var i = 0; i < n; ++i) {
            if (s.charAt(i) == '1') pre = Math.min(pre + 2, i + 1);
            ans = Math.min(ans, pre + n - 1 - i);
        }
        return ans;
    }
}
```

```go [sol2-Go]
func minimumTime(s string) int {
	n := len(s)
	ans := n
	pre := 0
	for i, ch := range s {
		if ch == '1' {
			pre = min(pre+2, i+1)
		}
		ans = min(ans, pre+n-1-i)
	}
	return ans
}

func min(a, b int) int { if a > b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.1 一维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.1 一维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
