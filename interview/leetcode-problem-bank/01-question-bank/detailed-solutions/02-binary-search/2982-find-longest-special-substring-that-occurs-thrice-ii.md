# 2982. 找出出现至少三次的最长特殊子字符串 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-longest-special-substring-that-occurs-thrice-ii/
- 题目 slug：`find-longest-special-substring-that-occurs-thrice-ii`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.2 求最大
- 难度分：1773
- 外部题解来源：https://leetcode.cn/problems/find-longest-special-substring-that-occurs-thrice-ii/solutions/2585801/fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-671l/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论，简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/find-longest-special-substring-that-occurs-thrice-ii/solutions/2585801/fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-671l/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-jian-ji-xie-fa-pythonjav-671l`
- topic id：`2585801`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于特殊子串只包含单一字母，我们按照相同字母分组，每组统计相同字母连续出现的长度。例如字符串 $\texttt{aaaabbbabb}$ 分成 $\texttt{aaaa}+\texttt{bbb}+\texttt{a}+\texttt{bb}$ 四组，其中字母 $\texttt{a}$ 有长度 $4$ 和长度 $1$，字母 $\texttt{b}$ 有长度 $3$ 和长度 $2$。所以字母 $\texttt{a}$ 的长度列表为 $[4,1]$，字母 $\texttt{b}$ 的长度列表为 $[3,2]$。

遍历每个字母对应的长度列表 $a$，把 $a$ 从大到小排序。

有哪些取出三个特殊子串的方法呢？

- 从最长的特殊子串（$a[0]$）中取三个长度均为 $a[0]-2$ 的特殊子串。例如示例 1 的 $\texttt{aaaa}$ 可以取三个 $\texttt{aa}$。
- 或者，从最长和次长的特殊子串（$a[0],a[1]$）中取三个长度一样的特殊子串：
  - 如果 $a[0]=a[1]$，那么可以取三个长度均为 $a[0]-1$ 的特殊子串。
  - 如果 $a[0]>a[1]$，那么可以取三个长度均为 $a[1]$ 的特殊子串：从最长中取两个，从次长中取一个。
  - 这两种情况可以合并成 $\min(a[0]-1, a[1])$，如果 $a[0]-1 < a[1]$，这只能是第一种情况，因为 $a[0]\ge a[1]$，我们取二者较小值 $a[0]-1$；如果 $a[0]-1\ge a[1]$，即 $a[0] > a[1]$，这是第二种情况，我们也取的是二者较小值 $a[1]$。
- 又或者，从最长、次长、第三长的的特殊子串（$a[0],a[1],a[2]$）中各取一个长为 $a[2]$ 的特殊子串。

这三种情况取最大值，即

$$
\max(a[0]-2, \min(a[0]-1, a[1]), a[2])
$$

对每个长度列表计算上式，取最大值即为答案。

如果答案是 $0$，返回 $-1$。

代码实现时，在数组末尾加两个 $0$，就无需特判 $a$ 长度小于 $3$ 的情况了。

附：[视频讲解](https://www.bilibili.com/video/BV1XG411B7bX?t=1m44s) 第二题。

```py [sol-Python3]
class Solution:
    def maximumLength(self, s: str) -> int:
        groups = defaultdict(list)
        cnt = 0
        for i, ch in enumerate(s):
            cnt += 1
            if i + 1 == len(s) or ch != s[i + 1]:
                groups[ch].append(cnt)  # 统计连续字符长度
                cnt = 0

        ans = 0
        for a in groups.values():
            a.sort(reverse=True)
            a.extend([0, 0])  # 假设还有两个空串
            ans = max(ans, a[0] - 2, min(a[0] - 1, a[1]), a[2])

        return ans if ans else -1
```

```java [sol-Java]
class Solution {
    public int maximumLength(String S) {
        char[] s = S.toCharArray();
        List<Integer>[] groups = new ArrayList[26];
        Arrays.setAll(groups, i -> new ArrayList<>());
        int cnt = 0;
        for (int i = 0; i < s.length; i++) {
            cnt++;
            if (i + 1 == s.length || s[i] != s[i + 1]) {
                groups[s[i] - 'a'].add(cnt); // 统计连续字符长度
                cnt = 0;
            }
        }

        int ans = 0;
        for (List<Integer> a : groups) {
            if (a.isEmpty()) continue;
            a.sort(Collections.reverseOrder());
            a.add(0);
            a.add(0); // 假设还有两个空串
            ans = Math.max(ans, Math.max(a.get(0) - 2, Math.max(Math.min(a.get(0) - 1, a.get(1)), a.get(2))));
        }

        return ans > 0 ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumLength(string s) {
        vector<int> groups[26];
        int cnt = 0, n = s.length();
        for (int i = 0; i < n; i++) {
            cnt++;
            if (i + 1 == n || s[i] != s[i + 1]) {
                groups[s[i] - 'a'].push_back(cnt); // 统计连续字符长度
                cnt = 0;
            }
        }

        int ans = 0;
        for (auto& a: groups) {
            if (a.empty()) continue;
            ranges::sort(a, greater());
            a.push_back(0);
            a.push_back(0); // 假设还有两个空串
            ans = max({ans, a[0] - 2, min(a[0] - 1, a[1]), a[2]});
        }

        return ans ? ans : -1;
    }
};
```

```go [sol-Go]
func maximumLength(s string) int {
    groups := [26][]int{}
    cnt := 0
    for i := range s {
        cnt++
        if i+1 == len(s) || s[i] != s[i+1] {
            groups[s[i]-'a'] = append(groups[s[i]-'a'], cnt) // 统计连续字符长度
            cnt = 0
        }
    }

    ans := 0
    for _, a := range groups {
        if len(a) == 0 {
            continue
        }
        slices.SortFunc(a, func(a, b int) int { return b - a })
        a = append(a, 0, 0) // 假设还有两个空串
        ans = max(ans, a[0]-2, min(a[0]-1, a[1]), a[2])
    }

    if ans == 0 {
        return -1
    }
    return ans
}
```

```js [sol-JavaScript]
var maximumLength = function(s) {
    const n = s.length;
    const groups = Array.from({ length: 26 }, () => []);
    let cnt = 0;
    for (let i = 0; i < n; i++) {
        cnt++;
        if (i + 1 === n || s[i] !== s[i + 1]) {
            groups[s[i].charCodeAt(0) - 'a'.charCodeAt(0)].push(cnt); // 统计连续字符长度
            cnt = 0;
        }
    }

    let ans = 0;
    for (let a of groups) {
        if (a.length === 0) {
            continue;
        }
        a.sort((x, y) => y - x);
        a.push(0, 0); // 假设还有两个空串
        ans = Math.max(ans, a[0] - 2, Math.min(a[0] - 1, a[1]), a[2]);
    }

    return ans ? ans : -1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_length(s: String) -> i32 {
        let mut groups = vec![vec![]; 26];
        let s = s.as_bytes();
        let mut cnt = 0;
        for (i, &c) in s.iter().enumerate() {
            cnt += 1;
            if i + 1 == s.len() || c != s[i + 1] {
                groups[(c - b'a') as usize].push(cnt); // 统计连续字符长度
                cnt = 0;
            }
        }

        let mut ans = 0;
        for a in groups.iter_mut() {
            if a.is_empty() {
                continue;
            }
            a.sort_unstable_by(|x, y| y.cmp(x));
            a.push(0);
            a.push(0); // 假设还有两个空串
            ans = ans.max(a[0] - 2).max(a[1].min(a[0] - 1)).max(a[2]);
        }

        if ans > 0 { ans } else { -1 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $s$ 的长度。如果改用堆维护前三大可以做到 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

额外输入一个正整数 $k$，把「至少三次」改成「至少 $k$ 次」，怎么做？

欢迎在评论区分享你的思路/代码。

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

本题来自 `二、二分答案 / §2.2 求最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.2 求最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
