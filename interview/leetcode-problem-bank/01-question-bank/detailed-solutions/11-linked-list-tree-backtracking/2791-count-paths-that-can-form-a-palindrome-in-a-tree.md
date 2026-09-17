# 2791. 树中可以形成回文的路径数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/
- 题目 slug：`count-paths-that-can-form-a-palindrome-in-a-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.2 自顶向下 DFS
- 难度分：2677
- 外部题解来源：https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/solutions/2355288/yong-wei-yun-suan-chu-li-by-endlesscheng-n9ws/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[状态压缩+两数异或（Python/Java/C++/Go）](https://leetcode.cn/problems/count-paths-that-can-form-a-palindrome-in-a-tree/solutions/2355288/yong-wei-yun-suan-chu-li-by-endlesscheng-n9ws/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-wei-yun-suan-chu-li-by-endlesscheng-n9ws`
- topic id：`2355288`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

[视频讲解](https://www.bilibili.com/video/BV1n8411m7Fs/) 第四题。

## 前置知识：从集合论到位运算

请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

## 提示 1

回文串等价于至多一个字母出现奇数次，其余字母出现偶数次。

## 提示 2

用一个长为 $26$ 的二进制数来压缩存储每个字母的奇偶性。

一条边可以看成是 `1<<(s[i]-'a')`。

那么路径所对应的二进制数，就是路径上的所有边的异或和（因为异或就是模 $2$ 剩余系中的加法，刚好可以表示奇偶性）。

只有 $27$ 个二进制数符合要求：

- $0$，表示每个字母都出现偶数次。
- $2^0,2^1,\cdots,2^{25}$，表示第 $i$ 个字母出现奇数次，其余字母出现偶数次。

## 提示 3

设 $v$ 和 $w$ 的最近公共祖先为 $lca$，设从根到 $i$ 的路径异或和为 $\textit{XOR}_{i}$。

$v$ 到 $w$ 的路径可以看成是 $v-\textit{lca}-w$，其中 $\textit{lca}$ 到 $v$ 的路径异或和，等于根到 $\textit{v}$ 的异或和，再异或上根到 $\textit{lca}$ 的异或和（从根到 $\textit{lca}$ 的边异或了两次，等于 $0$ 抵消掉）。$\textit{lca}$ 到 $w$ 的路径异或和也同理。

所以 $v-\textit{lca}-w$ 的异或和为

$$
(\textit{XOR}_{v} \oplus \textit{XOR}_{lca}) \oplus (\textit{XOR}_{w} \oplus \textit{XOR}_{lca})
$$

$\textit{XOR}_{lca}$ 异或了两次，抵消掉，所以上式为

$$
\textit{XOR}_{v} \oplus \textit{XOR}_{w}
$$

把所有 $\textit{XOR}_i$ 求出来，就变成判断这 $n-1$ 个数当中：

- 两数异或和是否为 $0$？这意味着路径上的每个字母都出现偶数次。
- 两数异或和是否为 $2$ 的幂？这意味着路径上恰好有个字母出现奇数次，其余字母出现偶数次。
- 特殊情况：$\textit{XOR}_{i}=0$ 或者 $\textit{XOR}_{i}$ 为 $2$ 的幂，表示从根到 $i$ 的路径符合要求，我们可以异或上一条「空路径」对应的异或值，即 $0$，就转换成了上面两数异或和的情况。

回想一下力扣第一题的 [哈希表做法](https://leetcode.cn/problems/two-sum/solution/dong-hua-cong-liang-shu-zhi-he-zhong-wo-0yvmj/)，我们可以用哈希表记录 $\textit{XOR}_{i}$ 的个数，设当前算出的异或和为 $x$，去哈希表中找 $x$ 的出现次数以及 $x\oplus 2^k$ 的出现次数。

```py [sol-Python3]
class Solution:
    def countPalindromePaths(self, parent: List[int], s: str) -> int:
        n = len(s)
        g = [[] for _ in range(n)]
        for i in range(1, n):
            g[parent[i]].append(i)

        cnt = Counter([0])  # 一条「空路径」
        def dfs(v: int, xor: int) -> int:
            res = 0
            for w in g[v]:
                bit = 1 << (ord(s[w]) - ord('a'))
                x = xor ^ bit
                res += cnt[x] + sum(cnt[x ^ (1 << i)] for i in range(26))
                cnt[x] += 1
                res += dfs(w, x)
            return res
        return dfs(0, 0)
```

```java [sol-Java]
class Solution {
    public long countPalindromePaths(List<Integer> parent, String s) {
        int n = parent.size();
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int i = 1; i < n; i++) {
            int p = parent.get(i);
            g[p].add(i);
        }

        Map<Integer, Integer> cnt = new HashMap<>();
        cnt.put(0, 1); // 一条「空路径」
        return dfs(0, 0, g, s.toCharArray(), cnt);
    }

    private long dfs(int v, int xor, List<Integer>[] g, char[] s, Map<Integer, Integer> cnt) {
        long res = 0;
        for (int w : g[v]) {
            int x = xor ^ (1 << (s[w] - 'a'));
            res += cnt.getOrDefault(x, 0); // x ^ x = 0
            for (int i = 0; i < 26; i++) {
                res += cnt.getOrDefault(x ^ (1 << i), 0); // x ^ (x^(1<<i)) = 1<<i
            }
            cnt.merge(x, 1, Integer::sum);
            res += dfs(w, x, g, s, cnt);
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long countPalindromePaths(vector<int> &parent, string s) {
        int n = parent.size();
        vector<vector<int>> g(n);
        for (int i = 1; i < n; i++) {
            int p = parent[i];
            g[p].push_back(i);
        }

        long long ans = 0;
        unordered_map<int, int> cnt;
        cnt[0] = 1; // 一条「空路径」
        function<void(int, int)> dfs = [&](int v, int xor_) {
            for (int w: g[v]) {
                int x = xor_ ^ (1 << (s[w] - 'a'));
                ans += cnt.contains(x) ? cnt[x] : 0; // x ^ x = 0
                for (int i = 0; i < 26; i++) {
                    ans += cnt.contains(x ^ (1 << i)) ? cnt[x ^ (1 << i)] : 0; // x ^ (x^(1<<i)) = 1<<i
                }
                cnt[x]++;
                dfs(w, x);
            }
        };
        dfs(0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func countPalindromePaths(parent []int, s string) int64 {
	n := len(parent)
	g := make([][]int, n)
	for i := 1; i < n; i++ {
		p := parent[i]
		g[p] = append(g[p], i)
	}

	ans := 0
	cnt := map[int]int{0: 1} // 一条「空路径」
	var dfs func(int, int)
	dfs = func(v, xor int) {
		for _, w := range g[v] {
			x := xor ^ (1 << (s[w] - 'a'))
			ans += cnt[x] // x ^ x = 0
			for i := 0; i < 26; i++ {
				ans += cnt[x^(1<<i)] // x ^ (x^(1<<i)) = 1<<i
			}
			cnt[x]++
			dfs(w, x)
		}
	}
	dfs(0, 0)
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$，其中 $n$ 为 $s$ 的长度，$|\Sigma|$ 为字符集合的大小，本题中字符均为小写字母，所以 $|\Sigma|=26$。
- 空间复杂度：$\mathcal{O}(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.2 自顶向下 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.2 自顶向下 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
