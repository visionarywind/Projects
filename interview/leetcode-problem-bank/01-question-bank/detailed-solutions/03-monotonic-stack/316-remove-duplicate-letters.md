# 316. 去除重复字母

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-duplicate-letters/
- 题目 slug：`remove-duplicate-letters`
- 来源专题：单调栈
- 来源分类路径：四、最小字典序
- 难度分：2185
- 外部题解来源：https://leetcode.cn/problems/remove-duplicate-letters/solutions/2381483/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-zd6u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[跟着我过一遍示例 2，你就明白怎么做了（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-duplicate-letters/solutions/2381483/gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-zd6u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gen-zhao-wo-guo-yi-bian-shi-li-2ni-jiu-m-zd6u`
- topic id：`2381483`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

对于 $s=\texttt{cbacdcbc}$，从左到右遍历其中的字母。

1. $s[0]=\texttt{c}$。由于只遍历了一个字母，目前已知字典序最小的字符串是 $\texttt{c}$。
2. $s[1]=\texttt{b}$。如果右边没有字母 $\texttt{c}$，那么 $s[0]=\texttt{c}$ 必须保留；实际上右边还有字母 $\texttt{c}$，由于 $\texttt{b} < \texttt{c}$，我们可以去掉 $\texttt{c}$，改用 $\texttt{b}$ 当作目前字典序最小的字符串。
3. $s[2]=\texttt{a}$。同样的，由于 $\texttt{a} < \texttt{b}$ 且比右边还有字母 $\texttt{b}$，我们可以去掉 $\texttt{b}$，改用 $\texttt{a}$ 当作目前字典序最小的字符串（下面记作 $\textit{ans}$）。
4. $s[3]=\texttt{c}$。由于 $\texttt{c}$ 比 $\texttt{a}$ 大，只能添加到 $\texttt{a}$ 后面，现在 $\textit{ans}=\texttt{ac}$。
5. $s[4]=\texttt{d}$。由于 $\texttt{d}$ 比 $\texttt{c}$ 大，只能添加到 $\texttt{c}$ 后面，现在 $\textit{ans}=\texttt{acd}$。
6. $s[5]=\texttt{c}$。由于 $\texttt{acd}$ 里面已经有 $\texttt{c}$ 了，直接跳过。现在 $\textit{ans}=\texttt{acd}$。
7. $s[6]=\texttt{b}$。我们发现 $\texttt{b}$ 比 $\texttt{d}$ 小，能不能像上面 $s[1]$ 和 $s[2]$ 那样，去掉 $\texttt{d}$，添加 $\texttt{b}$ 呢？这是不行的，因为后面没有 $\texttt{d}$ 了，我们只能老老实实地添加到 $\texttt{d}$ 后面，现在 $\textit{ans}=\texttt{acdb}$。
8. $s[7]=\texttt{c}$。由于 $\texttt{acdb}$ 里面已经有 $\texttt{c}$ 了，直接跳过。

遍历完毕，我们得到了答案 $\textit{ans}=\texttt{acdb}$。

你可能会问，怎么知道右边是否还有某个字母 $x$？我们可以在遍历 $s$ 之前，先统计出每个字母的出现次数，记到一个哈希表或者数组 $\textit{left}$ 中。在遍历 $s$ 时，减少 $s[i]$ 的出现次数，也就是把 $\textit{left}[s[i]]$ 减一。如果发现 $\textit{left}[x]=0$ 就说明右边没有 $x$ 了。

具体算法如下：

1. 统计每个字母的出现次数，记到一个哈希表或者数组 $\textit{left}$ 中。
2. 遍历 $s$，先把 $\textit{left}[s[i]]$ 减一。
3. 如果 $s[i]$ 在 $\textit{ans}$ 中，直接 $\texttt{continue}$。为了快速判断 $s[i]$ 是否在 $\textit{ans}$ 中，可以用一个哈希表或者布尔数组 $\textit{inAns}$ 辅助判断。
4. 如果 $s[i]$ 不在 $\textit{ans}$ 中，那么判断 $s[i]$ 是否小于 $\textit{ans}$ 的最后一个字母（记作 $x$），如果 $s[i]<x$ 且 $\textit{left}[x]>0$，那么可以把 $x$ 从 $\textit{ans}$ 中去掉，同时标记 $\textit{inAns}[x]=\texttt{false}$。
5. 反复执行第 4 步，直到 $\textit{ans}$ 为空，或者 $s[i]>x$，或者 $\textit{left}[x]=0$。
6. 把 $s[i]$ 添加到 $\textit{ans}$ 末尾，同时标记 $\textit{inAns}[s[i]]=\texttt{true}$。然后继续遍历 $s$ 的下一个字母。
7. 遍历完 $s$ 后，返回 $\textit{ans}$。

```py [sol-Python3]
class Solution:
    def removeDuplicateLetters(self, s: str) -> str:
        left = Counter(s)  # 统计每个字母的出现次数
        ans = []  # 当作栈
        in_ans = set()
        for c in s:
            left[c] -= 1
            if c in in_ans:  # ans 中不能有重复字母
                continue
            # (设 x=ans[-1]) 如果 c < x，且右边还有 x，那么可以把 x 去掉，
            # 因为后面可以重新把 x 加到 ans 中
            while ans and c < ans[-1] and left[ans[-1]]:
                in_ans.remove(ans.pop())  # 标记栈顶不在 ans 中
            ans.append(c)  # 把 c 加到 ans 的末尾
            in_ans.add(c)  # 标记 c 在 ans 中
        return ''.join(ans)
```

```java [sol-Java]
class Solution {
    public String removeDuplicateLetters(String S) {
        char[] s = S.toCharArray();
        int[] left = new int[26];
        for (char c : s) {
            left[c - 'a']++; // 统计每个字母的出现次数
        }

        StringBuilder ans = new StringBuilder(26); // 当作栈
        boolean[] inAns = new boolean[26];
        for (char c : s) {
            left[c - 'a']--;
            if (inAns[c - 'a']) { // ans 中不能有重复字母
                continue;
            }
            // 设 x = ans.charAt(ans.length() - 1)，
            // 如果 c < x，且右边还有 x，那么可以把 x 去掉，因为后面可以重新把 x 加到 ans 中
            while (!ans.isEmpty() && c < ans.charAt(ans.length() - 1) && left[ans.charAt(ans.length() - 1) - 'a'] > 0) {
                inAns[ans.charAt(ans.length() - 1) - 'a'] = false; // 标记栈顶不在 ans 中
                ans.deleteCharAt(ans.length() - 1);
            }
            ans.append(c); // 把 c 加到 ans 的末尾
            inAns[c - 'a'] = true; // 标记 c 在 ans 中
        }
        return ans.toString();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    string removeDuplicateLetters(string s) {
        int left[26]{};
        for (char c : s) {
            left[c - 'a']++; // 统计每个字母的出现次数
        }

        string ans; // 当作栈
        bool in_ans[26]{};
        for (char c : s) {
            left[c - 'a']--;
            if (in_ans[c - 'a']) { // ans 中不能有重复字母
                continue;
            }
            while (!ans.empty() && c < ans.back() && left[ans.back() - 'a']) {
                // (设 x=ans.back()) 如果 c < x，且右边还有 x，那么可以把 x 去掉，
                // 因为后面可以重新把 x 加到 ans 中
                in_ans[ans.back() - 'a'] = false; // 标记栈顶不在 ans 中
                ans.pop_back();
            }
            ans += c; // 把 c 加到 ans 的末尾
            in_ans[c - 'a'] = true; // 标记 c 在 ans 中
        }
        return ans;
    }
};
```

```c [sol-C]
char* removeDuplicateLetters(char* s) {
    int left[26] = {};
    for (int i = 0; s[i]; i++) {
        left[s[i] - 'a']++; // 统计每个字母的出现次数
    }

    bool in_ans[26] = {};
    int top = -1; // 直接用 s 作为栈，top 为栈顶下标
    for (int i = 0; s[i]; i++) {
        char c = s[i];
        left[c - 'a']--;
        if (in_ans[c - 'a']) { // ans 中不能有重复字母
            continue;
        }
        while (top >= 0 && c < s[top] && left[s[top] - 'a'] > 0) {
            // (设 x=s[top]) 如果 c < x，且右边还有 x，那么可以把 x 去掉，
            // 因为后面可以重新把 x 加到 ans 中
            in_ans[s[top] - 'a'] = false; // 标记栈顶不在 ans 中
            top--;
        }
        s[++top] = c; // 把 c 加到 ans 的末尾
        in_ans[c - 'a'] = true; // 标记 c 在 ans 中
    }

    s[top + 1] = '\0';
    return s;
}
```

```go [sol-Go]
func removeDuplicateLetters(s string) string {
    left := ['z' + 1]int{} // 相比创建一个长为 26 的数组，多开一点空间更方便
    for _, c := range s {
        left[c]++ // 统计每个字母的出现次数
    }

    ans := []rune{} // 当作栈
    inAns := ['z' + 1]bool{}
    for _, c := range s {
        left[c]--
        if inAns[c] { // ans 中不能有重复字母
            continue
        }
        for len(ans) > 0 && c < ans[len(ans)-1] && left[ans[len(ans)-1]] > 0 {
            // 如果 c < x，且右边还有 x，那么可以把 x 去掉，因为后面可以重新把 x 加到 ans 中
            x := ans[len(ans)-1]
            ans = ans[:len(ans)-1]
            inAns[x] = false // 标记栈顶不在 ans 中
        }
        ans = append(ans, c) // 把 c 加到 ans 的末尾
        inAns[c] = true // 标记 c 在 ans 中
    }
    return string(ans)
}
```

```js [sol-JavaScript]
var removeDuplicateLetters = function(s) {
    const left = _.countBy(s); // 统计每个字母的出现次数
    const ans = [] // 当作栈
    const ansSet = new Set();
    for (const c of s) {
        left[c]--;
        if (ansSet.has(c)) { // ans 中不能有重复字母
            continue;
        }
        // 设 x = ans[ans.length-1]，
        // 如果 c < x，且右边还有 x，那么可以把 x 去掉，因为后面可以重新把 x 加到 ans 中
        while (ans && c < ans[ans.length - 1] && left[ans[ans.length - 1]]) {
            ansSet.delete(ans.pop()); // 标记栈顶不在 ans 中
        }
        ans.push(c); // 把 c 加到 ans 的末尾
        ansSet.add(c); // 标记 c 在 ans 中
    }
    return ans.join("");
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_duplicate_letters(s: String) -> String {
        let mut left = [0; 26];
        for c in s.bytes() {
            left[(c - b'a') as usize] += 1; // 统计每个字母的出现次数
        }

        let mut ans = vec![]; // 当作栈
        let mut in_ans = [false; 26];
        for c in s.bytes() {
            left[(c - b'a') as usize] -= 1;
            if in_ans[(c - b'a') as usize] { // ans 中不能有重复字母
                continue;
            }
            while let Some(&top) = ans.last() {
                if c > top || left[(top - b'a') as usize] == 0 {
                    break;
                }
                // (设 x=ans.last()) 如果 c < x，且右边还有 x，那么可以把 x 去掉，
                // 因为后面可以重新把 x 加到 ans 中
                in_ans[(top - b'a') as usize] = false; // 标记栈顶不在 ans 中
                ans.pop();
            }
            ans.push(c); // 把 c 加到 ans 的末尾
            in_ans[(c - b'a') as usize] = true; // 标记 c 在 ans 中
        }

        unsafe { String::from_utf8_unchecked(ans) }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $s$ 的长度。我们写了一个二重循环，看上去是 $\mathcal{O}(n^2)$ 的，但是考虑到每个 $s[i]$ 加到 $\textit{ans}$ 中至多一次，从 $\textit{ans}$ 中去掉也至多一次。所以整体上看，算法的时间复杂度是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$，其中 $|\Sigma|$ 为字符集的大小，本题中字符均为小写字母，所以 $|\Sigma|=26$。注意 $\textit{ans}$ 的长度不会超过 $|\Sigma|$。 

## 思考题

把问题改成：去掉尽量少的字母，且剩余的每种字母至多出现 $\textit{limit}$ 次。这里 $\textit{limit}$ 是额外输入的一个正整数（本题相当于 $\textit{limit}=1$）。

这题是力扣之前的比赛题 [天池-03. 整理书架](https://leetcode.cn/contest/tianchi2022/problems/ev2bru/)。

欢迎在评论区发表你的做法。

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

本题来自 `四、最小字典序`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、最小字典序`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
