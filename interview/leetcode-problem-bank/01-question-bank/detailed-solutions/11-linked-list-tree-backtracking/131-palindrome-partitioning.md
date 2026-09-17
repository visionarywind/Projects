# 131. 分割回文串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindrome-partitioning/
- 题目 slug：`palindrome-partitioning`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.3 划分型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/palindrome-partitioning/solutions/2059414/hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-fues/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】回溯不会写？套路在此！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/palindrome-partitioning/solutions/2059414/hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-fues/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-fues`
- topic id：`2059414`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 视频讲解

请看[【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)，制作不易，欢迎点赞关注~

视频介绍了两种基本回溯思想：**选或不选**、**枚举选哪个**。

## 方法一：输入的视角（逗号选或不选）

假设每对相邻字符之间有个逗号，那么就看每个逗号是选还是不选。

也可以理解成：是否要在 $i$ 和 $i+1$ 处分割，把 $s[i]$ 作为当前子串的最后一个字符，把 $s[i+1]$ 作为下一个子串的第一个字符。

注意 $s[n-1]$ 一定是最后一个字符，所以在 $i=n-1$ 的时候一定要分割。

#### 答疑

**问**：代码遇到回文串就加入 $\textit{path}$，如何保证这种分割方案一定合法？

**答**：如果最后一个字符串不是回文串，我们不会递归到 $i=n$ 的边界，不会把不合法的分割加入答案。

```py [sol-Python3]
class Solution:
    def partition(self, s: str) -> List[List[str]]:
        n = len(s)
        ans = []
        path = []

        # 现在 s 未被分割的部分为 [start, n-1]
        # 当前位于下标 i，讨论是否在 i 和 i+1 之间切一刀
        def dfs(i: int, start: int) -> None:
            if i == n:  # s 分割完毕
                ans.append(path.copy())  # 复制 path
                return

            # 不分割
            if i < n - 1:  # i=n-1 时必须分割（这是最后一段），i<n-1 时才可以不分割
                dfs(i + 1, start)

            # 分割，那么得到子串 [start, i]
            t = s[start: i + 1]
            if t == t[::-1]:  # 判断 t 是不是回文串
                path.append(t)
                # 现在 s 未被分割的部分为 [i+1, n-1]
                dfs(i + 1, i + 1)
                path.pop()  # 恢复现场

        dfs(0, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<String>> partition(String s) {
        List<List<String>> ans = new ArrayList<>();
        List<String> path = new ArrayList<>();
        dfs(0, 0, s, path, ans);
        return ans;
    }

    // 现在 s 未被分割的部分为 [start, n-1]
    // 当前位于下标 i，讨论是否在 i 和 i+1 之间切一刀
    private void dfs(int i, int start, String s, List<String> path, List<List<String>> ans) {
        if (i == s.length()) { // s 分割完毕
            ans.add(new ArrayList<>(path)); // 复制 path
            return;
        }

        // 不分割
        if (i < s.length() - 1) { // i=n-1 时必须分割（这是最后一段），i<n-1 时才可以不分割
            dfs(i + 1, start, s, path, ans);
        }

        // 分割，那么得到子串 [start, i]
        if (isPalindrome(s, start, i)) { // 判断子串 [start, i] 是不是回文串
            path.add(s.substring(start, i + 1));
            // 现在 s 未被分割的部分为 [i+1, n-1]
            dfs(i + 1, i + 1, s, path, ans);
            path.removeLast(); // path.remove(path.size() - 1);
        }
    }

    private boolean isPalindrome(String s, int left, int right) {
        while (left < right) {
            if (s.charAt(left++) != s.charAt(right--)) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
    bool is_palindrome(const string& s, int left, int right) {
        while (left < right) {
            if (s[left++] != s[right--]) {
                return false;
            }
        }
        return true;
    }

public:
    vector<vector<string>> partition(string s) {
        int n = s.size();
        vector<vector<string>> ans;
        vector<string> path;

        // 现在 s 未被分割的部分为 [start, n-1]
        // 当前位于下标 i，讨论是否在 i 和 i+1 之间切一刀
        auto dfs = [&](this auto&& dfs, int i, int start) {
            if (i == n) { // s 分割完毕
                ans.emplace_back(path);
                return;
            }

            // 不分割
            if (i < n - 1) { // i=n-1 时必须分割（这是最后一段），i<n-1 时才可以不分割
                dfs(i + 1, start);
            }

            // 分割，那么得到子串 [start, i]
            if (is_palindrome(s, start, i)) { // 判断子串 [start, i] 是不是回文串
                path.emplace_back(s.substr(start, i - start + 1));
                // 现在 s 未被分割的部分为 [i+1, n-1]
                dfs(i + 1, i + 1);
                path.pop_back(); // 恢复现场
            }
        };

        dfs(0, 0);
        return ans;
    }
};
```

```go [sol-Go]
func isPalindrome(s string) bool {
    n := len(s)
    for i := range n / 2 {
        if s[i] != s[n-1-i] {
            return false
        }
    }
    return true
}

func partition(s string) (ans [][]string) {
    n := len(s)
    path := []string{}

    // 现在 s 未被分割的部分为 [start, n-1]
    // 当前位于下标 i，讨论是否在 i 和 i+1 之间切一刀
    var dfs func(int, int)
    dfs = func(i, start int) {
        if i == n { // s 分割完毕
            ans = append(ans, slices.Clone(path))
            return
        }

        // 不分割
        if i < n-1 { // i=n-1 时必须分割（这是最后一段），i<n-1 时才可以不分割
            dfs(i+1, start)
        }

        // 分割，那么得到子串 [start, i]
        substr := s[start : i+1]
        if isPalindrome(substr) { // 判断子串 substr 是不是回文串
            path = append(path, substr)
            // 现在 s 未被分割的部分为 [i+1, n-1]
            dfs(i+1, i+1)
            path = path[:len(path)-1] // 恢复现场
        }
    }

    dfs(0, 0)
    return
}
```

```js [sol-JavaScript]
var isPalindrome = function(s, left, right) {
    while (left < right) {
        if (s.charAt(left++) !== s.charAt(right--)) {
            return false;
        }
    }
    return true;
}

var partition = function(s) {
    const n = s.length;
    const ans = [];
    const path = [];

    // 现在 s 未被分割的部分为 [start, n-1]
    // 当前位于下标 i，讨论是否在 i 和 i+1 之间切一刀
    function dfs(i, start) {
        if (i === n) { // s 分割完毕
            ans.push(path.slice()); // 复制 path
            return;
        }

        // 不分割
        if (i < n - 1) { // i=n-1 时必须分割（这是最后一段），i<n-1 时才可以不分割
            dfs(i + 1, start);
        }

        // 分割，那么得到子串 [start, i]
        if (isPalindrome(s, start, i)) { // 判断子串 [start, i] 是不是回文串
            path.push(s.substring(start, i + 1));
            // 现在 s 未被分割的部分为 [i+1, n-1]
            dfs(i + 1, i + 1);
            path.pop(); // 恢复现场
        }
    }

    dfs(0, 0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn partition(s: String) -> Vec<Vec<String>> {
        fn is_palindrome(s: &str) -> bool {
            let s = s.as_bytes();
            let n = s.len();
            for i in 0..n / 2 {
                if s[i] != s[n - 1 - i] {
                    return false;
                }
            }
            true
        }

        // 现在 s 未被分割的部分为 [start, n-1]
        // 当前位于下标 i，讨论是否在 i 和 i+1 之间切一刀
        fn dfs(i: usize, start: usize, s: &str, path: &mut Vec<String>, ans: &mut Vec<Vec<String>>) {
            if i == s.len() { // s 分割完毕
                ans.push(path.clone()); // 复制 path
                return;
            }

            // 不分割
            if i < s.len() - 1 { // i=n-1 时必须分割（这是最后一段），i<n-1 时才可以不分割
                dfs(i + 1, start, s, path, ans);
            }

            // 分割，那么得到子串 [start, i]
            let substr = &s[start..=i];
            if is_palindrome(substr) { // 判断 substr 是不是回文串
                path.push(substr.to_string());
                // 现在 s 未被分割的部分为 [i+1, n-1]
                dfs(i + 1, i + 1, s, path, ans);
                path.pop(); // 恢复现场
            }
        }

        let mut ans = vec![];
        let mut path = vec![];
        dfs(0, 0, &s, &mut path, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n2^n)$，其中 $n$ 为 $s$ 的长度。每次都是选或不选，递归次数为一个满二叉树的节点个数，那么一共会递归 $\mathcal{O}(2^n)$ 次（等比数列和），再算上判断回文和加入答案时需要 $\mathcal{O}(n)$ 的时间，所以时间复杂度为 $\mathcal{O}(n2^n)$。
- 空间复杂度：$\mathcal{O}(n)$。返回值的空间不计。

## 方法二：答案的视角（枚举子串结束位置）

```py [sol-Python3]
class Solution:
    def partition(self, s: str) -> List[List[str]]:
        n = len(s)
        ans = []
        path = []

        # 现在 s 未被分割的部分为 [i, n-1]
        # 枚举下一刀切在哪
        def dfs(i: int) -> None:
            if i == n:  # s 分割完毕
                ans.append(path.copy())  # 复制 path
                return
            for j in range(i, n):  # 枚举子串的结束位置
                t = s[i: j + 1]  # 分割出子串 t
                if t == t[::-1]:  # 判断 t 是不是回文串
                    path.append(t)
                    # 考虑剩余的 s[j+1:] 怎么分割
                    dfs(j + 1)
                    path.pop()  # 恢复现场

        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<String>> partition(String s) {
        List<List<String>> ans = new ArrayList<>();
        List<String> path = new ArrayList<>();
        dfs(0, s, path, ans);
        return ans;
    }

    // 现在 s 未被分割的部分为 [i, n-1]
    // 枚举下一刀切在哪
    private void dfs(int i, String s, List<String> path, List<List<String>> ans) {
        if (i == s.length()) { // s 分割完毕
            ans.add(new ArrayList<>(path)); // 复制 path
            return;
        }
        for (int j = i; j < s.length(); j++) { // 枚举子串的结束位置
            if (isPalindrome(s, i, j)) { // 判断 [i, j] 是不是回文串
                path.add(s.substring(i, j + 1)); // 分割！
                // 现在 s 未被分割的部分为 [j+1, n-1]
                dfs(j + 1, s, path, ans);
                path.removeLast(); // path.remove(path.size() - 1);
            }
        }
    }

    private boolean isPalindrome(String s, int left, int right) {
        while (left < right) {
            if (s.charAt(left++) != s.charAt(right--)) {
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
    bool isPalindrome(const string& s, int left, int right) {
        while (left < right) {
            if (s[left++] != s[right--]) {
                return false;
            }
        }
        return true;
    }

public:
    vector<vector<string>> partition(string s) {
        int n = s.size();
        vector<vector<string>> ans;
        vector<string> path;

        // 现在 s 未被分割的部分为 [i, n-1]
        // 枚举下一刀切在哪
        auto dfs = [&](this auto&& dfs, int i) {
            if (i == n) { // s 分割完毕
                ans.emplace_back(path);
                return;
            }
            for (int j = i; j < n; j++) { // 枚举子串的结束位置
                if (isPalindrome(s, i, j)) { // 判断 [i, j] 是不是回文串
                    path.push_back(s.substr(i, j - i + 1)); // 分割！
                    // 现在 s 未被分割的部分为 [j+1, n-1]
                    dfs(j + 1);
                    path.pop_back(); // 恢复现场
                }
            }
        };

        dfs(0);
        return ans;
    }
};
```

```go [sol-Go]
func isPalindrome(s string) bool {
    n := len(s)
    for i := range n / 2 {
        if s[i] != s[n-1-i] {
            return false
        }
    }
    return true
}

func partition(s string) (ans [][]string) {
    path := []string{}

    var dfs func(string)
    dfs = func(s string) {
        n := len(s)
        if n == 0 { // s 分割完毕
            ans = append(ans, slices.Clone(path))
            return
        }
        for i := 1; i <= n; i++ { // 枚举子串长度
            substr := s[:i]
            if isPalindrome(substr) { // 判断 substr 是不是回文串
                path = append(path, substr) // 分割！
                dfs(s[i:]) // 继续分割剩余部分
                path = path[:len(path)-1] // 恢复现场
            }
        }
    }

    dfs(s)
    return
}
```

```js [sol-JavaScript]
var isPalindrome = function(s, left, right) {
    while (left < right) {
        if (s.charAt(left++) !== s.charAt(right--)) {
            return false;
        }
    }
    return true;
}

var partition = function(s) {
    const n = s.length;
    const ans = [];
    const path = [];

    // 现在 s 未被分割的部分为 [i, n-1]
    // 枚举下一刀切在哪
    function dfs(i) {
        if (i === n) { // s 分割完毕
            ans.push(path.slice()); // 复制 path
            return;
        }
        for (let j = i; j < n; j++) { // 枚举子串的结束位置
            if (isPalindrome(s, i, j)) { // 判断 [i, j] 是不是回文串
                path.push(s.substring(i, j + 1)); // 分割！
                // 现在 s 未被分割的部分为 [j+1, n-1]
                dfs(j + 1);
                path.pop(); // 恢复现场
            }
        }
    }

    dfs(0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn partition(s: String) -> Vec<Vec<String>> {
        fn is_palindrome(s: &str) -> bool {
            let s = s.as_bytes();
            let n = s.len();
            for i in 0..n / 2 {
                if s[i] != s[n - 1 - i] {
                    return false;
                }
            }
            true
        }

        fn dfs(s: &str, path: &mut Vec<String>, ans: &mut Vec<Vec<String>>) {
            if s.is_empty() { // s 分割完毕
                ans.push(path.clone()); // 复制 path
                return;
            }
            for i in 1..=s.len() { // 枚举子串长度
                let substr = &s[..i];
                if is_palindrome(substr) { // 判断 substr 是不是回文串
                    path.push(substr.to_string()); // 分割！
                    dfs(&s[i..], path, ans); // 继续分割剩余部分
                    path.pop(); // 恢复现场
                }
            }
        }

        let mut ans = vec![];
        let mut path = vec![];
        dfs(&s, &mut path, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n2^n)$，其中 $n$ 为 $s$ 的长度。答案的长度至多为逗号子集的个数，即 $\mathcal{O}(2^n)$，因此会递归 $\mathcal{O}(2^n)$ 次，再算上判断回文和加入答案时需要 $\mathcal{O}(n)$ 的时间，所以时间复杂度为 $\mathcal{O}(n2^n)$。
- 空间复杂度：$\mathcal{O}(n)$。返回值的空间不计入。

## 专题训练

见下面回溯题单的「**§4.3 划分型回溯**」。

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

本题来自 `四、回溯 / §4.3 划分型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.3 划分型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
