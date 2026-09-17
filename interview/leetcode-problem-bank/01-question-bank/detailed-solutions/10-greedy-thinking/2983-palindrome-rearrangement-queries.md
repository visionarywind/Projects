# 2983. 回文串重新排列查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindrome-rearrangement-queries/
- 题目 slug：`palindrome-rearrangement-queries`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2780
- 外部题解来源：https://leetcode.cn/problems/palindrome-rearrangement-queries/solutions/2585862/fen-lei-tao-lun-by-endlesscheng-jxg0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论（Python/Java/C++/Go）](https://leetcode.cn/problems/palindrome-rearrangement-queries/solutions/2585862/fen-lei-tao-lun-by-endlesscheng-jxg0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-by-endlesscheng-jxg0`
- topic id：`2585862`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

[本题视频讲解](https://www.bilibili.com/video/BV1XG411B7bX/)

把输入的字符串记作大写 $S$。为方便计算，把 $S$ 均分为左右两个字符串，其中右半部分反转。左半和右半分别记作 $s$ 和 $t$。

我们需要预处理三个前缀和：

- $s$ 的每个前缀的每个字符的出现次数，记作 $\textit{sumS}$。
- $t$ 的每个前缀的每个字符的出现次数，记作 $\textit{sumT}$。
- 前缀中 $s[i] \ne t[i]$ 的次数，记作 $\textit{sumNe}$（Not Equal）。

由于 $S$ 的右半部分反转了，所以询问中的 $c$ 和 $d$ 要替换成 $m-1-d$ 和 $m-1-c$，其中 $m$ 是 $S$ 的长度。

为方便描述，把 $a,b,c,d$ 改称为 $l_1, r_1, l_2, r_2$。

不失一般性，假设 $l_1 \le l_2$（如果不成立就交换），分类讨论：

- 首先，$[0,l_1-1]$ 和 $[\max(r_1,r_2)+1,n-1]$ 这两个区间，区间内的每个下标 $i$ 必须满足 $s[i]=t[i]$，这可以用 $\textit{sumNe}$ 判断。
- 如果**区间包含**，即 $r_2 \le r_1$。这是最简单的情况。由于在 $[l_1,r_1]$ 中 $s$ 可以随意排列，所以甚至不需要排列 $t$ 中的字符，我们只需要判断 $s$ 和 $t$ 在 $[l_1,r_1]$ 中每个字符的出现次数是否相等即可。
- 如果**区间不相交**，即 $r_1 < l_2$。先判断 $[r_1+1,l_2-1]$ 内的每个下标 $i$ 是否满足 $s[i]=t[i]$，不满足直接返回 `false`。然后算出 $s$ 的 $[l_1,r_1]$ 中每个字符的出现次数，这必须与 $t$ 中 $[l_1,r_1]$ 中每个字符的出现次数相等。同样地，$t$ 的 $[l_2,r_2]$ 中每个字符的出现次数，必须与 $s$ 中 $[l_2,r_2]$ 中每个字符的出现次数相等。
- 如果**区间相交但不包含**，即 $l_2 \le r_1$。先算出 $s$ 的 $[l_1,r_1]$ 中每个字符的出现次数，减去 $t$ 中 $[l_1,l_2-1]$ 中每个字符的出现次数。然后算出 $t$ 中 $[l_2,r2]$ 中每个字符的出现次数，减去 $s$ 中 $[r_1+1,r_2]$ 中每个字符的出现次数。最后判断出现次数不能为负数，且剩余的 $s$ 和 $t$ 中的字符出现次数必须相等。

```py [sol-Python3]
class Solution:
    def canMakePalindromeQueries(self, s: str, queries: List[List[int]]) -> List[bool]:
        # 分成左右两半，右半反转
        n = len(s) // 2
        t = s[n:][::-1]
        s = s[:n]

        # 预处理三种前缀和
        sum_s = [[0] * 26 for _ in range(n + 1)]
        for i, b in enumerate(s):
            sum_s[i + 1] = sum_s[i][:]
            sum_s[i + 1][ord(b) - ord('a')] += 1

        sum_t = [[0] * 26 for _ in range(n + 1)]
        for i, b in enumerate(t):
            sum_t[i + 1] = sum_t[i][:]
            sum_t[i + 1][ord(b) - ord('a')] += 1

        sum_ne = list(accumulate((x != y for x, y in zip(s, t)), initial=0))

        # 计算子串中各个字符的出现次数，闭区间 [l,r]
        def count(sum: List[List[int]], l: int, r: int) -> List[int]:
            return [x - y for x, y in zip(sum[r + 1], sum[l])]

        def subtract(s1: List[int], s2: List[int]) -> List[int]:
            for i, s in enumerate(s2):
                s1[i] -= s
                if s1[i] < 0:
                    return False
            return s1

        def check(l1: int, r1: int, l2: int, r2: int, sumS: List[List[int]], sumT: List[List[int]]) -> bool:
            # [0,l1-1] 有 s[i] != t[i] 或者 [max(r1,r2)+1,n-1] 有 s[i] != t[i]
            if sum_ne[l1] > 0 or sum_ne[n] - sum_ne[max(r1, r2) + 1] > 0:
                return False
            if r2 <= r1:  # 区间包含
                return count(sumS, l1, r1) == count(sumT, l1, r1)
            if r1 < l2:  # 区间不相交
                # [r1+1,l2-1] 都满足 s[i] == t[i]
                return sum_ne[l2] - sum_ne[r1 + 1] == 0 and \
                    count(sumS, l1, r1) == count(sumT, l1, r1) and \
                    count(sumS, l2, r2) == count(sumT, l2, r2)
            # 区间相交但不包含
            s1 = subtract(count(sumS, l1, r1), count(sumT, l1, l2 - 1))
            s2 = subtract(count(sumT, l2, r2), count(sumS, r1 + 1, r2))
            return s1 and s2 and s1 == s2

        ans = [False] * len(queries)
        for i, (l1, r1, c, d) in enumerate(queries):
            l2, r2 = n * 2 - 1 - d, n * 2 - 1 - c
            ans[i] = check(l1, r1, l2, r2, sum_s, sum_t) if l1 <= l2 else \
                     check(l2, r2, l1, r1, sum_t, sum_s)
        return ans
```

```java [sol-Java]
class Solution {
    public boolean[] canMakePalindromeQueries(String S, int[][] queries) {
        char[] s = S.toCharArray();
        int m = s.length;
        int n = m / 2;

        // 预处理三种前缀和
        int[][] sumS = new int[n + 1][26];
        for (int i = 0; i < n; i++) {
            sumS[i + 1] = sumS[i].clone();
            sumS[i + 1][s[i] - 'a']++;
        }

        int[][] sumT = new int[n + 1][26];
        for (int i = 0; i < n; i++) {
            sumT[i + 1] = sumT[i].clone();
            sumT[i + 1][s[m - 1 - i] - 'a']++;
        }

        int[] sumNe = new int[n + 1];
        for (int i = 0; i < n; i++) {
            sumNe[i + 1] = sumNe[i] + (s[i] != s[m - 1 - i] ? 1 : 0);
        }

        boolean[] ans = new boolean[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int[] q = queries[i];
            int l1 = q[0], r1 = q[1], l2 = m - 1 - q[3], r2 = m - 1 - q[2];
            ans[i] = l1 <= l2 ? check(l1, r1, l2, r2, sumS, sumT, sumNe) :
                                check(l2, r2, l1, r1, sumT, sumS, sumNe);
        }
        return ans;
    }

    private boolean check(int l1, int r1, int l2, int r2, int[][] sumS, int[][] sumT, int[] sumNe) {
        if (sumNe[l1] > 0 || // [0,l1-1] 有 s[i] != t[i]
            sumNe[sumNe.length - 1] - sumNe[Math.max(r1, r2) + 1] > 0) { // [max(r1,r2)+1,n-1] 有 s[i] != t[i]
            return false;
        }
        if (r2 <= r1) { // 区间包含
            return Arrays.equals(count(sumS, l1, r1), count(sumT, l1, r1));
        }
        if (r1 < l2) { // 区间不相交
            return sumNe[l2] - sumNe[r1 + 1] <= 0 && // [r1+1,l2-1] 都满足 s[i] == t[i]
                   Arrays.equals(count(sumS, l1, r1), count(sumT, l1, r1)) &&
                   Arrays.equals(count(sumS, l2, r2), count(sumT, l2, r2));
        }
        // 区间相交但不包含
        int[] s1 = subtract(count(sumS, l1, r1), count(sumT, l1, l2 - 1));
        int[] s2 = subtract(count(sumT, l2, r2), count(sumS, r1 + 1, r2));
        return s1 != null && s2 != null && Arrays.equals(s1, s2);
    }

    // 计算子串中各个字符的出现次数，闭区间 [l,r]
    private int[] count(int[][] sum, int l, int r) {
        int[] res = sum[r + 1].clone();
        for (int i = 0; i < 26; i++) {
            res[i] -= sum[l][i];
        }
        return res;
    }

    private int[] subtract(int[] s1, int[] s2) {
        for (int i = 0; i < 26; i++) {
            s1[i] -= s2[i];
            if (s1[i] < 0) {
                return null;
            }
        }
        return s1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> canMakePalindromeQueries(string s, vector<vector<int>> &queries) {
        // 分成左右两半，右半反转
        int n = s.length() / 2;
        string t = s.substr(n);
        reverse(t.begin(), t.end());

        // 预处理三种前缀和
        vector<vector<int>> sum_s(n + 1, vector<int>(26));
        for (int i = 0; i < n; i++) {
            sum_s[i + 1] = sum_s[i];
            sum_s[i + 1][s[i] - 'a']++;
        }

        vector<vector<int>> sum_t(n + 1, vector<int>(26));
        for (int i = 0; i < n; i++) {
            sum_t[i + 1] = sum_t[i];
            sum_t[i + 1][t[i] - 'a']++;
        }

        vector<int> sum_ne(n + 1);
        for (int i = 0; i < n; i++) {
            sum_ne[i + 1] = sum_ne[i] + (s[i] != t[i]);
        }

        // 计算子串中各个字符的出现次数，闭区间 [l,r]
        auto count = [](vector<vector<int>> &sum, int l, int r) -> vector<int> {
            auto res = sum[r + 1];
            for (int i = 0; i < 26; i++) {
                res[i] -= sum[l][i];
            }
            return res;
        };

        auto subtract = [](vector<int> s1, vector<int> s2) -> vector<int> {
            for (int i = 0; i < 26; i++) {
                s1[i] -= s2[i];
                if (s1[i] < 0) {
                    return {};
                }
            }
            return s1;
        };

        auto check = [&](int l1, int r1, int l2, int r2, vector<vector<int>> &sumS, vector<vector<int>> &sumT) -> bool {
            if (sum_ne[l1] > 0 || // [0,l1-1] 有 s[i] != t[i]
                sum_ne[n] - sum_ne[max(r1, r2) + 1] > 0) { // [max(r1,r2)+1,n-1] 有 s[i] != t[i]
                return false;
            }
            if (r2 <= r1) { // 区间包含
                return count(sumS, l1, r1) == count(sumT, l1, r1);
            }
            if (r1 < l2) { // 区间不相交
                return sum_ne[l2] - sum_ne[r1 + 1] == 0 && // [r1+1,l2-1] 都满足 s[i] == t[i]
                       count(sumS, l1, r1) == count(sumT, l1, r1) &&
                       count(sumS, l2, r2) == count(sumT, l2, r2);
            }
            // 区间相交但不包含
            auto s1 = subtract(count(sumS, l1, r1), count(sumT, l1, l2 - 1));
            auto s2 = subtract(count(sumT, l2, r2), count(sumS, r1 + 1, r2));
            return !s1.empty() && !s2.empty() && s1 == s2;
        };

        vector<bool> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            auto &q = queries[i];
            int l1 = q[0], r1 = q[1], l2 = n * 2 - 1 - q[3], r2 = n * 2 - 1 - q[2];
            ans[i] = l1 <= l2 ? check(l1, r1, l2, r2, sum_s, sum_t) :
                                check(l2, r2, l1, r1, sum_t, sum_s);
        }
        return ans;
    }
};
```

```go [sol-Go]
func canMakePalindromeQueries(s string, queries [][]int) []bool {
	// 分成左右两半，右半反转
	n := len(s) / 2
	t := []byte(s[n:])
	slices.Reverse(t)
	s = s[:n]

	// 预处理三种前缀和
	sumS := make([][26]int, n+1)
	for i, b := range s {
		sumS[i+1] = sumS[i]
		sumS[i+1][b-'a']++
	}

	sumT := make([][26]int, n+1)
	for i, b := range t {
		sumT[i+1] = sumT[i]
		sumT[i+1][b-'a']++
	}

	sumNe := make([]int, n+1)
	for i := range s {
		sumNe[i+1] = sumNe[i]
		if s[i] != t[i] {
			sumNe[i+1]++
		}
	}

	// 计算子串中各个字符的出现次数，闭区间 [l,r]
	count := func(sum [][26]int, l, r int) []int {
		res := sum[r+1]
		for i, s := range sum[l][:] {
			res[i] -= s
		}
		return res[:]
	}

	subtract := func(s1, s2 []int) []int {
		for i, s := range s2 {
			s1[i] -= s
			if s1[i] < 0 {
				return nil
			}
		}
		return s1
	}

	check := func(l1, r1, l2, r2 int, sumS, sumT [][26]int) bool {
		if sumNe[l1] > 0 || // [0,l1-1] 有 s[i] != t[i]
			sumNe[n]-sumNe[max(r1, r2)+1] > 0 { // [max(r1,r2)+1,n-1] 有 s[i] != t[i]
			return false
		}
		if r2 <= r1 { // 区间包含
			return slices.Equal(count(sumS, l1, r1), count(sumT, l1, r1))
		}
		if r1 < l2 { // 区间不相交
			return sumNe[l2]-sumNe[r1+1] == 0 && // [r1+1,l2-1] 都满足 s[i] == t[i]
				slices.Equal(count(sumS, l1, r1), count(sumT, l1, r1)) &&
				slices.Equal(count(sumS, l2, r2), count(sumT, l2, r2))
		}
		// 区间相交但不包含
		s1 := subtract(count(sumS, l1, r1), count(sumT, l1, l2-1))
		s2 := subtract(count(sumT, l2, r2), count(sumS, r1+1, r2))
		return s1 != nil && s2 != nil && slices.Equal(s1, s2)
	}

	ans := make([]bool, len(queries))
	for i, q := range queries {
		l1, r1, l2, r2 := q[0], q[1], n*2-1-q[3], n*2-1-q[2]
		if l1 <= l2 {
			ans[i] = check(l1, r1, l2, r2, sumS, sumT)
		} else {
			ans[i] = check(l2, r2, l1, r1, sumT, sumS)
		}
	}
	return ans
}
```

#### 复杂度分析
  
- 时间复杂度：$\mathcal{O}((n+q)|\Sigma|)$，其中 $n$ 为 $s$ 的长度，$q$ 为 $\textit{queries}$ 的长度，$|\Sigma|$ 为字符集合的大小，本题中字符均为小写字母，所以 $|\Sigma|=26$。回答每个询问的时间是 $\mathcal{O}(|\Sigma|)$。
- 空间复杂度：$\mathcal{O}(n|\Sigma|)$。返回值的空间不计入。

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
