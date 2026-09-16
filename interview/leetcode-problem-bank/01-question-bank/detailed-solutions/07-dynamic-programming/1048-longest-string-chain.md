# 1048. 最长字符串链

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-string-chain/
- 题目 slug：`longest-string-chain`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：1599
- 外部题解来源：https://leetcode.cn/problems/longest-string-chain/solutions/2247269/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-wdkm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划！（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-string-chain/solutions/2247269/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-wdkm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-wdkm`
- topic id：`2247269`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

## 方法一：记忆化搜索

对于字符串 $s$ 来说，假设它是词链的最后一个单词，那么去掉 $s$ 中的一个字母，设新字符串为 $t$，问题就变成计算以 $t$ 结尾的词链的最长长度。由于这是一个和原问题相似的子问题，因此可以用递归解决。

直接把字符串作为递归的参数，定义 $\textit{dfs}(s)$ 表示以 $s$ 结尾的词链的最长长度。由于字符串的长度不超过 $16$，暴力枚举去掉的字符，设新字符串为 $t$ 且在 $\textit{words}$ 中，则有

$$
\textit{dfs}(s) = \max\{\textit{dfs}(t)\} + 1
$$

为了快速判断字符串是否在 $\textit{words}$ 中，需要将所有 $\textit{words}[i]$ 存入哈希表 $\textit{ws}$ 中。

由于 $\texttt{"aba"}$ 和 $\texttt{"aca"}$ 去掉中间字母都会变成 $\texttt{"aa"}$，为避免重复计算，代码实现时可以用记忆化搜索。进一步地，可以直接把计算结果存到 $\textit{ws}$ 中。（Python 还是用 `@cache`）

```py [sol-Python3]
class Solution:
    def longestStrChain(self, words: List[str]) -> int:
        ws = set(words)
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(s: str) -> int:
            res = 0
            for i in range(len(s)):  # 枚举去掉 s[i]
                t = s[:i] + s[i + 1:]
                if t in ws:  # t 在 words 中
                    res = max(res, dfs(t))
            return res + 1
        return max(dfs(s) for s in ws)
```

```java [sol-Java]
class Solution {
    private Map<String, Integer> ws = new HashMap<>();

    public int longestStrChain(String[] words) {
        for (String s : words) {
            ws.put(s, 0); // 0 表示未被计算
        }
        int ans = 0;
        for (String s : ws.keySet()) {
            ans = Math.max(ans, dfs(s));
        }
        return ans;
    }

    private int dfs(String s) {
        int res = ws.get(s);
        if (res > 0) {
            return res; // 之前计算过
        }
        for (int i = 0; i < s.length(); i++) { // 枚举去掉 s[i]
            var t = s.substring(0, i) + s.substring(i + 1);
            if (ws.containsKey(t)) { // t 在 words 中
                res = Math.max(res, dfs(t));
            }
        }
        ws.put(s, res + 1); // 记忆化
        return res + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestStrChain(vector<string>& words) {
        unordered_map<string, int> ws;
        for (auto& s : words) {
            ws[s] = 0; // 0 表示未被计算
        }
        auto dfs = [&](this auto&& dfs, const string& s) -> int {
            int res = ws[s];
            if (res) {
                return res; // 之前计算过
            }
            for (int i = 0; i < s.length(); i++) { // 枚举去掉 s[i]
                auto t = s.substr(0, i) + s.substr(i + 1);
                if (ws.count(t)) { // t 在 words 中
                    res = max(res, dfs(t));
                }
            }
            return ws[s] = res + 1; // 记忆化
        };
        int ans = 0;
        for (auto& [s, _] : ws) {
            ans = max(ans, dfs(s));
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestStrChain(words []string) (ans int) {
    ws := map[string]int{}
    for _, s := range words {
        ws[s] = 0 // 0 表示未被计算
    }
    var dfs func(string) int
    dfs = func(s string) int {
        res := ws[s]
        if res > 0 { // 之前计算过
            return res
        }
        for i := range s { // 枚举去掉 s[i]
            t := s[:i] + s[i+1:]
            if _, ok := ws[t]; ok {
                res = max(res, dfs(t))
            }
        }
        ws[s] = res + 1 // 记忆化
        return res + 1
    }
    for s := range ws {
        ans = max(ans, dfs(s))
    }
    return
}
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(nL^2)$，其中 $n$ 为 $\textit{words}$ 的长度，$L$ 为字符串的最大长度，本题不超过 $16$。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(L^2)$，因此时间复杂度为 $\mathcal{O}(nL^2)$。
- 空间复杂度：$\mathcal{O}(nL)$。每个状态都需要 $\mathcal{O}(L)$ 的空间。

## 方法二：1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

对于本题，只需要把递归改成循环。

由于我们总是从短的字符串转移到长的字符串，所以要先把字符串按长度从小到大排序，然后从短的开始递推。

```py [sol-Python3]
class Solution:
    def longestStrChain(self, words: List[str]) -> int:
        words.sort(key=len)
        f = {}  # 不要用 defaultdict，这会在字符串不存在的时候插入字符串
        for s in words:
            res = 0
            for i in range(len(s)):  # 枚举去掉 s[i]
                res = max(res, f.get(s[:i] + s[i + 1:], 0))
            f[s] = res + 1
        return max(f.values())
```

```java [sol-Java]
class Solution {
    public int longestStrChain(String[] words) {
        Arrays.sort(words, (a, b) -> a.length() - b.length());
        int ans = 0;
        Map<String, Integer> f = new HashMap<>();
        for (String s : words) {
            int res = 0;
            for (int i = 0; i < s.length(); i++) { // 枚举去掉 s[i]
                String t = s.substring(0, i) + s.substring(i + 1);
                res = Math.max(res, f.getOrDefault(t, 0));
            }
            f.put(s, res + 1);
            ans = Math.max(ans, res + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestStrChain(vector<string>& words) {
        ranges::sort(words, {}, &string::length); // 按照长度从小到大排序
        int ans = 0;
        unordered_map<string, int> f;
        for (auto& s : words) {
            int res = 0;
            for (int i = 0; i < s.length(); i++) { // 枚举去掉 s[i]
                auto it = f.find(s.substr(0, i) + s.substr(i + 1));
                if (it != f.end()) {
                    res = max(res, it->second);
                }
            }
            f[s] = res + 1;
            ans = max(ans, f[s]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func longestStrChain(words []string) (ans int) {
    slices.SortFunc(words, func(a, b string) int { return len(a) - len(b) })
    f := map[string]int{}
    for _, s := range words {
        res := 0
        for i := range s { // 枚举去掉 s[i]
            res = max(res, f[s[:i]+s[i+1:]])
        }
        f[s] = res + 1
        ans = max(ans, res+1)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + nL^2)$，其中 $n$ 为 $\textit{words}$ 的长度，$L$ 为字符串的最大长度，本题不超过 $16$。排序的时间复杂度为 $\mathcal{O}(n\log n)$（注意只比较长度）。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(L^2)$，因此时间复杂度为 $\mathcal{O}(nL^2)$。总的时间复杂度为 $\mathcal{O}(n\log n + nL^2)$。
- 空间复杂度：$\mathcal{O}(nL)$。每个状态都需要 $\mathcal{O}(L)$ 的空间。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.4 合法子序列 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.4 合法子序列 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
