# 763. 划分字母区间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/partition-labels/
- 题目 slug：`partition-labels`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.5 合并区间
- 难度分：1443
- 外部题解来源：https://leetcode.cn/problems/partition-labels/solutions/2806706/ben-zhi-shi-he-bing-qu-jian-jian-ji-xie-ygsn8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[本质是合并区间，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/partition-labels/solutions/2806706/ben-zhi-shi-he-bing-qu-jian-jian-ji-xie-ygsn8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ben-zhi-shi-he-bing-qu-jian-jian-ji-xie-ygsn8`
- topic id：`2806706`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

## 思路

「同一字母最多出现在一个片段中」意味着，一个片段若要包含字母 $\texttt{a}$，那么所有的字母 $\texttt{a}$ 都必须在这个片段中。

示例 1 的 $s=\texttt{ababcbacadefegdehijhklij}$，其中字母 $\texttt{a}$ 出现在下标 $0,2,6,8$ 上，那么包含 $\texttt{a}$ 的片段至少要包含区间 $[0,8]$。

把所有出现在 $s$ 中的字母及其下标区间列出来：

|  字母 | 下标 | 下标区间  |
|---|---|---|
|$\texttt{a}$| $0, 2, 6, 8$ | $[0,8]$|
|$\texttt{b}$| $1, 3, 5$ | $[1,5]$|
|$\texttt{c}$| $4, 7$ | $[4,7]$|
|$\texttt{d}$| $9, 14$ | $[9,14]$|
|$\texttt{e}$| $10, 12, 15$ | $[10,15]$|
|$\texttt{f}$| $11$ | $[11,11]$|
|$\texttt{g}$| $13$ | $[13,13]$|
|$\texttt{h}$| $16, 19$ | $[16,19]$|
|$\texttt{i}$| $17, 22$ | $[17,22]$|
|$\texttt{j}$| $18, 23$ | $[18,23]$|
|$\texttt{k}$| $20$ | $[20,20]$|
|$\texttt{l}$| $21$ | $[21,21]$|

例如字母 $\texttt{d}$ 的区间为 $[9,14]$，片段要包含 $\texttt{d}$，必须包含区间 $[9,14]$，但区间 $[9,14]$ 中还有其它字母 $\texttt{e},\texttt{f},\texttt{g}$，所以该片段也必须包含这些字母对应的区间 $[10,15],[11,11],[13,13]$，合并后得到区间 $[9,15]$。

将表格中的区间**合并**为如下几个大区间：

$$
[0,8],[9,15],[16,23]
$$

这些区间满足「同一字母最多出现在一个片段中」的要求，区间长度分别为 $9,7,8$。

由于题目要求划分出尽量多的片段，而我们又无法将上述区间的任何区间划分开，所以合并后的区间长度即为答案。

## 算法

1. 遍历 $s$，计算字母 $c$ 在 $s$ 中的最后出现的下标 $\textit{last}[c]$。
2. 初始化当前正在合并的区间左右端点 $\textit{start}=0,\ \textit{end}=0$。
3. 再次遍历 $s$，由于当前区间必须包含所有 $s[i]$，所以用 $\textit{last}[s[i]]$ 更新区间右端点 $\textit{end}$ 的最大值。
4. 如果发现 $\textit{end}=i$，那么当前区间合并完毕，把区间长度 $\textit{end}-\textit{start}+1$ 加入答案。然后更新 $\textit{start}=\textit{end}+1$ 作为下一个区间的左端点。
5. 遍历完毕，返回答案。

```py [sol-Python3]
class Solution:
    def partitionLabels(self, s: str) -> List[int]:
        last = {c: i for i, c in enumerate(s)}  # 每个字母最后出现的下标
        ans = []
        start = end = 0
        for i, c in enumerate(s):
            end = max(end, last[c])  # 更新当前区间右端点的最大值
            if end == i:  # 当前区间合并完毕
                ans.append(end - start + 1)  # 区间长度加入答案
                start = end + 1  # 下一个区间的左端点
        return ans
```

```java [sol-Java]
class Solution {
    public List<Integer> partitionLabels(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[] last = new int[26];
        for (int i = 0; i < n; i++) {
            last[s[i] - 'a'] = i; // 每个字母最后出现的下标
        }

        List<Integer> ans = new ArrayList<>();
        int start = 0, end = 0;
        for (int i = 0; i < n; i++) {
            end = Math.max(end, last[s[i] - 'a']); // 更新当前区间右端点的最大值
            if (end == i) { // 当前区间合并完毕
                ans.add(end - start + 1); // 区间长度加入答案
                start = end + 1; // 下一个区间的左端点
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> partitionLabels(string s) {
        int n = s.size();
        int last[26];
        for (int i = 0; i < n; i++) {
            last[s[i] - 'a'] = i; // 每个字母最后出现的下标
        }

        vector<int> ans;
        int start = 0, end = 0;
        for (int i = 0; i < n; i++) {
            end = max(end, last[s[i] - 'a']); // 更新当前区间右端点的最大值
            if (end == i) { // 当前区间合并完毕
                ans.push_back(end - start + 1); // 区间长度加入答案
                start = end + 1; // 下一个区间的左端点
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int* partitionLabels(char* s, int* returnSize) {
    int last[26];
    for (int i = 0; s[i]; i++) {
        last[s[i] - 'a'] = i; // 每个字母最后出现的下标
    }

    int* ans = malloc(26 * sizeof(int)); // 至多 26 个区间
    *returnSize = 0;

    int start = 0, end = 0;
    for (int i = 0; s[i]; i++) {
        end = MAX(end, last[s[i] - 'a']); // 更新当前区间右端点的最大值
        if (end == i) { // 当前区间合并完毕
            ans[(*returnSize)++] = end - start + 1; // 区间长度加入答案
            start = end + 1; // 下一个区间的左端点
        }
    }

    return ans;
}
```

```go [sol-Go]
func partitionLabels(s string) (ans []int) {
    last := [26]int{}
    for i, c := range s {
        last[c-'a'] = i // 每个字母最后出现的下标
    }

    start, end := 0, 0
    for i, c := range s {
        end = max(end, last[c-'a']) // 更新当前区间右端点的最大值
        if end == i { // 当前区间合并完毕
            ans = append(ans, end-start+1) // 区间长度加入答案
            start = end + 1 // 下一个区间的左端点
        }
    }
    return
}
```

```js [sol-JavaScript]
var partitionLabels = function(s) {
    const n = s.length;
    const last = Array(26);
    for (let i = 0; i < n; i++) {
        last[s.charCodeAt(i) - 'a'.charCodeAt(0)] = i; // 每个字母最后出现的下标
    }

    const ans = [];
    let start = 0, end = 0;
    for (let i = 0; i < n; i++) {
        end = Math.max(end, last[s.charCodeAt(i) - 'a'.charCodeAt(0)]); // 更新当前区间右端点的最大值
        if (end === i) { // 当前区间合并完毕
            ans.push(end - start + 1); // 区间长度加入答案
            start = end + 1; // 下一个区间的左端点
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn partition_labels(s: String) -> Vec<i32> {
        let n = s.len();
        let mut last = [0; 26];
        for (i, c) in s.bytes().enumerate() {
            last[(c - b'a') as usize] = i; // 每个字母最后出现的下标
        }

        let mut ans = vec![];
        let mut start = 0;
        let mut end = 0;
        for (i, c) in s.bytes().enumerate() {
            end = end.max(last[(c - b'a') as usize]); // 更新当前区间右端点的最大值
            if end == i { // 当前区间合并完毕
                ans.push((end - start + 1) as i32); // 区间长度加入答案
                start = end + 1; // 下一个区间的左端点
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。其中 $|\Sigma|$ 是字符集合的大小，本题字符均为小写字母，所以 $|\Sigma|=26$。

## 相似题目

- [56. 合并区间](https://leetcode.cn/problems/merge-intervals/)

更多相似题目，见下面贪心题单中的「**§2.5 合并区间**」。

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

本题来自 `二、区间贪心 / §2.5 合并区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.5 合并区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
