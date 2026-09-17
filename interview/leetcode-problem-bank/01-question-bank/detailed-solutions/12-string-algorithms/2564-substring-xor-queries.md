# 2564. 子字符串异或查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/substring-xor-queries/
- 题目 slug：`substring-xor-queries`
- 来源专题：字符串
- 来源分类路径：八、后缀数组/后缀自动机
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/substring-xor-queries/solutions/2107060/yu-chu-li-suo-you-s-zhong-de-shu-zi-by-e-yxl2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理 s 中的所有数字（Python/Java/C++/Go）](https://leetcode.cn/problems/substring-xor-queries/solutions/2107060/yu-chu-li-suo-you-s-zhong-de-shu-zi-by-e-yxl2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-suo-you-s-zhong-de-shu-zi-by-e-yxl2`
- topic id：`2107060`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 13:14:52 +0800

把等式 $\textit{val} \oplus \textit{first} = \textit{second}$ 两边同时异或 $\textit{first}$，得到

$$
\textit{val} \oplus \textit{first} \oplus \textit{first} = \textit{second} \oplus \textit{first}
$$

由于 $\textit{first} \oplus \textit{first} = 0$，因此上式化简为

$$
\textit{val} = \textit{second}\oplus \textit{first}
$$

所以问题等价于在 $s$ 中找到值为 $\textit{second}\oplus \textit{first}$ 的数。

由于 $10^9<2^{30}$，可以直接预处理 $s$ 中长度不超过 $30$ 的子串（转成数字）及其对应的 $\textit{left}$ 和 $\textit{right}$，记到一个哈希表中，然后 $\mathcal{O}(1)$ 地回答询问。

特别地，如果存在 $s[i]=0$，我们可以单独记录 $0$ 对应的 $\textit{left}=i,\ \textit{right}=i$，记录到哈希表中，这样预处理 $s$ 子串的时候，就只需要考虑从 $1$ 开始的子串，效率更高。

本题 [视频讲解](https://www.bilibili.com/video/BV1GY411i7RP/)。

```py [sol-Python3]
class Solution:
    def substringXorQueries(self, s: str, queries: List[List[int]]) -> List[List[int]]:
        n, m = len(s), {}
        if (i := s.find('0')) >= 0:
            m[0] = (i, i)

        for l, c in enumerate(s):
            if c == '0':
                continue
            x = 0
            for r in range(l, min(l + 30, n)):
                x = (x << 1) | (ord(s[r]) & 1)
                if x not in m:
                    m[x] = (l, r)

        NOT_FOUND = (-1, -1)
        return [m.get(x ^ y, NOT_FOUND) for x, y in queries]
```

```java [sol-Java]
class Solution {
    private static final int[] NOT_FOUND = new int[]{-1, -1};

    public int[][] substringXorQueries(String S, int[][] queries) {
        Map<Integer, int[]> m = new HashMap<>();
        int i = S.indexOf('0');
        if (i >= 0) {
            m.put(0, new int[]{i, i}); // 这样下面就可以直接跳过 '0' 了，效率更高
        }

        char[] s = S.toCharArray();
        int n = s.length;
        for (int l = 0; l < n; l++) {
            if (s[l] == '0') {
                continue;
            }
            for (int r = l, x = 0; r < Math.min(l + 30, n); r++) {
                x = (x << 1) | (s[r] & 1);
                m.putIfAbsent(x, new int[]{l, r});
            }
        }

        int[][] ans = new int[queries.length][];
        for (i = 0; i < queries.length; i++) {
            ans[i] = m.getOrDefault(queries[i][0] ^ queries[i][1], NOT_FOUND);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> substringXorQueries(string s, vector<vector<int>>& queries) {
        unordered_map<int, pair<int, int>> m;
        if (auto i = s.find('0'); i != string::npos) {
            m[0] = {i, i};
        }
        for (int l = 0, n = s.length(); l < n; l++) {
            if (s[l] == '0') {
                continue;
            }
            for (int r = l, x = 0; r < min(l + 30, n); r++) {
                x = (x << 1) | (s[r] & 1);
                if (!m.contains(x)) {
                    m[x] = {l, r};
                }
            }
        }

        vector<vector<int>> ans;
        for (auto& q : queries) {
            auto it = m.find(q[0] ^ q[1]);
            if (it == m.end()) {
                ans.push_back({-1, -1});
            } else {
                ans.push_back({it->second.first, it->second.second});
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func substringXorQueries(s string, queries [][]int) [][]int {
	type pair struct{ l, r int }
	m := map[int]pair{}
	if i := strings.IndexByte(s, '0'); i >= 0 {
		m[0] = pair{i, i}
	}

	for l, c := range s {
		if c == '0' {
			continue
		}
		for r, x := l, 0; r < l+30 && r < len(s); r++ {
			x = x<<1 | int(s[r]&1)
			if _, ok := m[x]; !ok {
				m[x] = pair{l, r}
			}
		}
	}

	ans := make([][]int, len(queries))
	notFound := []int{-1, -1} // 避免重复创建
	for i, q := range queries {
		p, ok := m[q[0]^q[1]]
		if !ok {
			ans[i] = notFound
		} else {
			ans[i] = []int{p.l, p.r}
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U + q)$，其中 $n$ 为 $s$ 的长度，$U=max(\textit{queries})$，$q$ 为 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\log U)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `八、后缀数组/后缀自动机`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、后缀数组/后缀自动机`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
