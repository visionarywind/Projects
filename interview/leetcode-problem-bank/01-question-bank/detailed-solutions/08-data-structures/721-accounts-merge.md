# 721. 账户合并

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/accounts-merge/
- 题目 slug：`accounts-merge`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/accounts-merge/solutions/2844186/ha-xi-biao-dfspythonjavacgojsrust-by-end-z9nh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[哈希表 + DFS（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/accounts-merge/solutions/2844186/ha-xi-biao-dfspythonjavacgojsrust-by-end-z9nh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ha-xi-biao-dfspythonjavacgojsrust-by-end-z9nh`
- topic id：`2844186`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![lc721-cut.png](https://pic.leetcode.cn/1721006607-hZyyQx-lc721-cut.png)

看示例 1，把每个账户用其在 $\textit{accounts}$ 中的下标表示，即节点 $0$ 到 $3$。

同时，把每个账户的邮箱地址，也视作节点。我们在**账户下标**与其**邮箱地址**之间连边，得到一个**无向图**（二分图）。

题目相当于求出这个图的每个**连通块**，这可以用 **DFS** 解决。

### 算法

1. 把 $\textit{accounts}$ 中的信息提取到哈希表 $\textit{emailToIdx}$ 中，key 为邮箱地址，value 为这个邮箱对应的账户下标列表。
2. 初始化一个长为 $n$ 的全为 $\texttt{false}$ 的布尔数组 $\textit{vis}$，用来标记访问过的账户下标。
3. 遍历 $\textit{vis}$，如果 $i$ 没有访问过，即 $\textit{vis}[i]=\texttt{false}$，则从 $i$ 开始 DFS。
     1. DFS 之前，创建一个哈希集合 $\textit{emails}$，用来保存 DFS 中访问到的邮箱地址。
     2. 开始 DFS。首先标记 $\textit{vis}[i]=\texttt{true}$。
     3. 遍历 $\textit{accounts}[i]$ 的邮箱地址 $\textit{email}$。
     4. 如果 $\textit{email}$ 在哈希集合 $\textit{emails}$ 中，则跳过；否则把 $\textit{email}$ 加入哈希集合 $\textit{emails}$。
     5. 遍历 $\textit{emailToIdx}[\textit{email}]$，也就是所有包含该邮箱地址的账户下标 $j$，如果 $j$ 没有访问过，即 $\textit{vis}[j]=\texttt{false}$，则继续 DFS $j$。
4. DFS 结束后，把 $\textit{emails}$ 中的元素按照字典序从小到大排序，然后和 $\textit{accounts}[i][0]$ 一起加入答案。
5. 返回答案。

```py [sol-Python3]
class Solution:
    def accountsMerge(self, accounts: List[List[str]]) -> List[List[str]]:
        email_to_idx = defaultdict(list)
        for i, account in enumerate(accounts):
            for email in account[1:]:
                email_to_idx[email].append(i)

        def dfs(i: int) -> None:
            vis[i] = True
            for email in accounts[i][1:]:  # 遍历 i 的所有邮箱地址
                if email in email_set:
                    continue
                email_set.add(email)
                for j in email_to_idx[email]:  # 遍历所有包含该邮箱地址的账户下标 j
                    if not vis[j]:  # j 没有访问过
                        dfs(j)

        ans = []
        vis = [False] * len(accounts)
        for i, b in enumerate(vis):
            if not b:  # i 没有访问过
                email_set = set()  # 用于收集 DFS 中访问到的邮箱地址
                dfs(i)
                ans.append([accounts[i][0]] + sorted(email_set))
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<String>> accountsMerge(List<List<String>> accounts) {
        Map<String, List<Integer>> emailToIdx = new HashMap<>();
        for (int i = 0; i < accounts.size(); i++) {
            for (int k = 1; k < accounts.get(i).size(); k++) {
                emailToIdx.computeIfAbsent(accounts.get(i).get(k), x -> new ArrayList<>()).add(i);
            }
        }

        List<List<String>> ans = new ArrayList<>();
        boolean[] vis = new boolean[accounts.size()];
        Set<String> emailSet = new HashSet<>(); // 用于收集 DFS 中访问到的邮箱地址
        for (int i = 0; i < accounts.size(); i++) {
            if (vis[i]) {
                continue;
            }
            emailSet.clear();
            dfs(i, accounts, emailToIdx, vis, emailSet);

            List<String> res = new ArrayList<>(emailSet);
            Collections.sort(res);
            res.add(0, accounts.get(i).get(0));

            ans.add(res);
        }
        return ans;
    }

    private void dfs(int i, List<List<String>> accounts, Map<String, List<Integer>> emailToIdx, boolean[] vis, Set<String> emailSet) {
        vis[i] = true;
        for (int k = 1; k < accounts.get(i).size(); k++) { // 遍历 i 的所有邮箱地址
            String email = accounts.get(i).get(k);
            if (emailSet.contains(email)) {
                continue;
            }
            emailSet.add(email);
            for (int j : emailToIdx.get(email)) { // 遍历所有包含该邮箱地址的账户下标 j
                if (!vis[j]) { // j 没有访问过
                    dfs(j, accounts, emailToIdx, vis, emailSet);
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
        unordered_map<string, vector<int>> email_to_idx;
        for (int i = 0; i < accounts.size(); i++) {
            for (int k = 1; k < accounts[i].size(); k++) {
                email_to_idx[accounts[i][k]].push_back(i);
            }
        }

        unordered_set<string> email_set; // 用于收集 DFS 中访问到的邮箱地址
        vector<int> vis(accounts.size());
        auto dfs = [&](auto&& dfs, int i) -> void {
            vis[i] = true;
            for (int k = 1; k < accounts[i].size(); k++) { // 遍历 i 的所有邮箱地址
                string& email = accounts[i][k];
                if (email_set.contains(email)) {
                    continue;
                }
                email_set.insert(email);
                for (int j : email_to_idx[email]) { // 遍历所有包含该邮箱地址的账户下标 j
                    if (!vis[j]) { // j 没有访问过
                        dfs(dfs, j);
                    }
                }
            }
        };

        vector<vector<string>> ans;
        for (int i = 0; i < vis.size(); i++) {
            if (vis[i]) {
                continue;
            }
            email_set.clear();
            dfs(dfs, i);

            vector<string> res = {accounts[i][0]};
            res.insert(res.end(), email_set.begin(), email_set.end());
            sort(res.begin() + 1, res.end());

            ans.push_back(res);
        }
        return ans;
    }
};
```

```go [sol-Go]
func accountsMerge(accounts [][]string) (ans [][]string) {
    emailToIdx := map[string][]int{}
    for i, account := range accounts {
        for _, email := range account[1:] {
            emailToIdx[email] = append(emailToIdx[email], i)
        }
    }

    vis := make([]bool, len(accounts))
    emailSet := map[string]struct{}{} // 用于收集 DFS 中访问到的邮箱地址
    var dfs func(int)
    dfs = func(i int) {
        vis[i] = true
        for _, email := range accounts[i][1:] { // 遍历 i 的所有邮箱地址
            if _, has := emailSet[email]; has {
                continue
            }
            emailSet[email] = struct{}{}
            for _, j := range emailToIdx[email] { // 遍历所有包含该邮箱地址的账户下标 j
                if !vis[j] { // j 没有访问过
                    dfs(j)
                }
            }
        }
    }

    for i, b := range vis {
        if b {
            continue
        }
        clear(emailSet)
        dfs(i)

        res := make([]string, 1, len(emailSet)+1)
        res[0] = accounts[i][0]
        for email := range emailSet {
            res = append(res, email)
        }
        slices.Sort(res[1:])

        ans = append(ans, res)
    }
    return ans
}
```

```js [sol-JavaScript]
var accountsMerge = function(accounts) {
    const emailToIdx = new Map();
    for (let i = 0; i < accounts.length; i++) {
        for (let k = 1; k < accounts[i].length; k++) {
            const email = accounts[i][k];
            if (!emailToIdx.has(email)) {
                emailToIdx.set(email, []);
            }
            emailToIdx.get(email).push(i);
        }
    }

    const vis = Array(accounts.length).fill(false);
    const emailSet = new Set(); // 用于收集 DFS 中访问到的邮箱地址
    function dfs(i) {
        vis[i] = true;
        for (let k = 1; k < accounts[i].length; k++) { // 遍历 i 的所有邮箱地址
            const email = accounts[i][k];
            if (emailSet.has(email)) {
                continue;
            }
            emailSet.add(email);
            for (const j of emailToIdx.get(email)) { // 遍历所有包含该邮箱地址的账户下标 j
                if (!vis[j]) { // j 没有访问过
                    dfs(j);
                }
            }
        }
    }

    const ans = [];
    for (let i = 0; i < vis.length; i++) {
        if (!vis[i]) { // i 没有访问过
            emailSet.clear();
            dfs(i);
            ans.push([accounts[i][0], ...[...emailSet].sort()]);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::{HashMap, HashSet};

impl Solution {
    pub fn accounts_merge(accounts: Vec<Vec<String>>) -> Vec<Vec<String>> {
        let mut email_to_idx = HashMap::new();
        for (i, account) in accounts.iter().enumerate() {
            for email in account.iter().skip(1) {
                email_to_idx.entry(email.clone()).or_insert_with(Vec::new).push(i);
            }
        }

        fn dfs(i: usize, accounts: &Vec<Vec<String>>, email_to_idx: &HashMap<String, Vec<usize>>, vis: &mut Vec<bool>, email_set: &mut HashSet<String>) {
            vis[i] = true;
            for email in accounts[i].iter().skip(1) { // 遍历 i 的所有邮箱地址
                if email_set.contains(email) {
                    continue;
                }
                email_set.insert(email.clone());
                for &j in email_to_idx.get(email).unwrap() { // 遍历所有包含该邮箱地址的账户下标 j
                    if !vis[j] { // j 没有访问过
                        dfs(j, accounts, email_to_idx, vis, email_set);
                    }
                }
            }
        }

        let mut ans = vec![];
        let mut vis = vec![false; accounts.len()];
        for (i, account) in accounts.iter().enumerate() {
            if vis[i] {
                continue;
            }
            let mut email_set = HashSet::new(); // 用于收集 DFS 中访问到的邮箱地址
            dfs(i, &accounts, &email_to_idx, &mut vis, &mut email_set);

            let mut res = email_set.into_iter().collect::<Vec<_>>();
            res.sort_unstable();
            res.insert(0, account[0].clone());

            ans.push(res);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L\log M)$，其中 $L$ 是 $accounts$ 中所有字符串的长度之和，$M$ 是 $accounts$ 中的字符串个数。瓶颈在排序上，把 $\mathcal{O}(M)$ 个总长度为 $\mathcal{O}(L)$ 的字符串排序的时间复杂度为 $\mathcal{O}(L\log M)$。
- 空间复杂度：$\mathcal{O}(L)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
