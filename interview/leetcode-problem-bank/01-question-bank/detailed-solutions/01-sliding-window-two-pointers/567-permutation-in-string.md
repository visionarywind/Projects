# 567. 字符串的排列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/permutation-in-string/
- 题目 slug：`permutation-in-string`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/permutation-in-string/solutions/3747426/ding-chang-hua-dong-chuang-kou-cong-o26n-ulxp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[定长滑动窗口，从 O(26n) 到 O(n)（Python/Java/C++/Go）](https://leetcode.cn/problems/permutation-in-string/solutions/3747426/ding-chang-hua-dong-chuang-kou-cong-o26n-ulxp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ding-chang-hua-dong-chuang-kou-cong-o26n-ulxp`
- topic id：`3747426`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $s_1$ 的长度为 $m$。问题相当于：

- 判断 $s_2$ 中是否有长为 $m$ 的子串 $t$，满足 $t$ 中每种字母的出现次数均与 $s_1$ 的相同。如果相同，那么 $s_1$ 可以重排成 $t$。

这可以用**定长滑动窗口**解决，见[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

## 优化前

```py [sol-Python3]
class Solution:
    def checkInclusion(self, s1: str, s2: str) -> bool:
        m = len(s1)
        if m > len(s2):
            return False

        cnt_s1 = Counter(s1)  # 统计 s1 的每种字母的出现次数
        cnt_t = Counter()  # 对于 s2 的长为 m 的子串 t，统计 t 的每种字母的出现次数
        for i, c in enumerate(s2):
            # 1. 进入窗口
            cnt_t[c] += 1
            if i < m - 1:  # 窗口大小不足 m
                continue
            # 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if cnt_t == cnt_s1:
                return True
            # 3. 离开窗口，为下一个循环做准备
            cnt_t[s2[i - m + 1]] -= 1
        return False
```

```java [sol-Java]
class Solution {
    public boolean checkInclusion(String s1, String s2) {
        int m = s1.length();
        if (m > s2.length()) {
            return false;
        }

        // 统计 s1 的每种字母的出现次数
        int[] cntS1 = new int[26];
        for (char c : s1.toCharArray()) {
            cntS1[c - 'a']++;
        }

        char[] s = s2.toCharArray();
        int[] cntT = new int[26]; // 对于 s2 的长为 m 的子串 t，统计 t 的每种字母的出现次数
        for (int i = 0; i < s.length; i++) {
            // 1. 进入窗口
            cntT[s[i] - 'a']++;
            if (i < m - 1) { // 窗口大小不足 m
                continue;
            }
            // 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if (Arrays.equals(cntS1, cntT)) {
                return true;
            }
            // 3. 离开窗口，为下一个循环做准备
            cntT[s[i - m + 1] - 'a']--;
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool checkInclusion(string s1, string s2) {
        int m = s1.size();
        if (m > s2.size()) {
            return false;
        }

        // 统计 s1 的每种字母的出现次数
        array<int, 26> cnt_s1{};
        for (char c : s1) {
            cnt_s1[c - 'a']++;
        }

        array<int, 26> cnt_t{}; // 对于 s2 的长为 m 的子串 t，统计 t 的每种字母的出现次数
        for (int i = 0; i < s2.size(); i++) {
            // 1. 进入窗口
            cnt_t[s2[i] - 'a']++;
            if (i < m - 1) { // 窗口大小不足 m
                continue;
            }
            // 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if (cnt_s1 == cnt_t) {
                return true;
            }
            // 3. 离开窗口，为下一个循环做准备
            cnt_t[s2[i - m + 1] - 'a']--;
        }
        return false;
    }
};
```

```go [sol-Go]
func checkInclusion(s1, s2 string) bool {
    m := len(s1)
    if m > len(s2) {
        return false
    }

    // cntS1 统计 s1 的每种字母的出现次数
    cntS1 := [26]int{}
    for _, c := range s1 {
        cntS1[c-'a']++
    }

    cntT := [26]int{} // 对于 s2 的长为 m 的子串 t，统计 t 的每种字母的出现次数
    for i, c := range s2 {
        // 1. 进入窗口
        cntT[c-'a']++
        if i < m-1 { // 窗口大小不足 m
            continue
        }
        // 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
        if cntS1 == cntT {
            return true
        }
        // 3. 离开窗口，为下一个循环做准备
        cntT[s2[i-m+1]-'a']--
    }
    return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n|\Sigma|)$，其中 $n$ 是 $s_2$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 优化

把每次循环的 `cntS1 == cntT` 从 $\mathcal{O}(|\Sigma|)$ 优化成 $\mathcal{O}(1)$。

该方法只用到 $\textit{cntS}_1$，将其重命名为 $\textit{cnt}$。

用一个变量 $\textit{less}$ 维护 $t$ 中有 $\textit{less}$ 种字母的出现次数**小于** $s_1$ 中字母的出现次数。

- 一开始，$\textit{less}$ 等于 $s_1$ 中的不同字母个数。
- 字母进入窗口后，从 $\textit{cnt}$ 中减去字母的出现次数。如果减少之后是 $0$，说明 $t$ 中该字母的出现次数等于 $s_1$ 的，把 $\textit{less}$ 减一。
- 字母离开窗口后，从 $\textit{cnt}$ 中增加字母的出现次数。如果增加之前是 $0$，说明增加后，$t$ 中该字母的出现次数小于 $s_1$ 的，把 $\textit{less}$ 加一。

注意我们只需关心「小于」的个数。如果 $\textit{less}=0$，说明 $t$ 中每种字母的出现次数一定都恰好等于 $s_1$ 的。由于 $t$ 的长度等于 $s_1$ 的长度，当 $\textit{less}=0$ 时，不可能出现「大于」的情况，那样 $t$ 的长度就大于 $s_1$ 的长度了。

```py [sol-Python3]
class Solution:
    def checkInclusion(self, s1: str, s2: str) -> bool:
        m = len(s1)
        if m > len(s2):
            return False

        cnt = Counter(s1)
        for i, c in enumerate(s2):
            # 1. 进入窗口
            cnt[c] -= 1
            if cnt[c] == 0:
                del cnt[c]

            if i < m - 1:  # 窗口大小不足 m
                continue

            # 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if len(cnt) == 0:
                return True

            # 3. 离开窗口，为下一个循环做准备
            out = s2[i - m + 1]
            cnt[out] += 1
            if cnt[out] == 0:
                del cnt[out]
        return False
```


```py [sol-Python3 写法二]
class Solution:
    def checkInclusion(self, s1: str, s2: str) -> bool:
        m = len(s1)
        if m > len(s2):
            return False

        cnt = Counter(s1)
        less = len(cnt)
        for i, c in enumerate(s2):
            # 1. 进入窗口
            cnt[c] -= 1
            if cnt[c] == 0:
                less -= 1

            if i < m - 1:  # 窗口大小不足 m
                continue

            # 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if less == 0:
                return True

            # 3. 离开窗口，为下一个循环做准备
            out = s2[i - m + 1]
            if cnt[out] == 0:
                less += 1
            cnt[out] += 1
        return False
```

```java [sol-Java]
class Solution {
    public boolean checkInclusion(String s1, String s2) {
        int m = s1.length();
        if (m > s2.length()) {
            return false;
        }

        int[] cnt = new int[26];
        int less = 0;
        for (char c : s1.toCharArray()) {
            if (cnt[c - 'a'] == 0) {
                less++;
            }
            cnt[c - 'a']++;
        }

        char[] s = s2.toCharArray();
        for (int i = 0; i < s.length; i++) {
            // 1. 进入窗口
            int c = s[i] - 'a';
            cnt[c]--;
            if (cnt[c] == 0) {
                less--;
            }

            if (i < m - 1) { // 窗口大小不足 m
                continue;
            }

            // 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if (less == 0) {
                return true;
            }

            // 3. 离开窗口，为下一个循环做准备
            int out = s[i - m + 1] - 'a';
            if (cnt[out] == 0) {
                less++;
            }
            cnt[out]++;
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool checkInclusion(string s1, string s2) {
        int m = s1.size();
        if (m > s2.size()) {
            return false;
        }

        int cnt[26]{};
        int less = 0;
        for (char c : s1) {
            if (cnt[c - 'a'] == 0) {
                less++;
            }
            cnt[c - 'a']++;
        }

        for (int i = 0; i < s2.size(); i++) {
            // 1. 进入窗口
            int c = s2[i] - 'a';
            cnt[c]--;
            if (cnt[c] == 0) {
                less--;
            }

            if (i < m - 1) { // 窗口大小不足 m
                continue;
            }

            // 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
            if (less == 0) {
                return true;
            }

            // 3. 离开窗口，为下一个循环做准备
            int out = s2[i - m + 1] - 'a';
            if (cnt[out] == 0) {
                less++;
            }
            cnt[out]++;
        }
        return false;
    }
};
```

```go [sol-Go]
func checkInclusion(s1, s2 string) bool {
    m := len(s1)
    if m > len(s2) {
        return false
    }

    cnt := [26]int{}
    less := 0
    for _, c := range s1 {
        if cnt[c-'a'] == 0 {
            less++
        }
        cnt[c-'a']++
    }

    for i, c := range s2 {
        // 1. 进入窗口
        cnt[c-'a']--
        if cnt[c-'a'] == 0 {
            less--
        }

        if i < m-1 { // 窗口大小不足 m
            continue
        }

        // 2. 判断子串 t 的每种字母的出现次数是否均与 s1 的相同
        if less == 0 {
            return true
        }

        // 3. 离开窗口，为下一个循环做准备
        out := s2[i-m+1] - 'a'
        if cnt[out] == 0 {
            less++
        }
        cnt[out]++
    }
    return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(n + |\Sigma|)$，其中 $n$ 是 $s_2$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 专题训练

见下面滑动窗口题单的「**一、定长滑动窗口**」。

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

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
