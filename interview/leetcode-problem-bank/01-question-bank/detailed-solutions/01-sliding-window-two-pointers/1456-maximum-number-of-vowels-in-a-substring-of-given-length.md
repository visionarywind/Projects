# 1456. 定长子串中元音的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/
- 题目 slug：`maximum-number-of-vowels-in-a-substring-of-given-length`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.1 基础
- 难度分：1263
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo`
- topic id：`2809359`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思想

我们要计算所有长度**恰好**为 $k$ 的子串中，最多可以包含多少个元音字母。

暴力枚举所有子串？时间复杂度是 $\mathcal{O}(nk)$，太慢了。能否 $\mathcal{O}(1)$ 计算子串的元音个数？

这是可以做到的，对于下图的字符串 $\texttt{abci}$，假如我们已经计算出了子串 $\texttt{abc}$ 的元音个数，那么从子串 $\texttt{abc}$ 到子串 $\texttt{bci}$，只需要考虑移除（离开窗口）的字母 $\texttt{a}$ 是不是元音，以及添加（进入窗口）的字母 $\texttt{i}$ 是不是元音即可，因为中间的字母 $\texttt{b}$ 和 $\texttt{c}$ 都在这两个子串中。

![lc1456.png](https://pic.leetcode.cn/1718242805-sqSjqc-lc1456.png)

## 举例

示例 1，$s = \texttt{abciiidef},\ k = 3$。

1. 从左到右遍历 $s$。
2. 首先统计前 $k-1=2$ 个字母的元音个数，这有 $1$ 个。
3. $s[2]=\texttt{c}$ 进入窗口，此时找到了第一个长为 $k$ 的子串 $\texttt{abc}$，现在元音个数有 $1$ 个，更新答案最大值。然后 $s[0]=\texttt{a}$ 离开窗口，现在元音个数有 $0$ 个。
4. $s[3]=\texttt{i}$ 进入窗口，此时找到了第二个长为 $k$ 的子串 $\texttt{bci}$，现在元音个数有 $1$ 个，更新答案最大值。然后 $s[1]=\texttt{b}$ 离开窗口，现在元音个数有 $1$ 个。
5. $s[4]=\texttt{i}$ 进入窗口，此时找到了第三个长为 $k$ 的子串 $\texttt{cii}$，现在元音个数有 $2$ 个，更新答案最大值。然后 $s[2]=\texttt{c}$ 离开窗口，现在元音个数有 $2$ 个。
6. $s[5]=\texttt{i}$ 进入窗口，此时找到了第四个长为 $k$ 的子串 $\texttt{iii}$，现在元音个数有 $3$ 个，更新答案最大值。然后 $s[3]=\texttt{i}$ 离开窗口，现在元音个数有 $2$ 个。
7. $s[6]=\texttt{d}$ 进入窗口，此时找到了第五个长为 $k$ 的子串 $\texttt{iid}$，现在元音个数有 $2$ 个，更新答案最大值。然后 $s[4]=\texttt{i}$ 离开窗口，现在元音个数有 $1$ 个。
8. $s[7]=\texttt{e}$ 进入窗口，此时找到了第六个长为 $k$ 的子串 $\texttt{ide}$，现在元音个数有 $2$ 个，更新答案最大值。然后 $s[5]=\texttt{i}$ 离开窗口，现在元音个数有 $1$ 个。
9. $s[8]=\texttt{f}$ 进入窗口，此时找到了第七个长为 $k$ 的子串 $\texttt{def}$，现在元音个数有 $1$ 个，更新答案最大值。遍历结束。

## 定长滑窗套路

窗口右端点在 $i$ 时，由于窗口长度为 $k$，所以窗口左端点为 $i-k+1$。

我总结成三步：**入-更新答案-出**。

1. **入**：下标为 $i$ 的元素进入窗口，更新相关统计量（本题为窗口内的元音个数）。如果窗口左端点 $i-k+1<0$，说明尚未形成第一个窗口，重复第一步。
2. **更新答案**：此时窗口长度为 $k$，符合题目要求，用相关统计量（本题为窗口内的元音个数）更新答案。
3. **出**：下标为 $i-k+1$ 的元素离开窗口，更新相关统计量（本题为窗口内的元音个数），为下一个循环做准备。**注**：元素离开窗口后，窗口长度为 $k-1$。下一轮循环元素进入窗口后，窗口长度为 $k$。

以上三步适用于所有定长滑窗题目。

## 答疑

**问**：为什么窗口右端点为 $i$ 的时候，左端点是 $i-k+1$？

**答**：对于窗口（闭区间）$[L,R]$ 来说，$[L,R]$ 里面的元素个数为 $R-L+1$。比如 $[2,5]$ 里面有 $2,3,4,5$ 一共 $5-2+1=4$ 个数。如果窗口长度为 $k$，即 $R-L+1=k$，解得 $L=R-k+1$。所以右端点为 $R$ 的时候，左端点为 $R-k+1$。

```py [sol-Python3]
class Solution:
    def maxVowels(self, s: str, k: int) -> int:
        ans = vowel = 0
        for i, c in enumerate(s):  # 枚举窗口右端点 i
            # 1. 右端点进入窗口
            if c in "aeiou":
                vowel += 1

            left = i - k + 1  # 窗口左端点
            if left < 0:  # 窗口长度不足 k，尚未形成第一个窗口
                continue

            # 2. 更新答案
            ans = max(ans, vowel)

            # 3. 左端点离开窗口，为下一个循环做准备
            if s[left] in "aeiou":
                vowel -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maxVowels(String S, int k) {
        char[] s = S.toCharArray();
        int ans = 0;
        int vowel = 0;
        for (int i = 0; i < s.length; i++) { // 枚举窗口右端点 i
            // 1. 右端点进入窗口
            if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u') {
                vowel++;
            }

            int left = i - k + 1; // 窗口左端点
            if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            ans = Math.max(ans, vowel);

            // 3. 左端点离开窗口，为下一个循环做准备
            char out = s[left];
            if (out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u') {
                vowel--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxVowels(string s, int k) {
        int ans = 0, vowel = 0;
        for (int i = 0; i < s.size(); i++) { // 枚举窗口右端点 i
            // 1. 右端点进入窗口
            if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u') {
                vowel++;
            }

            int left = i - k + 1; // 窗口左端点
            if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            ans = max(ans, vowel);

            // 3. 左端点离开窗口，为下一个循环做准备
            char out = s[left];
            if (out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u') {
                vowel--;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxVowels(char* s, int k) {
    int ans = 0, vowel = 0;
    for (int i = 0; s[i]; i++) { // 枚举窗口右端点 i
        // 1. 右端点进入窗口
        if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u') {
            vowel++;
        }

        int left = i - k + 1; // 窗口左端点
        if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
            continue;
        }

        // 2. 更新答案
        ans = MAX(ans, vowel);

        // 3. 左端点离开窗口，为下一个循环做准备
        char out = s[left];
        if (out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u') {
            vowel--;
        }
    }
    return ans;
}
```

```go [sol-Go]
func maxVowels(s string, k int) (ans int) {
    vowel := 0
    for i, in := range s { // 枚举窗口右端点 i
        // 1. 右端点进入窗口
        if in == 'a' || in == 'e' || in == 'i' || in == 'o' || in == 'u' {
            vowel++
        }

        left := i - k + 1 // 窗口左端点
        if left < 0 { // 窗口长度不足 k，尚未形成第一个窗口
            continue
        }

        // 2. 更新答案
        ans = max(ans, vowel)

        // 3. 左端点离开窗口，为下一个循环做准备
        out := s[left]
        if out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u' {
            vowel--
        }
    }
    return
}
```

```js [sol-JavaScript]
var maxVowels = function(s, k) {
    let ans = 0, vowel = 0;
    for (let i = 0; i < s.length; i++) { // 枚举窗口右端点 i
        // 1. 右端点进入窗口
        if (s[i] === 'a' || s[i] === 'e' || s[i] === 'i' || s[i] === 'o' || s[i] === 'u') {
            vowel++;
        }

        const left = i - k + 1; // 窗口左端点
        if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
            continue;
        }

        // 2. 更新答案
        ans = Math.max(ans, vowel);

        // 3. 左端点离开窗口，为下一个循环做准备
        let out = s[left];
        if (out === 'a' || out === 'e' || out === 'i' || out === 'o' || out === 'u') {
            vowel--;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_vowels(s: String, k: i32) -> i32 {
        let s = s.as_bytes();
        let k = k as usize;
        let mut ans = 0;
        let mut vowel = 0;
        for (i, &c) in s.iter().enumerate() { // 枚举窗口右端点 i
            // 1. 右端点进入窗口
            if c == b'a' || c == b'e' || c == b'i' || c == b'o' || c == b'u' {
                vowel += 1;
            }
            if i + 1 < k { // 窗口长度不足 k，尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            ans = ans.max(vowel);

            // 3. 左端点离开窗口，为下一个循环做准备
            let out = s[i + 1 - k];
            if out == b'a' || out == b'e' || out == b'i' || out == b'o' || out == b'u' {
                vowel -= 1;
            }
        }
        ans
    }
}
```

## 优化

答案的最大值是 $k$，即窗口中全是元音。

如果发现答案等于 $k$，由于答案无法再变大，可以退出循环。

```py [sol-Python3]
class Solution:
    def maxVowels(self, s: str, k: int) -> int:
        ans = vowel = 0
        for i, c in enumerate(s):  # 枚举窗口右端点 i
            # 1. 右端点进入窗口
            if c in "aeiou":
                vowel += 1

            left = i - k + 1  # 窗口左端点
            if left < 0:  # 窗口长度不足 k，尚未形成第一个窗口
                continue

            # 2. 更新答案
            ans = max(ans, vowel)
            if ans == k:  # 答案已经等于理论最大值
                break  # 无需再循环

            # 3. 左端点离开窗口，为下一个循环做准备
            if s[left] in "aeiou":
                vowel -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maxVowels(String S, int k) {
        char[] s = S.toCharArray();
        int ans = 0;
        int vowel = 0;
        for (int i = 0; i < s.length; i++) { // 枚举窗口右端点 i
            // 1. 右端点进入窗口
            if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u') {
                vowel++;
            }

            int left = i - k + 1; // 窗口左端点
            if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            ans = Math.max(ans, vowel);
            if (ans == k) { // 答案已经等于理论最大值
                break; // 无需再循环
            }

            // 3. 左端点离开窗口，为下一个循环做准备
            char out = s[left];
            if (out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u') {
                vowel--;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxVowels(string s, int k) {
        int ans = 0, vowel = 0;
        for (int i = 0; i < s.size(); i++) { // 枚举窗口右端点 i
            // 1. 右端点进入窗口
            if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u') {
                vowel++;
            }

            int left = i - k + 1; // 窗口左端点
            if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            ans = max(ans, vowel);
            if (ans == k) { // 答案已经等于理论最大值
                break; // 无需再循环
            }

            // 3. 左端点离开窗口，为下一个循环做准备
            char out = s[left];
            if (out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u') {
                vowel--;
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxVowels(char* s, int k) {
    int ans = 0, vowel = 0;
    for (int i = 0; s[i]; i++) { // 枚举窗口右端点 i
        // 1. 右端点进入窗口
        if (s[i] == 'a' || s[i] == 'e' || s[i] == 'i' || s[i] == 'o' || s[i] == 'u') {
            vowel++;
        }

        int left = i - k + 1; // 窗口左端点
        if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
            continue;
        }

        // 2. 更新答案
        ans = MAX(ans, vowel);
        if (ans == k) { // 答案已经等于理论最大值
            break; // 无需再循环
        }

        // 3. 左端点离开窗口，为下一个循环做准备
        char out = s[left];
        if (out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u') {
            vowel--;
        }
    }
    return ans;
}
```

```go [sol-Go]
func maxVowels(s string, k int) (ans int) {
    vowel := 0
    for i, in := range s { // 枚举窗口右端点 i
        // 1. 右端点进入窗口
        if in == 'a' || in == 'e' || in == 'i' || in == 'o' || in == 'u' {
            vowel++
        }

        left := i - k + 1 // 窗口左端点
        if left < 0 { // 窗口长度不足 k，尚未形成第一个窗口
            continue
        }

        // 2. 更新答案
        ans = max(ans, vowel)
        if ans == k { // 答案已经等于理论最大值
            break // 无需再循环
        }

        // 3. 左端点离开窗口，为下一个循环做准备
        out := s[left]
        if out == 'a' || out == 'e' || out == 'i' || out == 'o' || out == 'u' {
            vowel--
        }
    }
    return
}
```

```js [sol-JavaScript]
var maxVowels = function(s, k) {
    let ans = 0, vowel = 0;
    for (let i = 0; i < s.length; i++) { // 枚举窗口右端点 i
        // 1. 右端点进入窗口
        if (s[i] === 'a' || s[i] === 'e' || s[i] === 'i' || s[i] === 'o' || s[i] === 'u') {
            vowel++;
        }

        const left = i - k + 1; // 窗口左端点
        if (left < 0) { // 窗口长度不足 k，尚未形成第一个窗口
            continue;
        }

        // 2. 更新答案
        ans = Math.max(ans, vowel);
        if (ans === k) { // 答案已经等于理论最大值
            break; // 无需再循环
        }

        // 3. 左端点离开窗口，为下一个循环做准备
        let out = s[left];
        if (out === 'a' || out === 'e' || out === 'i' || out === 'o' || out === 'u') {
            vowel--;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_vowels(s: String, k: i32) -> i32 {
        let s = s.as_bytes();
        let k = k as usize;
        let mut ans = 0;
        let mut vowel = 0;
        for (i, &c) in s.iter().enumerate() { // 枚举窗口右端点 i
            // 1. 右端点进入窗口
            if c == b'a' || c == b'e' || c == b'i' || c == b'o' || c == b'u' {
                vowel += 1;
            }

            if i + 1 < k { // 窗口长度不足 k，尚未形成第一个窗口
                continue;
            }

            // 2. 更新答案
            ans = ans.max(vowel);
            if ans == k { // 答案已经等于理论最大值
                break; // 无需再循环
            }

            // 3. 左端点离开窗口，为下一个循环做准备
            let out = s[i + 1 - k];
            if out == b'a' || out == b'e' || out == b'i' || out == b'o' || out == b'u' {
                vowel -= 1;
            }
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `一、定长滑动窗口 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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
    int maxVowels(string s, int k) {
        auto isVowel = [](char c) {
            return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
        };
        int cur = 0, ans = 0;
        for (int i = 0; i < (int)s.size(); ++i) {
            cur += isVowel(s[i]);
            if (i >= k) cur -= isVowel(s[i - k]);
            if (i + 1 >= k) ans = max(ans, cur);
        }
        return ans;
    }
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

回到来源分类 `一、定长滑动窗口 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
